#version 330 core
in vec3 normal;
in vec3 world_pos;
in vec2 tex_coord;
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

uniform vec3 view_pos;

void main()
{
    vec3 unit_normal = normalize(normal);
    vec3 light_dir = normalize(light.position - world_pos);

    vec3 view_dir = normalize(view_pos - world_pos);
    vec3 reflect_dir = reflect(-light_dir, unit_normal);

    //specular
    float specular_intensity = pow(max(dot(view_dir, reflect_dir), 0.), 32);
    vec3 specular_color = specular_intensity * material.specular * light.specular;

    //diffuse
    float diffuse_intensity = max(dot(unit_normal, light_dir), 0.);
    vec3 diffuse_color = diffuse_intensity * vec3(texture(material.diffuse, tex_coord)) * light.diffuse;

    //ambient
    vec3 ambient_color = vec3(texture(material.diffuse, tex_coord)) * light.ambient;

    vec3 reflected_color = (specular_color + diffuse_color + ambient_color);

    FragColor = vec4(reflected_color, 1.0);
}