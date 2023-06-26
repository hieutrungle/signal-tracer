#version 330 core

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};


// struct Light {
// 	vec3 position;

// 	vec3 ambient;
// 	vec3 diffuse;
// 	vec3 specular;
// };

struct DirectionalLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

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

#define NUM_POINT_LIGHTS 4

out vec4 frag_color;

in vec2 texcoord;
in vec3 normal_camera;
in vec3 frag_pos_camera;

uniform vec3 light_color;
uniform mat4 view;
uniform Material material;
uniform DirectionalLight directional_light;
uniform PointLight point_lights[NUM_POINT_LIGHTS];
uniform SpotLight spot_light;

// function prototypes
vec3 calc_directional_light_color(DirectionalLight light, vec3 normal, vec3 view_direction);
vec3 calc_point_light_color(PointLight light, vec3 normal, vec3 view_direction, vec3 frag_pos_camera);
vec3 calc_spot_light_color(SpotLight light, vec3 normal, vec3 view_direction, vec3 frag_pos_camera);

void main() {

	vec3 normal_normalized = normalize(normal_camera);
	vec3 view_direction = normalize(-frag_pos_camera);
	vec3 result = vec3(0.0);

	for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
		vec3 point_color = calc_point_light_color(point_lights[i], normal_normalized, view_direction, frag_pos_camera)* light_color;
		result += point_color;
	}
	result += calc_directional_light_color(directional_light, normal_normalized, view_direction);
	result += calc_spot_light_color(spot_light, normal_normalized, view_direction, frag_pos_camera);

	frag_color = vec4(result, 1.0);
}

vec3 calc_directional_light_color(DirectionalLight light, vec3 normal, vec3 view_direction) {
	
	vec3 light_direction = normalize(-light.direction);
	float diff = max(dot(normal, light_direction), 0.0);

	vec3 reflect_direction = reflect(-light_direction, normal);
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, texcoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texcoord));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, texcoord));

	return (ambient + diffuse + specular);
}

vec3 calc_point_light_color(PointLight light, vec3 normal, vec3 view_direction, vec3 frag_pos_camera) {
	vec3 light_direction = normalize(light.position - frag_pos_camera);
	vec3 reflect_direction = reflect(-light_direction, normal);
	float diff = max(dot(normal, light_direction), 0.0);
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);

	float distance = length(light.position - frag_pos_camera);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, texcoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texcoord));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, texcoord));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}

vec3 calc_spot_light_color(SpotLight light, vec3 normal, vec3 view_direction, vec3 frag_pos_camera) {
	vec3 light_direction = normalize(-frag_pos_camera);
	vec3 reflect_direction = reflect(-light_direction, normal);
	float diff = max(dot(normal, light_direction), 0.0);
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);

	float distance = length(-frag_pos_camera);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	float theta = dot(light_direction, normalize(-light.direction));
	float epsilon = light.inner_cut_off - light.outer_cut_off;
	float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);
	
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, texcoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texcoord));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, texcoord));
	return (ambient + diffuse + specular)* attenuation * intensity;
}
