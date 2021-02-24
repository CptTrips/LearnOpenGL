#version 330 core
in vec2 tex_coord;

out vec4 FragColor;

uniform sampler2D screen_texture;

void main()
{
    vec4 tex_color = texture(screen_texture, tex_coord);
	FragColor = tex_color;
}