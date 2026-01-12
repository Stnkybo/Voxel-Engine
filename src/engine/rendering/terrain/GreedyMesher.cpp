#include "GreedyMesher.h"

BlockTextureAtlas GreedyMesher::blockTextureAtlas;
int GreedyMesher::ATLAS_COLS = GreedyMesher::blockTextureAtlas.GetAtlasWidth() / blockTextureAtlas.GetTileSize();
int GreedyMesher::ATLAS_ROWS = GreedyMesher::blockTextureAtlas.GetAtlasHeight() / blockTextureAtlas.GetTileSize();


void GreedyMesher::GreedyMeshChunk(Chunk& chunk, ChunkMeshing::ChunkMesh& chunkMesh) {
	for (int dir = 0; dir < 6; dir++) {
		ProcessDirection(chunk, chunkMesh, dir);
	}
	// std::cout << "Vertices: " << chunk.mesh.vertices.size()
	//       << ", Indices: " << chunk.mesh.indices.size() << std::endl;

}

void GreedyMesher::ProcessDirection(Chunk& chunk, ChunkMeshing::ChunkMesh& chunkMesh, int dir) {
	int axisU, axisV, axisW;
	int sizeU, sizeV, sizeW;
	bool isPositiveDir = (dir % 2 == 0);

	axisInfo(dir, axisU, axisV, axisW, sizeU, sizeV, sizeW);

	for (int w = 0; w < sizeW; ++w) {
		std::vector<MaskCell> mask(sizeU * sizeV);
		buildMask(chunk, mask, axisU, axisV, axisW, sizeU, sizeV, w, dir);
		mergeMaskIntoQuads(mask, chunkMesh, axisU, axisV, axisW,
			sizeU, sizeV, w, isPositiveDir, dir);
	}
}

void GreedyMesher::axisInfo(int dir, int& axisU, int& axisV, int& axisW,
	int& sizeU, int& sizeV, int& sizeW) {
	// even = positive direction (+X,+Y,+Z), odd = negative
	switch (dir / 2) {
	case 0: // ±X
		axisU = 2; axisV = 1; axisW = 0;
		sizeU = CHUNK_SIZE_Z;
		sizeV = CHUNK_SIZE_Y;
		sizeW = CHUNK_SIZE_X;
		break;
	case 1: // ±Y
		axisU = 0; axisV = 2; axisW = 1;
		sizeU = CHUNK_SIZE_X;
		sizeV = CHUNK_SIZE_Z;
		sizeW = CHUNK_SIZE_Y;
		break;
	default: // ±Z
		axisU = 0; axisV = 1; axisW = 2;
		sizeU = CHUNK_SIZE_X;
		sizeV = CHUNK_SIZE_Y;
		sizeW = CHUNK_SIZE_Z;
		break;
	}

}

void GreedyMesher::buildMask(Chunk& chunk, std::vector<MaskCell>& mask,
	int axisU, int axisV, int axisW,
	int sizeU, int sizeV, int w, int dir) {
	for (int v = 0; v < sizeV; ++v) {
		for (int u = 0; u < sizeU; ++u) {
			int coords[3] = { 0, 0, 0 };
			coords[axisU] = u;
			coords[axisV] = v;
			coords[axisW] = w;

			const Voxel& voxel = chunk.at(coords[0], coords[1], coords[2]);
			bool visible = (voxel.type != static_cast<uint8_t>(BlockType::AIR)) &&
				IsFaceVisible(chunk, coords[0], coords[1], coords[2], dir);

			mask[u + v * sizeU] = { visible, visible ? voxel.type : static_cast<uint8_t>(0) };
		}
	}
}

void GreedyMesher::mergeMaskIntoQuads(std::vector<MaskCell>& mask,
	ChunkMeshing::ChunkMesh& chunkMesh,
	int axisU, int axisV, int axisW,
	int sizeU, int sizeV, int w,
	bool isPositiveDir, int dir)
{
	for (int v = 0; v < sizeV; ++v) {
		for (int u = 0; u < sizeU;) {
			MaskCell& cell = mask[u + v * sizeU];
			if (!cell.visible) { ++u; continue; }

			int width = 1;
			while (u + width < sizeU &&
				mask[(u + width) + v * sizeU].visible &&
				mask[(u + width) + v * sizeU].blockType == cell.blockType)
				++width;

			int height = 1;
			bool canExpand = true;
			while (canExpand && v + height < sizeV) {
				for (int k = 0; k < width; ++k) {
					MaskCell& testCell = mask[(u + k) + (v + height) * sizeU];
					if (!testCell.visible || testCell.blockType != cell.blockType) {
						canExpand = false;
						break;
					}
				}
				if (canExpand) ++height;
			}

			// Mark merged cells as processed
			for (int dv = 0; dv < height; ++dv)
				for (int du = 0; du < width; ++du)
					mask[(u + du) + (v + dv) * sizeU].visible = false;

			// Compute origin in world space
			int start[3] = { 0, 0, 0 };
			start[axisU] = u;
			start[axisV] = v;
			start[axisW] = isPositiveDir ? w + 1 : w;
			glm::vec3 origin(start[0], start[1], start[2]);

			// du/dv aligned to face
			glm::vec3 duVec(0.0f), dvVec(0.0f);
			switch (dir) {
			case 0: case 1: duVec.z = 1.0f; dvVec.y = 1.0f; break; // ±X
			case 2: case 3: duVec.x = 1.0f; dvVec.z = 1.0f; break; // ±Y
			case 4: case 5: duVec.x = 1.0f; dvVec.y = 1.0f; break; // ±Z
			}

			glm::vec3 normal = GetNormal(dir);

			// Add quad with width/height in voxel units
			AddQuad(chunkMesh, origin, normal, duVec, dvVec, width, height, cell.blockType, dir);

			u += width;
		}
	}
}


void GreedyMesher::AddQuad(ChunkMeshing::ChunkMesh& mesh,
	const glm::vec3& origin,
	const glm::vec3& normal,
	const glm::vec3& duVec,
	const glm::vec3& dvVec,
	int width, int height,
	uint8_t blockType, int faceDir)
{
	// Compute world-space corners
	glm::vec3 corners[4];
	corners[0] = origin;
	corners[1] = origin + duVec * float(width);
	corners[2] = origin + duVec * float(width) + dvVec * float(height);
	corners[3] = origin + dvVec * float(height);

	// For each corner, compute the UV using the atlas
	glm::vec2 uvs[4];
	for (int i = 0; i < 4; ++i) {
		uvs[i] = CalculateUVs(blockType, faceDir, i, width, height);
	}

	for (int i = 0; i < 4; ++i)
		mesh.vertices.push_back({ corners[i], normal, uvs[i] });

	unsigned int start = mesh.vertices.size() - 4;
	mesh.indices.insert(mesh.indices.end(),
		{ start, start + 1, start + 2, start, start + 2, start + 3 });
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
	const float depth = (dir % 2) ? 0.0f : 1.0f;  // -X/-Y/-Z=0.0, +X/+Y/+Z=1.0f

	// Base position (aligned to voxel grid)
	glm::vec3 base(x, y, 0);
	base[depthAxis] += depth;

	// Local axes for quad expansion
	const glm::vec3 right = (depthAxis == 0) ? glm::vec3(0, 0, 1) : glm::vec3(1, 0, 0);
	const glm::vec3 up = (depthAxis == 1) ? glm::vec3(0, 0, 1) : glm::vec3(0, 1, 0);

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

glm::vec2 GreedyMesher::CalculateUVs(
	uint8_t blockType, int faceDir, int cornerIdx, int quadWidth, int quadHeight)
{
	// Get the atlas offset for this block type
	glm::vec2 AtlasOffset = blockTextureAtlas.GetUVTileOffset(blockType, faceDir);

	// Use public getters instead of private members
	float atlasWidth = blockTextureAtlas.GetAtlasWidth();
	float atlasHeight = blockTextureAtlas.GetAtlasHeight();
	int tileSize = blockTextureAtlas.GetTileSize();

	// Determine corner offsets for tiling
	float NewU = 0.0f;
	float NewV = 0.0f;

	 float normalisedTileW = static_cast<float>(tileSize) / atlasWidth;
	 float normalisedTileH = static_cast<float>(tileSize) / atlasHeight;

	switch (cornerIdx) {
		case 0: 
			NewU = 0.0f;
			NewV = 0.0f;           break; // bottom-left
		case 1: 
			NewU = normalisedTileW;
			NewV = 0.0f;         break; // bottom-right
		case 2: 
			NewU = normalisedTileW;
			NewV = normalisedTileH; break; // top-right
		case 3: 
			NewU = 0.0f;
			NewV = normalisedTileH; // top-left
	}

	return { NewU + AtlasOffset.x * normalisedTileW, NewV + AtlasOffset.y * normalisedTileH };
}

glm::vec3 GreedyMesher::GetDUVec(int faceDir) {
	switch (faceDir) {
	case 0: case 1: return glm::vec3(0, 0, 1); // ±X -> Z
	case 2: case 3: return glm::vec3(1, 0, 0); // ±Y -> X
	case 4: case 5: return glm::vec3(1, 0, 0); // ±Z -> X
	default: return glm::vec3(1, 0, 0);
	}
}

glm::vec3 GreedyMesher::GetDVVec(int faceDir) {
	switch (faceDir) {
	case 0: case 1: return glm::vec3(0, 1, 0); // ±X -> Y
	case 2: case 3: return glm::vec3(0, 0, 1); // ±Y -> Z
	case 4: case 5: return glm::vec3(0, 1, 0); // ±Z -> Y
	default: return glm::vec3(0, 1, 0);
	}
}
