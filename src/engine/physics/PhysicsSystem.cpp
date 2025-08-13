//
// Created by Lamad on 13/08/2025.
//

#include "PhysicsSystem.h"

#include <iostream>

#include "../../entities/entity.h"

#include <memory>
#include <vector>
#include <glm/glm.hpp>


void PhysicsSystem::RegisterEntity(std::shared_ptr<Entity> entity) {
    // Only add if it has a PhysicsComponent
    if (entity->physics) {
        entities.push_back(entity);
    }
}

void PhysicsSystem::UnregisterEntity(std::shared_ptr<Entity> entity) {
    entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
}

void PhysicsSystem::Update(float deltaTime) {
    for (auto& entity : entities) {
        auto& phys = entity->physics;
        if (!phys) continue;

        if (phys->affectedByGravity)
            phys->velocity += gravity * deltaTime;

        entity->setPosition(entity->getPosition()+phys->velocity * deltaTime);

        HandleCollisions(entity);
    }
}

void PhysicsSystem::HandleCollisions(std::shared_ptr<Entity> entity) {
}
