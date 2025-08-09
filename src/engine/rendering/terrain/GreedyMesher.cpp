#include "GreedyMesher.h"

void GreedyMesher::GreedyMeshChunk(Chunk& chunk, ChunkMeshing::ChunkMesh& chunkMesh) {
    for (int dir = 0; dir < 6; dir++) {
        ProcessDirection(chunk, chunkMesh, dir);
    }
    std::cout << "Vertices: " << chunk.mesh.vertices.size()
          << ", Indices: " << chunk.mesh.indices.size() << std::endl;

}

// Replace your ProcessDirection with this:
void GreedyMesher::ProcessDirection(Chunk& chunk, ChunkMeshing::ChunkMesh& chunkMesh, int dir) {
    struct MaskCell { bool visible; uint8_t blockType; };

    // Determine which axis is U, V (the 2D plane) and W (depth)
    int axisU, axisV, axisW;
    bool positiveDir = (dir % 2 == 0); // even = +X,+Y,+Z

    switch (dir / 2) {
        case 0: axisU = 2; axisV = 1; axisW = 0; break; // ±X: U=Z, V=Y, W=X
        case 1: axisU = 0; axisV = 2; axisW = 1; break; // ±Y: U=X, V=Z, W=Y
        default: axisU = 0; axisV = 1; axisW = 2; break; // ±Z: U=X, V=Y, W=Z
    }

    int sizeU = (axisU == 0 ? CHUNK_SIZE_X : (axisU == 1 ? CHUNK_SIZE_Y : CHUNK_SIZE_Z));
    int sizeV = (axisV == 0 ? CHUNK_SIZE_X : (axisV == 1 ? CHUNK_SIZE_Y : CHUNK_SIZE_Z));
    int sizeW = (axisW == 0 ? CHUNK_SIZE_X : (axisW == 1 ? CHUNK_SIZE_Y : CHUNK_SIZE_Z));

    // For each slice along the depth axis
    for (int w = 0; w < sizeW; ++w) {
        // mask sized to current slice
        std::vector<MaskCell> mask(sizeU * sizeV);
        // Build mask for this slice
        // Build mask for this slice
        for (int v = 0; v < sizeV; ++v) {
            for (int u = 0; u < sizeU; ++u) {
                int c[3] = {0,0,0};
                c[axisU] = u;
                c[axisV] = v;
                c[axisW] = w;

                const Voxel& voxel = chunk.at(c[0], c[1], c[2]);
                // IMPORTANT: only consider non-AIR voxels
                if (voxel.type != static_cast<uint8_t>(BlockType::AIR) &&
                    IsFaceVisible(chunk, c[0], c[1], c[2], dir))
                {
                    mask[u + v * sizeU] = { true, voxel.type };
                } else {
                    mask[u + v * sizeU] = { false, 0 };
                }
            }
        }


        // Greedy merge rectangles on this mask
        for (int v = 0; v < sizeV; ++v) {
            for (int u = 0; u < sizeU; ) {
                MaskCell &cell = mask[u + v * sizeU];
                if (!cell.visible) { ++u; continue; }

                uint8_t currentType = cell.blockType;
                int width = 1;
                while (u + width < sizeU && mask[(u + width) + v * sizeU].visible &&
                       mask[(u + width) + v * sizeU].blockType == currentType) {
                    ++width;
                }

                int height = 1;
                bool canExpandHeight = true;
                while (canExpandHeight && v + height < sizeV) {
                    for (int k = 0; k < width; ++k) {
                        MaskCell &c2 = mask[(u + k) + (v + height) * sizeU];
                        if (!c2.visible || c2.blockType != currentType) {
                            canExpandHeight = false;
                            break;
                        }
                    }
                    if (canExpandHeight) ++height;
                }

                // Mark processed
                for (int hh = 0; hh < height; ++hh)
                    for (int ww = 0; ww < width; ++ww)
                        mask[(u + ww) + (v + hh) * sizeU].visible = false;

                // Convert (u,v,w) slice coords to world chunk coords (x,y,z)
                int startCoords[3] = {0,0,0};
                startCoords[axisU] = u;
                startCoords[axisV] = v;
                startCoords[axisW] = w;

                // IMPORTANT: pass the full 3D start coordinates and the width/height
                AddQuad(chunkMesh, startCoords[0], startCoords[1], startCoords[2],
                        width, height, dir, currentType);

                u += width;
            }
        }
    }
}



int GreedyMesher::GetDepthForDirection(int z, int dir) {
    // Returns z coordinate for the face in given direction
    return (dir == 4) ? z + 1 : ((dir == 5) ? z - 1 : z);
}

bool GreedyMesher::IsFaceVisible(Chunk& chunk, int x, int y, int z, int dir) {
    int nx = x + (dir == 0 ? 1 : (dir == 1 ? -1 : 0));
    int ny = y + (dir == 2 ? 1 : (dir == 3 ? -1 : 0));
    int nz = z + (dir == 4 ? 1 : (dir == 5 ? -1 : 0));

    if (nx < 0 || ny < 0 || nz < 0 || nx >= CHUNK_SIZE_X || ny >= CHUNK_SIZE_Y || nz >= CHUNK_SIZE_Z)
        return true;

    Voxel neighbor = chunk.at(nx, ny, nz);
    return (neighbor.type == static_cast<uint8_t>(BlockType::AIR));
}

// Replace your AddQuad with this signature (takes full start x,y,z)
void GreedyMesher::AddQuad(ChunkMeshing::ChunkMesh& chunkMesh,
                           int startX, int startY, int startZ,
                           int width, int height,
                           int dir, uint8_t blockType)
{
    if (blockType == static_cast<uint8_t>(BlockType::AIR)) return;

    // Determine axis mapping again (same logic as ProcessDirection)
    int axisU, axisV, axisW;
    bool positiveDir = (dir % 2 == 0);
    switch (dir / 2) {
        case 0: axisU = 2; axisV = 1; axisW = 0; break; // ±X
        case 1: axisU = 0; axisV = 2; axisW = 1; break; // ±Y
        default: axisU = 0; axisV = 1; axisW = 2; break; // ±Z
    }

    // Base voxel corner (the "lower-left" in UV/u/v sense) in world voxel coords
    glm::vec3 base(
        static_cast<float>(startX),
        static_cast<float>(startY),
        static_cast<float>(startZ)
    );

    // For positive faces (+X/+Y/+Z) the face sits at the high side of the voxel (w + 1.0)
    // For negative faces (-X/-Y/-Z) the face sits at the low side (w + 0.0)
    if (positiveDir) {
        base[axisW] += 1.0f;
    } // else base[axisW] remains startW

    // Unit directions along the mask's U and V axes (in world coords)
    glm::vec3 uDir(0.0f), vDir(0.0f);
    uDir[axisU] = 1.0f;
    vDir[axisV] = 1.0f;

    glm::vec3 uStep = uDir * static_cast<float>(width);
    glm::vec3 vStep = vDir * static_cast<float>(height);

    // corners in consistent order (base, +U, +U+V, +V)
    glm::vec3 corners[4];
    corners[0] = base;
    corners[1] = base + uStep;
    corners[2] = base + uStep + vStep;
    corners[3] = base + vStep;

    // Flip corners (winding) for negative directions so the normal points outward:
    if (!positiveDir) std::swap(corners[1], corners[3]);

    // Push vertices and indices
    auto baseIndex = static_cast<uint32_t>(chunkMesh.vertices.size());
    for (int i = 0; i < 4; ++i) {
        ChunkMeshing::Vertex v;
        v.Position = corners[i];
        v.Normal = GetNormal(dir);
        v.TexCoord = CalculateUVs(blockType, dir, i, width, height); // keep your UV helper for now
        chunkMesh.vertices.push_back(v);
    }

    // Two triangles (winding is already corrected above)
    chunkMesh.indices.insert(chunkMesh.indices.end(), {
        baseIndex, baseIndex + 1, baseIndex + 2,
        baseIndex, baseIndex + 2, baseIndex + 3
    });
}



glm::vec3 GreedyMesher::GetNormal(int dir) {
    // Direction to normal lookup table
    static const glm::vec3 normals[6] = {
        { 1, 0, 0},  // +X
        {-1, 0, 0},  // -X
        { 0, 1, 0},  // +Y
        { 0,-1, 0},  // -Y
        { 0, 0, 1},  // +Z
        { 0, 0,-1}   // -Z
    };
    return normals[dir];
}

void GreedyMesher::CalculateQuadCorners(int x, int y, int width, int height, int dir, glm::vec3 corners[4]) {
    // Determine depth axis and sign
    const int depthAxis = dir / 2;  // 0=X, 1=Y, 2=Z
    const float depth = (dir % 2) ? 0.0f : 1.0f;  // -X/-Y/-Z=0.0, +X/+Y/+Z=1.0

    // Base position (aligned to voxel grid)
    glm::vec3 base(x, y, 0);
    base[depthAxis] += depth;

    // Local axes for quad expansion
    const glm::vec3 right = (depthAxis == 0) ? glm::vec3(0,0,1) : glm::vec3(1,0,0);
    const glm::vec3 up = (depthAxis == 1) ? glm::vec3(0,0,1) : glm::vec3(0,1,0);

    // Calculate corners
    corners[0] = base;
    corners[1] = base + right * static_cast<float>(width);
    corners[2] = base + right * static_cast<float>(width) + up * static_cast<float>(height);
    corners[3] = base + up * static_cast<float>(height);

    // Fix winding order for negative faces
    if (dir % 2) {  // Negative directions (-X, -Y, -Z)
        std::swap(corners[1], corners[3]);
    }
}

glm::vec2 GreedyMesher::CalculateUVs(uint8_t type, int face,
                                    int cornerIdx, int quadWidth, int quadHeight) {
    // Get base UV offset from atlas
    glm::vec2 baseUV = blockTextureAtlas.GetUVOffset(type, face);

    // Scale UVs by quad dimensions
    glm::vec2 uvCorners[4] = {
        {0.0f,         0.0f},
        {quadWidth,    0.0f},
        {quadWidth,    quadHeight},
        {0.0f,         quadHeight}
    };

    // Convert to normalized coordinates
    float u = baseUV.x + (uvCorners[cornerIdx].x / blockTextureAtlas.GetAtlasWidth());
    float v = baseUV.y + (uvCorners[cornerIdx].y / blockTextureAtlas.GetAtlasHeight());
    
    return {u, v};
}
glm::ivec2 GreedyMesher::GetTexturePosition(uint8_t blockType, int faceDir) {

    return {0, 0}; // TEMP
    // Texture tile size in pixels (e.g., 16x16)
    const int TILE_SIZE = 16;

    // Atlas layout coordinates (example values)
    switch (static_cast<BlockType>(blockType)) {
        case BlockType::GRASS:
            return {0, 0};  // First tile in atlas

        case BlockType::DIRT:
            if (faceDir == 2) return {0, 2};   // Grass top
            if (faceDir == 3) return {2, 0};   // Dirt bottom
            return {1, 0};                     // Grass sides

        case BlockType::STONE:
            return {2, 0};  // Shared with grass bottom

        case BlockType::BRICK:
            if (faceDir == 2 || faceDir == 3) return {3, 1};  // Wood end grain
            return {3, 0};                                   // Wood sides

        default:
            return {0, 0};  // Default/error texture
    }
}