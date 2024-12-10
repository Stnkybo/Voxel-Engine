//
// Created by Lamad on 10/12/2024.
//

#ifndef MESH_H
#define MESH_H
#include <vector>

#include "Triangle.h"


class Mesh {

public:


    std::vector<Triangle> m_triangles;

    explicit Mesh() {

    }

    void addTriangle(const Triangle &triangle) {
        m_triangles.push_back(triangle);
    }

};



#endif //MESH_H
