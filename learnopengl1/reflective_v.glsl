#version 330 core
layout(location=0) in vec3 pos;
layout(location=1) in vec3 normal;

out vec3 tex_dir;

layout (std140) uniform shared_matrices
{
	uniform mat4 projection;
	uniform mat4 view;
};
uniform mat4 model;

uniform vec3 cam_pos;

void main()
{
	vec3 world_pos = vec3(model * vec4(pos, 1.));

	vec3 world_normal = transpose(inverse(mat3(model))) * normal;

	vec3 cam_to_vertex = world_pos - cam_pos;

	tex_dir = reflect(cam_to_vertex, world_normal);

	gl_Position = projection * view * model * vec4(pos, 1.0);
}
