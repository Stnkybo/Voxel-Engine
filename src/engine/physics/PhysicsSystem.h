//
// Created by Lamad on 13/08/2025.
//

#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include <memory>
#include <vector>
#include <glm/glm.hpp>

class Entity; // forward declaration

class PhysicsComponent {
public:
    glm::vec3 velocity = {0.0f, 0.0f, 0.0f};
    float mass = 1.0f;
    bool affectedByGravity = true;
    bool collisionsEnabled = true;

    PhysicsComponent(float mass_) : mass(mass_) {}
};

class PhysicsSystem {
public:
    std::vector<std::shared_ptr<Entity>> entities;

    void RegisterEntity(std::shared_ptr<Entity> entity);

    void UnregisterEntity(std::shared_ptr<Entity> entity);

    void Update(float deltaTime);

    glm::vec3 gravity = {0.0f, -9.81f, 0.0f};

private:
    void HandleCollisions(std::shared_ptr<Entity> entity);
};




#endif //PHYSICSSYSTEM_H
