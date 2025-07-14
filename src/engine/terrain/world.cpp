//
// Created by Lamad on 14/07/2025.
//

#include "world.h"


void World::generateChunk(ChunkCoord coord) {
    Chunk* newChunk = new Chunk();
    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
            for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                if (y < 8) {
                    newChunk->at(x, y, z).type = 1; // solid block
                } else {
                    newChunk->at(x, y, z).type = 0; // air
                }
            }
        }
    }
    chunkMap[coord] = newChunk;
}

Chunk * World::getChunk(ChunkCoord coord) {
    return chunkMap[coord];
}

void World::removeChunk(ChunkCoord coord) {
    chunkMap.erase(coord);
}
