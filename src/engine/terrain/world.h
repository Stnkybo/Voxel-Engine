//
// Created by Lamad on 14/07/2025.
//

#ifndef WORLD_H
#define WORLD_H
#include <unordered_map>

#include "chunk.h"



class World {

  std::unordered_map<ChunkCoord, Chunk*> chunkMap;

  public:
    void generateChunk(ChunkCoord coord);
    Chunk* getChunk(ChunkCoord coord);
    void removeChunk(ChunkCoord coord);

};



#endif //WORLD_H
