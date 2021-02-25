#version 330 core
layout(location=0) in vec3 pos;

out vec3 tex_dir;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	tex_dir = vec3(1., 1., 1.) * vec3(model * vec4(pos, 1.));
	gl_Position = projection * view * model * vec4(pos, 1.0);
}
