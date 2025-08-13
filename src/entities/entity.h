//
// Created by Lamad on 11/12/2024.
//

#ifndef ENTITY_H
#define ENTITY_H

#include <memory>

#include "../engine/physics/collisions/AABB.h"
#include "../engine/physics/PhysicsSystem.h"

class Entity {
public:
    Entity(glm::vec3 positionIn, glm::vec3 sizeIn);
    ~Entity() = default;
    void setPosition(glm::vec3 positionIn);
    glm::vec3 getPosition();
    std::shared_ptr<PhysicsComponent> physics;
    glm::vec3 position;

protected:
    glm::vec3 velocity;
    glm::vec3 size;
    AABB boundingBox;
};



#endif //ENTITY_H
