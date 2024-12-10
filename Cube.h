//
// Created by Lamad on 10/12/2024.
//

#ifndef CUBE_H
#define CUBE_H
#include "Mesh.h"


class Cube {
    Mesh *cubeMesh;
    public:
    Cube() {
        this->cubeMesh = new Mesh();
    }

};



#endif //CUBE_H
