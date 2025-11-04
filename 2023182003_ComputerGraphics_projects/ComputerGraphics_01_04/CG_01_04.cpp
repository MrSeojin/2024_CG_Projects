#include <iostream>
#include <random>
#include <math.h>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_real_distribution<GLclampf> color(0.0f, 1.0f), point(-0.1f, 0.1f);

struct sqare {
	GLclampf x1, y1, x2, y2, r, g, b, length;
	int dir;
	BOOL check;
};
sqare sq[5], return_sq[5];
GLclampf size_x, size_y;
int moving_form, to_out_move_sqare = false;

GLvoid drawScene (GLvoid);
GLvoid Reshape (int w, int h);
GLvoid Keyboard (unsigned char key, int x, int y);
GLvoid move_sqare (int value);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid move_before (int value);
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 
{ //--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(0, 0); // 윈도우의 위치 지정
	glutInitWindowSize(900, 900); // 윈도우의 크기 지정
	glutCreateWindow("Project_01_04"); // 윈도우 생성(윈도우 이름)
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
	glutMainLoop(); // 이벤트 처리 시작
}
GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수 
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // 바탕색을 ‘blue’로 지정
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	for (int i = 0; i < 5; ++i) {
		if (sq[i].check) {
			glColor3f(sq[i].r, sq[i].g, sq[i].b);
			glRectf(sq[i].x1 - size_x / 4, sq[i].y2 - size_y / 4, sq[i].x2 + size_x / 4, sq[i].y1 + size_y / 4);
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
	case '1':
		if (to_out_move_sqare&& moving_form == 1)
			to_out_move_sqare = false;
		else if (!to_out_move_sqare) {
			moving_form = 1;
			to_out_move_sqare = true;
			glutTimerFunc(100, move_sqare, moving_form);
		}
		else
			moving_form = 1;
		break;
	case '2':
		if (to_out_move_sqare && moving_form == 2)
			to_out_move_sqare = false;
		else if (!to_out_move_sqare) {
			moving_form = 2;
			to_out_move_sqare = true;
			glutTimerFunc(100, move_sqare, moving_form);
		}
		else
			moving_form = 2;
		break;
	case '3':
		if (to_out_move_sqare && moving_form == 3)
			to_out_move_sqare = false;
		else if (!to_out_move_sqare) {
			moving_form = 3;
			to_out_move_sqare = true;
			glutTimerFunc(100, move_sqare, moving_form);
		}
		else
			moving_form = 3;
		break;
	case '4':
		if (to_out_move_sqare && moving_form == 4)
			to_out_move_sqare = false;
		else if (!to_out_move_sqare) {
			moving_form = 4;
			to_out_move_sqare = true;
			glutTimerFunc(100, move_sqare, moving_form);
		}
		else
			moving_form = 4;
		break;
	case 's':
		to_out_move_sqare = false;
		break;
	case 'm':
		/*
		for (int i = 0; i < 5; ++i) 
			sq[i] = return_sq[i];
		size_x = 0;
		size_y = 0;
		glutPostRedisplay();
		*/
			/*
		for (int i = 0; i < 5; ++i) {
			if (sq[i].check) {
				GLclampf before_x = (sq[i].x1 + sq[i].x2) / 2 - (return_sq[i].x1 + return_sq[i].x2) / 2,
					before_y = (sq[i].y1 + sq[i].y2) / 2 - (return_sq[i].y1 + return_sq[i].y2) / 2;
				return_sq[i].length = sqrt(before_x * before_x + before_y * before_y);
				glutTimerFunc(100, move_before, i);
			}
		}
			*/
		if (to_out_move_sqare) 
			to_out_move_sqare = false;
		for (int i = 0; i < 5; ++i) {
			return_sq[i].x1 -= sq[i].x1;
			return_sq[i].x2 -= sq[i].x2;
			return_sq[i].y1 -= sq[i].y1;
			return_sq[i].y2 -= sq[i].y2;
			return_sq[i].r -= sq[i].r;
			return_sq[i].g -= sq[i].g;
			return_sq[i].b -= sq[i].b;
		}
		glutTimerFunc(100, move_before, 10);
		break;
	case 'r':
		for (int i = 0; i < 5; ++i) {
			sq[i].check = false;
			return_sq[i].check = false;
		}
		size_x = 0;
		size_y = 0;
		glutPostRedisplay();
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
}
GLvoid move_sqare(int value)
{
	switch (value) {
	case 1:	// 위치 변화1-> 사각형들은 각각 대각선으로 이동하고 벽에 닿으면 튕겨 다른 방향으로 이동
		for (int i = 0; i < 5; ++i) {
			if (sq[i].check) {
				switch (sq[i].dir) {
				case 1:	// cross right up
					sq[i].x1 += 0.05f;
					sq[i].x2 += 0.05f;
					sq[i].y1 += 0.05f;
					sq[i].y2 += 0.05f;
					if (sq[i].x2 >= 1.0f || sq[i].y1 >= 1.0f) 
						sq[i].dir = 2;
					if (sq[i].x2 > 1.0f) {
						sq[i].x1 = sq[i].x1 - (sq[i].x2 - 1.0f);
						sq[i].x2 = 1.0f;
					}
					if (sq[i].y1 > 1.0f) {
						sq[i].y2 = sq[i].y2 - (sq[i].y1 - 1.0f);
						sq[i].y1 = 1.0f;
					}
					break;
				case 2:	// cross right down
					sq[i].x1 += 0.05f;
					sq[i].x2 += 0.05f;
					sq[i].y1 -= 0.05f;
					sq[i].y2 -= 0.05f;
					if (sq[i].x2 >= 1.0f || sq[i].y2 <= -1.0f) 
						sq[i].dir = 3;
					if (sq[i].x2 > 1.0f) {
						sq[i].x1 = sq[i].x1 - (sq[i].x2 - 1.0f);
						sq[i].x2 = 1.0f;
					}
					if (sq[i].y2 < -1.0f) {
						sq[i].y1 = sq[i].y1 - (sq[i].y2 + 1.0f);
						sq[i].y2 = -1.0f;
					}
					break;
				case 3:	// cross left down
					sq[i].x1 -= 0.05f;
					sq[i].x2 -= 0.05f;
					sq[i].y1 -= 0.05f;
					sq[i].y2 -= 0.05f;
					if (sq[i].x1 <= -1.0f || sq[i].y2 <= -1.0f)
						sq[i].dir = 4;
					if (sq[i].x1 < -1.0f) {
						sq[i].x2 = sq[i].x2 - (sq[i].x1 + 1.0f);
						sq[i].x1 = -1.0f;
					}
					if (sq[i].y2 < -1.0f) {
						sq[i].y1 = sq[i].y1 - (sq[i].y2 + 1.0f);
						sq[i].y2 = -1.0f;
					}
					break;
				case 4:	// cross left up
					sq[i].x1 -= 0.05f;
					sq[i].x2 -= 0.05f;
					sq[i].y1 += 0.05f;
					sq[i].y2 += 0.05f;
					if (sq[i].x1 <= -1.0f || sq[i].y1 >= 1.0f)
						sq[i].dir = 1;
					if (sq[i].x1 < -1.0f) {
						sq[i].x2 = sq[i].x2 - (sq[i].x1 + 1.0f);
						sq[i].x1 = -1.0f;
					}
					if (sq[i].y1 > 1.0f) {
						sq[i].y2 = sq[i].y2 - (sq[i].y1 - 1.0f);
						sq[i].y1 = 1.0f;
					}
					break;
				}
			}
		}
		break;
	case 2: // 위치 변화2-> 사각형들이 지그재그로 이동
		for (int i = 0; i < 5; ++i) {
			if (sq[i].check) {
				switch (sq[i].dir) {
				case 1:	// cross right up
					sq[i].x1 += 0.1f;
					sq[i].x2 += 0.1f;
					sq[i].y1 += 0.05f;
					sq[i].y2 += 0.05f;
					if (sq[i].x2 > 1.0f) {
						sq[i].x1 = sq[i].x1 - (sq[i].x2 - 1.0f);
						sq[i].x2 = 1.0f;
					}
					if (sq[i].y1 > 1.0f) {
						sq[i].y2 = sq[i].y2 - (sq[i].y1 - 1.0f);
						sq[i].y1 = 1.0f;
					}
					if (sq[i].y1 == 1.0f)
						sq[i].dir += 2;
					else if(sq[i].x2 == 1.0f)
						sq[i].dir += 1;
					break;
				case 2:	// cross left up
					sq[i].x1 -= 0.1f;
					sq[i].x2 -= 0.1f;
					sq[i].y1 += 0.05f;
					sq[i].y2 += 0.05f;
					if (sq[i].x1 < -1.0f) {
						sq[i].x2 = sq[i].x2 - (sq[i].x1 + 1.0f);
						sq[i].x1 = -1.0f;
					}
					if (sq[i].y1 > 1.0f) {
						sq[i].y2 = sq[i].y2 - (sq[i].y1 - 1.0f);
						sq[i].y1 = 1.0f;
					}
					if (sq[i].y1 == 1.0f)
						sq[i].dir += 2;
					else if (sq[i].x1 == -1.0f)
						sq[i].dir -= 1;
					break;
				case 3:	// cross right down
					sq[i].x1 += 0.1f;
					sq[i].x2 += 0.1f;
					sq[i].y1 -= 0.05f;
					sq[i].y2 -= 0.05f;
					if (sq[i].x2 > 1.0f) {
						sq[i].x1 = sq[i].x1 - (sq[i].x2 - 1.0f);
						sq[i].x2 = 1.0f;
					}
					if (sq[i].y2 < -1.0f) {
						sq[i].y1 = sq[i].y1 - (sq[i].y2 + 1.0f);
						sq[i].y2 = -1.0f;
					}
					if (sq[i].y2 == -1.0f)
						sq[i].dir -= 2;
					else if (sq[i].x2 == 1.0f)
						sq[i].dir += 1;
					break;
				case 4:	// cross left down
					sq[i].x1 -= 0.1f;
					sq[i].x2 -= 0.1f;
					sq[i].y1 -= 0.05f;
					sq[i].y2 -= 0.05f;
					if (sq[i].x1 < -1.0f) {
						sq[i].x2 = sq[i].x2 - (sq[i].x1 + 1.0f);
						sq[i].x1 = -1.0f;
					}
					if (sq[i].y2 < -1.0f) {
						sq[i].y1 = sq[i].y1 - (sq[i].y2 + 1.0f);
						sq[i].y2 = -1.0f;
					}
					if (sq[i].y2 == -1.0f)
						sq[i].dir -= 2;
					else if(sq[i].x1 == -1.0f)
						sq[i].dir -= 1;
					break;
				}
			}
		}
		break;
	case 3:	// 크기 변화 -> 사각형의 크기가 다양하게 변화
		size_x = point(dre);
		size_y = point(dre);
		break;
	case 4:	// 색상 변화 -> 사각형의 색상이 랜덤하게 변화
		for (int i = 0; i < 5; ++i) {
			if (sq[i].check) {
				sq[i].r = color(dre);
				sq[i].g = color(dre);
				sq[i].b = color(dre);
			}
		}
		break;
	}
	glutPostRedisplay();
	if (to_out_move_sqare)
		glutTimerFunc(100, move_sqare, moving_form); // (밀리세컨, 함수명, 함수에 전달할 값)
}
GLvoid Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		for (int i = 0; i < 5; ++i) {
			if (!sq[i].check) {
				GLclampf center_x = static_cast<GLclampf>(x) / 450 - 1, center_y = 1 - static_cast<GLclampf>(y) / 450;
				sq[i].check = true;
				sq[i].x1 = center_x - 0.1f;
				sq[i].x2 = center_x + 0.1f;
				sq[i].y1 = center_y + 0.1f;
				sq[i].y2 = center_y - 0.1f;
				sq[i].dir = 1;
				sq[i].r = color(dre);
				sq[i].g = color(dre);
				sq[i].b = color(dre);
				return_sq[i] = sq[i];
				break;
			}
		}
	}
	glutPostRedisplay();
}
GLvoid move_before(int value) {
	for (int i = 0; i < 5; ++i) {
		sq[i].x1 += return_sq[i].x1 / 10;
		sq[i].x2 += return_sq[i].x2 / 10;
		sq[i].y1 += return_sq[i].y1 / 10;
		sq[i].y2 += return_sq[i].y2 / 10;
		sq[i].r += return_sq[i].r / 10;
		sq[i].g += return_sq[i].g / 10;
		sq[i].b += return_sq[i].b / 10;
	}
	size_x /= 2;
	size_y /= 2;
	glutPostRedisplay();
	if (value == 0) {
		for (int i = 0; i < 5; ++i)
			return_sq[i] = sq[i];
		size_x = 0;
		size_y = 0;
	}
	else 
		glutTimerFunc(100, move_before, --value);
}
