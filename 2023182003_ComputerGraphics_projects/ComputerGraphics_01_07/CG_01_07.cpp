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
std::uniform_int_distribution<int> uid(0, 9);
std::uniform_real_distribution<GLclampf> color(0.0f, 1.0f);

GLint width, height;
GLuint shaderProgramID;	//셰이더 프로그램 이름
GLuint vertexShader;	// 버텍스 셰이더 객체
GLuint fragmentShader;	// 프래그먼트 세이더 객체
int ShapeMod[10];	// 1->점, 2->선 3->삼각형 4->4각형;
GLuint vao[10], vbo[10][2];
int shapeNum = 0;

char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
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
	glutInitWindowSize(width, height);					// 윈도우의 크기 지정
	glutCreateWindow("project_01_07");	// 윈도우 생성 (윈도우 이름)

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
	glShaderSource(vertexShader, 1, (const GLchar**) & vertexSource, NULL);	//vertexShader하나 만들기
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
GLvoid drawScene()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);			// 바탕색을 파랑색으로 지정
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);					// 설정된 색으로 전체를 칠하기
	//shader 그리기----------------------
	glUseProgram(shaderProgramID);	// 활성화 한 번만 해주면 된다.
	for (int i = 0; i < shapeNum; ++i) {
		glBindVertexArray(vao[i]);
		glPointSize(5);
		glLineWidth(5);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		switch (ShapeMod[i]) {
		case 1:
			glDrawArrays(GL_POINTS, 0, 1);
			break;
		case 2:
			glDrawArrays(GL_LINES, 0, 2);
			break;
		case 3:
			glDrawArrays(GL_TRIANGLES, 0, 3);
			break;
		case 4:
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
			break;
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
	case 'p':
		for (int i = shapeNum; i < 10; ++i)
			ShapeMod[i] = 1;
		break;
	case 'l':
		for (int i = shapeNum; i < 10; ++i)
			ShapeMod[i] = 2;
		break;
	case 't':
		for (int i = shapeNum; i < 10; ++i) 
			ShapeMod[i] = 3;
		break;
	case 'r':
		for (int i = shapeNum; i < 10; ++i)
			ShapeMod[i] = 4;
		break;
	case 'w':
		if (shapeNum == 0) {
			std::cerr << "ERROR: there's no shape.\n" << std::endl;
		}
		else {
		int rand_index = uid(dre);
		while(rand_index >= shapeNum)
			rand_index = uid(dre);

		glBindVertexArray(vao[rand_index]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[rand_index][0]);

		GLfloat change[3] = {0.0, 0.05, 0.0};
		switch (ShapeMod[rand_index]) {
		case 1:
		{
			GLfloat* vPos = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
			for (int i = 0; i < 1; ++i) {
				vPos[0 + i * 3] += change[0];
				vPos[1 + i * 3] += change[1];
				vPos[2 + i * 3] += change[2];
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		break;
		case 2:
		{
			GLfloat* vPos = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
			for (int i = 0; i < 2; ++i) {
				vPos[0 + i * 3] += change[0];
				vPos[1 + i * 3] += change[1];
				vPos[2 + i * 3] += change[2];
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		break;
		case 3:
		{
			GLfloat* vPos = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
			for (int i = 0; i < 3; ++i) {
				vPos[0 + i * 3] += change[0];
				vPos[1 + i * 3] += change[1];
				vPos[2 + i * 3] += change[2];
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		break;
		case 4:
		{
			GLfloat* vPos = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
			for (int i = 0; i < 6; ++i) {
				vPos[0 + i * 3] += change[0];
				vPos[1 + i * 3] += change[1];
				vPos[2 + i * 3] += change[2];
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		break;
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
		break;
	case 'a':
		if (shapeNum == 0) {
			std::cerr << "ERROR: there's no shape.\n" << std::endl;
		}
		else {
		int rand_index = uid(dre);
		while (rand_index >= shapeNum)
			rand_index = uid(dre);

		glBindVertexArray(vao[rand_index]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[rand_index][0]);

		GLfloat change[3] = { -0.05, 0.0, 0.0 };
		switch (ShapeMod[rand_index]) {
		case 1:
		{
			GLfloat* vPos = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
			for (int i = 0; i < 1; ++i) {
				vPos[0 + i * 3] += change[0];
				vPos[1 + i * 3] += change[1];
				vPos[2 + i * 3] += change[2];
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		break;
		case 2:
		{
			GLfloat* vPos = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
			for (int i = 0; i < 2; ++i) {
				vPos[0 + i * 3] += change[0];
				vPos[1 + i * 3] += change[1];
				vPos[2 + i * 3] += change[2];
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		break;
		case 3:
		{
			GLfloat* vPos = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
			for (int i = 0; i < 3; ++i) {
				vPos[0 + i * 3] += change[0];
				vPos[1 + i * 3] += change[1];
				vPos[2 + i * 3] += change[2];
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		break;
		case 4:
		{
			GLfloat* vPos = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
			for (int i = 0; i < 6; ++i) {
				vPos[0 + i * 3] += change[0];
				vPos[1 + i * 3] += change[1];
				vPos[2 + i * 3] += change[2];
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		break;
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
		break;
	case 's':
		if (shapeNum == 0) {
			std::cerr << "ERROR: there's no shape.\n" << std::endl;
		}
		else {
		int rand_index = uid(dre);
		while (rand_index >= shapeNum)
			rand_index = uid(dre);

		glBindVertexArray(vao[rand_index]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[rand_index][0]);

		GLfloat change[3] = { 0.0, -0.05, 0.0 };
		switch (ShapeMod[rand_index]) {
		case 1:
		{
			GLfloat* vPos = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
			for (int i = 0; i < 1; ++i) {
				vPos[0 + i * 3] += change[0];
				vPos[1 + i * 3] += change[1];
				vPos[2 + i * 3] += change[2];
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		break;
		case 2:
		{
			GLfloat* vPos = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
			for (int i = 0; i < 2; ++i) {
				vPos[0 + i * 3] += change[0];
				vPos[1 + i * 3] += change[1];
				vPos[2 + i * 3] += change[2];
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		break;
		case 3:
		{
			GLfloat* vPos = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
			for (int i = 0; i < 3; ++i) {
				vPos[0 + i * 3] += change[0];
				vPos[1 + i * 3] += change[1];
				vPos[2 + i * 3] += change[2];
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		break;
		case 4:
		{
			GLfloat* vPos = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
			for (int i = 0; i < 6; ++i) {
				vPos[0 + i * 3] += change[0];
				vPos[1 + i * 3] += change[1];
				vPos[2 + i * 3] += change[2];
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		break;
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
		break;
	case 'd':
		if (shapeNum == 0) {
			std::cerr << "ERROR: there's no shape.\n" << std::endl;
		}
		else {
		int rand_index = uid(dre);
		while (rand_index >= shapeNum)
			rand_index = uid(dre);

		glBindVertexArray(vao[rand_index]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[rand_index][0]);

		GLfloat change[3] = { 0.05, 0.0, 0.0 };
		switch (ShapeMod[rand_index]) {
		case 1:
		{
			GLfloat* vPos = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
			for (int i = 0; i < 1; ++i) {
				vPos[0 + i * 3] += change[0];
				vPos[1 + i * 3] += change[1];
				vPos[2 + i * 3] += change[2];
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		break;
		case 2:
		{
			GLfloat* vPos = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
			for (int i = 0; i < 2; ++i) {
				vPos[0 + i * 3] += change[0];
				vPos[1 + i * 3] += change[1];
				vPos[2 + i * 3] += change[2];
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		break;
		case 3:
		{
			GLfloat* vPos = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
			for (int i = 0; i < 3; ++i) {
				vPos[0 + i * 3] += change[0];
				vPos[1 + i * 3] += change[1];
				vPos[2 + i * 3] += change[2];
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		break;
		case 4:
		{
			GLfloat* vPos = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
			for (int i = 0; i < 6; ++i) {
				vPos[0 + i * 3] += change[0];
				vPos[1 + i * 3] += change[1];
				vPos[2 + i * 3] += change[2];
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		break;
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
		break;
	case 'c':
		while (shapeNum != 0) {
			shapeNum--;
			ShapeMod[shapeNum] = 0;
		}
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}
GLvoid Mouse(int button, int state, int x, int y) {
	if (ShapeMod[shapeNum] == 0) {
		std::cerr << "ERROR: set shape\n" << std::endl;
	}
	else if (shapeNum < 10 && state == GLUT_DOWN && button == GLUT_LEFT) {
		GLclampf mouseX = static_cast<GLclampf>(x) / (static_cast<GLclampf>(width) / 2) - 1, mouseY = 1 - static_cast<GLclampf>(y) / (static_cast<GLclampf>(height) / 2);
		const GLfloat set_color[3] = { color(dre), color(dre), color(dre) };
		const GLfloat colors[6][3] = { { set_color[0], set_color[1], set_color[2] }, { set_color[0], set_color[1], set_color[2] }, { set_color[0], set_color[1], set_color[2] }, { set_color[0], set_color[1], set_color[2] }, { set_color[0], set_color[1], set_color[2] }, { set_color[0], set_color[1], set_color[2] } };
		switch (ShapeMod[shapeNum]) {
		case 1:		// 점 ㅋ
		{
			const GLfloat spotShape[1][3] = { {mouseX, mouseY, 0.0} };
			glGenVertexArrays(1, &vao[shapeNum]);
			glBindVertexArray(vao[shapeNum]);
			glGenBuffers(2, vbo[shapeNum]);

			glBindBuffer(GL_ARRAY_BUFFER, vbo[shapeNum][0]);
			glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(GLfloat), spotShape, GL_STREAM_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[shapeNum][1]);
			glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
		break;
		case 2:		// 선 ㅋ
		{
			const GLfloat lineShape[2][3] = { {mouseX - 0.05, mouseY, 0.0}, {mouseX + 0.05, mouseY, 0.0} };
			glGenVertexArrays(1, &vao[shapeNum]);
			glBindVertexArray(vao[shapeNum]);
			glGenBuffers(2, vbo[shapeNum]);

			glBindBuffer(GL_ARRAY_BUFFER, vbo[shapeNum][0]);
			glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), lineShape, GL_STREAM_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0); //
			glBindBuffer(GL_ARRAY_BUFFER, vbo[shapeNum][1]);
			glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
		break;
		case 3:		// 삼각형 ㅋ
		{
			const GLfloat triShape[3][3] = { {mouseX, mouseY + 0.05, 0.0}, {mouseX - 0.05, mouseY - 0.05, 0.0}, {mouseX + 0.05, mouseY - 0.05, 0.0} };
			glGenVertexArrays(1, &vao[shapeNum]);
			glBindVertexArray(vao[shapeNum]);
			glGenBuffers(2, vbo[shapeNum]);

			glBindBuffer(GL_ARRAY_BUFFER, vbo[shapeNum][0]);
			glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), triShape, GL_STREAM_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[shapeNum][1]);
			glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
		break;
		case 4:		// 사각형 ㅋ
		{
			const GLfloat quadShape[6][3] = { {mouseX - 0.05, mouseY + 0.05, 0.0}, {mouseX + 0.05, mouseY + 0.05, 0.0}, { mouseX - 0.05, mouseY - 0.05, 0.0 }, {mouseX + 0.05, mouseY + 0.05, 0.0}, { mouseX - 0.05, mouseY - 0.05, 0.0 }, {mouseX + 0.05, mouseY - 0.05, 0.0} };
			glGenVertexArrays(1, &vao[shapeNum]);
			glBindVertexArray(vao[shapeNum]);
			glGenBuffers(2, vbo[shapeNum]);

			glBindBuffer(GL_ARRAY_BUFFER, vbo[shapeNum][0]);
			glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), quadShape, GL_STREAM_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[shapeNum][1]);
			glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
		break;
		}
		make_shaderProgram();

		shapeNum++;
	}
}
