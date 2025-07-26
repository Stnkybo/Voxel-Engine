//
// Created by Lamad on 26/07/2025.
//

#ifndef CHUNKMESH_H
#define CHUNKMESH_H
#include <vector>

#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace ChunkMeshing {
    struct Vertex {
        glm::vec3 Position;  // 3D position
        glm::vec3 Normal;    // Face direction (for lighting)
        glm::vec2 TexCoord;  // Texture coordinates
    };
    struct ChunkMesh {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        GLuint VAO, VBO, EBO;
        bool gpuUploaded = false;

        void Clear() {
            vertices.clear();
            indices.clear();
            if (VAO != 0) {
                glDeleteVertexArrays(1, &VAO);
                VAO = 0;
            }
            if (VBO != 0) {
                glDeleteBuffers(1, &VBO);
                VBO = 0;
            }
            if (EBO != 0) {
                glDeleteBuffers(1, &EBO);
                EBO = 0;
            }
            gpuUploaded = false;
        }

        void UploadToGPU() {
            if (gpuUploaded) return;
            if (vertices.empty() || indices.empty()) return;  // Nothing to upload

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

            // Setup vertex attributes
            // Position (vec3) - location 0
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));

            // Normal (vec3) - location 1
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

            // TexCoord (vec2) - location 2
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));

            glBindVertexArray(0);

            gpuUploaded = true;
        }


        void Draw() const {
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        }

        void DeleteGPUResources() {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
        }
    };

}



#endif //CHUNKMESH_H
