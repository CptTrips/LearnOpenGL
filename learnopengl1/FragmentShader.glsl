#version 330 core
in vec3 normal;
in vec3 world_pos;
in vec2 tex_coord;
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emissive;
    float shininess;
};

uniform Material material;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float intensity;
};

uniform Light light;


struct FarLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float intensity;
};

uniform FarLight far_light;

uniform vec3 view_pos;

vec3 specular_color(vec3 incident_intensity, vec3 unit_normal, vec3 light_dir)
{

    vec3 view_dir = normalize(view_pos - world_pos);
    vec3 reflect_dir = reflect(-light_dir, unit_normal);

    vec3 specular_intensity = pow(max(dot(view_dir, reflect_dir), 0.), 32) * incident_intensity;
    vec3 specular_color = specular_intensity * vec3(texture(material.specular, tex_coord));

    return specular_color;
}

vec3 diffuse_color(vec3 incident_intensity, vec3 unit_normal, vec3 light_dir)
{

    vec3 diffuse_intensity = max(dot(unit_normal, light_dir), 0.) * incident_intensity;
    vec3 diffuse_color = diffuse_intensity * vec3(texture(material.diffuse, tex_coord));

    return diffuse_color;
}

vec3 local_light_illumination(vec3 unit_normal) {
    
    vec3 light_to_world = light.position - world_pos;
    vec3 light_dir = normalize(light_to_world);
    float incident_intensity = light.intensity / dot(light_to_world, light_to_world);

    vec3 sc = specular_color(incident_intensity*light.specular, unit_normal, light_dir);

    //diffuse
    vec3 dc = diffuse_color(incident_intensity * light.diffuse, unit_normal, light_dir);

    //ambient
    vec3 ambient_color = vec3(texture(material.diffuse, tex_coord)) * light.ambient;

    //emissive
    vec3 emissive_color = vec3(0.);//vec3(texture(material.emissive, tex_coord));

    vec3 reflected_color = emissive_color + sc + dc + ambient_color;

    return reflected_color;

}

vec3 far_light_illumination(vec3 unit_normal) {

    vec3 light_dir = normalize(far_light.direction);

    vec3 sc = specular_color(far_light.intensity*far_light.specular, unit_normal, light_dir);

    //diffuse
    vec3 dc = specular_color(far_light.intensity*far_light.diffuse, unit_normal, light_dir);

    //ambient
    vec3 ambient_color = vec3(texture(material.diffuse, tex_coord)) * far_light.ambient;

    vec3 reflected_color = sc + dc + ambient_color;

    return reflected_color;
}

void main()
{

    vec3 unit_normal = normalize(normal);

    vec3 local_color = local_light_illumination(unit_normal);
    vec3 far_color = far_light_illumination(unit_normal);
    FragColor = vec4(local_color + far_color, 1.0);
}