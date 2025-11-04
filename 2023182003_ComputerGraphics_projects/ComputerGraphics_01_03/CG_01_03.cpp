#include <iostream>
#include <random>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_real_distribution<GLclampf> uid(0.0f, 1.0f), point(-0.8f, 0.8f);

struct sqare {
	GLclampf x1, y1, x2, y2, r, g, b;
	BOOL check;
};
GLclampf width = 1200, height = 800, mouseX, mouseY;
sqare sq[10];
BOOL left_button;
int move_sq;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
int inSqare(int i, int a) {		//충돌
	if (!sq[i].check)
		return -1;
	else if (sq[i].x1 <= sq[a].x1 && sq[a].x1 <= sq[i].x2 && sq[i].y1 >= sq[a].y1 && sq[a].y1 >= sq[i].y2)
		return i;
	else if (sq[i].x1 <= sq[a].x2 && sq[a].x2 <= sq[i].x2 && sq[i].y1 >= sq[a].y1 && sq[a].y1 >= sq[i].y2)
		return i;
	else if (sq[i].x1 <= sq[a].x1 && sq[a].x1 <= sq[i].x2 && sq[i].y1 >= sq[a].y2 && sq[a].y2 >= sq[i].y2)
		return i; 
	else if (sq[i].x1 <= sq[a].x2 && sq[a].x2 <= sq[i].x2 && sq[i].y1 >= sq[a].y2 && sq[a].y2 >= sq[i].y2)
		return i;
	else
		return -1;
}
int pointerSqare(int i, GLclampf x, GLclampf y) {
	if (!sq[i].check)
		return -1;
	else if (sq[i].x1 <= x && x <= sq[i].x2 && sq[i].y1 >= y && y >= sq[i].y2)
		return i;
	else
		return -1;
}
int till_the_end(int start, int a) {
	for (int i = start; i < 10; ++i) {
		if (i == a);
		else if (!sq[i].check)
			return -1;
		else if (inSqare(i, a) != -1)
			return i;
		else if (inSqare(a, i) != -1)
			return i;
	}
	return -1;
}

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 
{ //--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치 지정
	glutInitWindowSize(width, height); // 윈도우의 크기 지정
	glutCreateWindow("Project_01_03"); // 윈도우 생성(윈도우 이름)
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
	for (int i = 0; i < 10; ++i) {
		if (sq[i].check) {
			glColor3f(sq[i].r, sq[i].g, sq[i].b);
			glRectf(sq[i].x1, sq[i].y2, sq[i].x2, sq[i].y1);
		}
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
	case 'a':
	{
		int add_index;
		for (int i = 0; i < 10; ++i) {
			if (!sq[i].check) {
				add_index = i;
				break;
			}
			else if (i == 9)
				add_index = -1;
		}
		if (add_index != -1) {
			sq[add_index].r = uid(dre);
			sq[add_index].b = uid(dre);
			sq[add_index].g = uid(dre);
			sq[add_index].x1 = point(dre);
			sq[add_index].y1 = point(dre);
			sq[add_index].x2 = sq[add_index].x1 + 0.1f;
			sq[add_index].y2 = sq[add_index].y1 - 0.1f;
			while (till_the_end(0, add_index) != -1) {
				sq[add_index].x1 = point(dre);
				sq[add_index].y1 = point(dre);
				sq[add_index].x2 = sq[add_index].x1 + 0.1f;
				sq[add_index].y2 = sq[add_index].y1 - 0.1f;
			}
			sq[add_index].check = true;
		}
		glutPostRedisplay();
	}
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
}
GLvoid Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		left_button = true;
		mouseX = static_cast<GLclampf>(x) / (width / 2) - 1;
		mouseY = 1 - static_cast<GLclampf>(y) / (height / 2);
		for (int i = 0; i < 10; ++i) {
			if (pointerSqare(i, mouseX, mouseY) != -1) {
				move_sq = i;
				break;
			}
			else if (i == 9)
				move_sq = -1;
		}
	}
	else if (state == GLUT_UP && left_button) {
		left_button = false;
		if (move_sq == -1);
		else {
			while (till_the_end(0, move_sq) != -1) {
				int unite_index = till_the_end(0, move_sq);
				GLclampf new_x1, new_x2, new_y1, new_y2;
				if (sq[move_sq].x1 <= sq[unite_index].x1)
					new_x1 = sq[move_sq].x1;
				else
					new_x1 = sq[unite_index].x1;
				if (sq[move_sq].x2 >= sq[unite_index].x2)
					new_x2 = sq[move_sq].x2;
				else
					new_x2 = sq[unite_index].x2;
				if (sq[move_sq].y1 >= sq[unite_index].y1)
					new_y1 = sq[move_sq].y1;
				else
					new_y1 = sq[unite_index].y1;
				if (sq[move_sq].y2 < sq[unite_index].y2)
					new_y2 = sq[move_sq].y2;
				else
					new_y2 = sq[unite_index].y2;
				if (unite_index == 9 || move_sq == 9)
					sq[9].check = false;
				if (move_sq < unite_index) {
					for (int i = unite_index; i < 9; ++i) {
						if (!sq[i].check)
							break;
						sq[i] = sq[i + 1];
						if (i == 8)
							sq[9].check = false;
					}
					for (int i = move_sq; i < 9; ++i) {
						if (!sq[i].check)
							break;
						sq[i] = sq[i + 1];
						if (i == 8)
							sq[9].check = false;
					}
				}
				else {
					for (int i = move_sq; i < 9; ++i) {
						if (!sq[i].check)
							break;
						sq[i] = sq[i + 1];
						if (i == 8)
							sq[9].check = false;
					}
					if(unite_index == 9)
						sq[9].check = false;
					for (int i = unite_index; i < 9; ++i) {
						if (!sq[i].check)
							break;
						sq[i] = sq[i + 1];
						if (i == 8) 
							sq[9].check = false;
					}
				}
				for (int i = 0; i < 10; ++i) {
					if (!sq[i].check) {
						sq[i].x1 = new_x1;
						sq[i].x2 = new_x2;
						sq[i].y1 = new_y1;
						sq[i].y2 = new_y2;
						sq[i].check = true;
						sq[i].r = uid(dre);
						sq[i].g = uid(dre);
						sq[i].b = uid(dre);
						move_sq = i;
						break;
					}
				}
			}
		}
	}
}
GLvoid Motion(int x, int y) {
	GLclampf pointX = static_cast<GLclampf>(x) / (width / 2) - 1, pointY = 1 - static_cast<GLclampf>(y) / (height / 2);
	if (left_button == true) {
		GLclampf moveSizeX = mouseX - pointX, moveSizeY = mouseY - pointY;
		sq[move_sq].x1 -= moveSizeX;
		sq[move_sq].y1 -= moveSizeY;
		sq[move_sq].x2 -= moveSizeX;
		sq[move_sq].y2 -= moveSizeY;
		glutPostRedisplay();
	}
	mouseX = pointX;
	mouseY = pointY;
}