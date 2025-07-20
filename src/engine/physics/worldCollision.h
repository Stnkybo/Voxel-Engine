//
// Created by Lamad on 18/07/2025.
//

#ifndef WORLDCOLLISION_H
#define WORLDCOLLISION_H
#include <cmath>

#include "AABB.h"
#include "../terrain/world.h"


class worldCollision {
    public:
    static bool isSolid(int x, int y, int z, World* world);

    static void resolveCollisions(AABB player, World *world);


};



#endif //WORLDCOLLISION_H
