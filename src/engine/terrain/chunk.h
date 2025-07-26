//
// Created by Lamad on 14/07/2025.
//

#ifndef CHUNK_H
#define CHUNK_H
#include <array>

#include "../rendering/cube.h"
#include "voxel.h"
#include "../rendering/terrain/ChunkMesh.h"

inline int floor_div(int a, int b) {
  int result = a / b;
  // Adjust for negative results where a % b != 0 and signs differ
  if ((a % b != 0) && ((a < 0) != (b < 0))) {
    result--;
  }
  return result;
}

// define the dimensions of a chunk
constexpr int CHUNK_SIZE_X = 16;
constexpr int CHUNK_SIZE_Y = 256;
constexpr int CHUNK_SIZE_Z = 16;

// for identifying chunks
struct ChunkCoord {
  int x, z;
  bool operator==(const ChunkCoord& other) const {
    return x == other.x && z == other.z;
  }
};

//for converting voxel coordinates to Chunk Coords
inline ChunkCoord VoxelToChunkCoords(int x, int z) {
  ChunkCoord chunk_coord{floor_div(x, CHUNK_SIZE_X), floor_div(z, CHUNK_SIZE_Z)};
  return chunk_coord;
}

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
  std::array<Voxel, CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z> voxels;
  ChunkMeshing::ChunkMesh mesh;
  Chunk() {
    for (int i = 0; i < CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z; ++i) {
      makeVoxel(BlockType::AIR);
    }

    }
  ~Chunk();

  inline Voxel& at(int x, int y, int z) {
    return voxels[x + CHUNK_SIZE_X * (y + CHUNK_SIZE_Y * z)];
  }

  inline const Voxel& at(int x, int y, int z) const {
    return voxels[x + CHUNK_SIZE_X * (y + CHUNK_SIZE_Y * z)];
  }



};



#endif //CHUNK_H
