#version 330 core
layout(location = 0) in vec3 aPos; // the position variable has attribute position 0
layout(location=2) in vec2 tex_coord_in;

out vec2 tex_coord;

uniform mat4 model;

void main()
{
    vec4 model_pos = model * vec4(aPos, 1.0);
    gl_Position = vec4(model_pos.x, model_pos.y, 0., 1.);

    tex_coord = vec2(tex_coord_in.x, -tex_coord_in.y);
}