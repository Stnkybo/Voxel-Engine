//
// Created by Lamad on 11/12/2024.
//

#include "player.h"

Player::Player(glm::vec3 positionIn): Entity(positionIn, {0.5f,2,0.5f}) {
    camera = new Camera(position + glm::vec3(0.0f, 0.8f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    boundingBox = AABB::fromCenterSize(position, size);
}

void Player::setPosition(glm::vec3 positionIn) {
    position = positionIn;
    boundingBox = AABB::fromCenterSize(position, size);
    camera->Position = position + glm::vec3(0.0f, 0.8f, 0.0f);

}

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Player::ProcessMovement(Camera_Movement direction, Uint64 deltaTime) {
    float velocity = movementSpeed * (static_cast<float>(deltaTime) / 1000.0f);
    switch (direction) {
        case FORWARD:  setPosition(position + camera->Front * velocity); break;
        case BACKWARD: setPosition(position - camera->Front * velocity); break;
        case LEFT:     setPosition(position - camera->Right * velocity); break;
        case RIGHT:    setPosition(position + camera->Right * velocity); break;
    }
}

void Player::setMovementSpeed(float speed) {
    movementSpeed = speed;
}

float Player::getMovementSpeed() const {
    return movementSpeed;
}

glm::vec3 Player::getFeet() const {
    return boundingBox.min + glm::vec3(0.25f,0,0.25f);
}

glm::vec3 Player::getHead() const {
    return boundingBox.max - glm::vec3(0.25f,0,0.25f);
}

AABB &Player::getBoundingBox() {
    return boundingBox;
}

