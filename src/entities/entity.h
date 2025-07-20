//
// Created by Lamad on 11/12/2024.
//

#ifndef ENTITY_H
#define ENTITY_H

#include "../engine/physics/AABB.h"

class Entity {
public:
    Entity(glm::vec3 positionIn, glm::vec3 sizeIn);
    ~Entity() = default;
    glm::vec3 velocity;
    glm::vec3 position;
    glm::vec3 size;
    AABB boundingBox;

private:

    //Collider m_collider;

};



#endif //ENTITY_H
