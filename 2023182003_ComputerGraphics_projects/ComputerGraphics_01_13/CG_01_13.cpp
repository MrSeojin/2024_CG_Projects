#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>>
#include <stdlib.h>
#include <string.h>
#include <random>
#include <gl\glew.h>
#include <gl\freeglut.h>
#include <gl\freeglut_ext.h>
#include <gl\glm\glm\glm.hpp>
#include <gl\glm\glm\ext.hpp>
#include <gl\glm\glm\gtc\matrix_transform.hpp>

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_int_distribution<int> rd_tetra(0, 3), rd_hexa(0, 5);
std::uniform_real_distribution<GLclampf> rd_color(0.0f, 1.0f);

GLuint coord_vao[3], hexahedron[6], tetrahedron[4], plus_vao, vbo[2];
GLfloat background_r = 0.2f, background_g = 0.2f, background_b = 0.2f;
GLint width, height;
GLuint shaderProgramID;	//셰이더 프로그램 이름
GLuint vertexShader;	// 버텍스 셰이더 객체
GLuint fragmentShader;	// 프래그먼트 세이더 객체
int shape_type, aspect_1 = -1, aspect_2 = -1, aspect_3 = -1, aspect_4 = -1;

char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
void InitBuffer();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);

void main(int argc, char** argv)	// 윈도우 생성
{
	width = 800, height = 800;
	glutInit(&argc, argv);							// glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);	// 디스플레이 모드 설정
	glutInitWindowPosition(50, 50);					// 윈도우의 위치 지정
	glutInitWindowSize(width, height);					// 윈도우의 크기 지정
	glutCreateWindow("project_01_13");	// 윈도우 생성 (윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)						// glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	make_shaderProgram();
	InitBuffer();

	// 축 만들기
	glutDisplayFunc(drawScene);						// 출력 함수의 지정
	glutReshapeFunc(Reshape);						// 다시 그리기 함수 지정
	glutKeyboardFunc(Keyboard);						// 키보드 입력 콜백함수 지정

	glutMainLoop();									// 이벤트 처리 시작
}

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;

	fptr = fopen(file, "rb"); // Open file for reading
	if (!fptr) // Return NULL on failure
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file
	length = ftell(fptr); // Find out how many bytes into the file we are
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
	fclose(fptr); // Close the file
	buf[length] = 0; // Null terminator

	return buf; // Return the buffer 
}
void make_vertexShaders()
{
	GLchar* vertexSource;

	vertexSource = filetobuf("vertex.glsl");	//text파일
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, NULL);	//vertexShader하나 만들기
	glCompileShader(vertexShader);

	// 만들어지지 않으면 오류 출력
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader error\n" << errorLog << std::endl;
		return;
	}
}
void make_fragmentShaders()
{
	GLchar* fragmentSource;

	fragmentSource = filetobuf("fragment.glsl");
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, NULL);
	glCompileShader(fragmentShader);

	// 만들어지지 않으면 오류 출력
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader error\n" << errorLog << std::endl;
		return;
	}
}
GLuint make_shaderProgram()
{
	make_vertexShaders();
	make_fragmentShaders();

	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);

	glLinkProgram(shaderProgramID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// 연결되지 않으면 오류 출력
	GLint result;
	GLchar errorLog[512];
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderProgramID, 512, NULL, errorLog);
		std::cerr << "ERROR:shader program 연결 실패\n" << errorLog << std::endl;
		return false;
	}
	return true;
}
void InitBuffer()
{
	// 축
	{
		//x
		glGenVertexArrays(1, &coord_vao[0]);
		glBindVertexArray(coord_vao[0]);
		glGenBuffers(2, vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		const GLfloat x_coord[2][3]{ {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), x_coord, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		const GLfloat x_color[2][3]{ { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } };
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), x_color, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
		//y
		glGenVertexArrays(1, &coord_vao[1]);
		glBindVertexArray(coord_vao[1]);
		glGenBuffers(2, vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		const GLfloat y_coord[2][3]{ {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f} };
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), y_coord, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		const GLfloat y_color[2][3]{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } };
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), y_color, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
		//z
		glGenVertexArrays(1, &coord_vao[2]);
		glBindVertexArray(coord_vao[2]);
		glGenBuffers(2, vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		const GLfloat z_coord[2][3]{ {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 1.0f} };
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), z_coord, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		const GLfloat z_color[2][3]{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } };
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), z_color, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}
	GLfloat color[3]{ rd_color(dre), rd_color(dre), rd_color(dre) };
	// 6면체
	{
		GLfloat spots[6][3]{
			{0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 0.0f} };
		GLfloat colors[6][3]{};
		// 앞 
		{
			glGenVertexArrays(1, &hexahedron[0]);
			glBindVertexArray(hexahedron[0]);
			glGenBuffers(2, vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			spots[0][0] = -0.5f, 	spots[0][1] = 0.5f, 	spots[0][2] = 0.5f;
			spots[1][0] = -0.5f, 	spots[1][1] = -0.5f, 	spots[1][2] = 0.5f;
			spots[2][0] = 0.5f, 	spots[2][1] = 0.5f, 	spots[2][2] = 0.5f;
			spots[3][0] = 0.5f, 	spots[3][1] = 0.5f, 	spots[3][2] = 0.5f;
			spots[4][0] = -0.5f, 	spots[4][1] = -0.5f,	spots[4][2] = 0.5f;
			spots[5][0] = 0.5f, 	spots[5][1] = -0.5f, 	spots[5][2] = 0.5f;
			glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), spots, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
			for (int i = 0; i < 6; ++i) {
				for (int j = 0; j < 3; ++j)
					colors[i][j] = color[j];
			}
			glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
		// 오른	
		{
			glGenVertexArrays(1, &hexahedron[1]);
			glBindVertexArray(hexahedron[1]);
			glGenBuffers(2, vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			spots[0][0] = 0.5f, 	spots[0][1] = 0.5f, 	spots[0][2] = 0.5f;
			spots[1][0] = 0.5f, 	spots[1][1] = -0.5f, 	spots[1][2] = 0.5f;
			spots[2][0] = 0.5f, 	spots[2][1] = 0.5f, 	spots[2][2] = -0.5f;
			spots[3][0] = 0.5f, 	spots[3][1] = 0.5f, 	spots[3][2] = -0.5f;
			spots[4][0] = 0.5f, 	spots[4][1] = -0.5f,	spots[4][2] = 0.5f;
			spots[5][0] = 0.5f, 	spots[5][1] = -0.5f, 	spots[5][2] = -0.5f;
			glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), spots, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
			for (int i = 0; i < 3; ++i)
				color[i] = rd_color(dre);
			for (int i = 0; i < 6; ++i) {
				for (int j = 0; j < 3; ++j)
					colors[i][j] = color[j];
			}
			glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
		// 뒤
		{
			glGenVertexArrays(1, &hexahedron[2]);
			glBindVertexArray(hexahedron[2]);
			glGenBuffers(2, vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			spots[0][0] = 0.5f,		spots[0][1] = 0.5f, 	spots[0][2] = -0.5f;
			spots[1][0] = 0.5f,		spots[1][1] = -0.5f, 	spots[1][2] = -0.5f;
			spots[2][0] = -0.5f,	spots[2][1] = 0.5f, 	spots[2][2] = -0.5f;
			spots[3][0] = -0.5f,	spots[3][1] = 0.5f, 	spots[3][2] = -0.5f;
			spots[4][0] = 0.5f,		spots[4][1] = -0.5f, 	spots[4][2] = -0.5f;
			spots[5][0] = -0.5f,	spots[5][1] = -0.5f, 	spots[5][2] = -0.5f;
			glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), spots, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
			for (int i = 0; i < 3; ++i)
				color[i] = rd_color(dre);
			for (int i = 0; i < 6; ++i) {
				for (int j = 0; j < 3; ++j)
					colors[i][j] = color[j];
			}
			glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
		// 왼
		{
			glGenVertexArrays(1, &hexahedron[3]);
			glBindVertexArray(hexahedron[3]);
			glGenBuffers(2, vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			spots[0][0] = -0.5f, 	spots[0][1] = 0.5f, 	spots[0][2] = -0.5f;
			spots[1][0] = -0.5f, 	spots[1][1] = -0.5f, 	spots[1][2] = -0.5f;
			spots[2][0] = -0.5f, 	spots[2][1] = 0.5f, 	spots[2][2] = 0.5f;
			spots[3][0] = -0.5f, 	spots[3][1] = 0.5f, 	spots[3][2] = 0.5f;
			spots[4][0] = -0.5f, 	spots[4][1] = -0.5f, 	spots[4][2] = -0.5f;
			spots[5][0] = -0.5f, 	spots[5][1] = -0.5f, 	spots[5][2] = 0.5f;
			glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), spots, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
			for (int i = 0; i < 3; ++i)
				color[i] = rd_color(dre);
			for (int i = 0; i < 6; ++i) {
				for (int j = 0; j < 3; ++j)
					colors[i][j] = color[j];
			}
			glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
		// 위
		{
			glGenVertexArrays(1, &hexahedron[4]);
			glBindVertexArray(hexahedron[4]);
			glGenBuffers(2, vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			spots[0][0] = 0.5f, 	spots[0][1] = 0.5f, 	spots[0][2] = 0.5f;
			spots[1][0] = 0.5f, 	spots[1][1] = 0.5f, 	spots[1][2] = -0.5f;
			spots[2][0] = -0.5f, 	spots[2][1] = 0.5f, 	spots[2][2] = 0.5f;
			spots[3][0] = -0.5f, 	spots[3][1] = 0.5f, 	spots[3][2] = 0.5f;
			spots[4][0] = 0.5f, 	spots[4][1] = 0.5f, 	spots[4][2] = -0.5f;
			spots[5][0] = -0.5f, 	spots[5][1] = 0.5f, 	spots[5][2] = -0.5f;
			glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), spots, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
			for (int i = 0; i < 3; ++i)
				color[i] = rd_color(dre);
			for (int i = 0; i < 6; ++i) {
				for (int j = 0; j < 3; ++j)
					colors[i][j] = color[j];
			}
			glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
		// 밑
		{
			glGenVertexArrays(1, &hexahedron[5]);
			glBindVertexArray(hexahedron[5]);
			glGenBuffers(2, vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			spots[0][0] = -0.5f, 	spots[0][1] = -0.5f, 	spots[0][2] = -0.5f;
			spots[1][0] = 0.5f, 	spots[1][1] = -0.5f, 	spots[1][2] = -0.5f;
			spots[2][0] = -0.5f, 	spots[2][1] = -0.5f, 	spots[2][2] = 0.5f;
			spots[3][0] = -0.5f, 	spots[3][1] = -0.5f, 	spots[3][2] = 0.5f;
			spots[4][0] = 0.5f, 	spots[4][1] = -0.5f, 	spots[4][2] = -0.5f;
			spots[5][0] = 0.5f, 	spots[5][1] = -0.5f, 	spots[5][2] = 0.5f;
			glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), spots, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
			for (int i = 0; i < 3; ++i)
				color[i] = rd_color(dre);
			for (int i = 0; i < 6; ++i) {
				for (int j = 0; j < 3; ++j)
					colors[i][j] = color[j];
			}
			glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
	}
	// 4면체
	{
		GLfloat spots[3][3]{
			{0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 0.0f} };
		GLfloat	colors[3][3]{};
		// 왼
		{
			glGenVertexArrays(1, &tetrahedron[0]);
			glBindVertexArray(tetrahedron[0]);
			glGenBuffers(2, vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			spots[0][0] = 0.0f, 	spots[0][1] = 0.5f, 	spots[0][2] = 0.0f;
			spots[1][0] = -0.5f, 	spots[1][1] = -0.5f, 	spots[1][2] = -0.5f;
			spots[2][0] = 0.0f, 	spots[2][1] = -0.5f, 	spots[2][2] = 0.5f;
			glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), spots, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
			for (int i = 0; i < 3; ++i)
				color[i] = rd_color(dre);
			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 3; ++j)
					colors[i][j] = color[j];
			}
			glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
		// 오른
		{
			glGenVertexArrays(1, &tetrahedron[1]);
			glBindVertexArray(tetrahedron[1]);
			glGenBuffers(2, vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			spots[0][0] = 0.0f, spots[0][1] = 0.5f, spots[0][2] = 0.0f;
			spots[1][0] = 0.0f, spots[1][1] = -0.5f, spots[1][2] = 0.5f;
			spots[2][0] = 0.5f, spots[2][1] = -0.5f, spots[2][2] = -0.5f;
			glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), spots, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
			for (int i = 0; i < 3; ++i)
				color[i] = rd_color(dre);
			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 3; ++j)
					colors[i][j] = color[j];
			}
			glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
		// 뒷
		{
			glGenVertexArrays(1, &tetrahedron[2]);
			glBindVertexArray(tetrahedron[2]);
			glGenBuffers(2, vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			spots[0][0] = 0.0f,		spots[0][1] = 0.5f,		spots[0][2] = 0.0f;
			spots[1][0] = 0.5f,		spots[1][1] = -0.5f,	spots[1][2] = -0.5f;
			spots[2][0] = -0.5f,	spots[2][1] = -0.5f,	spots[2][2] = -0.5f;
			glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), spots, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
			for (int i = 0; i < 3; ++i)
				color[i] = rd_color(dre);
			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 3; ++j)
					colors[i][j] = color[j];
			}
			glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
		// 밑
		{
			glGenVertexArrays(1, &tetrahedron[3]);
			glBindVertexArray(tetrahedron[3]);
			glGenBuffers(2, vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			spots[0][0] = 0.0f,		spots[0][1] = -0.5f,	spots[0][2] = 0.5f;
			spots[1][0] = -0.5f,	spots[1][1] = -0.5f,	spots[1][2] = -0.5f;
			spots[2][0] = 0.5f,		spots[2][1] = -0.5f,	spots[2][2] = -0.5f;
			glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), spots, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
			for (int i = 0; i < 3; ++i)
				color[i] = rd_color(dre);
			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 3; ++j)
					colors[i][j] = color[j];
			}
			glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
	}
	{
		glGenVertexArrays(1, &plus_vao);
		glBindVertexArray(plus_vao);
		glGenBuffers(2, vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		const GLfloat spot[3]{0.5f, 0.5f, 0.5f};
		glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(GLfloat), spot, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		for (int i = 0; i < 3; ++i)
			color[i] = rd_color(dre);
		glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(GLfloat), color, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}
}
GLvoid drawScene()
{
	glClearColor(background_r, background_g, background_b, 1.0f);			// 바탕색을 파랑색으로 지정
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);					// 설정된 색으로 전체를 칠하기
	//shader 그리기----------------------
	glUseProgram(shaderProgramID);	// 활성화 한 번만 해주면 된다.
	glm::mat4 Rx = glm::mat4(1.0f);
	glm::mat4 Ry = glm::mat4(1.0f);
	glm::mat4 R = glm::mat4(1.0f);
	Rx = glm::rotate(Rx, glm::radians(10.0f), glm::vec3(1.0, 0.0, 0.0));
	Ry = glm::rotate(Ry, glm::radians(10.0f), glm::vec3(0.0, 1.0, 0.0));
	R = Rx * Ry;
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(R));
	glLineWidth(2);
	glEnable(GL_DEPTH_TEST);	// 깊이 검사 설정
	{
		//x축
		glBindVertexArray(coord_vao[0]);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
		//y축
		glBindVertexArray(coord_vao[1]);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
		//z축
		glBindVertexArray(coord_vao[2]);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (shape_type) {
		if (0 <= aspect_3 &&aspect_3< 4&& 0 <= aspect_4 && aspect_4 < 4) {
			glBindVertexArray(tetrahedron[aspect_3]);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glBindVertexArray(tetrahedron[aspect_4]);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
		else if (aspect_3 == -1 && aspect_4 == -1) {
			for (int i = 0; i < 4; i++) {
				glBindVertexArray(tetrahedron[i]);
				glDrawArrays(GL_TRIANGLES, 0, 3);
			}
		}
		else if(0 <= aspect_3 && aspect_3 < 4){
			glBindVertexArray(tetrahedron[aspect_3]);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
	}
	else {
		if (0 <= aspect_1 && aspect_1 < 6 && 0 <= aspect_2 && aspect_2 < 6) {
			glBindVertexArray(hexahedron[aspect_1]);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(hexahedron[aspect_2]);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		else if (aspect_1 == -1 && aspect_2 == -1) {
			for (int i = 0; i < 6; i++) {
				glBindVertexArray(hexahedron[i]);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
		}
		else if(0 <= aspect_1 && aspect_1 < 6){
			glBindVertexArray(hexahedron[aspect_1]);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
	glDisable(GL_DEPTH_TEST);	// 깊이 검사 해제
	//-----------------------------------
	glutSwapBuffers();
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
{
	glViewport(0, 0, w, h);
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case '1':
		shape_type = 0;
		aspect_1 = 0;
		aspect_2 = -1;
		break;
	case '2':
		shape_type = 0;
		aspect_1 = 1;
		aspect_2 = -1;
		break;
	case '3':
		shape_type = 0;
		aspect_1 = 2;
		aspect_2 = -1;
		break;
	case '4':
		shape_type = 0;
		aspect_1 = 3;
		aspect_2 = -1;
		break;
	case '5':
		shape_type = 0;
		aspect_1 = 4;
		aspect_2 = -1;
		break;
	case '6':
		shape_type = 0;
		aspect_1 = 5;
		aspect_2 = -1;
		break;
	case '7':
		shape_type = 1;
		aspect_3 = 0;
		aspect_4 = -1;
		break;
	case '8':
		shape_type = 1;
		aspect_3 = 1;
		aspect_4 = -1;
		break;
	case '9':
		shape_type = 1;
		aspect_3 = 2;
		aspect_4 = -1;
		break;
	case '0':
		shape_type = 1;
		aspect_3 = 3;
		aspect_4 = -1;
		break;
	case 'c':
		shape_type = 0;
		aspect_1 = rd_hexa(dre);
		aspect_2 = rd_hexa(dre);
		while(aspect_1==aspect_2)
			aspect_2 = rd_hexa(dre);
		break;
	case 't':
		shape_type = 1;
		aspect_3 = rd_tetra(dre);
		aspect_4 = rd_tetra(dre);
		while (aspect_3 == aspect_4)
			aspect_4 = rd_tetra(dre);
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}
