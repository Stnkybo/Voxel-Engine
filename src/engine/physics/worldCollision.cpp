//
// Created by Lamad on 18/07/2025.
//

#include "worldCollision.h"

#include "../../Entities/player.h"

bool worldCollision::isSolid(int x, int y, int z, World* world) {
        Voxel* block = world->getBlock(x,y,z);
        if ( block != nullptr) {

            return static_cast<uint8_t>(getBlockType(*block)) != static_cast<uint8_t>(BlockType::AIR);
        }
        return false;
}

void worldCollision::resolveCollisions(Player& player, World* world) {
    AABB& playerBox = player.getBoundingBox();
    for (int x = floor(playerBox.min.x); x <= ceil(playerBox.max.x); ++x) {
        for (int y = floor(playerBox.min.y); y <= ceil(playerBox.max.y); ++y) {
            for (int z = floor(playerBox.min.z); z <= ceil(playerBox.max.z); ++z) {
                if (isSolid(x, y, z, world)) {
                    AABB blockBox = AABB::fromCenterSize(glm::vec3(x, y, z) + glm::vec3(0.5f,0.5f,0.5f), {1,1,1});
                    if (playerBox.intersects(blockBox)) {
                        // Handle response here
                        glm::vec3 mtvVec = AABB::getMTV(playerBox, blockBox);
                        player.setPosition(player.getPosition() + mtvVec);
                        //std::cout << "RAHHHH" << std::endl;
                    }
                }
            }
        }
    }
}
