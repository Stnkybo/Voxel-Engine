#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;
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

    // Temp hardcoded
	vec2 tileSize = vec2(1.0/1, 1.0/1);		// Each tile is 1/2 of the atlas

    vec2 coord = tileSize * fract(TexCoord);

    // Sample texture (UVs can be >1 for tiling)
    vec4 texColor = texture(uTexture, coord);


    // FragColor = vec4(TexCoord.x, TexCoord.y, 0.0, 1.0);
    FragColor = vec4(texColor.rgb * (diffuse + ambient), texColor.a);
    // Fragment shader debug - should show color variations
}
