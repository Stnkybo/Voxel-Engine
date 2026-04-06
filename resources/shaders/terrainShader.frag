#version 330 core

in vec3 FragPos;
in vec3 Normal;

in vec2 vUV;
flat in vec2 vTileOffset;  // must match vertex shader qualifier exactly
flat in vec2 vTileSize;

out vec4 FragColor;

uniform sampler2D uAtlas; // Texture atlas
uniform vec3 lightDir = normalize(vec3(1.0, -1.0, 1.0));
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 ambientColor = vec3(0.3, 0.3, 0.3);


void main()
{
    // Normalize normal
    vec3 norm = normalize(Normal);

    // Simple directional lighting
    float diff = max(dot(norm, -lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 ambient = ambientColor;

    // UV stuff
    vec2 tiledUV = fract(vUV);

    vec2 texelSize = 1.0 / vec2(textureSize(uAtlas, 0));
    vec2 halfTexel = 0.5 * texelSize;
    tiledUV = clamp(tiledUV, halfTexel / vTileSize, 1.0 - halfTexel / vTileSize);

    vec2 atlasUV = vTileOffset + tiledUV * vTileSize;


    // Sample texture (UVs can be >1 for tiling)
    vec4 texColor = texture(uAtlas, atlasUV);


    // FragColor = vec4(TexCoord.x, TexCoord.y, 0.0, 1.0);
    FragColor = vec4(texColor.rgb * (diffuse + ambient), texColor.a);
    // Fragment shader debug - should show color variations
}
