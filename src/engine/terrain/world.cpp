//
// Created by Lamad on 14/07/2025.
//

#include "world.h"
#include "chunk.h"
#include "../math/mod.h"


Voxel* World::getBlock(int x, int y, int z) {
	//calc chunk map coords from vec3
	ChunkCoord chunk_coord = VoxelToChunkCoords(x, z);
	//get chunk if it exists
	Chunk* chunk = getChunk(chunk_coord);
	if (chunk != nullptr) {
		int local_x = mod(x, CHUNK_SIZE_X);
		int local_y = mod(y, CHUNK_SIZE_Y);
		int local_z = mod(z, CHUNK_SIZE_Z);
		return &chunk->at(local_x, local_y, local_z);
	}
	// if no chunk data exist
	return nullptr;
}

void World::setBlock(int x, int y, int z, BlockType blockType) {
	ChunkCoord chunk_coord = VoxelToChunkCoords(x, z);
	//get chunk if it exists
	Chunk* chunk = getChunk(chunk_coord);
	if (chunk != nullptr) {
		// find voxel data
		int local_x = mod(x, CHUNK_SIZE_X);
		int local_y = mod(y, CHUNK_SIZE_Y);
		int local_z = mod(z, CHUNK_SIZE_Z);

		chunk->at(local_x, local_y, local_z).type = static_cast<uint8_t>(blockType);

		// mark chunk as dirty
		dirtyChunks.push_back(chunk_coord);
	}

	return;
}

void World::generateChunk(ChunkCoord coord) {
	if (chunkMap.contains(coord)) return;  // Early exit if chunk exists
	auto* newChunk = new Chunk();
	for (int x = 0; x < CHUNK_SIZE_X; ++x) {
		for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
			for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
				if (y < 8) {
					setBlockType(newChunk->at(x, y, z), BlockType::BRICK); // solid block

				}
				else {
					setBlockType(newChunk->at(x, y, z), BlockType::AIR);
				}
			}
		}
	}
	chunkMap[coord] = newChunk;
	dirtyChunks.push_back(coord); // Mark for meshing
}

Chunk* World::getChunk(ChunkCoord coord) {
	return chunkMap[coord];
}

void World::removeChunk(ChunkCoord coord) {
	chunkMap[coord]->mesh.DeleteGPUResources();
	chunkMap.erase(coord);
}

// Takes all chunks modified/dirty, recreates their meshes and reuploads/cleans them
void World::updateDirtyChunks() {
	for (ChunkCoord coord : dirtyChunks) {
		if (auto it = chunkMap.find(coord); it != chunkMap.end()) {
			Chunk* chunk = it->second;
			chunk->mesh.Clear();
			mesher.GreedyMeshChunk(*chunk, chunk->mesh);
			chunk->mesh.UploadToGPU();
		}
	}
	dirtyChunks.clear();
}

void World::renderVisibleChunks(const Shader& shader) {
	shader.use();
	mesher.blockTextureAtlas.Bind();
	// Bind once for all chunks

	for (auto& [coord, chunk] : chunkMap) {
		if (chunk == nullptr) continue;

		// Frustum culling check here if implemented

		// squish
		glm::mat4 model = glm::translate(glm::mat4(1.0f),
			glm::vec3(coord.x * CHUNK_SIZE_X,
				0 * CHUNK_SIZE_Y,
				coord.z * CHUNK_SIZE_Z));

		shader.setMat4("model", model);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		chunk->mesh.Draw();
	}
}
