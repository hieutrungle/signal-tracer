#version 330 core

struct DirectionalLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 frag_color;

in vec2 tex_coords;
in vec3 frag_pos_camera;
in vec3 normal_camera;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
uniform DirectionalLight directional_light;

vec3 calc_directional_light_color(DirectionalLight light, vec3 normal, vec3 view_direction);

void main()
{
    vec3 normal_normalized = normalize(normal_camera);
	vec3 view_direction = normalize(-frag_pos_camera);
	vec3 result = vec3(0.0);
    result += calc_directional_light_color(directional_light, normal_normalized, view_direction);

    frag_color = vec4(result, 1.0);
}

vec3 calc_directional_light_color(DirectionalLight light, vec3 normal, vec3 view_direction) {
	
	vec3 light_direction = normalize(-light.direction);
	float diff = max(dot(normal, light_direction), 0.0);

	vec3 reflect_direction = reflect(-light_direction, normal);
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0), 1);
    
    vec3 texture_diffuse = vec3(texture(texture_diffuse0, tex_coords)).rgb;
    vec3 texture_specular = vec3(texture(texture_specular0, tex_coords)).rgb;
    if (texture_diffuse == vec3(0.0)) {
        texture_diffuse = vec3(1.0);
    }
	vec3 ambient = light.ambient * texture_diffuse;
	vec3 diffuse = light.diffuse * diff * texture_diffuse;
    vec3 specular = light.specular * spec * texture_specular;

	return (ambient + diffuse + specular);
}