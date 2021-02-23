#version 330 core
in vec2 tex_coord;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emissive;
    float shininess;
};

//uniform Material material;
#define MAX_TEXTURE 128
uniform Material materials[MAX_TEXTURE];

out vec4 FragColor;

void main()
{
	FragColor = texture(materials[0].diffuse, tex_coord);
}