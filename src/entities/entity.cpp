//
// Created by Lamad on 11/12/2024.
//

#include "entity.h"

Entity::Entity(glm::vec3 positionIn, glm::vec3 sizeIn) {
    position = positionIn;
    size = sizeIn;
    boundingBox = AABB::fromCenterSize(position, size);
}
