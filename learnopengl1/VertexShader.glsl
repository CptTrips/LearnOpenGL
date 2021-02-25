#version 330 core
layout(location = 0) in vec3 aPos; // the position variable has attribute position 0
layout(location = 1) in vec3 model_normal; // vertex normal
layout(location=2) in vec2 tex_coord_in;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 world_pos;
out vec2 tex_coord;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f); 

    world_pos = vec3(model * vec4(aPos, 1.0f));
    normal = mat3(transpose(inverse(mat3(model)))) * model_normal;

    tex_coord = tex_coord_in;
}
