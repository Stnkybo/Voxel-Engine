//
// Created by Lamad on 11/12/2024.
//

#include "entity.h"

Entity::Entity(glm::vec3 positionIn, glm::vec3 sizeIn) {
    position = positionIn;
    size = sizeIn;
    boundingBox = AABB::fromCenterSize(position, size);
}

void Entity::setPosition(const glm::vec3 positionIn) {
    position = positionIn;
    boundingBox = AABB::fromCenterSize(position, size);
}

glm::vec3 Entity::getPosition() {
    return position;
}
