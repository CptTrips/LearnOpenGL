#version 330 core
layout(location = 0) in vec3 aPos; // the position variable has attribute position 0
layout(location = 1) in vec3 model_normal; // vertex normal
layout(location=2) in vec2 tex_coord_in;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    vec3 enarlged_pos = aPos + 0.1*model_normal;
    gl_Position = projection * view * model * vec4(enarlged_pos, 1.0f); 
}