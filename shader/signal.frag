#version 330 core

out vec4 frag_color;

in vec2 tex_coords;
in vec3 frag_pos_camera;
in vec3 normal_camera;
in vec3 color_in;

uniform vec3 color;

void main()
{
    if (color_in != vec3(0.0))
        frag_color = vec4(color_in, 1.0);
    else {
        frag_color = vec4(color, 1.0);
    }
}