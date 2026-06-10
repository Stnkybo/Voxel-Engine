//
// Created by nikolas on 5/30/26.
//

#include <map>
#include <SDL3/SDL_vulkan.h>

#include "../../game.h"

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
            case vk::PhysicalDeviceType::eIntegratedGpu: qualityScore += 100;
            case vk::PhysicalDeviceType::eVirtualGpu: qualityScore += 10;
            case vk::PhysicalDeviceType::eCpu: qualityScore += 1;
            default: qualityScore += 0;
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
                                    features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().
                                    extendedDynamicState;

    // Return true if the physicalDevice meets all the criteria
    return supportsVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures;
}


void Game::vkCreateLogicalDevice() {
    // find the index of the first queue family that supports graphics
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_vkPhysicalDevice.getQueueFamilyProperties();

    // get the first index into queueFamilyProperties which supports graphics
    auto graphicsQueueFamilyProperty = std::ranges::find_if(queueFamilyProperties, [](auto const &qfp) {
        return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0);
    });
    assert(graphicsQueueFamilyProperty != queueFamilyProperties.end() && "No graphics queue family found!");

    auto graphicsIndex = static_cast<uint32_t>(
        std::distance(queueFamilyProperties.begin(), graphicsQueueFamilyProperty));

    // query for Vulkan 1.3 features
    vk::StructureChain<vk::PhysicalDeviceFeatures2,
                vk::PhysicalDeviceVulkan11Features,
                vk::PhysicalDeviceVulkan13Features,
                vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
            featureChain = {
                {}, // vk::PhysicalDeviceFeatures2
                {.shaderDrawParameters = true}, // vk::PhysicalDeviceVulkan11Features
                {.dynamicRendering = true}, // vk::PhysicalDeviceVulkan13Features
                {.extendedDynamicState = true} // vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
            };

    // create a Device
    float queuePriority = 0.5f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
        .queueFamilyIndex = graphicsIndex, .queueCount = 1, .pQueuePriorities = &queuePriority
    };
    vk::DeviceCreateInfo deviceCreateInfo{
        .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &deviceQueueCreateInfo,
        .enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtension.size()),
        .ppEnabledExtensionNames = requiredDeviceExtension.data()
    };

    m_vkDevice = vk::raii::Device(m_vkPhysicalDevice, deviceCreateInfo);
    m_vkGraphicsQueue = vk::raii::Queue(m_vkDevice, graphicsIndex, 0);
}


void Game::vkCreateSurface() {
    VkSurfaceKHR _surface;
    if (!SDL_Vulkan_CreateSurface(m_window, static_cast<VkInstance>(*m_vkInstance), nullptr, &_surface)) {
        throw std::runtime_error("failed to create window surface: " + std::string(SDL_GetError()));
    }
    m_vkSurface = vk::raii::SurfaceKHR(m_vkInstance, _surface);
    std::cout << "VK Surface created" << std::endl;
}