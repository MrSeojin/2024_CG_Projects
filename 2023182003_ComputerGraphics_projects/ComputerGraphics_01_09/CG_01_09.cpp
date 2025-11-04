#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <random>
#include <cmath>
#include <gl\glew.h>
#include <gl\freeglut.h>
#include <gl\freeglut_ext.h>

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_int_distribution<int> uid(0, 9), rd_dir(0, 1);
std::uniform_real_distribution<GLclampf> color(0.0f, 1.0f), size(0.1f, 0.2f), rd_speed(0.005f, 0.05f);

struct triangle {
	GLuint vao;
	GLfloat xyz[3][3], rgb[3][3];
	int dir = rd_dir(dre), mod, num;
};
triangle tri[4]{};
GLuint vbo[2];
GLint width, height;
GLuint shaderProgramID;	//셰이더 프로그램 이름
GLuint vertexShader;	// 버텍스 셰이더 객체
GLuint fragmentShader;	// 프래그먼트 세이더 객체
BOOL playing;
int sqare_num = 0, animation_mod;

GLfloat change_dir_x (GLfloat x) {
		if (x - 0.05f < -1.0f) 
			return (0.95f + x);
		if (x + 0.05f > 1.0f)
			return (x - 0.95f);
	return 0;
}
GLfloat change_dir_y (GLfloat y) {
		if (y - 0.05f < -1.0f) 
			return (0.95f + y);
		if (y + 0.05f > 1.0f)
			return (y - 0.95f);
	return 0;
}
GLfloat change_dir(GLfloat x, GLfloat y) {
	if (change_dir_x(x)!=0)
		return change_dir_x(x);
	if (change_dir_x(y)!=0)
		return change_dir_x(y);
	return 0;
}
char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
void InitBuffer(int index);
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid animation(int value);

void main(int argc, char** argv)	// 윈도우 생성
{
	width = 800, height = 800;
	glutInit(&argc, argv);							// glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);	// 디스플레이 모드 설정
	glutInitWindowPosition(50, 50);					// 윈도우의 위치 지정
	glutInitWindowSize(width, height);					// 윈도우의 크기 지정
	glutCreateWindow("project_01_09");	// 윈도우 생성 (윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)						// glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

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
	return true;
}
void InitBuffer(int index)
{
	make_shaderProgram();
	glGenVertexArrays(1, &tri[index].vao);
	glBindVertexArray(tri[index].vao);
	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), tri[index].xyz, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), tri[index].rgb, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}
GLvoid drawScene()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);			// 바탕색을 파랑색으로 지정
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);					// 설정된 색으로 전체를 칠하기
	//shader 그리기----------------------
	glUseProgram(shaderProgramID);	// 활성화 한 번만 해주면 된다.
	for (int i = 0; i < sqare_num; ++i) {
		glBindVertexArray(tri[i].vao);
		glLineWidth(3);
		glDrawArrays(GL_TRIANGLES, 0, 3);
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
		animation_mod = 0;
		break;
	case '2':
		animation_mod = 1;
		break;
	case '3':
		animation_mod = 2;
		break;
	case '4':
		animation_mod = 3;
		break;
	case 's':
		if (playing)
			playing = false;
		else
			playing = true;
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}
GLvoid Mouse(int button, int state, int x, int y) {
	GLclampf mouseX = static_cast<GLclampf>(x) / (static_cast<GLclampf>(width) / 2) - 1, 
		mouseY = 1 - static_cast<GLclampf>(y) / (static_cast<GLclampf>(height) / 2);
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
		if (sqare_num < 4) {
			GLfloat r = color(dre), g = color(dre), b = color(dre);
			GLfloat triangle[3][3] = { {mouseX, mouseY + 0.05f, 0.0f},{mouseX - 0.05f / 2, mouseY - 0.05f, 0.0f},{mouseX + 0.05f / 2, mouseY - 0.05f, 0.0f} };
			for (int i = 0; i < 3; ++i) {
				tri[sqare_num].xyz[i][0] = triangle[i][0];
				tri[sqare_num].xyz[i][1] = triangle[i][1];
				tri[sqare_num].xyz[i][2] = 0.0f;
				tri[sqare_num].rgb[i][0] = r;
				tri[sqare_num].rgb[i][1] = g;
				tri[sqare_num].rgb[i][2] = b;
			}
			InitBuffer(sqare_num);
			glutTimerFunc(100, animation, sqare_num);
			sqare_num++;
		}
		glutPostRedisplay();
	}
}
GLvoid animation(int value)
{
	if (playing) {
		GLfloat center_x{}, center_y{};
		switch (tri[value].mod) {
		case 0:
			center_x = (tri[value].xyz[0][0] + tri[value].xyz[1][0] + tri[value].xyz[2][0]) / 3;
			center_y = (tri[value].xyz[0][1] + tri[value].xyz[1][1]) / 2;
			break;
		case 1:
			center_x = (tri[value].xyz[0][0] + tri[value].xyz[1][0]) / 2;
			center_y = (tri[value].xyz[0][1] + tri[value].xyz[1][1] + tri[value].xyz[2][1]) / 3;
			break;
		case 2:
			center_x = (tri[value].xyz[0][0] + tri[value].xyz[1][0] + tri[value].xyz[2][0]) / 3;
			center_y = (tri[value].xyz[0][1] + tri[value].xyz[1][1]) / 2;
			break;
		case 3:
			center_x = (tri[value].xyz[0][0] + tri[value].xyz[1][0]) / 2;
			center_y = (tri[value].xyz[0][1] + tri[value].xyz[1][1] + tri[value].xyz[2][1]) / 3;
			break;
		}
		float radius = sqrt(pow(center_x, 2) + pow(center_y, 2));
		float angle = atan2(center_y, center_x);
		switch (animation_mod) {
		case 0:
		{
			switch (tri[value].mod) {
			case 0:
				if (tri[value].dir) {
					center_x += 0.02f;
					center_y += 0.01f;
				}
				else {
					center_x -= 0.02f;
					center_y += 0.01f;
				}
				break;
			case 1:
				if (tri[value].dir) {
					center_x -= 0.01f;
					center_y += 0.02f;
				}
				else {
					center_x -= 0.01f;
					center_y -= 0.02f;
				}
				break;
			case 2:
				if (tri[value].dir) {
					center_x -= 0.02f;
					center_y -= 0.01f;
				}
				else {
					center_x += 0.02f;
					center_y -= 0.01f;
				}
				break;
			case 3:
				if (tri[value].dir) {
					center_x += 0.01f;
					center_y -= 0.02f;
				}
				else {
					center_x += 0.01f;
					center_y += 0.02f;
				}
				break;
			}
			if (change_dir(center_x, center_y) != 0) {
				center_x -= change_dir_x(center_x);
				center_y -= change_dir_y(center_y);
				if (tri[value].dir)  //반시계
					tri[value].mod = (tri[value].mod + 1) % 4;
				else
					tri[value].mod = (4 + tri[value].mod - 1) % 4;
			}
		}
		break;
		case 1:
		{
			switch (tri[value].mod) {
			case 0:
				center_y += 0.02f;
				tri[value].num++;
				if (change_dir_y(center_y)!=0) {
					center_y -= change_dir_y(center_y);
					tri[value].dir = 1;
				}
				if (tri[value].num >= 3) {
					if (center_x > 0)
						tri[value].mod = 1;
					else
						tri[value].mod = 3;
				}
				break;
			case 1:
				center_x -= 0.02f;
				if (change_dir_x(center_x) != 0) {
					center_x -= change_dir_x(center_x);
					if (tri[value].dir)
						tri[value].mod = 2;
					else
						tri[value].mod = 0;
					tri[value].num = 0;
				}
				break;
			case 2:
				center_y -= 0.02f;
				tri[value].num++;
				if (change_dir_y(center_y) != 0) {
					center_y -= change_dir_y(center_y);
					tri[value].dir = 0;
				}
				if (tri[value].num >= 3) {
					if (center_x > 0)
						tri[value].mod = 1;
					else
						tri[value].mod = 3;
				}
				break;
			case 3:
				center_x += 0.02f;
				if (change_dir_x(center_x) != 0) {
					center_x -= change_dir_x(center_x);
					if (tri[value].dir)
						tri[value].mod = 2;
					else
						tri[value].mod = 0;
					tri[value].num = 0;
				}
				break;
			}
		}
		break;
		case 2:
		{
			if (change_dir(center_x, center_y)) {
				center_x = 0.0f;
				center_y = 0.0f;
			}
			float len_x = 1.0f + center_x, len_y = 1.0f + center_y;
			switch (tri[value].mod) {
			case 0:
				center_y += 0.01f;
				if (tri[value].dir) { //반시계
					if (2.0f - len_y < 2.0f - len_x)
						tri[value].mod = (tri[value].mod + 1) % 4;
				}
				else {
					if (2.0f - len_y < len_x)
						tri[value].mod = (3 + tri[value].mod) % 4;
				}
				break;
			case 1:
				center_x -= 0.01f;
				if (tri[value].dir) { //반시계
					if (len_x < 2.0f - len_y)
						tri[value].mod = (tri[value].mod + 1) % 4;
				}
				else {
					if (len_x < len_y)
						tri[value].mod = (3 + tri[value].mod) % 4;
				}
				break;
			case 2:
				center_y -= 0.01f;
				if (tri[value].dir) { //반시계
					if (len_y < len_x)
						tri[value].mod = (tri[value].mod + 1) % 4;
				}
				else {
					if (len_y < 2.0f - len_x)
						tri[value].mod = (3 + tri[value].mod) % 4;
				}
				break;
			case 3:
				center_x += 0.01f;
				if (tri[value].dir) { //반시계
					if (2.0f - len_x < len_y)
						tri[value].mod = (tri[value].mod + 1) % 4;
				}
				else {
					if (2.0f - len_x < 2.0f - len_y)
						tri[value].mod = (3 + tri[value].mod) % 4;
				}
				break;
			}
		}
		break;
		case 3:
		{
			if (radius < 0.05f)
				tri[value].num = 0;
			else if (radius > 0.95f)
				tri[value].num = 1;
			if (tri[value].num)
				radius -= 0.01f;
			else
				radius += 0.01f;
			if (tri[value].dir) {
				angle -= 0.1f;
				center_x = radius * cos(angle);
				center_y = radius * sin(angle);
			}
			else {
				angle += 0.1f;
				center_x = radius * cos(angle);
				center_y = radius * sin(angle);
			}
			if (center_x < 0)
				tri[value].mod = 3;
			else
				tri[value].mod = 1;
		}
		break;
		}
		//방향에 맞게 회전
		switch (tri[value].mod) {
		case 0: // up
			tri[value].xyz[0][0] = center_x;
			tri[value].xyz[0][1] = center_y + 0.05f;
			tri[value].xyz[1][0] = center_x - 0.05f / 2;
			tri[value].xyz[1][1] = center_y - 0.05f;
			tri[value].xyz[2][0] = center_x + 0.05f / 2;
			tri[value].xyz[2][1] = center_y - 0.05f;
			break;
		case 1: // left
			tri[value].xyz[0][0] = center_x - 0.05f;
			tri[value].xyz[0][1] = center_y;
			tri[value].xyz[1][0] = center_x + 0.05f;
			tri[value].xyz[1][1] = center_y - 0.05f / 2;
			tri[value].xyz[2][0] = center_x + 0.05f;
			tri[value].xyz[2][1] = center_y + 0.05f / 2;
			break;
		case 2:	//down
			tri[value].xyz[0][0] = center_x;
			tri[value].xyz[0][1] = center_y - 0.05f;
			tri[value].xyz[1][0] = center_x + 0.05f / 2;
			tri[value].xyz[1][1] = center_y + 0.05f;
			tri[value].xyz[2][0] = center_x - 0.05f / 2;
			tri[value].xyz[2][1] = center_y + 0.05f;
			break;
		case 3:	// right
			tri[value].xyz[0][0] = center_x + 0.05f;
			tri[value].xyz[0][1] = center_y;
			tri[value].xyz[1][0] = center_x - 0.05f;
			tri[value].xyz[1][1] = center_y + 0.05f / 2;
			tri[value].xyz[2][0] = center_x - 0.05f;
			tri[value].xyz[2][1] = center_y - 0.05f / 2;
			break;
		}
		InitBuffer(value);
	}
	glutPostRedisplay();
	glutTimerFunc(100, animation, value);
}
