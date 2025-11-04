#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <random>
#include <gl\glew.h>
#include <gl\freeglut.h>
#include <gl\freeglut_ext.h>
#include <gl\glm\glm\glm.hpp>
#include <gl\glm\glm\ext.hpp>
#include <gl\glm\glm\gtc\matrix_transform.hpp>

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_real_distribution<GLclampf> rd_speed(1.0f, 3.0f);

GLuint vbo[2];
GLfloat background_r = 0.2f, background_g = 0.2f, background_b = 0.2f, y_radian, z_radian, x_move, y_move, z_move;
GLint width, height, r_open;
GLuint shaderProgramID;	// 셰이더 프로그램 이름
GLuint vertexShader;	// 버텍스 셰이더 객체
GLuint fragmentShader;	// 프래그먼트 세이더 객체
BOOL m_switch, o_switch, r_switch, p_switch;
int y_switch, z_switch;
struct Planet {
	GLuint vao;
	GLfloat r, d, center[3], speed = rd_speed(dre), radian;
};
Planet solar, planet[3], moon[3];

char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
void InitBuffer();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid anime(int value);

void main(int argc, char** argv)	// 윈도우 생성
{
	width = 800, height = 800;
	glutInit(&argc, argv);							// glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);	// 디스플레이 모드 설정
	glutInitWindowPosition(50, 50);					// 윈도우의 위치 지정
	glutInitWindowSize(width, height);					// 윈도우의 크기 지정
	glutCreateWindow("project_01_18");	// 윈도우 생성 (윈도우 이름)

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
	glutTimerFunc(10, anime, 1);

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
	solar.r = 0.1f;
	for (int i = 0; i < 3; ++i) {
		{
			GLfloat road[180][3];
			for (int j = 0; j < 180; ++j) {
				road[j][0] = 0.6f * glm::cos(glm::radians(2.0f * j));
				road[j][1] = 0.6f * glm::cos(glm::radians(2.0f * j )) * (1 - i);
				road[j][2] = 0.6f * glm::sin(glm::radians(2.0f * j));
			}
			planet[i].r = 0.05f, planet[i].d = 0.6f;
			planet[i].center[0] = 0.6f * glm::cos(glm::radians(0.0f));
			planet[i].center[1] = 0.6f * glm::cos(glm::radians(0.0f)) * (1 - i);
			planet[i].center[2] = 0.6f * glm::sin(glm::radians(0.0f));

			glGenVertexArrays(1, &planet[i].vao);
			glBindVertexArray(planet[i].vao);
			glGenBuffers(2, vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			glBufferData(GL_ARRAY_BUFFER, 540 * sizeof(GLfloat), road, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
		} {
			GLfloat road[90][3];
			for (int j = 0; j < 90; ++j) {
				road[j][0] = 0.2f * glm::cos(glm::radians(4.0f * j));
				road[j][1] = 0.0f;
				road[j][2] = 0.2f * glm::sin(glm::radians(4.0f * j));
			}
			moon[i].r = 0.02f, moon[i].d = 0.2f;
			moon[i].center[0] = 0.2f * glm::cos(glm::radians(0.0f));
			moon[i].center[1] = 0.2f * glm::cos(glm::radians(0.0f)) * (1 - i);
			moon[i].center[2] = 0.2f * glm::sin(glm::radians(0.0f));

			glGenVertexArrays(1, &moon[i].vao);
			glBindVertexArray(moon[i].vao);
			glGenBuffers(2, vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			glBufferData(GL_ARRAY_BUFFER, 270 * sizeof(GLfloat), road, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
		}
	}
}
GLvoid drawScene()
{
	glClearColor(background_r, background_g, background_b, 1.0f);			// 바탕색을 파랑색으로 지정
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);					// 설정된 색으로 전체를 칠하기
	//shader 그리기----------------------
	glUseProgram(shaderProgramID);	// 활성화 한 번만 해주면 된다.

	glEnable(GL_DEPTH_TEST);

	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 R = glm::mat4(1.0f);	// 축 회전
	R = glm::rotate(R, glm::radians(05.0f), glm::vec3(1.0, 0.0, 0.0));


	if (p_switch) {	// 직각 투영 : [-2.0, 2.0]glUseProgram (shaderProgram);
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -1.0f); //--- 카메라 위치
		glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform"); //--- 뷰잉 변환 설정
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
		projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
	}
	else {// 원근 투영 : fovy 45.0, near 0.1, far 50.0
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
		glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform"); //--- 뷰잉 변환 설정
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
		projection = glm::perspective(glm::radians(45.0f), -1.0f, 0.1f, 50.0f);
	}
	unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform"); //--- 투영 변환 값 설정
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 Ry = glm::mat4(1.0f);
	glm::mat4 Rz = glm::mat4(1.0f);
	Ry = glm::rotate(Ry, glm::radians(y_radian), glm::vec3(0.0, 1.0, 0.0));
	Rz = glm::rotate(Rz, glm::radians(z_radian), glm::vec3(0.0, 0.0, 1.0));

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (int i = 0; i < 3; ++i) {
		glm::mat4 color = glm::mat4(1.0f);
		color = glm::translate(color, glm::vec3(0.0f, 0.0f, 0.0f));
		unsigned int colorLocation = glGetUniformLocation(shaderProgramID, "setColor");
		glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(color));

		glm::mat4 move = glm::mat4(1.0f);
		move = glm::translate(move, glm::vec3(x_move, y_move, z_move));
		{
			model = R * Ry * Rz * move;
			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

			glBindVertexArray(planet[i].vao);
			glDrawArrays(GL_LINE_STRIP, 0, 180);
		} {
			move = glm::translate(move, glm::vec3(planet[i].center[0], planet[i].center[1], planet[i].center[2]));
			model = R * Ry * Rz * move;
			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));


			glBindVertexArray(moon[i].vao);
			glDrawArrays(GL_LINE_STRIP, 0, 90);
		}
	}
	if (m_switch)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	{
		glm::mat4 color = glm::mat4(1.0f);
		color = glm::translate(color, glm::vec3(0.0f, -1.0f, -1.0f));
		unsigned int colorLocation = glGetUniformLocation(shaderProgramID, "setColor");
		glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(color));

		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		gluQuadricNormals(qobj, GLU_SMOOTH); // 생략 가능
		gluQuadricOrientation(qobj, GLU_OUTSIDE); // 생략 가능
		glm::mat4 move = glm::mat4(1.0f);
		move = glm::translate(move, glm::vec3(x_move, y_move, z_move));
		model = R * Ry * Rz * move;
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
		
		gluSphere(qobj, solar.r, 20, 20);
	}
	for (int i = 0; i < 3; ++i) {
		glm::mat4 move = glm::mat4(1.0f);
		move = glm::translate(move, glm::vec3(x_move, y_move, z_move));
		{
			glm::mat4 color = glm::mat4(1.0f);
			color = glm::translate(color, glm::vec3(-1.0f, 0.0f, -1.0f));
			unsigned int colorLocation = glGetUniformLocation(shaderProgramID, "setColor");
			glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(color));

			GLUquadricObj* qobj;
			qobj = gluNewQuadric(); 
			gluQuadricNormals(qobj, GLU_SMOOTH); // 생략 가능
			gluQuadricOrientation(qobj, GLU_OUTSIDE); // 생략 가능
			move = glm::translate(move, glm::vec3(planet[i].center[0], planet[i].center[1], planet[i].center[2]));
			model = R * Ry * Rz * move;
			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));


			gluSphere(qobj, planet[i].r, 20, 20);
		} {
			glm::mat4 color = glm::mat4(1.0f);
			color = glm::translate(color, glm::vec3(-1.0f, -1.0f, 0.0f));
			unsigned int colorLocation = glGetUniformLocation(shaderProgramID, "setColor");
			glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(color));

			GLUquadricObj* qobj;
			qobj = gluNewQuadric();
			gluQuadricNormals(qobj, GLU_SMOOTH); // 생략 가능
			gluQuadricOrientation(qobj, GLU_OUTSIDE); // 생략 가능
			move = glm::translate(move, glm::vec3(moon[i].center[0], moon[i].center[1], moon[i].center[2]));
			model = R * Ry * Rz * move;
			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

			gluSphere(qobj, moon[i].r, 20, 20);
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
	case 'm':
		m_switch = false;
		break;
	case 'M':
		m_switch = true;
		break;
	case 'y':
		if (y_switch == 1)
			y_switch = 0;
		else
			y_switch += 1;
		break;
	case 'Y':
		if (y_switch == -1)
			y_switch = 0;
		else
			y_switch -= 1;
		break;
	case 'z':
		if (z_switch == 1)
			z_switch = 0;
		else
			z_switch += 1;
		break;
	case 'Z':
		if (z_switch == -1)
			z_switch = 0;
		else
			z_switch -= 1;
		break;
	case 'o':
		o_switch = !o_switch;
		break;
	case 'r':
		r_switch = !r_switch;
		break;
	case 'w':
		y_move += 0.1f;
		break;
	case 'a':
		x_move += 0.1f;
		break;
	case 's':
		y_move -= 0.1f;
		break;
	case 'd':
		x_move -= 0.1f;
		break;
	case '-':
		z_move -= 0.1f;
		break;
	case '+':
		z_move += 0.1f;
		break;
	case 'p':
		p_switch = !p_switch;
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}
GLvoid anime(int value)
{
	if (y_switch !=0) {
		y_radian += y_switch;
	}
	else if (z_switch != 0) {
		z_radian += z_switch;
	}
	for (int i = 0; i < 3; ++i) {
		planet[i].radian += planet[i].speed;
		planet[i].center[0] = 0.6f * glm::cos(glm::radians(planet[i].radian));
		planet[i].center[1] = 0.6f * glm::cos(glm::radians(planet[i].radian)) * (1 - i);
		planet[i].center[2] = 0.6f * glm::sin(glm::radians(planet[i].radian));

		moon[i].radian += moon[i].speed;
		moon[i].center[0] = 0.2f * glm::cos(glm::radians(moon[i].radian));
		moon[i].center[1] = 0.0f;
		moon[i].center[2] = 0.2f * glm::sin(glm::radians(moon[i].radian));
	}

	glutPostRedisplay();
	glutTimerFunc(10, anime, value);
}
