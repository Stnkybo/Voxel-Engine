//
// Created by Lamad on 11/12/2024.
//

#include "player.h"

Player::Player(glm::vec3 positionIn): Entity(positionIn, {1,2,1}) {
    camera = new Camera(position, glm::vec3(0.0f, 1.0f, 0.0f));
}
