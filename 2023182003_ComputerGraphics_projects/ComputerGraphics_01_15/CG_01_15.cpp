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
std::uniform_real_distribution<GLclampf> rd_color(0.0f, 1.0f), rd_point(-1.0f, 1.0f);

GLuint coord_vao[3], hexa_vao, ebo[2], vbo[2];
GLfloat background_r = 0.2f, background_g = 0.2f, background_b = 0.2f, radian_x[2], radian_y[2], radian_one;
GLint width, height;
GLuint shaderProgramID;	//셰이더 프로그램 이름
GLuint vertexShader;	// 버텍스 셰이더 객체
GLuint fragmentShader;	// 프래그먼트 세이더 객체
GLUquadricObj* qobj;
BOOL rotate_x, rotate_y, revolution, shape_type;
int select;

char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
void InitBuffer();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid orbit(int value);
GLvoid rotation_x(int value);
GLvoid rotation_y(int value);

void main(int argc, char** argv)	// 윈도우 생성
{
	width = 800, height = 800;
	glutInit(&argc, argv);							// glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);	// 디스플레이 모드 설정
	glutInitWindowPosition(50, 50);					// 윈도우의 위치 지정
	glutInitWindowSize(width, height);					// 윈도우의 크기 지정
	glutCreateWindow("project_01_15");	// 윈도우 생성 (윈도우 이름)

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
	//8	
	{
		const GLfloat spots[8][3]{ {0.15f,0.15f,0.15f}, {-0.15f,0.15f,0.15f}, {0.15f,0.15f,-0.15f}, {-0.15f,0.15f,-0.15f}, {-0.15f,-0.15f,-0.15f}, {-0.15f,-0.15f,0.15f}, {0.15f,-0.15f,-0.15f}, {0.15f,-0.15f,0.15f} };
		const GLfloat colors[8][3]{ {rd_color(dre),rd_color(dre),rd_color(dre)}, {rd_color(dre),rd_color(dre),rd_color(dre)}, {rd_color(dre),rd_color(dre),rd_color(dre)}, {rd_color(dre),rd_color(dre),rd_color(dre)}, {rd_color(dre),rd_color(dre),rd_color(dre)}, {rd_color(dre),rd_color(dre),rd_color(dre)}, {rd_color(dre),rd_color(dre),rd_color(dre)}, {rd_color(dre),rd_color(dre),rd_color(dre)} };
		const int triangles[12][3]{ {0,1,3},{0,2,3},{0,2,6},{0,6,7},{1,0,7},{1,7,5},{5,3,1},{3,5,4},{2,3,4},{6,2,4},{4,5,6},{6,5,7} };
		glGenVertexArrays(1, &hexa_vao);
		glBindVertexArray(hexa_vao);
		glGenBuffers(2, vbo);
		glGenBuffers(2, ebo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), spots, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]); //--- GL_ELEMENT_ARRAY_BUFFER 버퍼 유형으로 바인딩
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[1]); //--- GL_ELEMENT_ARRAY_BUFFER 버퍼 유형으로 바인딩
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);
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
	glEnable(GL_DEPTH_TEST);	// 깊이 검사
	// x, y축 30도씩
	glm::mat4 Rx = glm::mat4(1.0f);
	glm::mat4 Ry = glm::mat4(1.0f);
	glm::mat4 R = glm::mat4(1.0f);
	Rx = glm::rotate(Rx, glm::radians(-30.0f), glm::vec3(1.0, 0.0, 0.0));
	Ry = glm::rotate(Ry, glm::radians(30.0f), glm::vec3(0.0, 1.0, 0.0));
	R = Rx * Ry;
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(R));
	glLineWidth(2);
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//x축
		glBindVertexArray(coord_vao[0]);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
		//y축
		glBindVertexArray(coord_vao[1]);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
		//z축
		glBindVertexArray(coord_vao[2]);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
	}
	// 좌	
	{
		glm::mat4 mRx = glm::mat4(1.0f);
		glm::mat4 mRy = glm::mat4(1.0f);
		glm::mat4 left = glm::mat4(1.0f);
		glm::mat4 T = glm::mat4(1.0f);
		T = glm::translate(T, glm::vec3(cos(radian_one) * 0.25, 0.0, sin(radian_one) * 0.25));
		mRx = glm::rotate(mRx, glm::radians(radian_x[1]), glm::vec3(1.0, 0.0, 0.0));
		mRy = glm::rotate(mRy, glm::radians(radian_y[1]), glm::vec3(0.0, 1.0, 0.0));
		left = R * T * mRx * mRy ;
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(left));

		qobj = gluNewQuadric();
		gluQuadricDrawStyle(qobj, GLU_LINE); // 도형 스타일
		gluQuadricNormals(qobj, GLU_SMOOTH); // 생략 가능
		gluQuadricOrientation(qobj, GLU_OUTSIDE); // 생략 가능
		if (shape_type)
			gluCylinder(qobj, 0.15, 0.0, 0.3, 20, 8);
		else 
			gluCylinder(qobj, 0.15, 0.15, 0.3, 20, 8);
	}
	// 우
	{
		glm::mat4 mRx = glm::mat4(1.0f);
		glm::mat4 mRy = glm::mat4(1.0f);
		glm::mat4 right = glm::mat4(1.0f);
		glm::mat4 T = glm::mat4(1.0f);
		T = glm::translate(T, glm::vec3(cos(radian_one+3.14) * 0.25, 0.0, sin(radian_one+3.14) * 0.25));
		mRx = glm::rotate(mRx, glm::radians(radian_x[0]), glm::vec3(1.0, 0.0, 0.0));
		mRy = glm::rotate(mRy, glm::radians(radian_y[0]), glm::vec3(0.0, 1.0, 0.0));
		right = R * T * mRx * mRy ;
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(right));

		if (shape_type) {
			qobj = gluNewQuadric();
			gluQuadricDrawStyle(qobj, GLU_LINE); // 도형 스타일
			gluQuadricNormals(qobj, GLU_SMOOTH); // 생략 가능
			gluQuadricOrientation(qobj, GLU_OUTSIDE); // 생략 가능
			gluSphere(qobj, 0.15, 50, 50); // 객체 만들기
		}
		else {
			glBindVertexArray(hexa_vao);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
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
	case 'x':
		if (!rotate_x) {
			rotate_x = true;
			glutTimerFunc(100, rotation_x, 1);
		}
		else
			rotate_x = false;
		break;
	case 'X':
		if (!rotate_x) {
			rotate_x = true;
			glutTimerFunc(100, rotation_x, -1);
		}
		else
			rotate_x = false;
		break;
	case 'y':
		if (!rotate_y) {
			rotate_y = true;
			glutTimerFunc(100, rotation_y, 1);
		}
		else
			rotate_y = false;
		break;
	case 'Y':
		if (!rotate_y) {
			rotate_y = true;
			glutTimerFunc(100, rotation_y, -1);
		}
		else
			rotate_y = false;
		break;
	case '1':
		select = 1;
		break;
	case '2':
		select = 2;
		break;
	case '3':
		select = 3;
		break;
	case 'r':
		if (!revolution) {
			revolution = true;
			glutTimerFunc(100, orbit, 1);
		}
		else
			revolution = false;
		break;
	case 'R':
		if (!revolution) {
			revolution = true;
			glutTimerFunc(100, orbit, -1);
		}
		else
			revolution = false;
		break;
	case 'c':
		if (shape_type)
			shape_type = false;
		else
			shape_type = true;
		break;
	case 's':
		radian_x[0] = 0.0f, radian_x[1] = 0.0f, radian_y[0] = 0.0f, radian_y[1] = 0.0f, radian_one = 0.0f;
		rotate_x = false, rotate_y = false, revolution = false, shape_type = false;
		select = 3;
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}
GLvoid orbit(int value)
{
	radian_one += value * 0.05f;
	glutPostRedisplay();
	if (revolution)
		glutTimerFunc(100, orbit, value);
}
GLvoid rotation_x(int value)
{
	if (select == 1) 
		radian_x[0] += value;
	else if (select == 2) 
		radian_x[1] += value;
	else {
		radian_x[0] += value;
		radian_x[1] += value;
	}
	glutPostRedisplay();
	if (rotate_x)
		glutTimerFunc(100, rotation_x, value);
}
GLvoid rotation_y(int value)
{
	if (select == 1)
		radian_y[0] += value;
	else if (select == 2)
		radian_y[1] += value;
	else {
		radian_y[0] += value;
		radian_y[1] += value;
	}
	glutPostRedisplay();
	if (rotate_y)
		glutTimerFunc(100, rotation_y, value);
}