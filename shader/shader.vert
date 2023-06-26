#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_tex;

out vec2 texcoord;
out vec3 normal_camera;
out vec3 frag_pos_camera;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normal_matrix;

void main() {
    frag_pos_camera = vec3(view * model * vec4(a_pos, 1.0));
    gl_Position =  projection *  vec4(frag_pos_camera, 1.0);

    texcoord = a_tex;
    normal_camera = mat3(normal_matrix) * a_normal;
}