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
std::uniform_real_distribution<GLclampf> rd_color(0.0f, 1.0f), rd_point(-1.0f, 1.0f);

GLuint vbo[2];
GLfloat background_r = 0.2f, background_g = 0.2f, background_b = 0.2f, prev_x, prev_y;
int make_type = 0, click_index = -1;
struct shape {
	GLuint vao;
	GLfloat spots[6][3], colors[6][3];
	int type;
	BOOL check;
};
struct shape sh[15];
GLint width, height;
GLuint shaderProgramID;	//셰이더 프로그램 이름
GLuint vertexShader;	// 버텍스 셰이더 객체
GLuint fragmentShader;	// 프래그먼트 세이더 객체
BOOL l_button;

char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
void InitBuffer(int index);
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
GLvoid move_spot(int value);
int index_in_shape(int i, GLfloat x, GLfloat y)
{
	switch (sh[i].type) {
	case 0:
		if (sh[i].spots[0][0] == x && sh[i].spots[0][1] == y)
			return i;
		break;
	case 1:
		if (sh[i].spots[1][0] <= x && x <= sh[i].spots[0][0] &&
			sh[i].spots[0][1] == y)
			return i;
		break;
	case 2:
		if (sh[i].spots[1][0] <= x && x <= sh[i].spots[2][0] &&
			sh[i].spots[1][1] <= y && y <= sh[i].spots[0][1])
			return i;
		break;
	case 3:
		if (sh[i].spots[0][0] <= x && x <= sh[i].spots[3][0] &&
			sh[i].spots[3][1] <= y && y <= sh[i].spots[0][1])
			return i;
		break;
	case 4:
		if (sh[i].spots[1][0] <= x && x <= sh[i].spots[2][0] &&
			sh[i].spots[4][1] <= y && y <= sh[i].spots[0][1])
			return i;
		break;
	case 5:
		if (sh[i].spots[3][0] <= x && x <= sh[i].spots[2][0] &&
			sh[i].spots[5][1] <= y && y <= sh[i].spots[0][1])
			return i;
		break;
	}
	return -1;
}
int pt_in_shape(GLfloat x, GLfloat y)
{
	for (int i = 0; i < 15; ++i) {
		if (sh[i].check) {
			if (index_in_shape(i, x, y) != -1)
				return index_in_shape(i, x, y);
		}
	}
	return -1;
}
int shape_in_shape(int index)
{
	for (int i = 0; i < 15; ++i) {
		if (i == index);
		else if (sh[i].check && sh[index].check) {
			for (int p = 0; p <= sh[index].type; ++p) {
				if (index_in_shape(i, sh[index].spots[p][0], sh[index].spots[p][1]) != -1)
					return index_in_shape(i, sh[index].spots[p][0], sh[index].spots[p][1]);
			}
			for (int p = 0; p <= sh[i].type; ++p) {
				if (index_in_shape(index, sh[i].spots[p][0], sh[i].spots[p][1]) != -1)
					return i;
			}
		}
	}
	return -1;
}
void make_newshape(int index, int type, GLfloat center_x, GLfloat center_y, GLfloat r, GLfloat g, GLfloat b) {
	sh[index].type = type;
	switch (sh[index].type) {
	case 0:
		sh[index].spots[0][0] = center_x;
		sh[index].spots[0][1] = center_y;
		sh[index].spots[0][2] = 0;
		break;
	case 1:
		sh[index].spots[0][0] = center_x + 0.05f;
		sh[index].spots[0][1] = center_y;
		sh[index].spots[0][2] = 0;
		sh[index].spots[1][0] = center_x - 0.05f;
		sh[index].spots[1][1] = center_y;
		sh[index].spots[1][2] = 0;
		break;
	case 2:
		sh[index].spots[0][0] = center_x;
		sh[index].spots[0][1] = center_y + 0.05f;
		sh[index].spots[0][2] = 0;
		sh[index].spots[1][0] = center_x - 0.05f;
		sh[index].spots[1][1] = center_y - 0.05f;
		sh[index].spots[1][2] = 0;
		sh[index].spots[2][0] = center_x + 0.05f;
		sh[index].spots[2][1] = center_y - 0.05f;
		sh[index].spots[2][2] = 0;
		break;
	case 3:
		sh[index].spots[0][0] = center_x - 0.05f;
		sh[index].spots[0][1] = center_y + 0.05f;
		sh[index].spots[0][2] = 0;
		sh[index].spots[1][0] = center_x - 0.05f;
		sh[index].spots[1][1] = center_y - 0.05f;
		sh[index].spots[1][2] = 0;
		sh[index].spots[2][0] = center_x + 0.05f;
		sh[index].spots[2][1] = center_y + 0.05f;
		sh[index].spots[2][2] = 0;
		sh[index].spots[3][0] = center_x + 0.05f;
		sh[index].spots[3][1] = center_y - 0.05f;
		sh[index].spots[3][2] = 0;
		break;
	case 4:
		sh[index].spots[0][0] = center_x;
		sh[index].spots[0][1] = center_y + 0.05f;
		sh[index].spots[0][2] = 0;
		sh[index].spots[1][0] = center_x - 0.05f;
		sh[index].spots[1][1] = center_y;
		sh[index].spots[1][2] = 0;
		sh[index].spots[2][0] = center_x + 0.05f;
		sh[index].spots[2][1] = center_y;
		sh[index].spots[2][2] = 0;
		sh[index].spots[3][0] = center_x - 0.03f;
		sh[index].spots[3][1] = center_y - 0.05f;
		sh[index].spots[3][2] = 0;
		sh[index].spots[4][0] = center_x + 0.03f;
		sh[index].spots[4][1] = center_y - 0.05f;
		sh[index].spots[4][2] = 0;
		break;
	case 5:
		sh[index].spots[0][0] = center_x + 0.025f;
		sh[index].spots[0][1] = center_y + 0.05f;
		sh[index].spots[0][2] = 0;
		sh[index].spots[1][0] = center_x - 0.025f;
		sh[index].spots[1][1] = center_y + 0.05f;
		sh[index].spots[1][2] = 0;
		sh[index].spots[2][0] = center_x + 0.05f;
		sh[index].spots[2][1] = center_y;
		sh[index].spots[2][2] = 0;
		sh[index].spots[3][0] = center_x - 0.05f;
		sh[index].spots[3][1] = center_y;
		sh[index].spots[3][2] = 0;
		sh[index].spots[4][0] = center_x + 0.025f;
		sh[index].spots[4][1] = center_y - 0.05f;
		sh[index].spots[4][2] = 0;
		sh[index].spots[5][0] = center_x - 0.025f;
		sh[index].spots[5][1] = center_y - 0.05f;
		sh[index].spots[5][2] = 0;
		break;
	}
	for (int c = 0; c <= type; ++c) {
		sh[index].colors[c][0] = r;
		sh[index].colors[c][1] = g;
		sh[index].colors[c][2] = b;
	}
	sh[index].check = true;
	InitBuffer(index);
}

void main(int argc, char** argv)	// 윈도우 생성
{
	width = 800, height = 800;
	glutInit(&argc, argv);							// glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);	// 디스플레이 모드 설정
	glutInitWindowPosition(50, 50);					// 윈도우의 위치 지정
	glutInitWindowSize(width, height);					// 윈도우의 크기 지정
	glutCreateWindow("project_01_12");	// 윈도우 생성 (윈도우 이름)

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

	for (int i = 0; i < 3; ++i) {
		make_newshape(5 * i, 0, rd_point(dre), rd_point(dre), rd_color(dre), rd_color(dre), rd_color(dre));
		make_newshape(5 * i + 1, 1, rd_point(dre), rd_point(dre), rd_color(dre), rd_color(dre), rd_color(dre));
		make_newshape(5 * i + 2, 2, rd_point(dre), rd_point(dre), rd_color(dre), rd_color(dre), rd_color(dre));
		make_newshape(5 * i + 3, 3, rd_point(dre), rd_point(dre), rd_color(dre), rd_color(dre), rd_color(dre));
		make_newshape(5 * i + 4, 4, rd_point(dre), rd_point(dre), rd_color(dre), rd_color(dre), rd_color(dre));
	}
	//make_newshape(0, 4, rd_point(dre), rd_point(dre), rd_color(dre), rd_color(dre), rd_color(dre));

	glutDisplayFunc(drawScene);						// 출력 함수의 지정
	glutReshapeFunc(Reshape);						// 다시 그리기 함수 지정
	glutKeyboardFunc(Keyboard);		// 키보드 입력 콜백함수 지정
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);

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
	glGenVertexArrays(1, &sh[index].vao);
	glBindVertexArray(sh[index].vao);
	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	switch (sh[index].type) {
	case 0:
		glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(GLfloat), sh[index].spots, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(GLfloat), sh[index].colors, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
		break;
	case 1:
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), sh[index].spots, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), sh[index].colors, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
		break;
	case 2:
		glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), sh[index].spots, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), sh[index].colors, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
		break;
	case 3:
		glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), sh[index].spots, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), sh[index].colors, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
		break;
	case 4:
		glBufferData(GL_ARRAY_BUFFER, 15 * sizeof(GLfloat), sh[index].spots, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, 15 * sizeof(GLfloat), sh[index].colors, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
		break;
	case 5:
		glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), sh[index].spots, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), sh[index].colors, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
		break;
	}
}
GLvoid drawScene()
{
	glClearColor(background_r, background_g, background_b, 1.0f);			// 바탕색을 파랑색으로 지정
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);					// 설정된 색으로 전체를 칠하기
	//shader 그리기----------------------
	glUseProgram(shaderProgramID);	// 활성화 한 번만 해주면 된다.
	for (int i = 0; i < 15; ++i) {
		if (sh[i].check) {
			glBindVertexArray(sh[i].vao);
			switch (sh[i].type) {
			case 0:	// 점
				glPointSize(5);
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				glDrawArrays(GL_POINTS, 0, sh[i].type + 1);
				break;
			case 1:	// 선
				glLineWidth(5);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDrawArrays(GL_LINE_STRIP, 0, sh[i].type + 1);
				break;
			case 2:	// 삼
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDrawArrays(GL_TRIANGLES, 0, sh[i].type + 1);
				break;
			case 3:	// 사
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, sh[i].type + 1);
				break;
			case 4:	// 오
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, sh[i].type + 1);
				break;
			case 5:	// 육
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, sh[i].type + 1);
				break;
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
	case '0':
		make_type = 0;
		break;
	case '1':
		make_type = 1;
		break;
	case '2':
		make_type = 2;
		break;
	case '3':
		make_type = 3;
		break;
	case '4':
		make_type = 4;
		break;
	case '5':
		make_type = 5;
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}
GLvoid Mouse(int button, int state, int x, int y) {
	GLfloat mouseX = static_cast<GLfloat>(x) / (static_cast<GLfloat>(width) / 2) - 1, mouseY = 1 - static_cast<GLfloat>(y) / (static_cast<GLfloat>(height) / 2);
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
		click_index = pt_in_shape(mouseX, mouseY);
		if (click_index != -1) {
			l_button = true;
			prev_x = mouseX;
			prev_y = mouseY;
		}
		else {
			for (int i = 0; i < 15; ++i) {
				if (sh[i].check == false) {
					make_newshape(i, make_type, mouseX, mouseY, rd_color(dre), rd_color(dre), rd_color(dre));
					break;
				}
			}
		}
	}
	else if (state == GLUT_UP) {
		if (l_button) {
			l_button = false;
			if (click_index == -1);
			else {
				int unite_index = shape_in_shape(click_index);
				if (unite_index == -1);
				else {
					GLfloat center_x = 0.0f, center_y = 0.0f;
					for (int i = 0; i <= sh[click_index].type; ++i) {
						center_x += sh[click_index].spots[i][0];
						center_y += sh[click_index].spots[i][1];
					}
					for (int i = 0; i <= sh[unite_index].type; ++i) {
						center_x += sh[unite_index].spots[i][0];
						center_y += sh[unite_index].spots[i][1];
					}
					center_x /= (sh[click_index].type + sh[unite_index].type + 2);
					center_y /= (sh[click_index].type + sh[unite_index].type + 2);
					make_newshape(click_index, (sh[click_index].type + sh[unite_index].type + 1) % 6, center_x, center_y, rd_color(dre), rd_color(dre), rd_color(dre));
					glutTimerFunc(100, move_spot, click_index);
					sh[unite_index].check = false;
				}
			}
			click_index = -1;
		}
	}
	glutPostRedisplay();
}
GLvoid Motion(int x, int y) {
	GLfloat mouseX = static_cast<GLfloat>(x) / (static_cast<GLfloat>(width) / 2) - 1, mouseY = 1 - static_cast<GLfloat>(y) / (static_cast<GLfloat>(height) / 2);
	if (l_button) {
		GLfloat x_size = mouseX - prev_x, y_size = mouseY - prev_y;
		for (int i = 0; i <= sh[click_index].type; ++i) {
			sh[click_index].spots[i][0] += x_size;
			sh[click_index].spots[i][1] += y_size;
		} 
		InitBuffer(click_index);
		std::cout << "x: " << x_size <<"y: " << y_size << std::endl;
		prev_x = mouseX;
		prev_y = mouseY;
		glutPostRedisplay();
	}
}
GLvoid move_spot(int value)
{
	GLfloat center_x = 0.0f, center_y = 0.0f;
	for (int i = 0; i <= sh[value].type; ++i) {
		center_x += sh[value].spots[i][0];
		center_y += sh[value].spots[i][1];
	}
	center_x /= (sh[value].type + 1);
	center_y /= (sh[value].type + 1);
	float radius = sqrt(pow(center_x, 2) + pow(center_y, 2));
	float angle = atan2(center_y, center_x) + 0.05f;
	for (int i = 0; i <= sh[value].type; ++i) {
		sh[value].spots[i][0] -= center_x - radius * cos(angle);
		sh[value].spots[i][1] -= center_y - radius * sin(angle);
		sh[value].colors[i][0] -= center_x - radius * cos(angle);
		sh[value].colors[i][1] -= center_x - radius * cos(angle);
		sh[value].colors[i][2] -= center_x - radius * cos(angle);
	}
	InitBuffer(value);
	// 도형 튕기기
	glutPostRedisplay();
	if (click_index != value)
		glutTimerFunc(100, move_spot, value);
}
