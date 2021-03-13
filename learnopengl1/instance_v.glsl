#version 330 core
layout(location = 0) in vec3 aPos; // the position variable has attribute position 0
layout(location = 1) in vec3 model_normal; // vertex normal
layout(location=2) in vec2 tex_coord_in;
layout(location=3) in mat4 instance_space;

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
    //vec3 offset_pos = aPos + offsets[gl_InstanceID]; //Using a uniform limits the number of instances
    gl_Position = projection * view * instance_space * vec4(aPos, 1.0); 

    world_pos = vec3(instance_space * vec4(aPos, 1.0));
    normal = transpose(inverse(mat3(instance_space))) * model_normal;

    tex_coord = tex_coord_in;
}
