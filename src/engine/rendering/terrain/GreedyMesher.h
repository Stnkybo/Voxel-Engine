//
// Created by Lamad on 26/07/2025.
//

#ifndef GREEDYMESHER_H
#define GREEDYMESHER_H

#include "BlockTextureAtlas.h"
#include "ChunkMesh.h"
#include "../../terrain/chunk.h"

struct MaskCell { bool visible; uint8_t blockType; };

class GreedyMesher {
    public:
        GreedyMesher() = default;
        ~GreedyMesher() = default;
        static void GreedyMeshChunk(Chunk& chunk, ChunkMeshing::ChunkMesh& chunkMesh);
        static BlockTextureAtlas blockTextureAtlas;
    private:
        static int ATLAS_COLS;
        static int ATLAS_ROWS;

        static void ProcessDirection(Chunk& chunk, ChunkMeshing::ChunkMesh& chunkMesh, int dir);


        static void axisInfo(int dir, int& axisU, int& axisV, int& axisW,
                         int& sizeU, int& sizeV, int& sizeW);

        static void buildMask(Chunk &chunk, std::vector<MaskCell> &mask,
                          int axisU, int axisV, int axisW,
                          int sizeU, int sizeV, int w, int dir);

        static void mergeMaskIntoQuads(std::vector<MaskCell>& mask,
                                   ChunkMeshing::ChunkMesh& chunkMesh,
                                   int axisU, int axisV, int axisW,
                                   int sizeU, int sizeV, int w,
                                   bool isPositiveDir, int dir);

        static int GetDepthForDirection(int z, int dir);

        static bool IsFaceVisible(Chunk& chunk, int x, int y, int z, int dir);

    static void AddQuad(ChunkMeshing::ChunkMesh& mesh,
                           const glm::vec3& origin,
                           const glm::vec3& normal,
                           const glm::vec3& duVec,
                           const glm::vec3& dvVec,
                           int width, int height);

        static glm::vec3 GetNormal(int dir);

        static void CalculateQuadCorners(int x, int y, int width, int height, int dir, glm::vec3 corners[4]);

        static glm::vec2 CalculateUVs(uint8_t blockType, int faceDir, int cornerIdx, int quadWidth, int quadHeight);

        static glm::vec3 GetDUVec(int faceDir);

        static glm::vec3 GetDVVec(int faceDir);

};



#endif //GREEDYMESHER_H
