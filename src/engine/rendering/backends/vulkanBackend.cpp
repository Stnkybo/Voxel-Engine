//
// Created by nikolas on 5/30/26.
//

#include "../../game.h"

void Game::vkPickPhysicalDevice() {
    auto physicalDevices = m_vkInstance.enumeratePhysicalDevices();

    if (physicalDevices.empty()) {
        throw std::runtime_error("No physical devices with Vulkan support found");
    }

    for (auto physicalDevice : physicalDevices) {
        auto deviceProperties = physicalDevice.getProperties();

        auto deviceFeatures = physicalDevice.getFeatures();
        break;
    }
}