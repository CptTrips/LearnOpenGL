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

uniform vec3 offsets[100];

out vec3 normal;
out vec3 world_pos;
out vec2 tex_coord;

void main()
{
    vec3 offset_pos = aPos + offsets[gl_InstanceID];
    gl_Position = projection * view * model * vec4(offset_pos, 1.0); 

    world_pos = vec3(model * vec4(offset_pos, 1.0));
    normal = transpose(inverse(mat3(model))) * model_normal;

    tex_coord = tex_coord_in;
}
