//
// Created by Lamad on 10/12/2024.
//

#ifndef CUBE_H
#define CUBE_H
#include "Mesh.hpp"


class Cube {
public:
    Mesh* cubeMesh;

    Cube(int x, int y, int z) {
        // Declare a vector for vertices
        std::vector<Vertex> vertices;

        // Generate vertices for the cube
        int counter = 0;
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    counter++;
                    Vertex vertex{};
                    vertex.Position = glm::vec3(i + x, j + y, k + z);
                    vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
                    vertex.TexCoords = glm::vec2(0.0f, 0.0f);
                    vertex.Tangent = glm::vec3(0.0f, 0.0f, 0.0f);
                    vertex.Bitangent = glm::vec3(0.0f, 0.0f, 0.0f);

                    vertices.push_back(vertex);
                }
            }
        }
                    //cout << "Counter: " << counter << endl;

        // Declare a vector for indices (fix with proper cube indices)
        std::vector<unsigned int> indices = {
            // front
            0, 1, 2,
            2, 3, 0,
            // right
            1, 5, 6,
            6, 2, 1,
            // back
            7, 6, 5,
            5, 4, 7,
            // left
            4, 0, 3,
            3, 7, 4,
            // bottom
            4, 5, 1,
            1, 0, 4,
            // top
            3, 2, 6,
            6, 7, 3
        };



        // Declare a vector for textures
        std::vector<Texture> textures;

        // Create a new mesh (ensure Mesh has a matching constructor)
        this->cubeMesh = new Mesh(vertices, indices, textures);
    }

    ~Cube() = default; // Smart pointer handles cleanup
};



#endif //CUBE_H
