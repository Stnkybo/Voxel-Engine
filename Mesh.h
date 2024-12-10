//
// Created by Lamad on 10/12/2024.
//

#ifndef MESH_H
#define MESH_H
#include <vector>

#include "Triangle.h"


class Mesh {

public:

    // Faze out in favor of the stuff below
    std::vector<Triangle> m_triangles;

    std::vector<SDL_Vertex> m_vertices;
    std::vector<SDL_Vertex> m_points;
    std::vector<SDL_Texture*> m_texture;

    explicit Mesh() = default;

    void addTriangle(const Triangle &triangle) {
        m_triangles.push_back(triangle);
    }

};



#endif //MESH_H
