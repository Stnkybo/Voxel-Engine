//
// Created by Lamad on 19/07/2025.
//

#ifndef AABB_H
#define AABB_H
#include <glm/vec3.hpp>


class AABB {
public:
    glm::vec3 min;
    glm::vec3 max;
    // Create from center + size
    static AABB fromCenterSize(const glm::vec3& center, const glm::vec3& size) {
        glm::vec3 halfSize = size * 0.5f;
        return AABB(center - halfSize, center + halfSize);
    }

    // AABB-AABB intersection test
    bool intersects(const AABB& other) const {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
               (min.y <= other.max.y && max.y >= other.min.y) &&
               (min.z <= other.max.z && max.z >= other.min.z);
    }

private:



};



#endif //AABB_H
