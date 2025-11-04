#version 330 core

in vec3 out_color;
out vec4 Fragcolor;

void main ()
{
	Fragcolor = vec4 (out_color, 1.0);
}