#version 330 core
out vec4 FragColor;

uniform vec4 ourColor; // defined in OpenGL code
in vec4 vertexColor; // the input variable from the vertex shader (same name and type)

void main()
{
    FragColor = ourColor;
}