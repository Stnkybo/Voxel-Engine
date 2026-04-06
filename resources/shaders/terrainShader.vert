#version 330 core

layout(location = 0) in vec3 aPos;    // Vertex position
layout(location = 1) in vec3 aNormal; // Vertex normal
layout(location = 2) in vec2 aUV;     // Vertex UVs
layout(location = 3) in vec2 aTileOffset;
layout(location = 4) in vec2 aTileSize;

out vec3 Normal;
out vec2 vUV;
flat out vec2 vTileOffset;  // flat — no interpolation
flat out vec2 vTileSize;    // flat — no interpolation

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    
    Normal = mat3(transpose(inverse(model))) * aNormal;
    vUV = aUV; // keep UV as-is, can be >1.0 for tiling
    vTileOffset = aTileOffset;
    vTileSize   = aTileSize;

    gl_Position = projection * view * vec4(vec3(model * vec4(aPos, 1.0)), 1.0);
}
