//
// Created by Lamad on 10/12/2024.
//

#ifndef CUBE_H
#define CUBE_H
#include "mesh.hpp"


class Cube {
    int posX, posY, posZ;

    // Declare a vector for vertices
     std::vector<Vertex> m_vertices;
     vector<unsigned> m_indices;
     std::vector<Texture> m_textures;
     glm::vec3 p_vec3;
    // Define cube face vertices with correct positions and TexCoords
    inline static glm::vec3 positions[] = {
        // Front face
        {0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1},
        // Back face
        {1, 0, 0}, {0, 0, 0}, {0, 1, 0}, {1, 1, 0},
        // Left face
        {0, 0, 0}, {0, 0, 1}, {0, 1, 1}, {0, 1, 0},
        // Right face
        {1, 0, 1}, {1, 0, 0}, {1, 1, 0}, {1, 1, 1},
        // Top face
        {0, 1, 1}, {1, 1, 1}, {1, 1, 0}, {0, 1, 0},
        // Bottom face
        {0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1},
    };
    inline static int faceIndices[][4] = {
        {0, 1, 2, 3},     // front
        {4, 5, 6, 7},     // back
        {8, 9,10,11},     // left
        {12,13,14,15},    // right
        {16,17,18,19},    // top
        {20,21,22,23}     // bottom
    };
    static Texture m_texture;

public:
    Mesh* cubeMesh;

    Cube(int x, int y, int z) {
        posX = x;
        posY = y;
        posZ = z;




        static glm::vec2 uvs[] = {
            {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}
        };



        m_vertices.clear();
        m_indices.clear();

        for (int f = 0; f < 6; f++) {
            for (int i = 0; i < 4; i++) {
                Vertex v{};
                glm::vec3 p_vec3 = positions[faceIndices[f][i]];
                v.Position = glm::vec3(p_vec3.x + posX, p_vec3.y + posY, p_vec3.z + posZ);
                v.TexCoords = uvs[i];
                v.Normal = glm::vec3(0.0f); // optional, set later
                v.Tangent = glm::vec3(0.0f);
                v.Bitangent = glm::vec3(0.0f);
                m_vertices.push_back(v);
            }
            // Add 2 triangles per face
            unsigned int base = f * 4;
            m_indices.insert(m_indices.end(), {
                base + 0, base + 1, base + 2,
                base + 2, base + 3, base + 0
            });
        }


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
        for (int f = 0; f < 6; f++) {
            for (int i = 0; i < 4; i++) {
                glm::vec3 p_vec3 = positions[faceIndices[f][i]];
                m_vertices[counter].Position = glm::vec3(p_vec3.x + posX, p_vec3.y + posY, p_vec3.z + posZ);
                // printf("p_vec3.y + posY: %f\n",p_vec3.y + posY);
                // printf("m_vertices[counter].Position.y: %f\n",m_vertices[counter].Position.y);
                counter++;
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
