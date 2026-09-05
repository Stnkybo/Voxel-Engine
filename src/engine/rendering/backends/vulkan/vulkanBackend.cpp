//
// Created by nikolas on 5/30/26.
//

#include <map>
#include <SDL3/SDL_vulkan.h>

#include "../../../game.h"
#include "vulkanBackend.h"
#include "colorVertex.h"

bool isDeviceSuitable(vk::raii::PhysicalDevice const &physicalDevice);

void Game::vkPickPhysicalDevice() {
    auto physicalDevices = vk::raii::PhysicalDevices(m_vkInstance);

    if (physicalDevices.empty()) {
        throw std::runtime_error("No physical devices with Vulkan support found");
    }

    // Use an ordered map to automatically sort candidates by increasing score
    multimap<int, vk::raii::PhysicalDevice> candidates;

    for (auto &physicalDevice: physicalDevices) {
        auto deviceProperties = physicalDevice.getProperties();
        auto deviceFeatures = physicalDevice.getFeatures();

        std::cout << "Found Device: " << deviceProperties.deviceName << std::endl;

        if (!isDeviceSuitable(physicalDevice))
            continue;


        uint32_t qualityScore = 0;

        // Rank Devices by type
        switch (deviceProperties.deviceType) {
            case vk::PhysicalDeviceType::eDiscreteGpu: qualityScore += 1000;
                break;
            case vk::PhysicalDeviceType::eIntegratedGpu: qualityScore += 100;
                break;
            case vk::PhysicalDeviceType::eVirtualGpu: qualityScore += 10;
                break;
            case vk::PhysicalDeviceType::eCpu: qualityScore += 1;
                break;
            default: qualityScore += 0;
                break;
        }

        // Maximum possible size of textures affects graphics quality
        qualityScore += deviceProperties.limits.maxImageDimension2D;


        std::cout << "Device Score: " << qualityScore << std::endl;

        // If suitable add to list of candidates
        candidates.insert(std::make_pair(qualityScore, physicalDevice));
    }

    // Check if the best candidate is suitable at all
    if (!candidates.empty() && candidates.rbegin()->first > 0) {
        m_vkPhysicalDevice = candidates.rbegin()->second;
        std::cout << "Selected Device: " << m_vkPhysicalDevice.getProperties().deviceName << std::endl;
    } else {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

std::vector<const char *> requiredDeviceExtension = {vk::KHRSwapchainExtensionName};

bool isDeviceSuitable(vk::raii::PhysicalDevice const &physicalDevice) {
    // Check if the physicalDevice supports the Vulkan 1.3 API version
    bool supportsVulkan1_3 = physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;

    // Check if any of the queue families support graphics operations
    auto queueFamilies = physicalDevice.getQueueFamilyProperties();
    bool supportsGraphics = std::ranges::any_of(queueFamilies, [](auto const &qfp) {
        return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
    });

    // Check if all required physicalDevice extensions are available
    auto availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
    bool supportsAllRequiredExtensions =
            std::ranges::all_of(requiredDeviceExtension,
                                [&availableDeviceExtensions](auto const &requiredDeviceExtension) {
                                    return std::ranges::any_of(availableDeviceExtensions,
                                                               [requiredDeviceExtension](
                                                           auto const &availableDeviceExtension) {
                                                                   return strcmp(availableDeviceExtension.extensionName,
                                                                              requiredDeviceExtension) == 0;
                                                               });
                                });

    // Check if the physicalDevice supports the required features (shader draw parameters, dynamic rendering and extended dynamic state)
    auto features = physicalDevice.template getFeatures2<vk::PhysicalDeviceFeatures2,
        vk::PhysicalDeviceVulkan11Features,
        vk::PhysicalDeviceVulkan13Features,
        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
    bool supportsRequiredFeatures = features.template get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
                                    features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
                                    features.template get<vk::PhysicalDeviceVulkan13Features>().synchronization2 &&
                                    features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().
                                    extendedDynamicState;

    // Return true if the physicalDevice meets all the criteria
    return supportsVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures;
}


void Game::vkCreateLogicalDevice() {
    // find the index of the first queue family that supports graphics
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_vkPhysicalDevice.getQueueFamilyProperties();

    for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++) {
        if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) &&
            m_vkPhysicalDevice.getSurfaceSupportKHR(qfpIndex, *m_vkSurface)) {
            // found a queue family that supports both graphics and present
            m_vkQueueIndex = qfpIndex;
            break;
        }
    }
    if (m_vkQueueIndex == ~0) {
        throw std::runtime_error("Could not find a queue for graphics and present -> terminating");
    }

    // query for Vulkan 1.3 features
    vk::StructureChain<vk::PhysicalDeviceFeatures2,
                vk::PhysicalDeviceVulkan11Features,
                vk::PhysicalDeviceVulkan13Features,
                vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
            featureChain = {
                {}, // vk::PhysicalDeviceFeatures2
                {.shaderDrawParameters = true}, // vk::PhysicalDeviceVulkan11Features
                {.synchronization2 = true, .dynamicRendering = true}, // vk::PhysicalDeviceVulkan13Features
                {.extendedDynamicState = true} // vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
            };

    // create a Device
    float queuePriority = 0.5f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
        .queueFamilyIndex = m_vkQueueIndex, .queueCount = 1, .pQueuePriorities = &queuePriority
    };

    vk::DeviceCreateInfo deviceCreateInfo{
        .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &deviceQueueCreateInfo,
        .enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtension.size()),
        .ppEnabledExtensionNames = requiredDeviceExtension.data()
    };

    m_vkDevice = vk::raii::Device(m_vkPhysicalDevice, deviceCreateInfo);
    m_vkGraphicsQueue = vk::raii::Queue(m_vkDevice, m_vkQueueIndex, 0);
}


void Game::vkCreateSurface() {
    VkSurfaceKHR _surface;
    if (!SDL_Vulkan_CreateSurface(m_window, static_cast<VkInstance>(*m_vkInstance), nullptr, &_surface)) {
        throw std::runtime_error("failed to create window surface: " + std::string(SDL_GetError()));
    }
    m_vkSurface = vk::raii::SurfaceKHR(m_vkInstance, _surface);
    std::cout << "VK Surface created" << std::endl;
}

// Swapchain Helpers
vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
    const auto formatIt = std::ranges::find_if(
        availableFormats,
        [](const auto &format) {
            return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
        });
    return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
}

vk::PresentModeKHR chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const &availablePresentModes) {
    assert(
        std::ranges::any_of(availablePresentModes, [](auto presentMode) { return presentMode == vk::PresentModeKHR::
            eFifo; }));
    return std::ranges::any_of(availablePresentModes,
                               [](const vk::PresentModeKHR value) { return vk::PresentModeKHR::eMailbox == value; })
               ? vk::PresentModeKHR::eMailbox
               : vk::PresentModeKHR::eFifo;
}

vk::Extent2D Game::chooseSwapExtent(vk::SurfaceCapabilitiesKHR const &capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    int width, height;
    SDL_GetWindowSizeInPixels(m_window, &width, &height);

    return {
        std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
    };
}

uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const &surfaceCapabilities) {
    auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
    if ((0 < surfaceCapabilities.maxImageCount) && (surfaceCapabilities.maxImageCount < minImageCount)) {
        minImageCount = surfaceCapabilities.maxImageCount;
    }
    return minImageCount;
}


void Game::vkCreateSwapchain() {
    // setup swapchain
    vk::SurfaceCapabilitiesKHR surfaceCapabilities = m_vkPhysicalDevice.getSurfaceCapabilitiesKHR(*m_vkSurface);
    m_vkSwapChainExtent = chooseSwapExtent(surfaceCapabilities);
    uint32_t minImageCount = chooseSwapMinImageCount(surfaceCapabilities);

    std::vector<vk::SurfaceFormatKHR> availableFormats = m_vkPhysicalDevice.getSurfaceFormatsKHR(*m_vkSurface);
    m_vkSwapChainSurfaceFormat = chooseSwapSurfaceFormat(availableFormats);

    std::vector<vk::PresentModeKHR> availablePresentModes = m_vkPhysicalDevice.getSurfacePresentModesKHR(m_vkSurface);

    vk::SwapchainCreateInfoKHR swapChainCreateInfo{
        .surface = *m_vkSurface,
        .minImageCount = minImageCount,
        .imageFormat = m_vkSwapChainSurfaceFormat.format,
        .imageColorSpace = m_vkSwapChainSurfaceFormat.colorSpace,
        .imageExtent = m_vkSwapChainExtent,
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform = surfaceCapabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = chooseSwapPresentMode(availablePresentModes),
        .clipped = true
    };

    m_vkSwapChain = vk::raii::SwapchainKHR(m_vkDevice, swapChainCreateInfo);
    m_vkSwapChainImages = m_vkSwapChain.getImages();

    std::cout << "VK Swapchain created successfully" << std::endl;
};

void Game::vkCreateImageViews() {
    assert(m_vkSwapChainImageViews.empty());

    vk::ImageViewCreateInfo imageViewCreateInfo{
        .viewType = vk::ImageViewType::e2D,
        .format = m_vkSwapChainSurfaceFormat.format,
        .subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
    };

    for (auto &image: m_vkSwapChainImages) {
        imageViewCreateInfo.image = image;
        m_vkSwapChainImageViews.emplace_back(m_vkDevice, imageViewCreateInfo);
    }
}

void Game::vkCreateGraphicsPipeline() {
    vk::raii::ShaderModule shaderModule = vkCreateShaderModule(readFile("resources/shaders/slang/slang.spv"));

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{
        .stage = vk::ShaderStageFlagBits::eVertex, .module = shaderModule, .pName = "vertMain"
    };
    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{
        .stage = vk::ShaderStageFlagBits::eFragment, .module = shaderModule, .pName = "fragMain"
    };
    vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    auto bindingDescription = colorVertex::getBindingDescription();
    auto attributeDescriptions = colorVertex::getAttributeDescriptions();
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &bindingDescription,
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
        .pVertexAttributeDescriptions = attributeDescriptions.data()
    };

    // Specify format for pipeline input, change to triangle strip for voxels
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{.topology = vk::PrimitiveTopology::eTriangleList};

    vk::PipelineViewportStateCreateInfo viewportState{.viewportCount = 1, .scissorCount = 1};


    // Create rasterizer
    vk::PipelineRasterizationStateCreateInfo rasterizer{
        .depthClampEnable = vk::False,
        .rasterizerDiscardEnable = vk::False,
        .polygonMode = vk::PolygonMode::eFill,
        .cullMode = vk::CullModeFlagBits::eBack,
        .frontFace = vk::FrontFace::eClockwise,
        .depthBiasEnable = vk::False,
        .lineWidth = 1.0f
    };

    vk::PipelineMultisampleStateCreateInfo multisampling{
        .rasterizationSamples = vk::SampleCountFlagBits::e1, .sampleShadingEnable = vk::False
    };

    // NEEDS COLOUR BLENDING
    vk::PipelineColorBlendAttachmentState colorBlendAttachment{
        .blendEnable = vk::False,
        .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                          vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
    };

    vk::PipelineColorBlendStateCreateInfo colorBlending{
        .logicOpEnable = vk::False, .logicOp = vk::LogicOp::eCopy, .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment
    };

    // Dynamic state stuff for window management
    std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

    vk::PipelineDynamicStateCreateInfo dynamicState{
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()), .pDynamicStates = dynamicStates.data()
    };


    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{.setLayoutCount = 0, .pushConstantRangeCount = 0};

    m_vkPipelineLayout = vk::raii::PipelineLayout(m_vkDevice, pipelineLayoutInfo);


    // Create Render Pass info
    vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo{
        .colorAttachmentCount = 1, .pColorAttachmentFormats = &m_vkSwapChainSurfaceFormat.format
    };

    vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain = {
        {
            .stageCount = 2,
            .pStages = shaderStages,
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssembly,
            .pViewportState = &viewportState,
            .pRasterizationState = &rasterizer,
            .pMultisampleState = &multisampling,
            .pColorBlendState = &colorBlending,
            .pDynamicState = &dynamicState,
            .layout = m_vkPipelineLayout,
            .renderPass = nullptr // null for dynamic rendering
        },
        {.colorAttachmentCount = 1, .pColorAttachmentFormats = &m_vkSwapChainSurfaceFormat.format}
    };

    // This is not required by us yet
    // vk::SubpassDependency dependency{
    //     .srcSubpass = vk::SubpassExternal,
    //     .dstSubpass = 0,
    //     .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
    //     .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
    //     .srcAccessMask = vk::AccessFlagBits::eNone,
    //     .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite
    // };


    // Finally Create the pipeline
    m_vkGraphicsPipeline = vk
            ::raii::Pipeline(m_vkDevice, nullptr,
                             pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());

    std::cout << "Created Vulkan Graphics Pipeline" << std::endl;
}

[[nodiscard]] vk::raii::ShaderModule Game::vkCreateShaderModule(const std::vector<char> &code) const {
    vk::ShaderModuleCreateInfo createInfo{
        .codeSize = code.size() * sizeof(char), .pCode = reinterpret_cast<const uint32_t *>(code.data())
    };
    vk::raii::ShaderModule shaderModule{m_vkDevice, createInfo};
    return shaderModule;
}

void Game::vkCreateCommandPool() {
    vk::CommandPoolCreateInfo poolInfo{
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = m_vkQueueIndex
    };

    m_vkCommandPool = vk::raii::CommandPool(m_vkDevice, poolInfo);
}

void Game::vkCreateCommandBuffer() {
    vk::CommandBufferAllocateInfo allocInfo{
        .commandPool = m_vkCommandPool, .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = MAX_FRAMES_IN_FLIGHT
    };

    m_vkCommandBuffers = vk::raii::CommandBuffers(m_vkDevice, allocInfo);
}

void Game::vkRecordCommandBuffer(uint32_t imageIndex) {
    auto &commandBuffer = m_vkCommandBuffers[m_frameIndex];
    commandBuffer.begin({});

    // Before starting rendering, transition the swapchain image to vk::ImageLayout::eColorAttachmentOptimal
    vkTransition_image_layout(
        imageIndex,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal,
        {}, // srcAccessMask (no need to wait for previous operations)
        vk::AccessFlagBits2::eColorAttachmentWrite, // dstAccessMask
        vk::PipelineStageFlagBits2::eColorAttachmentOutput, // srcStage
        vk::PipelineStageFlagBits2::eColorAttachmentOutput // dstStage
    );

    // Create info for rendering
    vk::ClearValue clearValue = vk::ClearValue{vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f})};

    vk::RenderingAttachmentInfo attachmentInfo = {
        .imageView = m_vkSwapChainImageViews[imageIndex],
        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = clearValue
    };

    vk::RenderingInfo renderingInfo = {
        .renderArea = {.offset = {0, 0}, .extent = m_vkSwapChainExtent},
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachmentInfo
    };

    // Add Render Commands to the Command buffer, set task for the GPU to do
    commandBuffer.beginRendering(renderingInfo);

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *m_vkGraphicsPipeline);


    commandBuffer.setViewport(0, vk::Viewport(0, 0, m_vkSwapChainExtent.width, m_vkSwapChainExtent.height, 0.0f,
                                                   1.0f));
    commandBuffer.setScissor(0, vk::Rect2D({0, 0}, m_vkSwapChainExtent));

    // Set to draw the color verts in the vertex buffer
    commandBuffer.bindVertexBuffers(0,*vertexBuffer, {0});
    commandBuffer.bindIndexBuffer(*indexBuffer, 0, vk::IndexType::eUint16);

    commandBuffer.drawIndexed(static_cast<glm::uint32_t>(colorIndices.size()), 1, 0, 0,0);

    commandBuffer.endRendering();


    // After rendering, transition the swapchain image to vk::ImageLayout::ePresentSrcKHR
    vkTransition_image_layout(
        imageIndex,
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        vk::AccessFlagBits2::eColorAttachmentWrite, // srcAccessMask
        {}, // dstAccessMask
        vk::PipelineStageFlagBits2::eColorAttachmentOutput, // srcStage
        vk::PipelineStageFlagBits2::eBottomOfPipe // dstStage
    );

    commandBuffer.end();
}

void Game::vkTransition_image_layout(
    uint32_t imageIndex,
    vk::ImageLayout old_layout,
    vk::ImageLayout new_layout,
    vk::AccessFlags2 src_access_mask,
    vk::AccessFlags2 dst_access_mask,
    vk::PipelineStageFlags2 src_stage_mask,
    vk::PipelineStageFlags2 dst_stage_mask) {
    vk::ImageMemoryBarrier2 barrier = {
        .srcStageMask = src_stage_mask,
        .srcAccessMask = src_access_mask,
        .dstStageMask = dst_stage_mask,
        .dstAccessMask = dst_access_mask,
        .oldLayout = old_layout,
        .newLayout = new_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = m_vkSwapChainImages[imageIndex],
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };
    vk::DependencyInfo dependency_info = {
        .dependencyFlags = {},
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &barrier
    };
    m_vkCommandBuffers[m_frameIndex].pipelineBarrier2(dependency_info);
}

void Game::vkCreateSyncObjects() {
    assert(presentCompleteSemaphores.empty() && renderFinishedSemaphores.empty() && inFlightFences.empty());

    for (size_t i = 0; i < m_vkSwapChainImages.size(); i++) {
        renderFinishedSemaphores.emplace_back(m_vkDevice, vk::SemaphoreCreateInfo());
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        presentCompleteSemaphores.emplace_back(m_vkDevice, vk::SemaphoreCreateInfo());
        inFlightFences.emplace_back(m_vkDevice, vk::FenceCreateInfo{.flags = vk::FenceCreateFlagBits::eSignaled});
    }
}

void Game::vkRecreateSwapChain() {


    m_vkDevice.waitIdle();

    vkCleanupSwapChain();

    vkCreateSwapchain();
    vkCreateImageViews();
}

void Game::vkCleanupSwapChain() {
    m_vkSwapChainImageViews.clear();
    m_vkSwapChain = nullptr;

}

void Game::vkCreateVertexBuffer()
{
    vk::DeviceSize bufferSize = sizeof(colorVertices[0]) * colorVertices.size();
    auto [stagingBuffer, stagingBufferMemory] = createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);


    // Fill Vertex Buffer
    void* dataStaging  = stagingBufferMemory.mapMemory(0, bufferSize);

    memcpy(dataStaging, colorVertices.data(), bufferSize);
    stagingBufferMemory.unmapMemory();

    std::tie(vertexBuffer, vertexBufferMemory) = createBuffer(bufferSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);

    copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

}
void Game::vkCreateIndexBuffer()
{
    vk::DeviceSize bufferSize = sizeof(colorIndices[0]) * colorIndices.size();
    auto [stagingBuffer, stagingBufferMemory] = createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);


    // Fill Vertex Buffer
    void* dataStaging  = stagingBufferMemory.mapMemory(0, bufferSize);

    memcpy(dataStaging, colorIndices.data(), bufferSize);
    stagingBufferMemory.unmapMemory();

    std::tie(indexBuffer, indexBufferMemory) = createBuffer(bufferSize, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);

    copyBuffer(stagingBuffer, indexBuffer, bufferSize);

}

uint32_t Game::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {

    vk::PhysicalDeviceMemoryProperties memProperties = m_vkPhysicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> Game::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties)
{
    vk::BufferCreateInfo   bufferInfo{.size = size, .usage = usage, .sharingMode = vk::SharingMode::eExclusive};
    vk::raii::Buffer       buffer          = vk::raii::Buffer(m_vkDevice, bufferInfo);
    vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();
    vk::MemoryAllocateInfo allocInfo{.allocationSize = memRequirements.size, .memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties)};
    vk::raii::DeviceMemory bufferMemory = vk::raii::DeviceMemory(m_vkDevice, allocInfo);
    buffer.bindMemory(*bufferMemory, 0);
    return {std::move(buffer), std::move(bufferMemory)};
}

void Game::copyBuffer(vk::raii::Buffer & srcBuffer, vk::raii::Buffer & dstBuffer, vk::DeviceSize size)
{
    vk::CommandBufferAllocateInfo allocInfo{ .commandPool = m_vkCommandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1 };
    vk::raii::CommandBuffer commandCopyBuffer = std::move(m_vkDevice.allocateCommandBuffers(allocInfo).front());

    commandCopyBuffer.begin({.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    commandCopyBuffer.copyBuffer(*srcBuffer, *dstBuffer, vk::BufferCopy(0, 0, size));
    commandCopyBuffer.end();

    m_vkGraphicsQueue.submit(vk::SubmitInfo{.commandBufferCount = 1, .pCommandBuffers = &*commandCopyBuffer}, nullptr);
    m_vkGraphicsQueue.waitIdle();
}