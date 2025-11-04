#version 330

in vec3 out_Color; //--- 버텍스 세이더에게서 전달 받음
in vec2 out_TexCoord;
out vec4 FragColor; //--- 색상 출력

uniform int index;
uniform sampler2D outTexture1;
uniform sampler2D outTexture2;
uniform sampler2D outTexture3;
uniform sampler2D outTexture4;
uniform sampler2D outTexture5;
uniform sampler2D outTexture6;
uniform sampler2D outTexture7;

void main(void) 
{
	FragColor = vec4 (out_Color, 1.0);

	if (index !=0)
		FragColor = texture (outTexture1, out_TexCoord) + texture (outTexture2, out_TexCoord) + texture (outTexture3, out_TexCoord) + texture (outTexture4, out_TexCoord) + texture (outTexture5, out_TexCoord) + texture (outTexture6, out_TexCoord) + texture (outTexture7, out_TexCoord);
}
