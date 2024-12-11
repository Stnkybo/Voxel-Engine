//
// Created by Lamad on 11/12/2024.
//

#ifndef ENTITY_H
#define ENTITY_H



class Entity {
public:
    Entity(double position_x, double position_y)
        : positionX(position_x),
          positionY(position_y) {
    }
    ~Entity() {}

private:
    double positionX;
    double positionY;
    //Collider m_collider;

};



#endif //ENTITY_H
