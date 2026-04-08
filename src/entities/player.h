//
// Created by Lamad on 11/12/2024.
//

#ifndef PLAYER_H
#define PLAYER_H
#include "entity.h"
#include "../engine/camera/camera.h"
#include "../engine/terrain/voxel.h"


class Player : public Entity {
  public:
  Player(glm::vec3 positionIn);
  ~Player() = default;
  void setPosition(glm::vec3 positionIn);

  void ProcessMovement(Camera_Movement direction, float deltaTime);
  void setMovementSpeed(float speed);
  float getMovementSpeed() const;
  glm::vec3 getFeet() const;
  glm::vec3 getHead() const;

  AABB &getBoundingBox();

  void toggleNoclip();

  Camera* camera;
  BlockType m_selected_block_type = BlockType::AIR;

private:
  int m_health = 100;
  int m_maxHealth = 100;
  float movementSpeed = 5.0f;

};



#endif //PLAYER_H
