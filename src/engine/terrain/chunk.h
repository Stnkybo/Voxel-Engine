//
// Created by Lamad on 14/07/2025.
//

#ifndef CHUNK_H
#define CHUNK_H
#include "../rendering/cube.h"
#include "voxel.h"

// define the dimensions of a chunk
constexpr int CHUNK_SIZE_X = 16;
constexpr int CHUNK_SIZE_Y = 256;
constexpr int CHUNK_SIZE_Z = 16;

// for indentifying chunks
struct ChunkCoord {
  int x, z;
  bool operator==(const ChunkCoord& other) const {
    return x == other.x && z == other.z;
  }
};

// define hash for storing chunks
namespace std {
  template<> struct hash<ChunkCoord> {
    std::size_t operator()(const ChunkCoord& c) const {
      return std::hash<int>()(c.x) ^ (std::hash<int>()(c.z) << 1);
    }
  };
}


class Chunk {
  public:
  std::vector<Voxel> voxels;
  Chunk() {
      voxels.resize(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);
    for (Voxel voxel:voxels) {
      voxel.type = 1;
    }

    }
  ~Chunk();

  inline Voxel& at(int x, int y, int z) {
    return voxels[x + CHUNK_SIZE_X * (z + CHUNK_SIZE_Z * y)];
  }

  inline const Voxel& at(int x, int y, int z) const {
    return voxels[x + CHUNK_SIZE_X * (z + CHUNK_SIZE_Z * y)];
  }



};



#endif //CHUNK_H
