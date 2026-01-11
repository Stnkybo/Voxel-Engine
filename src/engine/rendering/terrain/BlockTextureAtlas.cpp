//
// Created by Lamad on 27/07/2025.
//

#include "BlockTextureAtlas.h"

#include "GreedyMesher.h"
#include "stb_image.h"
BlockTextureAtlas::BlockTextureAtlas() : textureID(0), atlasWidth(2048), atlasHeight(1024), tileSize(16) {}

BlockTextureAtlas::~BlockTextureAtlas() {
    // Free the OpenGL texture if it exists
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
    }
}


bool BlockTextureAtlas::LoadFromFile(const std::string& path) {
    // 1. Load texture file
    int channels;
    unsigned char* data = stbi_load(path.c_str(), &atlasWidth, &atlasHeight, &channels, 4);

    // TEMPORARY
    tileSize = 1024;

    // 2. Create OpenGL texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlasWidth, atlasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // 3. Set parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Pixel-perfect
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // 4. Define texture coordinates
    textureCoordinates[BlockType::GRASS] = {0, 0};
    textureCoordinates[BlockType::DIRT] = {0, 1};     // First row, second tile
    textureCoordinates[BlockType::STONE] = {0, 0};
    textureCoordinates[BlockType::BRICK] = {0, 0};

    stbi_image_free(data);

    return true;
}

void BlockTextureAtlas::Bind(GLenum textureUnit) const {
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

// Returns width and height of Tile UV offset
glm::vec2 BlockTextureAtlas::GetUVOffset(uint8_t type, int face) const {

    glm::ivec2 tilePos = textureCoordinates.at(static_cast<BlockType>(type));
    return {
         static_cast<float>(tilePos.y),
         static_cast<float>(tilePos.x)
    };
}

float BlockTextureAtlas::GetAtlasWidth() const {
    return atlasWidth;
}

float BlockTextureAtlas::GetAtlasHeight() const {
    return atlasHeight;
}

int BlockTextureAtlas::GetTileSize() const {
    return tileSize;
}
