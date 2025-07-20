//
// Created by Lamad on 14/07/2025.
//

#include "world.h"
#include "chunk.h"


Voxel* World::getBlock(int x, int y, int z) {
    //calc chunk map coords from vec3
    ChunkCoord chunk_coord = VoxelToChunkCoords(x, z);
    //get chunk if it exists
    Chunk* chunk = getChunk(chunk_coord);
    if (chunk != nullptr) {
    return &chunk->at(x,y,z);
    }
    // if no chunk data exist
    return nullptr;
}

void World::generateChunk(ChunkCoord coord) {
    auto* newChunk = new Chunk();
    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
            for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                if (y < 8) {
                    setBlockType(newChunk->at(x, y, z), BlockType::BRICK); // solid block

                } else {
                    setBlockType(newChunk->at(x, y, z), BlockType::AIR);
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
