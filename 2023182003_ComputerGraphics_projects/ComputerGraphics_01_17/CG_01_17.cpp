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

GLuint coord_vao[3], hexa[6], penta[5], vbo[2];
GLfloat background_r = 0.2f, background_g = 0.2f, background_b = 0.2f, size = 0.5f
, radian_y, t_radian, f_radian, s_slice, b_size = 1.0f, o_radian, r_radian;
GLint width, height, r_open;
GLuint shaderProgramID;	// 셰이더 프로그램 이름
GLuint vertexShader;	// 버텍스 셰이더 객체
GLuint fragmentShader;	// 프래그먼트 세이더 객체
BOOL print_polygon, h_switch = true, y_switch, t_switch, f_switch, s_switch, b_switch, o_switch, r_switch, p_switch;

char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
void InitBuffer();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid y_anime(int value);
GLvoid t_anime(int value);
GLvoid f_anime(int value);
GLvoid s_anime(int value);
GLvoid b_anime(int value);
GLvoid o_anime(int value);
GLvoid r_anime(int value);

void main(int argc, char** argv)	// 윈도우 생성
{
	width = 800, height = 800;
	glutInit(&argc, argv);							// glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);	// 디스플레이 모드 설정
	glutInitWindowPosition(50, 50);					// 윈도우의 위치 지정
	glutInitWindowSize(width, height);					// 윈도우의 크기 지정
	glutCreateWindow("project_01_17");	// 윈도우 생성 (윈도우 이름)

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
	} {
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
	} {
		//z
		glGenVertexArrays(1, &coord_vao[2]);
		glBindVertexArray(coord_vao[2]);
		glGenBuffers(2, vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		const GLfloat z_coord[2][3]{ {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f} };
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), z_coord, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		const GLfloat z_color[2][3]{ { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } };
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), z_color, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}
	// 8
	{
		const GLfloat spot_8[8][3]{ {size,size,size}, {-size,size,size}, {size,size,-size}, {-size,size,-size}, {-size,-size,-size}, {-size,-size,size}, {size,-size,-size}, {size,-size,size} };
		const GLfloat color_8[8][3]{ {rd_color(dre),rd_color(dre),rd_color(dre)}, {rd_color(dre),rd_color(dre),rd_color(dre)}, {rd_color(dre),rd_color(dre),rd_color(dre)}, {rd_color(dre),rd_color(dre),rd_color(dre)}, {rd_color(dre),rd_color(dre),rd_color(dre)}, {rd_color(dre),rd_color(dre),rd_color(dre)}, {rd_color(dre),rd_color(dre),rd_color(dre)}, {rd_color(dre),rd_color(dre),rd_color(dre)} };
		const int triangles[6][4]{ {0,1,2,3},{2,0,6,7},{0,1,7,5},{1,3,5,4},{3,2,4,6},{5,4,7,6} };
		for (int i = 0; i < 6; ++i) {
			GLfloat spots[4][3]{ 
				{ spot_8[triangles[i][0]][0],spot_8[triangles[i][0]][1],spot_8[triangles[i][0]][2] },
				{ spot_8[triangles[i][1]][0],spot_8[triangles[i][1]][1],spot_8[triangles[i][1]][2] },
				{ spot_8[triangles[i][2]][0],spot_8[triangles[i][2]][1],spot_8[triangles[i][2]][2] },
				{ spot_8[triangles[i][3]][0],spot_8[triangles[i][3]][1],spot_8[triangles[i][3]][2] } },
				colors[4][3]{ 
				{ color_8[triangles[i][0]][0],color_8[triangles[i][0]][1],color_8[triangles[i][0]][2] },
				{ color_8[triangles[i][1]][0],color_8[triangles[i][1]][1],color_8[triangles[i][1]][2] },
				{ color_8[triangles[i][2]][0],color_8[triangles[i][2]][1],color_8[triangles[i][2]][2] },
				{ color_8[triangles[i][3]][0],color_8[triangles[i][3]][1],color_8[triangles[i][3]][2] } };
			glGenVertexArrays(1, &hexa[i]);
			glBindVertexArray(hexa[i]);
			glGenBuffers(2, vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), spots, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
			glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
	}
	// 5
	{
		const GLfloat spot_5[5][3]{ {0.0f, size * 2, 0.0f}, {-size,0,-size}, {-size,0,size}, {size,0,-size}, {size,0,size} };
		const GLfloat color_5[5][3]{ {rd_color(dre),rd_color(dre),rd_color(dre)},{rd_color(dre),rd_color(dre),rd_color(dre)},{rd_color(dre),rd_color(dre),rd_color(dre)},{rd_color(dre),rd_color(dre),rd_color(dre)},{rd_color(dre),rd_color(dre),rd_color(dre)} };
		const int triangles[4][3]{ {0,4,3},{0,2,4},{0,1,2},{0,3,1} };
		for (int i = 0; i < 4; ++i) {
			GLfloat spots[3][3]{ 
				{spot_5[triangles[i][0]][0],spot_5[triangles[i][0]][1],spot_5[triangles[i][0]][2]},
				{spot_5[triangles[i][1]][0],spot_5[triangles[i][1]][1],spot_5[triangles[i][1]][2]},
				{spot_5[triangles[i][2]][0],spot_5[triangles[i][2]][1],spot_5[triangles[i][2]][2]} },
				colors[3][3]{ 
				{color_5[triangles[i][0]][0],color_5[triangles[i][0]][1],color_5[triangles[i][0]][2]},
				{color_5[triangles[i][1]][0],color_5[triangles[i][1]][1],color_5[triangles[i][1]][2]},
				{color_5[triangles[i][2]][0],color_5[triangles[i][2]][1],color_5[triangles[i][2]][2]} };
			for (int a =0; a < 3; ++a) {
				if (i == 0) 
					spots[a][0] -= size;
				else if (i == 1)
					spots[a][2] -= size;
				else if (i == 2)
					spots[a][0] += size;
				else
					spots[a][2] += size;
			}
			glGenVertexArrays(1, &penta[i]);
			glBindVertexArray(penta[i]);
			glGenBuffers(2, vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), spots, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
			glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
		//밑면
		{
			const GLfloat spots[4][3]{ {-size,0,-size},{-size,0,size}, {size,0,-size}, {size,0,size} },
				colors[4][3]{
				{color_5[1][0],color_5[1][1],color_5[1][2]},
				{color_5[2][0],color_5[2][1],color_5[2][2]},
				{color_5[3][0],color_5[3][1],color_5[3][2]},
				{color_5[4][0],color_5[4][1],color_5[4][2]} };
			glGenVertexArrays(1, &penta[4]);
			glBindVertexArray(penta[4]);
			glGenBuffers(2, vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), spots, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
			glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
	}
}
GLvoid drawScene()
{
	glClearColor(background_r, background_g, background_b, 1.0f);			// 바탕색을 파랑색으로 지정
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);					// 설정된 색으로 전체를 칠하기
	//shader 그리기----------------------
	glUseProgram(shaderProgramID);	// 활성화 한 번만 해주면 된다.

	if (h_switch)
		glEnable(GL_DEPTH_TEST);	// 깊이 검사
	else
		glDisable(GL_DEPTH_TEST);

	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 R = glm::mat4(1.0f);	// 축 회전

	if (p_switch) {	// 직각 투영 : [-2.0, 2.0]
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -1.0f); //--- 카메라 위치
		glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 1.0f); //--- 카메라 바라보는 방향
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform"); //--- 뷰잉 변환 설정
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
		projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
	}
	else {// 원근 투영 : fovy 45.0, near 0.1, far 50.0
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -5.0f);
		glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform"); //--- 뷰잉 변환 설정
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
		projection = glm::perspective(glm::radians(45.0f), -1.0f, 0.1f, 50.0f);
	}

	unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform"); //--- 투영 변환 값 설정
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	// x, y, z축
	{
		glm::mat4 Rx = glm::mat4(1.0f);
		glm::mat4 Ry = glm::mat4(1.0f);
		Rx = glm::rotate(Rx, glm::radians(-30.0f), glm::vec3(1.0, 0.0, 0.0));
		Ry = glm::rotate(Ry, glm::radians(-30.0f), glm::vec3(0.0, 1.0, 0.0));
		R = Rx * Ry;
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(R));
		glLineWidth(2);
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
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 model_ry = glm::mat4(1.0f);
	model_ry = glm::rotate(model_ry, glm::radians(radian_y), glm::vec3(0.0, 1.0, 0.0));
	// 5
	if (print_polygon) {
		{	// 오른
			glm::mat4 go_back = glm::mat4(1.0f);
			glm::mat4 spin = glm::mat4(1.0f);
			spin = glm::rotate(spin, glm::radians(-o_radian), glm::vec3(0.0f, 0.0f, 1.0f));
			if (r_open == 0)
				spin = glm::rotate(spin, glm::radians(-r_radian), glm::vec3(0.0f, 0.0f, 1.0f));
			go_back = glm::translate(go_back, glm::vec3(size, 0.0f, 0.0f));
			model = R * model_ry * go_back * spin;
			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
			glBindVertexArray(penta[0]);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		} {	// 뒷면
			glm::mat4 go_back = glm::mat4(1.0f);
			glm::mat4 spin = glm::mat4(1.0f);
			spin = glm::rotate(spin, glm::radians(o_radian), glm::vec3(1.0f, 0.0f, 0.0f));
			if (r_open == 1) 
				spin = glm::rotate(spin, glm::radians(r_radian), glm::vec3(1.0f, 0.0f, 0.0f));
			go_back = glm::translate(go_back, glm::vec3(0.0f, 0.0f, size)); 
			model = R * model_ry * go_back * spin;
			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
			glBindVertexArray(penta[1]);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		} {	// 왼면
			glm::mat4 go_back = glm::mat4(1.0f);
			glm::mat4 spin = glm::mat4(1.0f);
			spin = glm::rotate(spin, glm::radians(o_radian), glm::vec3(0.0f, 0.0f, 1.0f));
			if (r_open == 2) 
				spin = glm::rotate(spin, glm::radians(r_radian), glm::vec3(0.0f, 0.0f, 1.0f));
			go_back = glm::translate(go_back, glm::vec3(-size, 0.0f, 0.0f));
			model = R * model_ry * go_back * spin;
			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
			glBindVertexArray(penta[2]);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		} {	// 앞면
			glm::mat4 go_back = glm::mat4(1.0f);
			glm::mat4 spin = glm::mat4(1.0f);
			spin = glm::rotate(spin, glm::radians(-o_radian), glm::vec3(1.0f, 0.0f, 0.0f));
			if (r_open == 3) 
				spin = glm::rotate(spin, glm::radians(-r_radian), glm::vec3(1.0f, 0.0f, 0.0f));
			go_back = glm::translate(go_back, glm::vec3(0.0f, 0.0f, -size));
			model = R * model_ry * go_back * spin;
			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
			glBindVertexArray(penta[3]);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		} {	// 아래
			model = R * model_ry;
			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
			glBindVertexArray(penta[4]);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	}
	// 8
	else {
		{	// 윗면
			glm::mat4 go_back = glm::mat4(1.0f);
			glm::mat4 round = glm::mat4(1.0f);
			go_back = glm::translate(go_back, glm::vec3(0.0f, size * cos(glm::radians(t_radian)) - size, - size * sin(glm::radians(t_radian))));
			round = glm::rotate(round, glm::radians(t_radian), glm::vec3(1.0f, 0.0f, 0.0f));
			model = R * model_ry * round * go_back;
			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
			glBindVertexArray(hexa[0]);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		} {	// 오른
			glm::mat4 side_slice = glm::mat4(1.0f);		// 옆면 슬라이스로 열리기
			side_slice = glm::translate(side_slice, glm::vec3(0.0, s_slice, 0.0));
			model = R * model_ry * side_slice;
			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
			glBindVertexArray(hexa[1]);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		} {	// 뒷면
			glm::mat4 S = glm::mat4(1.0f);
			S = glm::scale(S, glm::vec3(b_size, b_size, 1.0f));
			model = R * model_ry * S;
			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
			glBindVertexArray(hexa[2]);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		} {	// 왼면
			glm::mat4 side_slice = glm::mat4(1.0f);		// 옆면 슬라이스로 열리기
			side_slice = glm::translate(side_slice, glm::vec3(0.0, s_slice, 0.0));
			model = R * model_ry * side_slice;
			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
			glBindVertexArray(hexa[3]);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		} {	// 앞면
			glm::mat4 go_back = glm::mat4(1.0f);
			glm::mat4 round = glm::mat4(1.0f);
			go_back = glm::translate(go_back, glm::vec3(0.0f, size * -sin(glm::radians(-f_radian)) + size * (1 - cos(glm::radians(-f_radian))), size * (1 - cos(glm::radians(-f_radian))) + size * sin(glm::radians(-f_radian))));
			round = glm::rotate(round, glm::radians(-f_radian), glm::vec3(1.0f, 0.0f, 0.0f));
			model = R * model_ry * round * go_back;
			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
			glBindVertexArray(hexa[4]);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		} {	//아래
			glm::mat4 go_back = glm::mat4(1.0f);
			glm::mat4 gone = glm::mat4(1.0f);
			model = R * model_ry;
			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
			glBindVertexArray(hexa[5]);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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
	case 'h':
		if (h_switch)
			h_switch = false;
		else
			h_switch = true;
		break;
	case 'y':
		if (y_switch)
			y_switch = false;
		else
			y_switch = true;
		glutTimerFunc(100, y_anime, y_switch);
		break;
	case 't':
		print_polygon = false;
		if (t_switch)
			t_switch = false;
		else
			t_switch = true;
		glutTimerFunc(100, t_anime, 0);
		break;
	case 'f':
		print_polygon = false;
		if (f_switch)
			f_switch = false;
		else 
			f_switch = true;
		glutTimerFunc(100, f_anime, f_switch);
		break;
	case 's':
		print_polygon = false;
		if (s_switch)
			s_switch = false;
		else
			s_switch = true;
		glutTimerFunc(100, s_anime, s_switch);
		break;
	case 'b':
		print_polygon = false;
		if (b_switch)
			b_switch = false;
		else
			b_switch = true;
		glutTimerFunc(100, b_anime, b_switch);
		break;
	case 'o':
		print_polygon = true;
		if (o_switch)
			o_switch = false;
		else
			o_switch = true;
		r_switch = false;
		glutTimerFunc(100, o_anime, o_switch);
		break;
	case 'r':
		print_polygon = true;
		if (r_switch)
			r_switch = false;
		else
			r_switch = true;
		if (!o_switch)
			glutTimerFunc(100, r_anime, r_switch);
		break;
	case 'p':
		// 직각 투영 : [-2.0, 2.0]
		// 원근 투영 : fovy 45.0, near 0.1, far 50.0
		// 투영 공간을 z축으로  - 5.0 정도 이동
		p_switch = !p_switch;
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}
GLvoid y_anime(int value)
{
	radian_y += 1.0f;
	glutPostRedisplay();
	if (y_switch)
		glutTimerFunc(100, y_anime, value);
}
GLvoid t_anime(int value)
{
	if (t_switch) {
		t_radian += 1.0f;
		glutPostRedisplay();
		glutTimerFunc(100, t_anime, value);
	}
}
GLvoid f_anime(int value)
{
	if (value == f_switch) {
		if (f_switch) {
			f_radian += 1.0f;
			if (f_radian >= 90.0f)
				f_radian = 90.0f;
			glutPostRedisplay();
			if (f_radian < 90.0f)
				glutTimerFunc(100, f_anime, value);
		}
		else {
			f_radian -= 1.0f;
			if (f_radian <= 0.0f)
				f_radian = 0.0f;
			glutPostRedisplay();
			if (f_radian > 0.0f)
				glutTimerFunc(100, f_anime, value);
		}
	}
}
GLvoid s_anime(int value)
{
	if (s_switch == value) {
		if (s_switch)
			s_slice += 0.01f;
		else {
			s_slice -= 0.01f;
			if (s_slice < 0.0f)
				s_slice = 0.0f;
		}
		glutPostRedisplay();
		if (s_switch == true && s_slice < 1.0f)
			glutTimerFunc(100, s_anime, value);
		if (s_switch == false && s_slice != 0.0f)
			glutTimerFunc(100, s_anime, value);
	}
}
GLvoid b_anime(int value)
{
	if (value == b_switch) {
		if (b_switch) {
			b_size -= 0.05f;
			if (b_size < 0.0f)
				b_size = 0.0f;
			glutPostRedisplay();
			if (b_size > 0.0f)
				glutTimerFunc(100, b_anime, value);
		}
		else {
			b_size += 0.05f;
			if (b_size > 1.0f)
				b_size = 1.0f;
			glutPostRedisplay();
			if (b_size < 1.0f)
				glutTimerFunc(100, b_anime, value);
		}
	}
}
GLvoid o_anime(int value)
{
	if (value == o_switch) {
		if (r_radian != 0.0f) {
			r_radian -= 1.0f;
			if (r_radian < 0.0f)
				r_radian = 0.0f;
			glutPostRedisplay();
			glutTimerFunc(100, o_anime, value);
		}
		else {
			if (o_switch) {
				o_radian += 1.0f;
				if (o_radian > 233.333f)
					o_radian = 233.333f;
				glutPostRedisplay();
				if (o_radian < 233.333f)
					glutTimerFunc(100, o_anime, value);
			}
			else {
				o_radian -= 1.0f;
				if (o_radian < 0.0f)
					o_radian = 0.0f;
				glutPostRedisplay();
				if (o_radian > 0.0f)
					glutTimerFunc(100, o_anime, value);
			}
		}
	}
}
GLvoid r_anime(int value)
{
	if (!o_switch) {
		if (value && r_switch) {
			r_radian += 1.0f;
			if (r_radian > 117.0f)
				r_radian = 116.6665f;
			glutPostRedisplay();
			if (r_radian != 116.6665f)
				glutTimerFunc(100, r_anime, 1);
			else
				glutTimerFunc(100, r_anime, 0);
		}
		else {
			r_radian -= 1.0f;
			if (r_radian < 0.0f)
				r_radian = 0.0f;
			glutPostRedisplay();
			if (r_radian != 0.0f)
				glutTimerFunc(100, r_anime, 0);
			else if (r_switch) {
				glutTimerFunc(100, r_anime, 1);
				r_open = (r_open + 1) % 4;
			}
		}
	}
}
