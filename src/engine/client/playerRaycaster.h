//
// Created by nikolas on 5/8/26.

#ifndef SDL3PRJ_PLAYERRAYCASTER_H
#define SDL3PRJ_PLAYERRAYCASTER_H
#include "../terrain/world.h"
#include "glm/common.hpp"
#include "glm/vec3.hpp"

struct raycastResult {
    bool hit;
    glm::ivec3 blockPosition;
    glm::ivec3 normal;
    float distance;
};

/**
 * This function is for raycasting from the player to the terrain to interact with specific voxels
 */
inline raycastResult playerVoxelInteractionRaycast( glm::vec3 origin, glm::vec3 direction, float maxDistance, World *world) {
    //
    // take player head as origin and direction of camera as a vector

    // current voxel position
    glm::ivec3 currentPos = glm::floor(origin);

    // figure out which direction to probe on each axis
    glm::ivec3 step = glm::sign(direction);

    glm::vec3 deltaDistance = glm::abs(1.0f / direction);

    // finding the distance to the first voxel boundary on each axis
    glm::vec3 sideDistance;
    sideDistance.x = (direction.x > 0 ? (currentPos.x + 1 - origin.x) : (origin.x - currentPos.x)) * deltaDistance.x;
    sideDistance.y = (direction.y > 0 ? (currentPos.y + 1 - origin.y) : (origin.y - currentPos.y)) * deltaDistance.y;
    sideDistance.z = (direction.z > 0 ? (currentPos.z + 1 - origin.z) : (origin.z - currentPos.z)) * deltaDistance.z;

    glm::ivec3 normal = {0, 0, 0};

    // for each voxel in the path of the ray, until the max distance
    while (true) {

        // check the voxel to see if the block is solid
        const Voxel* currentVoxel = world->getBlock(currentPos);
            // if solid, mark as hit and calculate ray information
        if (currentVoxel && getBlockType(*currentVoxel) != BlockType::AIR) {
            return raycastResult{
                true,
                currentPos,
                normal,
                length(glm::vec3(currentPos) - origin)
            };
        }

        // if probe distance exceeds maxDistance, end the raycast
        if ( glm::length(glm::vec3(currentPos) - origin) > maxDistance )
            return {false};
        
        // step along the shortest axis
        if (sideDistance.x < sideDistance.y && sideDistance.x < sideDistance.z) {
            sideDistance.x += deltaDistance.x;
            currentPos.x += step.x;
            normal = { -step.x, 0, 0 };
        } else if (sideDistance.y < sideDistance.z) {
            sideDistance.y += deltaDistance.y;
            currentPos.y += step.y;
            normal = { 0, -step.y, 0 };
        } else {
            sideDistance.z += deltaDistance.z;
            currentPos.z += step.z;
            normal = { 0, 0, -step.z };
        }
        
    }
    


}
#endif //SDL3PRJ_PLAYERRAYCASTER_H

