#version 330 core
//--- vpos: attribute index 0
//--- in_color: attribute index 1

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 in_color;
out vec3 out_color;

uniform mat4 modelTransform;

void main ()
{
	gl_Position = modelTransform * vec4(vPos.x, vPos.y, vPos.z, 1.0);
	out_color = in_color;
}