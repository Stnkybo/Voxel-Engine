//
// Created by Lamad on 26/07/2025.
//

#ifndef CHUNKMESH_H
#define CHUNKMESH_H
#include <vector>

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
    };

}



#endif //CHUNKMESH_H
