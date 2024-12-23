//
// Created by Lamad on 10/12/2024.
//

#ifndef CUBE_H
#define CUBE_H
#include "Mesh.hpp"


class Cube {
public:
    Mesh* cubeMesh;

    Cube() {
        // Declare a vector for vertices
        std::vector<Vertex> vertices;

        // Generate vertices for the cube
        int counter = 0;
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    counter++;
                    Vertex vertex;
                    vertex.Position = glm::vec3(i, j, k);
                    vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
                    vertex.TexCoords = glm::vec2(0.0f, 0.0f);
                    vertex.Tangent = glm::vec3(0.0f, 0.0f, 0.0f);
                    vertex.Bitangent = glm::vec3(0.0f, 0.0f, 0.0f);

                    vertices.push_back(vertex);
                }
            }
        }
                    cout << "Counter: " << counter << endl;

        // Declare a vector for indices (fix with proper cube indices)
        std::vector<unsigned int> indices = {
            // Front face
            0, 1, 2, 2, 3, 0,

            // Back face
            4, 5, 6, 6, 7, 4,

            // Left face
            0, 3, 7, 7, 4, 0,

            // Right face
            1, 5, 6, 6, 2, 1,

            // Top face
            3, 2, 6, 6, 7, 3,

            // Bottom face
            0, 1, 5, 5, 4, 0
        };



        // Declare a vector for textures
        std::vector<Texture> textures;

        // Create a new mesh (ensure Mesh has a matching constructor)
        this->cubeMesh = new Mesh(vertices, indices, textures);
    }

    ~Cube() {
        delete cubeMesh; // Clean up dynamically allocated memory
    }
};



#endif //CUBE_H
