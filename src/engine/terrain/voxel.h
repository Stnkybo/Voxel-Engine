//
// Created by Lamad on 14/07/2025.
//

#ifndef VOXEL_H
#define VOXEL_H

enum class BlockType : uint8_t {
  AIR = 0,
  GRASS = 1,
  DIRT = 2,
  STONE = 3,
  BRICK = 4
};

enum class FaceDirection {
  XP, // +X face (right)
  XN, // -X face (left)
  YP, // +Y face (top)
  YN, // -Y face (bottom)
  ZP, // +Z face (front)
  ZN  // -Z face (back)
};

struct Voxel {
  uint8_t type;  // block type ID
  // maybe other data: light, metadata, etc.
};

inline BlockType getBlockType(const Voxel& v) {
  return static_cast<BlockType>(v.type);
}

inline void setBlockType(Voxel& v, BlockType type) {
  v.type = static_cast<uint8_t>(type);
}

inline Voxel makeVoxel(BlockType type) {
  return Voxel{static_cast<uint8_t>(type)};
}


#endif //VOXEL_H
