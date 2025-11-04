#include <iostream>
#include <random>
#include <gl\glew.h>
#include <gl\freeglut.h>
#include <gl\freeglut_ext.h>

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_int_distribution<int> many(5, 10);
std::uniform_real_distribution<GLclampf> color(0.0f, 1.0f), point(-1.0f, 1.0f), size(0.05f, 0.20f);

struct sqare {
	GLclampf x1, y1, x2, y2, r, g, b, size;
	int part;
};
sqare sq[10];
GLclampf W = 1200, H = 900, w_mouse, h_mouse;
int timer_type, move = -1;

void create_sq(int num) {
	for (int i = 0; i < num; ++i) {
		sq[i].r = color(dre);
		sq[i].g = color(dre);
		sq[i].b = color(dre);
		sq[i].x1 = point(dre);
		sq[i].y1 = point(dre);
		sq[i].size = size(dre);
		if (sq[i].x1 + sq[i].size > 1.0)
			sq[i].x1 -= sq[i].size;
		if (sq[i].y1 + sq[i].size > 1.0)
			sq[i].y1 -= sq[i].size;
		sq[i].x2 = sq[i].x1 + sq[i].size;
		sq[i].y2 = sq[i].y1 + sq[i].size;
		sq[i].part = 1;
	}
}
int pointerSqare(int i, GLclampf x, GLclampf y) {
	if (sq[i].part == 0)
		return -1;
	else if (sq[i].x1 <= x && x <= sq[i].x2 && sq[i].y1 <= y && y <= sq[i].y2)
		return i;
	else
		return -1;
}
int cheak_all(GLclampf x, GLclampf y) {
	for (int i = 0; i < 10; ++i) {
		if (pointerSqare(i, x, y) != -1)
			return pointerSqare(i, x, y);
	}
	return -1;
}
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
GLvoid timer_func(int value);
GLvoid remove_sq(int value);

void main(int argc, char** argv)	// 윈도우 생성
{
	glutInit(&argc, argv);							// glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);	// 디스플레이 모드 설정
	glutInitWindowPosition(0, 0);					// 윈도우의 위치 지정
	glutInitWindowSize(W, H);					// 윈도우의 크기 지정
	create_sq(many(dre));
	glutCreateWindow("project_01_06");				// 윈도우 생성 (윈도우 이름)

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
	glutKeyboardFunc(Keyboard);						// 키보드 입력 콜백함수 지정
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutTimerFunc(100, timer_func, timer_type);
	glutMainLoop();									// 이벤트 처리 시작
}

GLvoid drawScene()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);			// 바탕색을 파랑색으로 지정
	glClear(GL_COLOR_BUFFER_BIT);					// 설정된 색으로 전체를 칠하기
	for (int i = 0; i < 40; ++i) {
		glColor3f(sq[i].r, sq[i].g, sq[i].b);
		if (sq[i].part == 0);
		else if (sq[i].part == 1)
			glRectf(sq[i].x1, sq[i].y1, sq[i].x2, sq[i].y2);
		else if (sq[i].part == 2) {	// 4개로 나눠짐
			GLclampf part_8 = sq[i].size / 2;
			switch (timer_type) {
			case 0:			
				glRectf(sq[i].x1, sq[i].y1, sq[i].x1 + part_8, sq[i].y1 + part_8);
				glRectf(sq[i].x1, sq[i].y2 - part_8, sq[i].x1 + part_8, sq[i].y2);
				glRectf(sq[i].x2 - part_8, sq[i].y2 - part_8, sq[i].x2, sq[i].y2);
				glRectf(sq[i].x2 - part_8, sq[i].y1, sq[i].x2, sq[i].y1 + part_8);
				break;
			case 1:
				glRectf((sq[i].x1 + sq[i].x2) / 2 - part_8 / 2, sq[i].y1, (sq[i].x1 + sq[i].x2) / 2 + part_8 / 2, sq[i].y1 + part_8);
				glRectf(sq[i].x1, (sq[i].y1 + sq[i].y2) / 2 - part_8 / 2, sq[i].x1 + part_8, (sq[i].y1 + sq[i].y2) / 2 + part_8 / 2);
				glRectf((sq[i].x1 + sq[i].x2) / 2 - part_8 / 2, sq[i].y2 - part_8, (sq[i].x1 + sq[i].x2) / 2 + part_8 / 2, sq[i].y2);
				glRectf(sq[i].x2 - part_8, (sq[i].y1 + sq[i].y2) / 2 - part_8 / 2, sq[i].x2, (sq[i].y1 + sq[i].y2) / 2 + part_8 / 2);
				break;
			case 2:			
				glRectf(sq[i].x1, sq[i].y1, sq[i].x1 + part_8, sq[i].y1 + part_8);
				glRectf(sq[i].x1, sq[i].y2 - part_8, sq[i].x1 + part_8, sq[i].y2);
				glRectf(sq[i].x2 - part_8, sq[i].y2 - part_8, sq[i].x2, sq[i].y2);
				glRectf(sq[i].x2 - part_8, sq[i].y1, sq[i].x2, sq[i].y1 + part_8);
				break;
			case 3:				
				glRectf(sq[i].x1, sq[i].y1, sq[i].x1 + part_8, sq[i].y1 + part_8);
				glRectf(sq[i].x1, sq[i].y2 - part_8, sq[i].x1 + part_8, sq[i].y2);
				glRectf(sq[i].x2 - part_8, sq[i].y2 - part_8, sq[i].x2, sq[i].y2);
				glRectf(sq[i].x2 - part_8, sq[i].y1, sq[i].x2, sq[i].y1 + part_8);
				break;
			case 4:
				glRectf((sq[i].x1 + sq[i].x2) / 2 - part_8 / 2, sq[i].y1, (sq[i].x1 + sq[i].x2) / 2 + part_8 / 2, sq[i].y1 + part_8);
				glRectf(sq[i].x1, (sq[i].y1 + sq[i].y2) / 2 - part_8 / 2, sq[i].x1 + part_8, (sq[i].y1 + sq[i].y2) / 2 + part_8 / 2);
				glRectf((sq[i].x1 + sq[i].x2) / 2 - part_8 / 2, sq[i].y2 - part_8, (sq[i].x1 + sq[i].x2) / 2 + part_8 / 2, sq[i].y2);
				glRectf(sq[i].x2 - part_8, (sq[i].y1 + sq[i].y2) / 2 - part_8 / 2, sq[i].x2, (sq[i].y1 + sq[i].y2) / 2 + part_8 / 2);
				glRectf(sq[i].x1, sq[i].y1, sq[i].x1 + part_8, sq[i].y1 + part_8);
				glRectf(sq[i].x1, sq[i].y2 - part_8, sq[i].x1 + part_8, sq[i].y2);
				glRectf(sq[i].x2 - part_8, sq[i].y2 - part_8, sq[i].x2, sq[i].y2);
				glRectf(sq[i].x2 - part_8, sq[i].y1, sq[i].x2, sq[i].y1 + part_8);
				  break;
			}
		}
	}
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
		if (timer_type == 1)
			timer_type = 0;
		else
		timer_type = 1;
		break;
	case '2':
		if (timer_type == 1)
			timer_type = 0;
		else
		timer_type = 2;
		break;
	case '3':
		if (timer_type == 1)
			timer_type = 0;
		else
		timer_type = 3;
		break;
	case '4':
		if (timer_type == 1)
			timer_type = 0;
		else
		timer_type = 4;
		break;
	case 'r':
		for (int i = 0; i < 10; ++i) {
			sq[i] = {};
		}
		timer_type = 0;
		create_sq(many(dre));
		glutPostRedisplay();
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}
GLvoid Mouse(int button, int state, int x, int y) 
{
	GLclampf mouseX = static_cast<GLclampf>(x) / (W / 2) - 1, mouseY = 1 - static_cast<GLclampf>(y) / (H / 2);

	if (state == GLUT_DOWN) {
		move = cheak_all(mouseX, mouseY);
		w_mouse = mouseX - sq[move].x1;
		h_mouse = mouseY - sq[move].y1;
	}
	if (state == GLUT_UP) {
		if (move != -1) {
			sq[move].part = 2;
			glutTimerFunc(50, remove_sq, move);
		}
		move = -1;
	}
	glutPostRedisplay();
}
GLvoid Motion(int x, int y) 
{
	GLclampf mouseX = static_cast<GLclampf>(x) / (W / 2) - 1, mouseY = 1 - static_cast<GLclampf>(y) / (H / 2);
	if (move != -1) {
		sq[move].x2 = sq[move].x2 - sq[move].x1;
		sq[move].y2 = sq[move].y2 - sq[move].y1;
		sq[move].x1 = mouseX - w_mouse;
		sq[move].y1 = mouseY - h_mouse;
		sq[move].x2 = sq[move].x2 + sq[move].x1;
		sq[move].y2 = sq[move].y2 + sq[move].y1;

		glutPostRedisplay();
	}
}
GLvoid timer_func (int value)
{
	for (int i = 0; i < 10; ++i) {
		if (sq[i].part == 2) {
			switch (value) {
			case 1:
				sq[i].x1 -= 0.05f;
				sq[i].y1 -= 0.05f;
				sq[i].x2 += 0.05f;
				sq[i].y2 += 0.05f;
				break;
			case 2:
				sq[i].x1 -= 0.05f;
				sq[i].y1 -= 0.05f;
				sq[i].x2 += 0.05f;
				sq[i].y2 += 0.05f;
				break;
			case 3:
				if (sq[i].x1 + sq[i].x2 >= 0) {
					sq[i].x1 += 0.05f;
					sq[i].x2 += 0.05f;
				}
				else {
					sq[i].x1 -= 0.05f;
					sq[i].x2 -= 0.05f;
				}
				if (sq[i].y1 + sq[i].y2 >= 0) {
					sq[i].y1 += 0.05f;
					sq[i].y2 += 0.05f;
				}
				else {
					sq[i].y1 -= 0.05f;
					sq[i].y2 -= 0.05f;
				}
				break;
			case 4:
				sq[i].x1 -= 0.05f;
				sq[i].y1 -= 0.05f;
				sq[i].x2 += 0.05f;
				sq[i].y2 += 0.05f;
				break;
			}
			sq[i].r -= 0.1;
			sq[i].g -= 0.1;
			sq[i].b -= 0.1;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(100, timer_func, timer_type); // (밀리세컨, 함수명, 함수에 전달할 값)
}
GLvoid remove_sq(int value) 
{
	sq[value].size -= 0.01;
	glutPostRedisplay();
	if (timer_type == 0) {
		sq[value].x1 += 0.0025f;
		sq[value].y1 += 0.0025f;
		sq[value].x2 -= 0.0025f;
		sq[value].y2 -= 0.0025f;
	}
	if (sq[value].size <= 0)
		sq[value].part = 0;
	else
		glutTimerFunc(50, remove_sq, value);
}