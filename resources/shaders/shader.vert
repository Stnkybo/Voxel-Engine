#version 330 core

// Input attributes from the vertex buffer
layout (location = 0) in vec3 aPos; // Position of the vertex
layout (location = 1) in vec3 aNormal; // Normal (optional, if lighting is used)
layout (location = 2) in vec2 aUV;  // UV coordinates to draw the correct texture from the atlas

// Output to the fragment shader
out vec4 vertexColor;
out vec2 vUV;

// Uniforms
uniform mat4 model;        // Model matrix
uniform mat4 view;         // View matrix
uniform mat4 projection;   // Projection matrix
uniform vec3 meshCenter;   // Center of the mesh (world space)

// Main function
void main()
{
    // Transform the vertex position to world space
    vec4 worldPos = model * vec4(aPos, 1.0);

    // Compute distance from the center of the mesh
    float distance = length(worldPos.xyz - meshCenter);

    // Normalize the distance for coloring (you might need to scale this based on your mesh size)
    float normalizedDistance = clamp(distance / 5.0, 0.0, 1.0); // Adjust the denominator as needed

    // Assign color based on distance (e.g., gradient from blue to red)
    vertexColor = vec4(normalizedDistance, 0.0, 1.0 - normalizedDistance, 1.0);

    // Transform the vertex position to clip space
    gl_Position = projection * view * worldPos;
}
