#version 330
in vec3 out_Color; //--- 버텍스 세이더에게서 전달 받음
out vec4 FragColor; //--- 색상 출력

uniform mat4 objectColor;
//uniform mat4 lightColor;


void main(void) 
{
	FragColor = objectColor * vec4 (1.0,1.0,1.0, 1.0);
}
