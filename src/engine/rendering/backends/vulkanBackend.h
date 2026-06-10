//
// Created by nikolas on 5/30/26.
//

#ifndef SDL3PRJ_VULKANBACKEND_H
#define SDL3PRJ_VULKANBACKEND_H

#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#include <SDL3/SDL_vulkan.h>

#include "../../game.h"

const std::vector<char const *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void *)
{
    if (severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError || severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
    {
        std::cerr << "validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;
    }

    return vk::False;
}

inline void Game::vkSetupDebugMessenger()
{
    if (!enableValidationLayers)
        return;

    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
    vk::DebugUtilsMessageTypeFlagsEXT     messageTypeFlags(
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
    vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{.messageSeverity = severityFlags,
                                                                          .messageType     = messageTypeFlags,
                                                                          .pfnUserCallback = &debugCallback};
    m_vkDebugMessenger = m_vkInstance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
}

inline std::vector<const char *> getRequiredInstanceExtensions()
{


    uint32_t sdlVulkanExtensionCount = 0;

    auto sdlVulkanExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlVulkanExtensionCount);

    // Get required Vulkan instance extensions using SDL.
    if (!sdlVulkanExtensions) {
        throw std::runtime_error("Failed to get SDL Vulkan extensions: " + std::string(SDL_GetError()));
    }

    std::vector extensions(sdlVulkanExtensions, sdlVulkanExtensions + sdlVulkanExtensionCount);
    if (enableValidationLayers)
    {
        extensions.push_back(vk::EXTDebugUtilsExtensionName);
    }

    return extensions;
}

inline void Game::vkCreateInstance() {

    vk::ApplicationInfo appInfo{.pApplicationName   = m_title,
                                             .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                                             .pEngineName        = "Stnk Engine",
                                             .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
                                             .apiVersion         = vk::ApiVersion14};

    // Get the required layers
    std::vector<char const *> requiredLayers;
    if (enableValidationLayers)
    {
        requiredLayers.assign(validationLayers.begin(), validationLayers.end());
    }

    // Check if the required layers are supported by the Vulkan implementation.
    auto layerProperties    = m_vkContext.enumerateInstanceLayerProperties();
    auto unsupportedLayerIt = std::ranges::find_if(requiredLayers,
                                                   [&layerProperties](auto const &requiredLayer) {
                                                       return std::ranges::none_of(layerProperties,
                                                                                   [requiredLayer](auto const &layerProperty) { return strcmp(layerProperty.layerName, requiredLayer) == 0; });
                                                   });
    if (unsupportedLayerIt != requiredLayers.end())
    {
        throw std::runtime_error("Required layer not supported: " + std::string(*unsupportedLayerIt));
    }

    // Get the required extensions.
    auto requiredExtensions = getRequiredInstanceExtensions();

    // Check if the required extensions are supported by the Vulkan implementation.
    auto extensionProperties = m_vkContext.enumerateInstanceExtensionProperties();
    auto unsupportedPropertyIt =
        std::ranges::find_if(requiredExtensions,
                             [&extensionProperties](auto const &requiredExtension) {
                                 return std::ranges::none_of(extensionProperties,
                                                             [requiredExtension](auto const &extensionProperty) { return strcmp(extensionProperty.extensionName, requiredExtension) == 0; });
                             });
    if (unsupportedPropertyIt != requiredExtensions.end())
    {
        throw std::runtime_error("Required extension not supported: " + std::string(*unsupportedPropertyIt));
    }

    vk::InstanceCreateInfo createInfo{.pApplicationInfo        = &appInfo,
                                      .enabledLayerCount       = static_cast<uint32_t>(requiredLayers.size()),
                                      .ppEnabledLayerNames     = requiredLayers.data(),
                                      .enabledExtensionCount   = static_cast<uint32_t>(requiredExtensions.size()),
                                      .ppEnabledExtensionNames = requiredExtensions.data()};

    m_vkInstance = vk::raii::Instance(m_vkContext, createInfo);

    std::cout << "VK instance created successfully" << std::endl;
}

#endif //SDL3PRJ_VULKANBACKEND_H
