#version 330 core
in vec3 tex_dir;

out vec4 FragColor;

uniform samplerCube cubemap;

void main()
{
	FragColor = texture(cubemap, tex_dir);
}
