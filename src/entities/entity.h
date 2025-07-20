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
    void setPosition(glm::vec3 positionIn);
    glm::vec3 getPosition();

protected:
    glm::vec3 velocity;
    glm::vec3 position;
    glm::vec3 size;
    AABB boundingBox;
};



#endif //ENTITY_H
