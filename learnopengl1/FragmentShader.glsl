#version 330 core
in vec3 normal;
in vec3 world_pos;
out vec4 FragColor;

uniform vec3 object_color;
uniform vec3 light_color;
uniform vec3 light_pos;
uniform vec3 view_pos;

void main()
{
    vec3 unit_normal = normalize(normal);
    vec3 light_dir = normalize(light_pos - world_pos);

    vec3 view_dir = normalize(view_pos - world_pos);
    vec3 reflect_dir = reflect(-light_dir, unit_normal);

    float reflectance = 0.5;
    float specular_intensity = pow(max(dot(view_dir, reflect_dir), 0.), 32);
    vec3 specular =  specular_intensity * reflectance * light_color;

    vec3 diffuse_color = max(dot(unit_normal, light_dir), 0.) * light_color;

    float ambient_intensity = 0.1;
    vec3 ambient_color_density = ambient_intensity * light_color;

    vec3 reflected_color = (specular + diffuse_color + ambient_color_density) * object_color;

    FragColor = vec4(reflected_color, 1.0);
}