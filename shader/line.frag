#version 330 core

out vec4 frag_color;

in vec2 tex_coords;
in vec3 frag_pos_camera;
in vec3 normal_camera;

uniform vec3 color;

void main()
{
    frag_color = vec4(color, 1.0);
}