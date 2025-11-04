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
std::uniform_int_distribution<int> rd_dir(0, 1);
std::uniform_real_distribution<GLclampf> rd_color(0.0f, 1.0f), rd_point(-1.0f, 1.0f);

GLuint vbo[2];
GLfloat background_r = 0.2f, background_g = 0.2f, background_b = 0.2f;
int spiral_num = 0;
struct spiral {
	GLuint vao;
	BOOL check;
	int num;
};
spiral circle[5];
GLint width, height;
GLuint shaderProgramID;	//셰이더 프로그램 이름
GLuint vertexShader;	// 버텍스 셰이더 객체
GLuint fragmentShader;	// 프래그먼트 세이더 객체
BOOL line;

char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
void InitBuffer(int index, GLfloat x, GLfloat y);
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid plus_spot(int value);

void main(int argc, char** argv)	// 윈도우 생성
{
	width = 800, height = 800;
	glutInit(&argc, argv);							// glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);	// 디스플레이 모드 설정
	glutInitWindowPosition(50, 50);					// 윈도우의 위치 지정
	glutInitWindowSize(width, height);					// 윈도우의 크기 지정
	glutCreateWindow("project_01_10");	// 윈도우 생성 (윈도우 이름)

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

	glutDisplayFunc(drawScene);						// 출력 함수의 지정
	glutReshapeFunc(Reshape);						// 다시 그리기 함수 지정
	glutKeyboardFunc(Keyboard);		// 키보드 입력 콜백함수 지정
	glutMouseFunc(Mouse);

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
}
void InitBuffer(int index, GLfloat x, GLfloat y)
{
	make_shaderProgram();
	const GLfloat color[3] = {rd_color(dre), rd_color(dre), rd_color(dre)};
	static GLfloat spots[120][3], colors[120][3];
	GLfloat angle = 0.0f, radius = 0.0f;
	if (rd_dir(dre)) {
		for (int i = 0; i < 60; ++i) {
			radius += 0.002f;
			angle += 0.314f;
			spots[i][0] = x - radius * cos(angle);
			spots[i][1] = y - radius * sin(angle);
			spots[i][2] = 0;
			colors[i][0] = 1 - color[0] * (60 - i) / 60;
			colors[i][1] = 1 - color[1] * (60 - i) / 60;
			colors[i][2] = 1 - color[2] * (60 - i) / 60;
		}
		GLfloat x_size = radius;
		for (int i = 0; i < 60; ++i) {
			radius -= 0.002f;
			angle -= 0.314f;
			spots[60 + i][0] = x - x_size * 2 + radius * cos(angle);
			spots[60 + i][1] = y + radius * sin(angle);
			spots[60 + i][2] = 0;
			colors[60 + i][0] = 1 - color[0] *i / 60;
			colors[60 + i][1] = 1 - color[1] *i / 60;
			colors[60 + i][2] = 1 - color[2] * i / 60;
		}
	}
	else{
		for (int i = 0; i < 60; ++i) {
			radius += 0.002f;
			angle -= 0.314f;
			spots[i][0] = x + radius * cos(angle);
			spots[i][1] = y + radius * sin(angle);
			spots[i][2] = 0;
			colors[i][0] = 1 - color[0] * i / 60;
			colors[i][1] = 1 - color[1] * i / 60;
			colors[i][2] = 1 - color[2] * i / 60;
		}
		GLfloat x_size = radius;
		for (int i = 0; i < 60; ++i) {
			radius -= 0.002f;
			angle += 0.314f;
			spots[60 + i][0] = x + x_size * 2 - radius * cos(angle);
			spots[60 + i][1] = y - radius * sin(angle);
			spots[60 + i][2] = 0;
			colors[60 + i][0] = 1 - color[0] * (60 - i) / 60;
			colors[60 + i][1] = 1 - color[1] * (60 - i) / 60;
			colors[60 + i][2] = 1 - color[2] * (60 - i) / 60;
		}
	}
	glGenVertexArrays(1, &circle[index].vao);
	glBindVertexArray(circle[index].vao);
	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 120 * 3 * sizeof(GLfloat), spots, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 120 * 3 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}
GLvoid drawScene()
{
	glClearColor(background_r, background_g, background_b, 1.0f);			// 바탕색을 파랑색으로 지정
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);					// 설정된 색으로 전체를 칠하기
	//shader 그리기----------------------
	glUseProgram(shaderProgramID);	// 활성화 한 번만 해주면 된다.
	for (int i = 0; i < 5; ++i) {
		if (circle[i].check) {
			glBindVertexArray(circle[i].vao);
			if (line) {
				glLineWidth(5);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDrawArrays(GL_LINE_STRIP, 0, circle[i].num);
			}
			else {
				glPointSize(5);
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				glDrawArrays(GL_POINTS, 0, circle[i].num);
			}
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
	case '1':
		spiral_num = 1;
		break;
	case '2':
		spiral_num = 2;
		break;
	case '3':
		spiral_num = 3;
		break;
	case '4':
		spiral_num = 4;
		break;
	case '5':
		spiral_num = 5;
		break;
	case 'p':
		line = false;
		break;
	case 'l':
		line = true;
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}
GLvoid Mouse(int button, int state, int x, int y) {
	GLclampf mouseX = static_cast<GLclampf>(x) / (static_cast<GLclampf>(width) / 2) - 1, mouseY = 1 - static_cast<GLclampf>(y) / (static_cast<GLclampf>(height) / 2);
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
		for (int i = 0; i < 5; ++i) {
			circle[i].check = false;
			glDeleteVertexArrays(1, &circle[i].vao);
		}
		for (int i = 0; i < spiral_num; ++i) {
			if (i == 0)
				InitBuffer(i, mouseX, mouseY);
			else
				InitBuffer(i, rd_point(dre), rd_point(dre));
			circle[i].num = 0;
			circle[i].check = true;

			background_r = rd_color(dre);
			background_g = rd_color(dre);
			background_b = rd_color(dre);
			glutTimerFunc(100, plus_spot, i);
		}
	}
	glutPostRedisplay();
}
GLvoid plus_spot(int value) 
{
	circle[value].num += 1;
	glutPostRedisplay();
	if (circle[value].num < 119)
		glutTimerFunc(100, plus_spot, value);
}
