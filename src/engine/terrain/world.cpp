//
// Created by Lamad on 14/07/2025.
//

#include "world.h"


void World::generateChunk(ChunkCoord coord) {
    Chunk* newChunk = new Chunk();
    chunkMap[coord] = newChunk;
}

Chunk * World::getChunk(ChunkCoord coord) {
    return chunkMap[coord];
}

void World::removeChunk(ChunkCoord coord) {
    chunkMap.erase(coord);
}
