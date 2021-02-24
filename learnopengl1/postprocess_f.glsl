#version 330 core
in vec2 tex_coord;

out vec4 FragColor;

uniform sampler2D screen_texture;

void main()
{

	ivec2 tex_size = textureSize(screen_texture, 0);

	vec2 offset = 1. / tex_size;

	vec2 offsets[9] = vec2[](
		vec2(-offset.x, offset.y),
		vec2(0., offset.y),
		vec2(offset.x, offset.y),
		vec2(-offset.x, 0.),
		vec2(0., 0.),
		vec2(offset.x, 0.),
		vec2(-offset.x, -offset.y),
		vec2(0., -offset.y),
		vec2(offset.x, -offset.y)
	);

	float kernel[9] = float[] (
	    1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16  
	);

	vec3 color = vec3(0.);
	for (int i=0; i<9; i++)
	{
		color += kernel[i] * vec3(texture(screen_texture, tex_coord + offsets[i]));
	}

	FragColor = vec4(color, 1.);
}