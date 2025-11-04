#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <random>
#include <gl\glew.h>
#include <gl\freeglut.h>
#include <gl\freeglut_ext.h>

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_int_distribution<int> uid(1, 4);
std::uniform_real_distribution<GLclampf> color(0.0f, 1.0f), size(0.1f, 0.25f);

GLint width, height;
GLuint shaderProgramID;	//셰이더 프로그램 이름
GLuint vertexShader;	// 버텍스 셰이더 객체
GLuint fragmentShader;	// 프래그먼트 세이더 객체
GLuint vao[12], vbo[2];
BOOL vao_check[12], Fill = true;

char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
void InitBuffer(int index, GLfloat x, GLfloat y);
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);

void main(int argc, char** argv)	// 윈도우 생성
{
	width = 800, height = 800;
	glutInit(&argc, argv);							// glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);	// 디스플레이 모드 설정
	glutInitWindowPosition(50, 50);					// 윈도우의 위치 지정
	glutInitWindowSize(width, height);				// 윈도우의 크기 지정
	glutCreateWindow("project_01_08");				// 윈도우 생성 (윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)						// glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	InitBuffer(0, -0.5, 0.5);
	InitBuffer(1, 0.5, 0.5);
	InitBuffer(2, -0.5, -0.5);
	InitBuffer(3, 0.5, -0.5);

	glutDisplayFunc(drawScene);						// 출력 함수의 지정
	glutReshapeFunc(Reshape);						// 다시 그리기 함수 지정
	glutKeyboardFunc(Keyboard);						// 키보드 입력 콜백함수 지정
	glutMouseFunc(Mouse);

	glutMainLoop();									// 이벤트 처리 시작
}

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;

	fptr = fopen(file, "rb");						// Open file for reading
	if (!fptr)										// Return NULL on failure
		return NULL;
	fseek(fptr, 0, SEEK_END);						// Seek to the end of the file
	length = ftell(fptr);							// Find out how many bytes into the file we are
	buf = (char*)malloc(length + 1);				// Allocate a buffer for the entire length of the file and a null terminator
	fseek(fptr, 0, SEEK_SET);						// Go back to the beginning of the file
	fread(buf, length, 1, fptr);					// Read the contents of the file in to the buffer
	fclose(fptr);									// Close the file
	buf[length] = 0;								// Null terminator

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
}
void InitBuffer(int index, GLfloat x, GLfloat y)
{
	GLfloat r_size = size(dre), r = color(dre), g = color(dre), b = color(dre);
	const GLfloat triShape[3][3] = { {x, y + r_size, 0.0}, {x - r_size / 2, y - r_size, 0.0}, {x + r_size / 2, y - r_size, 0.0}};
	const GLfloat colors[3][3] = { { r, g, b }, { r, g, b }, { r, g, b } };
	glGenVertexArrays(1, &vao[index]);
	glBindVertexArray(vao[index]);
	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), triShape, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	vao_check[index] = true;
	make_shaderProgram();
}
GLvoid drawScene()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);					// 설정된 색으로 전체를 칠하기
	//그리기-----------------------------
	glUseProgram(shaderProgramID);	// 활성화 한 번만 해주면 된다.
	for (int i = 0; i < 12; ++i) {
		if(vao_check[i]){
		glBindVertexArray(vao[i]);
//		glLineWidth(3);
		if (Fill)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		}
	}
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
	case 'a':
		Fill = true;
		break;
	case 'b':
		Fill = false;
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}
GLvoid Mouse(int button, int state, int x, int y) {
	GLclampf mouseX = static_cast<GLclampf>(x) / (static_cast<GLclampf>(width) / 2) - 1, mouseY = 1 - static_cast<GLclampf>(y) / (static_cast<GLclampf>(height) / 2);
	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {
			if (-1 <= mouseX && mouseX < 0.0) {
				if (-1 <= mouseY && mouseY < 0.0) {
					for (int i = 2; i < 12; i += 4) 
						vao_check[i] = false;
					InitBuffer(2, mouseX, mouseY);				
				}
				else {
					for (int i = 0; i < 12; i += 4)
						vao_check[i] = false;
					InitBuffer(0, mouseX, mouseY);
				}
			}
			else {
				if (-1 <= mouseY && mouseY < 0) {
					for (int i = 3; i < 12; i += 4)
						vao_check[i] = false;
					InitBuffer(3, mouseX, mouseY);
				}
				else {
					for (int i = 1; i < 12; i += 4)
						vao_check[i] = false;
					InitBuffer(1, mouseX, mouseY);
				}
			}
		}
		else if (button == GLUT_RIGHT_BUTTON) {
			if (-1 <= mouseX && mouseX < 0.0) {
				if (-1 <= mouseY && mouseY < 0.0) {
					for (int i = 2; i < 12; i += 4) {
						if (vao_check[i]);
						else {
							InitBuffer(i, mouseX, mouseY);
							break;
						}
					}
				}
				else {
					for (int i = 0; i < 12; i += 4) {
						if (vao_check[i]);
						else {
							InitBuffer(i, mouseX, mouseY);
							break;
						}
					}
				}
			}
			else {
				if (-1 <= mouseY && mouseY < 0.0) {
					for (int i = 3; i < 12; i += 4) {
						if (vao_check[i]);
						else {
							InitBuffer(i, mouseX, mouseY);
							break;
						}
					}
				}
				else {
					for (int i = 1; i < 12; i += 4) {
						if (vao_check[i]);
						else {
							InitBuffer(i, mouseX, mouseY);
							break;
						}
					}
				}
			}
		}
	}
}
