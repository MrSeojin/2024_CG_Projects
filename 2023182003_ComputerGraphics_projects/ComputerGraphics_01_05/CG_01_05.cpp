#include <iostream>
#include <random>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_int_distribution<int> many(20, 40);
std::uniform_real_distribution<GLclampf> color(0.0f, 1.0f), point(-1.0f, 1.0f);

struct sqare {
	GLclampf x1, x2, y1, y2, r, g, b;
	BOOL check;
};
sqare sq[40];
GLclampf remove_size = 0.5f, mouse_to_removeX, mouse_to_removeY;
int remove_sq = -1;

int inSqare(int i, int a) {		//충돌
	if (!sq[i].check)
		return -1;
	else if (sq[i].x1 <= sq[a].x1 && sq[a].x1 <= sq[i].x2 && sq[i].y1 <= sq[a].y1 && sq[a].y1 <= sq[i].y2)
		return i;
	else if (sq[i].x1 <= sq[a].x2 && sq[a].x2 <= sq[i].x2 && sq[i].y1 <= sq[a].y1 && sq[a].y1 <= sq[i].y2)
		return i;
	else if (sq[i].x1 <= sq[a].x1 && sq[a].x1 <= sq[i].x2 && sq[i].y1 <= sq[a].y2 && sq[a].y2 <= sq[i].y2)
		return i;
	else if (sq[i].x1 <= sq[a].x2 && sq[a].x2 <= sq[i].x2 && sq[i].y1 <= sq[a].y2 && sq[a].y2 <= sq[i].y2)
		return i;
	else
		return -1;
}
int pointerSqare(int i, GLclampf x, GLclampf y) {
	if (!sq[i].check)
		return -1;
	else if (sq[i].x1 <= x && x <= sq[i].x2 && sq[i].y1 <= y && y <= sq[i].y2)
		return i;
	else
		return -1;
}
int till_the_end(int start, int a) {
	for (int i = start; i < 40; ++i) {
		if (i == a);
		else if (!sq[i].check);
		else if (inSqare(i, a) != -1)
			return i;
		else if (inSqare(a, i) != -1)
			return i;
	}
	return -1;
}
void create_sq(int size) {
	for (int i = 0; i < size; ++i) {
		sq[i].x1 = point(dre);
		if (sq[i].x1 > 0.95f)
			sq[i].x1 -= 0.05f;
		sq[i].y1 = point(dre);
		if (sq[i].y1 > 0.95f)
			sq[i].y1 -= 0.05f;
		sq[i].x2 = sq[i].x1 + 0.05f;
		sq[i].y2 = sq[i].y1 + 0.05f;
		sq[i].r = color(dre);
		sq[i].g = color(dre);
		sq[i].b = color(dre);
		if (i != 0 && till_the_end(0, i) != -1)
			i--;
		else
			sq[i].check = true;
	}
}
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 
{ //--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(200, 0); // 윈도우의 위치 지정
	glutInitWindowSize(900, 900); // 윈도우의 크기 지정
	create_sq(many(dre));
	glutCreateWindow("Project_01_05"); // 윈도우 생성(윈도우 이름)
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutMainLoop(); // 이벤트 처리 시작
}
GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수 
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 바탕색을 ‘blue’로 지정
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	for (int i = 0; i < 40; ++i) {
		glColor3f(sq[i].r, sq[i].g, sq[i].b);
		if (sq[i].check)
			glRectf(sq[i].x1, sq[i].y1, sq[i].x2, sq[i].y2);
	}
	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
{
	glViewport(0, 0, w, h);
}
GLvoid Keyboard(unsigned char key, int x, int y) 
{
	switch (key) {
	case 'r':
		for (int i = 0; i < 40; ++i) {
			sq[i] = {};
		}
		create_sq(many(dre));
		glutPostRedisplay();
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
}
GLvoid Mouse(int button, int state, int x, int y) 
{
	GLclampf mouse_x = static_cast<GLclampf>(x) / 450 - 1,
		mouse_y = 1 - static_cast<GLclampf>(y) / 450;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		for (int i = 0; i < 40; ++i) {
			if (sq[i].check) {
				if (pointerSqare(i, mouse_x, mouse_y) != -1) {
					remove_sq = pointerSqare(i, mouse_x, mouse_y);
					break;
				}
			}
		}
		if (remove_sq != -1) {
			sq[remove_sq].x1 -= (sq[remove_sq].x2 - sq[remove_sq].x1) / 2;
			sq[remove_sq].x2 += (sq[remove_sq].x2 - sq[remove_sq].x1) / 2;
			sq[remove_sq].y1 -= (sq[remove_sq].y2 - sq[remove_sq].y1) / 2;
			sq[remove_sq].y2 += (sq[remove_sq].y2 - sq[remove_sq].y1) / 2;
			mouse_to_removeX = mouse_x - sq[remove_sq].x1;
			mouse_to_removeY = mouse_y - sq[remove_sq].y1;
		}
	}
	else if (state == GLUT_UP) {
		/*
		sq[remove_sq].x1 += (sq[remove_sq].x2 - sq[remove_sq].x1) / 2;
		sq[remove_sq].x2 -= (sq[remove_sq].x2 - sq[remove_sq].x1) / 2;
		sq[remove_sq].y1 += (sq[remove_sq].y2 - sq[remove_sq].y1) / 2;
		sq[remove_sq].y2 -= (sq[remove_sq].y2 - sq[remove_sq].y1) / 2;
		if (sq[remove_sq].x2 - sq[remove_sq].x1 < 0.5f){
			sq[remove_sq].x1 = (sq[remove_sq].x1 + sq[remove_sq].x2) / 2 - 0.025;
			sq[remove_sq].x2 = sq[remove_sq].x1 + 0.05;
		}
		if (sq[remove_sq].y2 - sq[remove_sq].y1 < 0.5f) {
			sq[remove_sq].y1 = (sq[remove_sq].y1 + sq[remove_sq].y2) / 2 - 0.025;
			sq[remove_sq].y2 = sq[remove_sq].y1 + 0.05;
		}
		*/
		sq[remove_sq].x1 = (sq[remove_sq].x1 + sq[remove_sq].x2) / 2 - 0.025;
		sq[remove_sq].y1 = (sq[remove_sq].y1 + sq[remove_sq].y2) / 2 - 0.025;
		sq[remove_sq].x2 = sq[remove_sq].x1 + 0.05;
		sq[remove_sq].y2 = sq[remove_sq].y1 + 0.05;
		remove_sq = -1;
	}
	glutPostRedisplay();
}
GLvoid Motion(int x, int y)
{
	GLclampf mouse_x = static_cast<GLclampf>(x) / 450 - 1,
		mouse_y = 1 - static_cast<GLclampf>(y) / 450;
	if (remove_sq != -1) {
		sq[remove_sq].x2 = sq[remove_sq].x2 - sq[remove_sq].x1;
		sq[remove_sq].y2 = sq[remove_sq].y2 - sq[remove_sq].y1;
		sq[remove_sq].x1 = mouse_x - mouse_to_removeX;
		sq[remove_sq].y1 = mouse_y - mouse_to_removeY;
		sq[remove_sq].x2 = sq[remove_sq].x2 + sq[remove_sq].x1;
		sq[remove_sq].y2 = sq[remove_sq].y2 + sq[remove_sq].y1;
		while (till_the_end(0, remove_sq) != -1) {
			int remove_index = till_the_end(0, remove_sq);
			sq[remove_sq].r = sq[remove_index].r;
			sq[remove_sq].g = sq[remove_index].g;
			sq[remove_sq].b = sq[remove_index].b;
			sq[remove_sq].x1 -= (sq[remove_index].x2 - sq[remove_index].x1) / 2;
			sq[remove_sq].x2 += (sq[remove_index].x2 - sq[remove_index].x1) / 2;
			sq[remove_sq].y1 -= (sq[remove_index].y2 - sq[remove_index].y1) / 2;
			sq[remove_sq].y2 += (sq[remove_index].y2 - sq[remove_index].y1) / 2;
			mouse_to_removeX = mouse_x - sq[remove_sq].x1;
			mouse_to_removeY = mouse_y - sq[remove_sq].y1;
			sq[remove_index] = {};
		}
		glutPostRedisplay();
	}
}
