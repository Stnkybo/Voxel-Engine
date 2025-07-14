#version 330 core

in vec4 vertexColor; // Interpolated color from the vertex shader

out vec4 FragColor; // Final color output

void main()
{
    // Use the interpolated vertex position as the color
    FragColor = vertexColor;
}
