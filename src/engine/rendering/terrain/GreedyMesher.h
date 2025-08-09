//
// Created by Lamad on 26/07/2025.
//

#ifndef GREEDYMESHER_H
#define GREEDYMESHER_H

#include "BlockTextureAtlas.h"
#include "ChunkMesh.h"
#include "../../terrain/chunk.h"


class GreedyMesher {
    public:
    GreedyMesher() = default;
    ~GreedyMesher() = default;
        void GreedyMeshChunk(Chunk& chunk, ChunkMeshing::ChunkMesh& chunkMesh);
        BlockTextureAtlas blockTextureAtlas;
    private:
        void ProcessDirection(Chunk& chunk, ChunkMeshing::ChunkMesh& chunkMesh, int dir);

    static int GetDepthForDirection(int z, int dir);

        static bool IsFaceVisible(Chunk& chunk, int x, int y, int z, int dir);

    static void AddQuad(ChunkMeshing::ChunkMesh& mesh,
                    const glm::vec3& origin,
                    const glm::vec3& normal,
                    int du, int dv,
                    uint8_t blockTexIndex,
                    int axisU, int axisV);

        static glm::vec3 GetNormal(int dir);

    static void CalculateQuadCorners(int x, int y, int width, int height, int dir, glm::vec3 corners[4]);

        glm::vec2 CalculateUVs(uint8_t blockType, int faceDir, int cornerIdx, int quadWidth, int quadHeight);

        static glm::ivec2 GetTexturePosition(uint8_t blockType, int faceDir);

};



#endif //GREEDYMESHER_H
