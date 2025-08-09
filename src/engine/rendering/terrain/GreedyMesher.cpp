#include "GreedyMesher.h"

void GreedyMesher::GreedyMeshChunk(Chunk& chunk, ChunkMeshing::ChunkMesh& chunkMesh) {
    for (int dir = 0; dir < 6; dir++) {
        ProcessDirection(chunk, chunkMesh, dir);
    }
    std::cout << "Vertices: " << chunk.mesh.vertices.size()
          << ", Indices: " << chunk.mesh.indices.size() << std::endl;

}

void GreedyMesher::ProcessDirection(Chunk& chunk, ChunkMeshing::ChunkMesh& chunkMesh, int dir) {
    struct MaskCell { bool visible; uint8_t blockType; };

    // Figure out which axes are U, V (face plane) and W (depth)
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

    for (int w = 0; w < sizeW; ++w) {
        std::vector<MaskCell> mask(sizeU * sizeV);

        // Build visibility mask
        for (int v = 0; v < sizeV; ++v) {
            for (int u = 0; u < sizeU; ++u) {
                int c[3] = {0, 0, 0};
                c[axisU] = u;
                c[axisV] = v;
                c[axisW] = w;

                const Voxel& voxel = chunk.at(c[0], c[1], c[2]);
                if (voxel.type != static_cast<uint8_t>(BlockType::AIR) &&
                    IsFaceVisible(chunk, c[0], c[1], c[2], dir))
                {
                    mask[u + v * sizeU] = { true, voxel.type };
                } else {
                    mask[u + v * sizeU] = { false, 0 };
                }
            }
        }

        // Greedy rectangle merging
        for (int v = 0; v < sizeV; ++v) {
            for (int u = 0; u < sizeU;) {
                MaskCell &cell = mask[u + v * sizeU];
                if (!cell.visible) { ++u; continue; }

                uint8_t currentType = cell.blockType;

                // Width expansion
                int width = 1;
                while (u + width < sizeU &&
                       mask[(u + width) + v * sizeU].visible &&
                       mask[(u + width) + v * sizeU].blockType == currentType)
                {
                    ++width;
                }

                // Height expansion
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

                // Mark cells as processed
                for (int hh = 0; hh < height; ++hh)
                    for (int ww = 0; ww < width; ++ww)
                        mask[(u + ww) + (v + hh) * sizeU].visible = false;

                // Convert slice coords (u,v,w) to chunk coords (x,y,z)
                int startCoords[3] = {0,0,0};
                startCoords[axisU] = u;
                startCoords[axisV] = v;

                if (positiveDir) {
                    startCoords[axisW] = w + 1; // far side
                } else {
                    startCoords[axisW] = w;     // near side
                }

                glm::vec3 origin(startCoords[0], startCoords[1], startCoords[2]);
                glm::vec3 normal = GetNormal(dir);

                // Add the quad
                AddQuad(chunkMesh, origin, normal, width, height, 0, axisU, axisV);

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

void GreedyMesher::AddQuad(ChunkMeshing::ChunkMesh& mesh,
                           const glm::vec3& origin,
                           const glm::vec3& normal,
                           int du, int dv,
                           uint8_t blockTexIndex,
                           int axisU, int axisV)
{
    // Atlas params
    constexpr int ATLAS_COLS = 16;
    constexpr int ATLAS_ROWS = 16;
    const float atlasCellW = 1.0f / ATLAS_COLS;
    const float atlasCellH = 1.0f / ATLAS_ROWS;

    int texX = blockTexIndex % ATLAS_COLS;
    int texY = blockTexIndex / ATLAS_COLS;
    float baseU = texX * atlasCellW;
    float baseV = texY * atlasCellH;

    float u0 = baseU;
    float v0 = baseV;
    float u1 = baseU + static_cast<float>(du) * atlasCellW;
    float v1 = baseV + static_cast<float>(dv) * atlasCellH;

    // build axis aligned du/dv unit vectors
    glm::vec3 duVec(0.0f), dvVec(0.0f);
    duVec[axisU] = 1.0f;
    dvVec[axisV] = 1.0f;

    // Compute p0 such that p0 is the "lower-left" corner in (u,v) local space.
    // If duVec or dvVec is negative we must shift p0 by width/height so the rectangle
    // extents go into the positive du/dv local directions from p0.
    glm::vec3 shift(0.0f);
    if (duVec[axisU] < 0.0f) shift += duVec * static_cast<float>(du);
    if (dvVec[axisV] < 0.0f) shift += dvVec * static_cast<float>(dv);

    glm::vec3 p0 = origin + shift;
    glm::vec3 p1 = p0 + duVec * static_cast<float>(du);
    glm::vec3 p2 = p1 + dvVec * static_cast<float>(dv);
    glm::vec3 p3 = p0 + dvVec * static_cast<float>(dv);

    // Ensure winding gives the outward normal (if not, swap p1 <-> p3 and their UVs)
    // We compute face normal from (p1 - p0) x (p3 - p0)
    glm::vec3 faceDir = glm::cross(p1 - p0, p3 - p0);
    if (glm::dot(faceDir, normal) < 0.0f) {
        std::swap(p1, p3);
        // also swap uv mapping for those indices below
        std::swap(u1, u0); // we will reassign UVs accordingly below by indices swap
    }

    // assign UVs for the four corners in the same order as p0,p1,p2,p3
    glm::vec2 uv0 = { u0, v0 };
    glm::vec2 uv1 = { u1, v0 };
    glm::vec2 uv2 = { u1, v1 };
    glm::vec2 uv3 = { u0, v1 };

    // If we swapped p1/p3 above we already swapped u0/u1 — but to be safe:
    // (the swap of u0/u1 above is a simple way to keep uv->vertex mapping consistent)

    // push vertices
    mesh.vertices.push_back({ p0, normal, uv0 });
    mesh.vertices.push_back({ p1, normal, uv1 });
    mesh.vertices.push_back({ p2, normal, uv2 });
    mesh.vertices.push_back({ p3, normal, uv3 });

    uint32_t startIndex = static_cast<uint32_t>(mesh.vertices.size()) - 4;
    mesh.indices.insert(mesh.indices.end(), {
        startIndex, startIndex + 1, startIndex + 2,
        startIndex, startIndex + 2, startIndex + 3
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