#version 330 core
in vec3 normal;
in vec3 world_pos;
out vec4 FragColor;

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float intensity;
};

#define POINT_LIGHT_COUNT 16
uniform PointLight point_lights[POINT_LIGHT_COUNT];


struct PlanarLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float intensity;
};

uniform PlanarLight planar_light;

struct Flashlight {
    vec3 offset; //offset from camera
    vec3 direction;
    vec3 color;
    float intensity;
};

uniform Flashlight flashlight;

uniform vec3 ground_color;

uniform vec3 view_pos;


vec3 ground_shading(vec3 incident_intensity, vec3 unit_normal, vec3 light_dir)
{
    vec3 diffuse_intensity = max(dot(unit_normal, light_dir), 0.) * incident_intensity;
    vec3 diffuse_color = diffuse_intensity * ground_color;

    return diffuse_color;
}


vec3 point_light_illumination(vec3 unit_normal, PointLight point_light) {
    
    vec3 light_to_world = point_light.position - world_pos;
    vec3 light_dir = normalize(light_to_world);
    float incident_intensity = point_light.intensity / dot(light_to_world, light_to_world);

    //diffuse
    vec3 dc = ground_shading(incident_intensity * point_light.diffuse, unit_normal, light_dir);

    //ambient
    vec3 ambient_color = ground_color * point_light.ambient;

    //emissive
    vec3 reflected_color = dc + ambient_color;

    return reflected_color;

}

vec3 planar_light_illumination(vec3 unit_normal) {

    vec3 light_dir = normalize(planar_light.direction);

    vec3 dc = ground_shading(planar_light.intensity*planar_light.diffuse, unit_normal, light_dir);

    vec3 ambient_color = ground_color * planar_light.ambient;

    vec3 reflected_color = dc + ambient_color;

    return reflected_color;
}

vec3 flashlight_illumination(vec3 unit_normal) {

    vec3 light_to_world = world_pos - (view_pos); //+ flashlight.offset);

    vec3 light_dir = normalize(light_to_world);

    float costheta = dot(normalize(flashlight.direction), light_dir);

    vec3 color_intensity = (flashlight.intensity / dot(light_to_world, light_to_world)) * flashlight.color;

    vec3 ac = ground_color * color_intensity * 0.;

    if (costheta < 0.98) {
		return ac;
	}
    
    vec3 dc = ground_shading(color_intensity, unit_normal, -light_dir);

    return ac + dc;
}

void main()
{
    vec3 unit_normal = normalize(normal);

    vec3 planar_color = planar_light_illumination(unit_normal);

    vec3 point_color = vec3(0.);
    for (int i=0; i<POINT_LIGHT_COUNT; i++)
    {
		point_color += point_light_illumination(unit_normal, point_lights[i]);
    }

    //vec3 flash_color = flashlight_illumination(unit_normal);

    FragColor = vec4(planar_color + point_color, 1.0);

}