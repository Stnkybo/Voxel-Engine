//
// Created by Lamad on 4/09/2026.
//

#ifndef SDL3PRJ_COLORVERTEX_H
#define SDL3PRJ_COLORVERTEX_H
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#include <glm/glm.hpp>
#include <vulkan/vulkan_structs.hpp>

struct colorVertex
{
    glm::vec2 pos;
    glm::vec3 color;

    static vk::VertexInputBindingDescription getBindingDescription()
    {
        return {.binding = 0, .stride = sizeof(colorVertex), .inputRate = vk::VertexInputRate::eVertex};
    }

    static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions()
    {
        return {{{.location = 0, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = offsetof(colorVertex, pos)},
               {.location = 1, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(colorVertex, color)}}};
    }
};

const std::vector<colorVertex> colorVertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> colorIndices = {
    0, 1, 2, 2, 3, 0
};

#endif //SDL3PRJ_COLORVERTEX_H
