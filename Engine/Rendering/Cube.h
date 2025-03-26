//
// Created by Lamad on 10/12/2024.
//

#ifndef CUBE_H
#define CUBE_H
#include "Mesh.hpp"


class Cube {
    int posX, posY, posZ;

    // Declare a vector for vertices
    std::vector<Vertex> m_vertices;
    vector<unsigned> m_indices;
    std::vector<Texture> m_textures;
    static Texture m_texture;

public:
    Mesh* cubeMesh;

    Cube(int x, int y, int z) {
        posX = x;
        posY = y;
        posZ = z;


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

                    m_vertices.push_back(vertex);
                }
            }
        }

        m_indices = {
            // Front face
            0, 1, 2,
            2, 3, 1,
            // Right face
            1, 5, 3,
            3, 7, 5,
            // Back face
            5, 4, 6,
            7, 6, 5,
            // Left face
            4, 0, 2,
            2, 6, 4,
            // Bottom face
            4, 5, 1,
            1, 0, 4,
            // Top face
            3, 2, 6,
            6, 7, 3
        };


        m_textures.emplace_back(m_texture);
        //cout << m_textures.size()<< " " << m_textures[0].path<< endl;


        // Create a new mesh (ensure Mesh has a matching constructor)
        this->cubeMesh = new Mesh(m_vertices, m_indices, m_textures);
    }

    ~Cube() = default; // Smart pointer handles cleanup

    void setPosition(const int x, const int y, const int z) {
        this->posX = x;
        this->posY = y;
        this->posZ = z;

        //Update mesh code
        int counter = 0;
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    m_vertices[counter].Position = glm::vec3( static_cast<float>(posX + i),static_cast<float>(posY + j), static_cast<float>(posZ + k));

                    counter++;
                }
            }
        }

        // Create a new mesh (ensure Mesh has a matching constructor)
        delete this->cubeMesh;
        this->cubeMesh = new Mesh(m_vertices, m_indices, m_textures);
    }

    void setPositionY(int y) {


        this->posY = y;

        //Update mesh code
        int counter = 0;
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    m_vertices[counter].Position.y = static_cast<float>(posY + j);
                    counter++;
                }
            }
        }

        // Create a new mesh (ensure Mesh has a matching constructor)
        delete this->cubeMesh;
        this->cubeMesh = new Mesh(m_vertices, m_indices, m_textures);
    }

    void moveCube(int x, int y, int z) {
        this->posX += x;
        this->posY += y;
        this->posZ += z;

        for (Vertex vertex: m_vertices) {
            vertex.Position += glm::vec3(x, y, z);
        }

    }
    static void setTexture(const Texture &texture) {
        m_texture = texture;
    }

};



#endif //CUBE_H
