//
// Created by Lamad on 18/07/2025.
//

#ifndef WORLDCOLLISION_H
#define WORLDCOLLISION_H
#include <cmath>

#include "AABB.h"
#include "../terrain/world.h"


class worldCollision {
    World* world = &World::getInstance();
    bool isSolid(int x, int y, int z);

    void resolveCollisions(AABB& player);


};



#endif //WORLDCOLLISION_H
