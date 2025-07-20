//
// Created by Lamad on 14/07/2025.
//

#ifndef WORLD_H
#define WORLD_H
#include <unordered_map>

#include "chunk.h"



class World {
public:
    static World& getInstance() {
        static World instance;
        return instance;
    }
    Voxel* getBlock(int x, int y, int z);
    void generateChunk(ChunkCoord coord);
    Chunk* getChunk(ChunkCoord coord);
    void removeChunk(ChunkCoord coord);

private:
    World() = default;
    std::unordered_map<ChunkCoord, Chunk*> chunkMap;
};



#endif //WORLD_H
