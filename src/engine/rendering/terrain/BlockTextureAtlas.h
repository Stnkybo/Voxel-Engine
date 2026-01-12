//
// Created by Lamad on 27/07/2025.
//

#ifndef BLOCKTEXTUREATLAS_H
#define BLOCKTEXTUREATLAS_H
#include <string>
#include <unordered_map>

#include <glm/vec2.hpp>
#include <glad/glad.h>
#include "../../terrain/voxel.h"


enum class BlockType : uint8_t;

class BlockTextureAtlas {
public:
    BlockTextureAtlas();
    ~BlockTextureAtlas();

    bool LoadFromFile(const std::string& path);
    void Bind(GLenum textureUnit = GL_TEXTURE0) const;
    glm::vec2 GetUVTileOffset(uint8_t type, int face) const;

    float GetAtlasWidth() const;

    float GetAtlasHeight() const;
    int GetTileSize() const;

private:
    GLuint textureID;
    int atlasWidth, atlasHeight;
    int tileSize; 
    std::unordered_map<BlockType, glm::ivec2> textureCoordinates;

};



#endif //BLOCKTEXTUREATLAS_H
