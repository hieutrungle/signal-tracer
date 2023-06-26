#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coords;

out vec2 tex_coords;
out vec3 frag_pos_camera;
out vec3 normal_camera;

uniform mat4 model;
uniform mat4 view;
uniform mat4 view_model;
uniform mat4 projection;
uniform mat4 normal_matrix;

void main()
{
    frag_pos_camera = vec3(view_model * vec4(a_pos, 1.0));
    gl_Position =  projection *  vec4(frag_pos_camera, 1.0);

    tex_coords = a_tex_coords;
    normal_camera = mat3(normal_matrix) * a_normal;
}