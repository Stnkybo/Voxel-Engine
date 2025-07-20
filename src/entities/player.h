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
  void setPosition(glm::vec3 positionIn);

  void ProcessMovement(Camera_Movement direction, Uint64 deltaTime);
  void setMovementSpeed(float speed);
  float getMovementSpeed() const;

  Camera* camera;

private:
  int m_health = 100;
  int m_maxHealth = 100;
  float movementSpeed = 5.0f;
};



#endif //PLAYER_H
