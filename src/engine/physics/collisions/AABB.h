//
// Created by Lamad on 19/07/2025.
//

#ifndef AABB_H
#define AABB_H
#include <glm/vec3.hpp>
#include <cmath>


class AABB {
public:
    glm::vec3 min;
    glm::vec3 max;

    // Create an AABB from center + size
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
    // Calculate the minimum translation vector between two bounding boxes
    static glm::vec3 getMTV(const AABB& a, const AABB& b) {
        glm::vec3 mtv(0.0f);

        float dx1 = b.max.x - a.min.x; // overlap from b right to a left
        float dx2 = a.max.x - b.min.x; // overlap from a right to b left
        float ox = (dx1 < dx2) ? dx1 : -dx2;

        float dy1 = b.max.y - a.min.y;
        float dy2 = a.max.y - b.min.y;
        float oy = (dy1 < dy2) ? dy1 : -dy2;

        float dz1 = b.max.z - a.min.z;
        float dz2 = a.max.z - b.min.z;
        float oz = (dz1 < dz2) ? dz1 : -dz2;

        // Find the axis of least penetration (smallest absolute overlap)
        float absOx = std::abs(ox);
        float absOy = std::abs(oy);
        float absOz = std::abs(oz);

        if (absOx < absOy && absOx < absOz) {
            mtv.x = ox;
        } else if (absOy < absOz) {
            mtv.y = oy;
        } else {
            mtv.z = oz;
        }

        return mtv;
    }


private:



};



#endif //AABB_H
