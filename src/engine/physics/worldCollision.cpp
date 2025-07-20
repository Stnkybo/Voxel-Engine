//
// Created by Lamad on 18/07/2025.
//

#include "worldCollision.h"

bool worldCollision::isSolid(int x, int y, int z, World* world) {
        Voxel* block = world->getBlock(x,y,z);
        if ( block != nullptr) {

            return block->type =! static_cast<uint8_t>(BlockType::AIR);
        }
        return false;
}

void worldCollision::resolveCollisions(AABB player, World* world) {
    for (int x = floor(player.min.x); x <= floor(player.max.x); ++x) {
        for (int y = floor(player.min.y); y <= floor(player.max.y); ++y) {
            for (int z = floor(player.min.z); z <= floor(player.max.z); ++z) {
                if (isSolid(x, y, z, world)) {
                    AABB block = AABB::fromCenterSize({x, y, z}, {1,1,1});
                    if (player.intersects(block)) {
                        std::cout << "RAHHHH" << std::endl;
                        // Handle response here
                    }
                }
            }
        }
    }
}
