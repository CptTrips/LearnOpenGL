#version 330 core
layout(location = 0) in vec3 aPos; // the position variable has attribute position 0
layout(location = 1) in vec3 model_normal; // vertex normal
layout(location=2) in vec2 tex_coord_in;

uniform mat4 model;

layout (std140) uniform shared_matrices
{
	mat4 projection;
	mat4 view;
};

out VS_OUT
{
	vec3 normal;
	vec3 world_pos;
	vec2 tex_coord;
} vs_out;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0); 

    vs_out.world_pos = vec3(model * vec4(aPos, 1.0));
    vs_out.normal = transpose(inverse(mat3(model))) * model_normal;

    vs_out.tex_coord = tex_coord_in;
}
