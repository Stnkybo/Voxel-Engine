//
// Created by Lamad on 11/12/2024.
//

#ifndef ENTITY_H
#define ENTITY_H



class Entity {
public:
    Entity(float position_x, float position_y, float position_z)
        : positionX(position_x),
          positionY(position_y),
          positionZ(position_z) {
    }
    ~Entity() {}

private:
    float positionX;
    float positionY;
    float positionZ;
    //Collider m_collider;

};



#endif //ENTITY_H
