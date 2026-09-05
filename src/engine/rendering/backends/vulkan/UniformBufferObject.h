//
// Created by Lamad on 6/09/2026.
//

#ifndef SDL3PRJ_UNIFORMBUFFEROBJECT_H
#define SDL3PRJ_UNIFORMBUFFEROBJECT_H
#include <glm/glm.hpp>

struct UniformBufferObject
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

#endif //SDL3PRJ_UNIFORMBUFFEROBJECT_H
