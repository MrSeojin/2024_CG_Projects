#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <random>
#include <vector>
#include <gl\glew.h>
#include <gl\freeglut.h>
#include <gl\freeglut_ext.h>
#include <gl\glm\glm\glm.hpp>

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_int_distribution<int> rd_shape(3, 10), random(0, 1);
std::uniform_real_distribution<GLclampf> rd_color(0.0f, 1.0f);

struct polygon {
	GLuint vao;
	int spot_num, dir;
	GLfloat spots[50][3], colors[50][3], radian;
	bool slice, spin, on;
};
std::vector<polygon>polygons;
std::vector<polygon>basket_first_poly;
std::vector<polygon>basket_sec_poly;
std::vector<std::vector<GLfloat>>road;
GLfloat cut_line[2][3], basket_first[4][3] = { {-0.2f,-0.85f,0.0f}, {0.2f,-0.85f,0.0f}, {-0.2f,-0.9f,0.0f}, {0.2f,-0.9f,0.0f} }, basket_sec[4][3] = { {-0.2f,-0.85f,0.0f}, {0.2f,-0.85f,0.0f}, {-0.2f,-0.9f,0.0f}, {0.2f,-0.9f,0.0f} }, speed = 0.01f;
GLint width, height;
GLuint shaderProgramID;	//셰이더 프로그램 이름
GLuint vertexShader;	// 버텍스 셰이더 객체
GLuint fragmentShader;	// 프래그먼트 세이더 객체
GLuint cut_vao, basket_first_vao, basket_sec_vao, screen_vao, road_vao, vbo[2];
int now_polygon, print_polygon_first, print_polygon_sec;
bool l_button, l_flag, start, r_flag, game_over;

char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();

void InitBuffer();
GLvoid random_polygon(int value);
GLvoid get_road();
GLvoid get_line();
GLvoid get_basket();
GLvoid polygon_slice();
BOOL hit(GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2, GLfloat x3, GLfloat x4, GLfloat y3, GLfloat y4);
GLfloat x_out(GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2, GLfloat x3, GLfloat x4, GLfloat y3, GLfloat y4)
{
	if (x1 == x2)
		return x1;
	else if (x3 == x4)
		return x3;
	GLfloat m1 = (y1 - y2) / (x1 - x2), m2 = (y3 - y4) / (x3 - x4);
	if (m1 == m2)
		return -10.0f;
	else if (y1 == y2)
		return (x3 - x4) / (y3 - y4) * (y1 - y3) + x3;
	else if (y3 == y4)
		return (x1 - x2) / (y1 - y2) * (y3 - y1) + x1;
	return (m1 * x1 - y1 - m2 * x3 + y3) / (m1 - m2);
}
GLfloat y_out(GLfloat x, GLfloat y1, GLfloat y2, GLfloat x3, GLfloat x4, GLfloat y3, GLfloat y4)
{
	if (x == -10.0f)
		return 10.0f;
	if (y1 == y2)
		return y1;
	else if (y3 == y4)
		return y3;
	return (y3 - y4) / (x3 - x4) * (x - x3) + y3;
}
GLvoid gameover();
 
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid SpecialKeyboard(int key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
GLvoid animation(int value);

void main(int argc, char** argv)	// 윈도우 생성
{
	width = 800, height = 800;
	glutInit(&argc, argv);							// glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);	// 디스플레이 모드 설정
	glutInitWindowPosition(50, 50);					// 윈도우의 위치 지정
	glutInitWindowSize(width, height);					// 윈도우의 크기 지정
	glutCreateWindow("polygon_slice");	// 윈도우 생성 (윈도우 이름)

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
	glutTimerFunc(10, random_polygon, 0);
	glutTimerFunc(100, animation, 1);

	glutDisplayFunc(drawScene);						// 출력 함수의 지정
	glutReshapeFunc(Reshape);						// 다시 그리기 함수 지정
	glutKeyboardFunc(Keyboard);						// 키보드 입력 콜백함수 지정
	glutSpecialFunc(SpecialKeyboard);
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
}
void InitBuffer()
{
	std::vector<polygon>::iterator itor = polygons.begin();
	for (; itor != polygons.end(); itor++) {
		glGenVertexArrays(1, &itor->vao);
		glBindVertexArray(itor->vao);
		glGenBuffers(2, vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, 3 * itor->spot_num * sizeof(GLfloat), itor->spots, GL_STREAM_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, 3 * itor->spot_num * sizeof(GLfloat), itor->colors, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}
}
GLvoid random_polygon(int value)
{
	if (!game_over&&start) {
		polygon new_polygon{};
		new_polygon.spot_num = rd_shape(dre);
		GLfloat center = 0.0f, color[3] = { rd_color(dre), rd_color(dre) ,rd_color(dre) };
		if (random(dre)) {
			new_polygon.dir = 1;
			center = road[0][1];
		}
		else {
			new_polygon.dir = -1;
			center = road[4][1];
		}
		for (int i = 0; i < new_polygon.spot_num; ++i) {
			new_polygon.spots[i][0] = 0.2f * cos(glm::radians(i * 360.0f / new_polygon.spot_num)) - 1.2f * new_polygon.dir;
			new_polygon.spots[i][1] = 0.2f * sin(glm::radians(i * 360.0f / new_polygon.spot_num)) + center;
			new_polygon.spots[i][2] = 0.0f;
			new_polygon.colors[i][0] = color[0];
			new_polygon.colors[i][1] = color[1];
			new_polygon.colors[i][2] = color[2];
		}
		new_polygon.spin = random(dre);

		glGenVertexArrays(1, &new_polygon.vao);
		glBindVertexArray(new_polygon.vao);
		glGenBuffers(2, vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, 3 * new_polygon.spot_num * sizeof(GLfloat), new_polygon.spots, GL_STREAM_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, 3 * new_polygon.spot_num * sizeof(GLfloat), new_polygon.colors, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
		glutPostRedisplay();

		polygons.push_back(new_polygon);
	}
	if (!value)
		glutTimerFunc(100 / speed, random_polygon, value);
}
GLvoid get_road()
{
	for (int i = 0; i < 5; ++i) {
		int input = 0;
		for (int a = 0; a < 5 - i; ++a) {
			if (road[input][0] > road[a][0])
				input = a;
		}
		road.push_back(road[input]);
		road.erase(road.begin() + input);
	}

	GLfloat curve_road[5][3]{};
	GLfloat curve_color[5][3]{};
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 3; j++)
			curve_road[i][j] = road[i][j];
		curve_color[i][0] = 1.0f;
	}
	{
		glGenVertexArrays(1, &road_vao);
		glBindVertexArray(road_vao);
		glGenBuffers(2, vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, 15 * sizeof(GLfloat), curve_road, GL_STREAM_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, 15 * sizeof(GLfloat), curve_color, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	} {
		glGenVertexArrays(1, &screen_vao);
		glBindVertexArray(screen_vao);
		glGenBuffers(2, vbo);

		const GLfloat screen_spot[8][3] = { {-0.95f, 0.95f, 0.0f }, {-0.45f, 0.95f, 0.0f }, {-0.95f, 0.45f, 0.0f},{-0.45f, 0.45f, 0.0f},  {0.45f, 0.95f, 0.0f }, {0.95f, 0.95f, 0.0f }, {0.45f, 0.45f, 0.0f},{0.95f, 0.45f, 0.0f} };
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), screen_spot, GL_STREAM_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		const GLfloat screem_color[8][3] = { {0.9f, 0.9f, 0.9f }, {0.9f, 0.9f, 0.9f }, {0.9f, 0.9f, 0.9f }, {0.9f, 0.9f, 0.9f } , {0.9f, 0.9f, 0.9f }, {0.9f, 0.9f, 0.9f }, {0.9f, 0.9f, 0.9f }, {0.9f, 0.9f, 0.9f } };
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), screem_color, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}
	random_polygon(1);
	glutPostRedisplay();
}
GLvoid get_line()
{
	glGenVertexArrays(1, &cut_vao);
	glBindVertexArray(cut_vao);
	glGenBuffers(2, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), cut_line, GL_STREAM_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	const GLfloat line_color[2][3] = { {1.0f, 1.0f, 1.0f }, {1.0f, 1.0f, 1.0f } };
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), line_color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glutPostRedisplay();
}
GLvoid get_basket() 
{
	{
		glGenVertexArrays(1, &basket_first_vao);
		glBindVertexArray(basket_first_vao);
		glGenBuffers(2, vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), basket_first, GL_STREAM_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		const GLfloat line_color[4][3] = { {0.0f, 1.0f, 1.0f }, {0.0f, 1.0f, 1.0f }, {0.0f, 1.0f, 1.0f }, {0.0f, 1.0f, 1.0f } };
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), line_color, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	} {
		glGenVertexArrays(1, &basket_sec_vao);
		glBindVertexArray(basket_sec_vao);
		glGenBuffers(2, vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), basket_sec, GL_STREAM_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		const GLfloat line_color[4][3] = { {1.0f, 1.0f, 0.0f },   {1.0f, 1.0f, 0.0f },   {1.0f, 1.0f, 0.0f },   {1.0f, 1.0f, 0.0f } };
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), line_color, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}
}
GLvoid polygon_slice()
{
	int end = polygons.size();
	std::vector<int>pop_list;
	for (int i = 0; i < end; ++i) {
		GLfloat point[2][2] = { {-10.0f, 10.0f}, { -10.0f, 10.0f } };	// 다각형과 cut선의 교점
		std::vector<GLfloat>up_list[2], down_list[2];	// 새로 나눠진 다각형 2개..
		GLfloat min_x = polygons[i].spots[0][0], min_y = polygons[i].spots[0][1], max_x = polygons[i].spots[0][0], max_y = polygons[i].spots[0][1];
		for (int k = 0; k < polygons[i].spot_num; ++k) {
			if (min_x > polygons[i].spots[k][0])
				min_x = polygons[i].spots[k][0];
			if (max_x <  polygons[i].spots[k][0])
				max_x =  polygons[i].spots[k][0];
			if (min_y >  polygons[i].spots[k][1])
				min_y =  polygons[i].spots[k][1];
			if (max_y <  polygons[i].spots[k][1])
				max_y =  polygons[i].spots[k][1];
		}
		if (hit(cut_line[0][0], cut_line[1][0], cut_line[0][1], cut_line[1][1], min_x, max_x, min_y, max_y)) {
			for (int j = 0; j < polygons[i].spot_num; ++j) {
				GLfloat x1 = polygons[i].spots[(j - 1 + polygons[i].spot_num) % polygons[i].spot_num][0], x2 = polygons[i].spots[j][0],
					y1 = polygons[i].spots[(j - 1 + polygons[i].spot_num) % polygons[i].spot_num][1], y2 = polygons[i].spots[j][1], I_x = -10.0f, I_y = 10.0f;
				I_x = x_out(x1, x2, y1, y2, cut_line[0][0], cut_line[1][0], cut_line[0][1], cut_line[1][1]); //교점 x 
				I_y = y_out(I_x, y1, y2, cut_line[0][0], cut_line[1][0], cut_line[0][1], cut_line[1][1]);	// 교점 y
				I_y = floor(I_y * 1000) / 1000;
				y1 = floor(y1 * 1000) / 1000;
				y2 = floor(y2 * 1000) / 1000;
				if (((cut_line[0][0] <= I_x && I_x <= cut_line[1][0]) || (cut_line[1][0] <= I_x && I_x <= cut_line[0][0])) &&
					((cut_line[0][1] <= I_y && I_y <= cut_line[1][1]) || (cut_line[1][1] <= I_y && I_y <= cut_line[0][1])) &&
					((x1 <= I_x && I_x <= x2) || (x2 <= I_x && I_x <= x1)) &&
					((y1 <= I_y && I_y <= y2) || (y2 <= I_y && I_y <= y1))) {
					if (point[0][0] == -10.0f) {
						point[0][0] = I_x;
						point[0][1] = I_y;
						up_list[0].push_back(I_x);
						up_list[1].push_back(I_y);
						down_list[0].push_back(I_x);
						down_list[1].push_back(I_y);
					}
					else {
						point[1][0] = I_x;
						point[1][1] = I_y;
						up_list[0].push_back(I_x);
						up_list[1].push_back(I_y);
						down_list[0].push_back(I_x);
						down_list[1].push_back(I_y);
					}
				}
				if (point[0][0] == -10.0f) {
					up_list[0].push_back(polygons[i].spots[j][0]);
					up_list[1].push_back(polygons[i].spots[j][1]);
				}
				else if (point[1][1] != 10.0f) {
					up_list[0].push_back(polygons[i].spots[j][0]);
					up_list[1].push_back(polygons[i].spots[j][1]);
				}
				else {
					down_list[0].push_back(polygons[i].spots[j][0]);
					down_list[1].push_back(polygons[i].spots[j][1]);
				}
			}
			if (point[0][0] != -10.0f && point[1][1] != 10.f) {
				polygon left_polygon{}, right_polygon{};
				left_polygon.slice = true, right_polygon.slice = true;
				left_polygon.spot_num = up_list[0].size(), right_polygon.spot_num = down_list[0].size();
				for (int a = 0; a < left_polygon.spot_num; ++a) {
					left_polygon.spots[a][0] = up_list[0][a];
					left_polygon.spots[a][1] = up_list[1][a];
					left_polygon.colors[a][0] = polygons[i].colors[0][0];
					left_polygon.colors[a][1] = polygons[i].colors[0][1];
					left_polygon.colors[a][2] = polygons[i].colors[0][2];
				}
				for (int a = 0; a < right_polygon.spot_num; ++a) {
					right_polygon.spots[a][0] = down_list[0][a];
					right_polygon.spots[a][1] = down_list[1][a];
					right_polygon.colors[a][0] = polygons[i].colors[0][0];
					right_polygon.colors[a][1] = polygons[i].colors[0][1];
					right_polygon.colors[a][2] = polygons[i].colors[0][2];
				}
				GLfloat m = (cut_line[0][1] - cut_line[1][1]) / (cut_line[0][0] - cut_line[1][0]);
				left_polygon.dir = 1, right_polygon.dir = -1;
				left_polygon.spin = false, right_polygon.spin = false;
				{
					glGenVertexArrays(1, &left_polygon.vao);
					glBindVertexArray(left_polygon.vao);
					glGenBuffers(2, vbo);

					glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
					glBufferData(GL_ARRAY_BUFFER, 3 * left_polygon.spot_num * sizeof(GLfloat), left_polygon.spots, GL_STREAM_DRAW);
					glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
					glEnableVertexAttribArray(0);

					glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
					glBufferData(GL_ARRAY_BUFFER, 3 * left_polygon.spot_num * sizeof(GLfloat), left_polygon.colors, GL_STATIC_DRAW);
					glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
					glEnableVertexAttribArray(1);
				} {
					glGenVertexArrays(1, &right_polygon.vao);
					glBindVertexArray(right_polygon.vao);
					glGenBuffers(2, vbo);

					glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
					glBufferData(GL_ARRAY_BUFFER, 3 * right_polygon.spot_num * sizeof(GLfloat), right_polygon.spots, GL_STREAM_DRAW);
					glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
					glEnableVertexAttribArray(0);

					glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
					glBufferData(GL_ARRAY_BUFFER, 3 * right_polygon.spot_num * sizeof(GLfloat), right_polygon.colors, GL_STATIC_DRAW);
					glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
					glEnableVertexAttribArray(1);
				}

				polygons.push_back(left_polygon);
				polygons.push_back(right_polygon);

				pop_list.push_back(i);

				glutPostRedisplay();
			}
		}
	}
	for (int i = pop_list.size() - 1; i >= 0; i--) {
		polygons.erase(polygons.begin() + pop_list[i]);
	}
	pop_list.clear();
}
BOOL hit(GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2, GLfloat x3, GLfloat x4, GLfloat y3, GLfloat y4)
{
	if ((x1 <= x3 && x3 <= x2) || (x2 <= x3 && x3 <= x1) ||
		(x1 <= x4 && x4 <= x2) || (x2 <= x4 && x4 <= x1) ||
		(x3 <= x1 && x1 <= x4) || (x4 <= x1 && x1 <= x3) ||
		(x3 <= x2 && x2 <= x2) || (x4 <= x2 && x2 <= x3)) {
		if ((y1 <= y3 && y3 <= y2) || (y2 <= y3 && y3 <= y1) ||
			(y1 <= y4 && y4 <= y2) || (y2 <= y4 && y4 <= y1) ||
			(y3 <= y1 && y1 <= y4) || (y4 <= y1 && y1 <= y3) ||
			(y3 <= y2 && y2 <= y2) || (y4 <= y2 && y2 <= y3)) {
			return true;
		}
	}
	return false;
}
GLvoid gameover()
{
	game_over = true;
	polygons.clear();
	road.clear();
	speed = 0.01f;
	now_polygon = 0;
	l_flag = false, r_flag = false;
	if (basket_first_poly.size() == basket_sec_poly.size()) 
		std::cout << "무승부" << std::endl;
	else if(basket_first_poly.size() > basket_sec_poly.size())
		std::cout << "파란 바구니 우승" << std::endl;
	else
		std::cout << "노란 바구니 우승" << std::endl;
}
GLvoid drawScene()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);			// 바탕색을 파랑색으로 지정
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);					// 설정된 색으로 전체를 칠하기
	//shader 그리기----------------------
	glUseProgram(shaderProgramID);	// 활성화 한 번만 해주면 된다.

	if (start && !game_over) {
		if (r_flag) {
			glLineWidth(2);
			glBindVertexArray(road_vao);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_LINE_STRIP, 0, 5);
		}

		glBindVertexArray(basket_first_vao);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindVertexArray(basket_sec_vao);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindVertexArray(screen_vao);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

		if (l_flag)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if (!basket_first_poly.empty()) {
			glBindVertexArray(basket_first_poly[print_polygon_first].vao);
			glDrawArrays(GL_TRIANGLE_FAN, 0, basket_first_poly[print_polygon_first].spot_num);
		}
		if (!basket_sec_poly.empty()) {
			glBindVertexArray(basket_sec_poly[print_polygon_sec].vao);
			glDrawArrays(GL_TRIANGLE_FAN, 0, basket_sec_poly[print_polygon_sec].spot_num);
		}

		if (!polygons.empty()) {
			std::vector<polygon>::iterator itor = polygons.begin();
			for (; itor != polygons.end(); itor++) {
				glBindVertexArray(itor->vao);
				glDrawArrays(GL_TRIANGLE_FAN, 0, itor->spot_num);
			}
		}

		if (l_button) {
			glLineWidth(2);
			glBindVertexArray(cut_vao);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_LINES, 0, 2);
		}
	}
	else if (game_over) {
		glBindVertexArray(screen_vao);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

		if (l_flag)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if (!basket_first_poly.empty()) {
			glBindVertexArray(basket_first_poly[print_polygon_first].vao);
			glDrawArrays(GL_TRIANGLE_FAN, 0, basket_first_poly[print_polygon_first].spot_num);
		}
		if (!basket_sec_poly.empty()) {
			glBindVertexArray(basket_sec_poly[print_polygon_sec].vao);
			glDrawArrays(GL_TRIANGLE_FAN, 0, basket_sec_poly[print_polygon_sec].spot_num);
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
	case 's':
		gameover();
		system("cls");
		game_over = false;
		basket_first_poly.clear();
		basket_sec_poly.clear();
		print_polygon_first = 0;
		print_polygon_sec = 0;
		start = false;
		break;
	case 'a':
		if (!game_over&&start) {
			if (basket_sec[0][0] >= -1.0f) {
				for (int i = 0; i < 4; ++i)
					basket_sec[i][0] -= 0.05f;
			}
			get_basket();
		}
		break;
	case 'd':
		if (!game_over && start) {
			if (basket_sec[1][0] <= 1.0f) {
				for (int i = 0; i < 4; ++i)
					basket_sec[i][0] += 0.05f;
			}
			get_basket();
		}
		break;
	case 'n':
		if (!basket_first_poly.empty())
			print_polygon_first = (print_polygon_first + 1) % basket_first_poly.size();
		break;
	case 'm':
		if (!basket_sec_poly.empty())
			print_polygon_sec = (print_polygon_sec + 1) % basket_sec_poly.size();
		break;
	case 'r':
		if (!game_over && start)
			r_flag = !r_flag;
		break;
	case 'l':
		if (!game_over && start)
			l_flag = !l_flag;
		break;
	case '+':
		if (!game_over && start && speed < 0.05f)
			speed += 0.01f;
		break;
	case '-':
		if (!game_over && start && speed > 0.01f)
			speed -= 0.01f;
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}
GLvoid SpecialKeyboard(int key, int x, int y)
{
	if (!game_over) {
		if (basket_first[0][0] >= -1.0f && key == GLUT_KEY_LEFT) {
			for (int i = 0; i < 4; ++i)
				basket_first[i][0] -= 0.05f;
		}
		else if (basket_first[1][0] <= 1.0f && key == GLUT_KEY_RIGHT) {
			for (int i = 0; i < 4; ++i)
				basket_first[i][0] += 0.05f;
		}
		get_basket();
	}
	glutPostRedisplay();
}
GLvoid Mouse(int button, int state, int x, int y) 
{
	GLclampf mouse_x = static_cast<GLclampf>(x) / (static_cast<GLclampf>(width) / 2) - 1, mouse_y = 1 - static_cast<GLclampf>(y) / (static_cast<GLclampf>(height) / 2);
	if (!game_over) {
		if (start) {
			if (state == GLUT_DOWN && button == GLUT_LEFT) {
				l_button = true;
				cut_line[0][0] = mouse_x, cut_line[0][1] = mouse_y, cut_line[0][2] = 0.0f;
				cut_line[1][0] = mouse_x, cut_line[1][1] = mouse_y, cut_line[1][2] = 0.0f;
				get_line();
				glutPostRedisplay();
			}
			if (l_button && state == GLUT_UP) {
				l_button = false;
				cut_line[1][0] = mouse_x, cut_line[1][1] = mouse_y, cut_line[1][2] = 0.0f;
				polygon_slice();
				glutPostRedisplay();
			}
		}
		else if (state == GLUT_DOWN && button == GLUT_LEFT) {
			std::vector<GLfloat> footprint = { mouse_x, mouse_y, 0.0f };
			road.push_back(footprint);
			if (road.size() == 5) {
				start = true;
				get_basket();
				get_road();
			}
		}
	}
}
GLvoid Motion(int x, int y)
{
	GLclampf mouse_x = static_cast<GLclampf>(x) / (static_cast<GLclampf>(width) / 2) - 1, mouse_y = 1 - static_cast<GLclampf>(y) / (static_cast<GLclampf>(height) / 2);
	if (!game_over && l_button) {
		cut_line[1][0] = mouse_x, cut_line[1][1] = mouse_y, cut_line[1][2] = 0.0f;
		get_line();
		glutPostRedisplay();
	}
}
GLvoid animation(int value)
{
	int p = 0;
	std::vector<int>pop_list;
	std::vector<polygon>::iterator itor = polygons.begin();
	if (!game_over) {
		for (; itor != polygons.end(); itor++) {
			GLfloat m = 0.0f, center_x = 0.0f, center_y = 0.0f;
			for (int i = 0; i < itor->spot_num; ++i) {
				center_x += itor->spots[i][0];
				center_y += itor->spots[i][1];
			}
			center_x /= itor->spot_num;
			center_y /= itor->spot_num;
			if (itor->dir == 1) {
				if (center_x < road[0][0])
					m = 0;
				else if (center_x < road[1][0])
					m = (road[0][1] - road[1][1]) / (road[0][0] - road[1][0]);
				else if (center_x < road[2][0])
					m = (road[1][1] - road[2][1]) / (road[1][0] - road[2][0]);
				else if (center_x < road[3][0])
					m = (road[2][1] - road[3][1]) / (road[2][0] - road[3][0]);
				else if (center_x < road[4][0])
					m = (road[3][1] - road[4][1]) / (road[3][0] - road[4][0]);
				else
					m = 0;
			}
			else {
				if (center_x > road[4][0])
					m = 0;
				else if (center_x > road[3][0])
					m = (road[4][1] - road[3][1]) / (road[4][0] - road[3][0]);
				else if (center_x > road[2][0])
					m = (road[3][1] - road[2][1]) / (road[3][0] - road[2][0]);
				else if (center_x > road[1][0])
					m = (road[2][1] - road[1][1]) / (road[2][0] - road[1][0]);
				else if (center_x > road[0][0])
					m = (road[1][1] - road[0][1]) / (road[1][0] - road[0][0]);
				else
					m = 0;
			}
			bool out_check = true;
			for (int i = 0; i < itor->spot_num; ++i) {
				if (itor->slice) {
					itor->spots[i][0] += speed * itor->dir / 10.0f;
					itor->spots[i][1] -= speed;
				}
				else {
					itor->spots[i][0] += speed * itor->dir;
					itor->spots[i][1] += speed * m * itor->dir;
				}
				if (itor->dir < 0 && -1.0f < itor->spots[i][0])
					out_check = false;
				if (itor->dir > 0 && itor->spots[i][0] < 1.0f)
					out_check = false;
				if (itor->slice && itor->spots[i][1] < -1.4)
					out_check = true;
			}
			center_x = 0, center_y = 0;
			GLfloat min_x = itor->spots[0][0], min_y = itor->spots[0][1], max_x = itor->spots[0][0], max_y = itor->spots[0][1];
			for (int i = 0; i < itor->spot_num; ++i) {
				if (min_x > itor->spots[i][0])
					min_x = itor->spots[i][0];
				if (max_x < itor->spots[i][0])
					max_x = itor->spots[i][0];
				if (min_y > itor->spots[i][1])
					min_y = itor->spots[i][1];
				if (max_y < itor->spots[i][1])
					max_y = itor->spots[i][1];
					center_x += itor->spots[i][0];
					center_y += itor->spots[i][1];
			}
				center_x /= itor->spot_num;
				center_y /= itor->spot_num;
			if (itor->spin) {
				itor->radian += 10.0f * itor->dir;
				for (int i = 0; i < itor->spot_num; ++i) {
					itor->spots[i][0] = center_x + 0.2 * glm::cos(glm::radians(itor->radian + i * 360.0f / itor->spot_num));
					itor->spots[i][1] = center_y + 0.2 * glm::sin(glm::radians(itor->radian + i * 360.0f / itor->spot_num));
				}
			}

			if (hit(basket_first[0][0], basket_first[1][0], basket_first[0][1], basket_first[1][1], min_x, max_x, min_y, max_y) ||
				hit(basket_sec[0][0], basket_sec[1][0], basket_sec[0][1], basket_sec[1][1], min_x, max_x, min_y, max_y)) {
				int hit_check = 0;
				for (int j = 0; j < itor->spot_num; ++j) {
					GLfloat x1 = itor->spots[(j + 1) % itor->spot_num][0], x2 = itor->spots[j][0],
						y1 = itor->spots[(j + 1) % itor->spot_num][1], y2 = itor->spots[j][1], I_x = -10.0f, I_y = 10.0f;
					//first
					if (hit(basket_first[0][0], basket_first[1][0], basket_first[0][1], basket_first[1][1], min_x, max_x, min_y, max_y)) {
						for (int k = 0; k < 4; k++) {
							GLfloat x3 = basket_first[(k + 1) % 4][0], x4 = basket_first[k][0],
								y3 = basket_first[(k + 1) % 4][1], y4 = basket_first[k][1], I_x = -10.0f, I_y = 10.0f;
							I_x = x_out(x1, x2, y1, y2, x3, x4, y3, y4); //교점 x 
							I_y = y_out(I_x, y1, y2, x3, x4, y3, y4);	// 교점 y
							I_y = floor(I_y * 1000) / 1000;
							if (((x3 <= I_x && I_x <= x4) || (x4 <= I_x && I_x <= x3)) &&
								((y3 <= I_y && I_y <= y4) || (y4 <= I_y && I_y <= y3)) &&
								((x1 <= I_x && I_x <= x2) || (x2 <= I_x && I_x <= x1)) &&
								((y1 <= I_y && I_y <= y2) || (y2 <= I_y && I_y <= y1))) {
								hit_check = 1;
								break;
							}
						}
					}
					//sec
					if (hit(basket_sec[0][0], basket_sec[1][0], basket_sec[0][1], basket_sec[1][1], min_x, max_x, min_y, max_y)) {
						for (int k = 0; k < 4; k++) {
							GLfloat x3 = basket_sec[(k + 1) % 4][0], x4 = basket_sec[k][0],
								y3 = basket_sec[(k + 1) % 4][1], y4 = basket_sec[k][1], I_x = -10.0f, I_y = 10.0f;
							I_x = x_out(x1, x2, y1, y2, x3, x4, y3, y4); //교점 x 
							I_y = y_out(I_x, y1, y2, x3, x4, y3, y4);	// 교점 y
							I_y = floor(I_y * 1000) / 1000;
							if (((x3 <= I_x && I_x <= x4) || (x4 <= I_x && I_x <= x3)) &&
								((y3 <= I_y && I_y <= y4) || (y4 <= I_y && I_y <= y3)) &&
								((x1 <= I_x && I_x <= x2) || (x2 <= I_x && I_x <= x1)) &&
								((y1 <= I_y && I_y <= y2) || (y2 <= I_y && I_y <= y1))) {
								if (hit_check == 1)
									hit_check = 2;
								else
									hit_check = -1;
								break;
							}
						}
					}
					if (hit_check != 0)
						break;
				}
				if (hit_check != 0) {
					if (hit_check > 0) {
						for (int i = 0; i < itor->spot_num; ++i) {
							itor->spots[i][0] -= center_x + 0.7f;
							itor->spots[i][1] -= center_y - 0.75f;
						}
						InitBuffer();
						basket_first_poly.push_back(*itor);
					}
					else if (hit_check < 0) {
						for (int i = 0; i < itor->spot_num; ++i) {
							itor->spots[i][0] -= center_x - 0.7f;
							itor->spots[i][1] -= center_y - 0.75f;
						}
						InitBuffer();
						basket_sec_poly.push_back(*itor);
					}
					if (hit_check == 2) {
						for (int i = 0; i < itor->spot_num; ++i)
							itor->spots[i][0] += 1.4f;
						InitBuffer();
						basket_sec_poly.push_back(*itor);
					}
					out_check = true;
					system("cls");
					std::cout << "파란 바구니: " << basket_first_poly.size() << std::endl;
					std::cout << "노란 바구니: " << basket_sec_poly.size() << std::endl;
					if (basket_first_poly.size() >= 100 || basket_sec_poly.size() >= 100) {
						gameover();
						break;
					}
				}
			}
			if (out_check)
				pop_list.push_back(p);
			p++;
		}
	}
	if (!game_over) {
		for (int i = pop_list.size() - 1; i >= 0; i--)
			polygons.erase(polygons.begin() + pop_list[i]);
	}
	pop_list.clear();
	if (!game_over && polygons.empty())
		random_polygon(1);
	if (!game_over)
		InitBuffer();
	glutPostRedisplay();
	if (value)
		glutTimerFunc(100, animation, value); 
}
