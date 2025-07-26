#include "GreedyMesher.h"

void GreedyMesher::GreedyMeshChunk(Chunk& chunk, ChunkMeshing::ChunkMesh& chunkMesh) {
    for (int dir = 0; dir < 6; dir++) {
        ProcessDirection(chunk, chunkMesh, dir);
    }
}

void GreedyMesher::ProcessDirection(Chunk& chunk, ChunkMeshing::ChunkMesh& chunkMesh, int dir) {
    struct MaskCell {
        bool visible;
        uint8_t blockType;
    };
    
    MaskCell mask[CHUNK_SIZE_X][CHUNK_SIZE_Y] = {{{false, 0}}};

    // Step 1: Build visibility mask with block types
    for (int x = 0; x < CHUNK_SIZE_X; x++) {
        for (int y = 0; y < CHUNK_SIZE_Y; y++) {
            for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                if (IsFaceVisible(chunk, x, y, z, dir)) {
                    mask[x][y] = {
                        true,
                        chunk.at(x, y, GetDepthForDirection(z, dir)).type
                    };
                    break;
                }
            }
        }
    }

    // Step 2: Greedily merge faces with same block type
    for (int y = 0; y < CHUNK_SIZE_Y; y++) {
        for (int x = 0; x < CHUNK_SIZE_X; ) {
            if (!mask[x][y].visible) {
                x++;
                continue;
            }

            uint8_t currentType = mask[x][y].blockType;
            int width = 1, height = 1;

            // Expand width
            while (x + width < CHUNK_SIZE_X && 
                   mask[x + width][y].visible && 
                   mask[x + width][y].blockType == currentType) {
                width++;
            }

            // Expand height
            bool canExpandHeight = true;
            while (canExpandHeight && y + height < CHUNK_SIZE_Y) {
                for (int w = 0; w < width; w++) {
                    if (!mask[x + w][y + height].visible || 
                        mask[x + w][y + height].blockType != currentType) {
                        canExpandHeight = false;
                        break;
                    }
                }
                if (canExpandHeight) height++;
            }

            // Add quad
            AddQuad(chunkMesh, x, y, width, height, dir, currentType);

            // Mark as processed
            for (int h = 0; h < height; h++) {
                for (int w = 0; w < width; w++) {
                    mask[x + w][y + h].visible = false;
                }
            }

            x += width;
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

void GreedyMesher::AddQuad(ChunkMeshing::ChunkMesh& chunkMesh, int x, int y, int width, int height, 
                          int dir, uint8_t blockType) {
    glm::vec3 corners[4];
    CalculateQuadCorners(x, y, width, height, dir, corners);

    uint32_t baseIndex = chunkMesh.vertices.size();
    for (int i = 0; i < 4; i++) {
        ChunkMeshing::Vertex v;
        v.Position = corners[i];
        v.Normal = GetNormal(dir);
        v.TexCoord = CalculateUVs(blockType, dir, i, width, height);
        chunkMesh.vertices.push_back(v);
    }

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

glm::vec2 GreedyMesher::CalculateUVs(uint8_t blockType, int faceDir, int cornerIdx, 
                                    int quadWidth, int quadHeight) {
    const int ATLAS_TILE_SIZE = 16; // pixels per texture
    const int ATLAS_WIDTH = 256;    // total atlas width in pixels
    
    glm::ivec2 texTile = GetTexturePosition(blockType, faceDir);
    
    // UV corners with quad size scaling
    glm::vec2 uvCorners[4] = {
        {0.0f,         0.0f},
        {quadWidth,    0.0f},
        {quadWidth,    quadHeight},
        {0.0f,         quadHeight}
    };
    
    // Convert to normalized atlas coordinates
    float u = (texTile.x * ATLAS_TILE_SIZE + uvCorners[cornerIdx].x) / ATLAS_WIDTH;
    float v = (texTile.y * ATLAS_TILE_SIZE + uvCorners[cornerIdx].y) / ATLAS_WIDTH;
    
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