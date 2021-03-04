#version 330 core
layout(location = 0) in vec3 aPos; // the position variable has attribute position 0

uniform mat4 model;
layout (std140) uniform shared_matrices
{
	uniform mat4 projection;
	uniform mat4 view;
};


void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f); 
}
