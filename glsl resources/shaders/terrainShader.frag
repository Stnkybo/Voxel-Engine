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

    // TexCoord is expected to be normalized (0..1) already
    vec4 texColor = texture(uTexture, TexCoord);

    FragColor = vec4(texColor.rgb * (diffuse + ambient), texColor.a);
}