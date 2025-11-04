#version 330 core

in vec3 out_color;
out vec4 Fragcolor;

uniform mat4 setColor;

void main ()
{
	Fragcolor = setColor * vec4(1.0f,1.0f,1.0f,1.0f);
}