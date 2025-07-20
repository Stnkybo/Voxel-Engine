//
// Created by Lamad on 11/12/2024.
//

#ifndef PLAYER_H
#define PLAYER_H
#include "entity.h"
#include "../engine/camera/camera.h"


class Player : public Entity {
  public:
  Player(glm::vec3 positionIn);
  ~Player() = default;
  int m_health = 100;
  int m_maxHealth = 100;
  Camera* camera;
};



#endif //PLAYER_H
