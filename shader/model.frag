#version 330 core

struct DirectionalLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};


struct SpotLight {
	vec3 position;
	vec3 direction;
	float inner_cut_off;
	float outer_cut_off;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 frag_color;

in vec2 tex_coords;
in vec3 frag_pos_camera;
in vec3 normal_camera;

uniform sampler2D texture_diffuse0;
uniform DirectionalLight directional_light;
uniform SpotLight flash_light;

vec3 calc_directional_light_color(DirectionalLight light, vec3 normal, vec3 view_direction);
vec3 calc_spot_light_color(SpotLight light, vec3 normal, vec3 view_direction, vec3 frag_pos_camera);


void main()
{    
    vec3 normal_normalized = normalize(normal_camera);
	vec3 view_direction = normalize(-frag_pos_camera);
	vec3 result = vec3(0.0);
    result += calc_directional_light_color(directional_light, normal_normalized, view_direction)*0.75;
    result += calc_spot_light_color(flash_light, normal_normalized, view_direction, frag_pos_camera);

    frag_color = vec4(result, 1.0);
}


vec3 calc_directional_light_color(DirectionalLight light, vec3 normal, vec3 view_direction) {
	
	vec3 light_direction = normalize(-light.direction);
	float diff = max(dot(normal, light_direction), 0.0);

	vec3 reflect_direction = reflect(-light_direction, normal);
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0), 1);
    
    vec3 texture_color = vec3(texture(texture_diffuse0, tex_coords));
    if (texture_color == vec3(0.0)) {
        texture_color = vec3(1.0);
    }
	vec3 ambient = light.ambient;
	vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec;

	return (ambient + diffuse + specular)*texture_color;
}

vec3 calc_spot_light_color(SpotLight light, vec3 normal, vec3 view_direction, vec3 frag_pos_camera) {
	vec3 light_direction = normalize(-frag_pos_camera);
	vec3 reflect_direction = reflect(-light_direction, normal);
	float diff = max(dot(normal, light_direction), 0.0);
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0), 1);

	float distance = length(-frag_pos_camera);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	float theta = dot(light_direction, normalize(-light.direction));
	float epsilon = light.inner_cut_off - light.outer_cut_off;
	float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);

    vec3 texture_color = vec3(texture(texture_diffuse0, tex_coords));
    if (texture_color == vec3(0.0)) {
        texture_color = vec3(1.0);
    }

    vec3 ambient = light.ambient;
	vec3 diffuse = light.diffuse * diff;
    // vec3 specular = light.specular * spec;
    vec3 specular = vec3(0.0);

	return (ambient + diffuse + specular) * attenuation * intensity;
}

