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
std::uniform_real_distribution<GLclampf> rd_color(0.0f, 1.0f);

GLuint VAO, VBO[2];
GLfloat background_r = 0.2f, background_g = 0.2f, background_b = 0.2f, y_radian, light_color[3] = { rd_color(dre),rd_color(dre) ,rd_color(dre) };
GLint width, height, y_dir;
GLuint shaderProgramID;	// 셰이더 프로그램 이름
GLuint vertexShader;	// 버텍스 셰이더 객체
GLuint fragmentShader;	// 프래그먼트 세이더 객체
typedef struct Planet {
	GLfloat color[3], pos[3], radian;
}Planet;
Planet white{ {1.0f,1.0f,1.0f},{0.75f, 0.0f,0.0f,}, 0.1f }, solar{ {1.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},0.15f }, planet{ {0.0f,1.0f,0.0f},{-0.5f,0.0f,0.0f},0.1f }, moon{ {0.0f,0.0f,1.0f},{-0.8f,0.0f,0.0f},0.05f };

char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
void InitBuffer();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid anime(int value);

float vertexPosition[] = {
	-1.0f,1.0f,1.0f, //앞면
	-1.0f,-1.0f,1.0f,
	 1.0f,-1.0f,1.0f,
	 1.0f,1.0f,1.0f,

   -1.0f,1.0f,-1.0f, //윗면
  -1.0f,1.0f,1.0f,
   1.0f,1.0f,1.0f,
   1.0f,1.0f,-1.0f,

   -1.0f,1.0f,-1.0f, //왼쪽옆
   -1.0f,-1.0f,-1.0f,
   -1.0f,-1.0f,1.0f,
   -1.0f,1.0f,1.0f,

   1.0f,1.0f,-1.0f, //뒷면
   1.0f,-1.0f,-1.0f,
   -1.0f,-1.0f,-1.0f,
   -1.0f,1.0f,-1.0f,

   -1.0f,-1.0f,1.0f, //아랫면
   -1.0f,-1.0f,-1.0f,
   1.0f,-1.0f,-1.0f,
   1.0f,-1.0f,1.0f,

   1.0f,1.0f,1.0f, //오른쪽 옆
   1.0f,-1.0f,1.0f,
   1.0f,-1.0f,-1.0f,
   1.0f,1.0f,-1.0f
};//정육면체

void main(int argc, char** argv)	// 윈도우 생성
{
	width = 800, height = 800;
	glutInit(&argc, argv);							// glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);	// 디스플레이 모드 설정
	glutInitWindowPosition(50, 50);					// 윈도우의 위치 지정
	glutInitWindowSize(width, height);					// 윈도우의 크기 지정
	glutCreateWindow("project_01_25");	// 윈도우 생성 (윈도우 이름)

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

	vertexSource = filetobuf("vertexShaderSource.glsl");	//text파일
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

	fragmentSource = filetobuf("fragmentShaderSource.glsl");
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
	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO); 

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexPosition), vertexPosition, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexPosition), vertexPosition, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}
GLvoid drawScene()
{
	glClearColor(background_r, background_g, background_b, 1.0f);			// 바탕색을 파랑색으로 지정
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);					// 설정된 색으로 전체를 칠하기
	//shader 그리기----------------------
	glUseProgram(shaderProgramID);	// 활성화 한 번만 해주면 된다.

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	unsigned int sizeLocation = glGetUniformLocation(shaderProgramID, "sizeTransform");
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");//월드 변환 행렬값을 셰이더의 uniform mat4 modelTransform에게 넘겨줌
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");//위와 동일
	unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");//위와 동일

	//원근 투영
	glm::mat4 kTransform = glm::mat4(1.0f);
	kTransform = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
	kTransform = glm::translate(kTransform, glm::vec3(0.0, 0.0, -2.0f));
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &kTransform[0][0]);

	//뷰잉 변환
	glm::mat4 vTransform = glm::mat4(1.0f);
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f); //--- 카메라 위치
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향

	vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);

	unsigned int lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	unsigned int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	unsigned int objColorLocation = glGetUniformLocation(shaderProgramID, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
	glUniform3f(lightPosLocation, 1.2f * sin(glm::radians(y_radian)), 0.0f, 1.2f * cos(glm::radians(y_radian)));
	glUniform3f(lightColorLocation, light_color[0], light_color[1], light_color[2]);

	{
		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		gluQuadricNormals(qobj, GLU_SMOOTH); // 생략 가능
		gluQuadricOrientation(qobj, GLU_OUTSIDE); // 생략 가능

		glm::mat4 size = glm::mat4(1.0f);//변환 행렬 생성 T
		size = glm::scale(size, glm::vec3(solar.radian, solar.radian, solar.radian));
		glUniformMatrix4fv(sizeLocation, 1, GL_FALSE, glm::value_ptr(size));//변환 행렬을 셰이더에 전달

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(solar.pos[0], solar.pos[1], solar.pos[2]));//이동
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));//변환 행렬을 셰이더에 전달

		glUniform3f(objColorLocation, solar.color[0], solar.color[1], solar.color[2]);

		gluSphere(qobj, 1.0f, 20, 20);
	} {
		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		gluQuadricNormals(qobj, GLU_SMOOTH); // 생략 가능
		gluQuadricOrientation(qobj, GLU_OUTSIDE); // 생략 가능

		glm::mat4 size = glm::mat4(1.0f);//변환 행렬 생성 T
		size = glm::scale(size, glm::vec3(planet.radian, planet.radian, planet.radian));
		glUniformMatrix4fv(sizeLocation, 1, GL_FALSE, glm::value_ptr(size));//변환 행렬을 셰이더에 전달

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(planet.pos[0], planet.pos[1], planet.pos[2]));//이동
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));//변환 행렬을 셰이더에 전달

		glUniform3f(objColorLocation, planet.color[0], planet.color[1], planet.color[2]);

		gluSphere(qobj, 1.0f, 20, 20);
	} {
		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		gluQuadricNormals(qobj, GLU_SMOOTH); // 생략 가능
		gluQuadricOrientation(qobj, GLU_OUTSIDE); // 생략 가능

		glm::mat4 size = glm::mat4(1.0f);//변환 행렬 생성 T
		size = glm::scale(size, glm::vec3(moon.radian, moon.radian, moon.radian));
		glUniformMatrix4fv(sizeLocation, 1, GL_FALSE, glm::value_ptr(size));//변환 행렬을 셰이더에 전달

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(moon.pos[0], moon.pos[1], moon.pos[2]));//이동
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));//변환 행렬을 셰이더에 전달

		glUniform3f(objColorLocation, moon.color[0], moon.color[1], moon.color[2]);

		gluSphere(qobj, 1.0f, 20, 20);
	} {
		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		gluQuadricNormals(qobj, GLU_SMOOTH); // 생략 가능
		gluQuadricOrientation(qobj, GLU_OUTSIDE); // 생략 가능

		glm::mat4 size = glm::mat4(1.0f);//변환 행렬 생성 T
		size = glm::scale(size, glm::vec3(white.radian, white.radian, white.radian));
		glUniformMatrix4fv(sizeLocation, 1, GL_FALSE, glm::value_ptr(size));//변환 행렬을 셰이더에 전달

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(white.pos[0], white.pos[1], white.pos[2]));//이동
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));//변환 행렬을 셰이더에 전달

		glUniform3f(objColorLocation, white.color[0], white.color[1], white.color[2]);

		gluSphere(qobj, 1.0f, 20, 20);
	}/*조명*/ {
		glBindVertexArray(VAO);
		glUniform3f(objColorLocation, 1.0, 1.0, 1.0);

		glm::mat4 size = glm::mat4(1.0f);//변환 행렬 생성 T
		size = glm::rotate(size, glm::radians(-180.0f), glm::vec3(0.0, 1.0, 0.0));//y축 공전
		size = glm::translate(size, glm::vec3(0.0f, 0.0f, 1.0f));//이동
		size = glm::scale(size, glm::vec3(0.02, 0.01, 0.02));
		glUniformMatrix4fv(sizeLocation, 1, GL_FALSE, glm::value_ptr(size));//변환 행렬을 셰이더에 전달

		glm::mat4 shapeTransForm = glm::mat4(1.0f);//변환 행렬 생성 T
		shapeTransForm = glm::rotate(shapeTransForm, glm::radians(y_radian + 180.0f), glm::vec3(0.0, 1.0, 0.0));//y축 공전
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(shapeTransForm));//변환 행렬을 셰이더에 전달

		glDrawArrays(GL_QUADS, 0, 24); //정육면체
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
	case 'c':
		for (int i = 0; i < 3; ++i)
			light_color[i] = rd_color(dre);
		break;
	case 'r':
		if (y_dir == 1)
			y_dir = 0;
		else
			y_dir = 1;
		break;
	case 'R':
		if (y_dir == -1)
			y_dir = 0;
		else
			y_dir = -1;
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}
GLvoid anime(int value)
{
	y_radian += y_dir;

	glutPostRedisplay();
	glutTimerFunc(10, anime, value);
}
