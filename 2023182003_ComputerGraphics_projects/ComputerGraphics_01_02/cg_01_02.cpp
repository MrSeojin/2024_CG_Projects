#include <iostream>
#include <random>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/GL.h>

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_real_distribution<GLclampf> uid(0.0f, 1.0f);

GLfloat width = 1200, height = 900;
GLfloat backGround_r = 0.0f, backGround_g = 0.0f, backGround_b = 1.0f;
typedef struct sqare{
	float x1, y1, x2, y2, r, g, b;
}sq;
sq back_sq[4];
void set_backSqare()
{
	for (int i = 0; i < 4; ++i) {
		if (i % 2 == 1) {//짝수 x가 뒷 부분
			back_sq[i].x1 = width / 2;
			back_sq[i].x2 = width;
		}
		else {
			back_sq[i].x1 = 0;
			back_sq[i].x2 = width / 2;
		}
		if (i < 2) {//위의 두 네모
			back_sq[i].y1 = 0;
			back_sq[i].y2 = height / 2;
		}
		else {
			back_sq[i].y1 = height / 2;
			back_sq[i].y2 = height;
		}
		back_sq[i].r = uid(dre);
		back_sq[i].g = uid(dre);
		back_sq[i].b = uid(dre);
	}
}

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Mouse(int button, int state, int x, int y);
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 
{ //--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(0, 0); // 윈도우의 위치 지정
	glutInitWindowSize(width, height); // 윈도우의 크기 지정
	set_backSqare();
	glutCreateWindow("Project_01_02"); // 윈도우 생성(윈도우 이름)
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
	glutMouseFunc(Mouse);
	glutMainLoop(); // 이벤트 처리 시작
}
GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수 
{
	glClearColor(backGround_r, backGround_g, backGround_b, 1.0f); // 바탕색을 ‘blue’로 지정
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	// 사각형 그리기 ------------------------------------------------
	for (int i = 0; i < 4; ++i) {
		glColor3f(back_sq[i].r, back_sq[i].g, back_sq[i].b);
		glRectf((back_sq[i].x1 - width / 2) / (width / 2), (height / 2 - back_sq[i].y2) / (height / 2), (back_sq[i].x2 - width / 2) / (width / 2), (height / 2 - back_sq[i].y1) / (height / 2));
	}
	// --------------------------------------------------------------
	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
{
	glViewport(0, 0, w, h);
}
void Mouse(int button, int state, int x, int y) 
{
	if (state == GLUT_DOWN) {
		for (int i = 0; i < 4; ++i) {
			if (button == GLUT_LEFT_BUTTON) {
				if (back_sq[i].x1 < x && x < back_sq[i].x2 &&
					back_sq[i].y1 < y && y < back_sq[i].y2) {
					back_sq[i].r = uid(dre);
					back_sq[i].g = uid(dre);
					back_sq[i].b = uid(dre);
					break;
				}
				else if (i == 3) {
					backGround_r = uid(dre);
					backGround_g = uid(dre);
					backGround_b = uid(dre);
					// glColor3f(backGround_r, backGround_g, backGround_b);
				}
				// (i % 2 * width / 2 < x && x < (i / 2 + 1) * width / 2
				// && i / 2 * height / 2 < y && y < (i / 2 + 1) * height / 2)
			}
			else if (button == GLUT_RIGHT_BUTTON) {
				if (back_sq[i].x1 < x && x < back_sq[i].x2 &&
					back_sq[i].y1 < y && y < back_sq[i].y2) {
					back_sq[i].x1 += 5;
					back_sq[i].x2 -= 5;
					back_sq[i].y1 += 5;
					back_sq[i].y2 -= 5;
					if (back_sq[i].x1 >= back_sq[i].x2 || back_sq[i].y1 >= back_sq[i].y2) {
						back_sq[i].x1 -= 5;
						back_sq[i].x2 += 5;
						back_sq[i].y1 -= 5;
						back_sq[i].y2 += 5;
					}
					break;
				}
				else if (i == 3) {
					for (int j = 0; j < 4; ++j) {
						if (back_sq[j].x2 - back_sq[j].x1 < width / 2) {
							back_sq[j].x1 -= 5;
							back_sq[j].x2 += 5;
						}
						else {
							if (j % 2 == 1) {//짝수 x가 뒷 부분
								back_sq[j].x1 = width / 2;
								back_sq[j].x2 = width;
							}
							else {
								back_sq[j].x1 = 0;
								back_sq[j].x2 = width / 2;
							}
						}
						if (back_sq[j].y2 - back_sq[j].y1 < height / 2) {
							back_sq[j].y1 -= 5;
							back_sq[j].y2 += 5;
						}
						else {
							if (j < 2) {//위의 두 네모
								back_sq[j].y1 = 0;
								back_sq[j].y2 = height / 2;
							}
							else {
								back_sq[j].y1 = height / 2;
								back_sq[j].y2 = height;
							}
						}
					}
				}
			}
		}
	}
	glutPostRedisplay();
}
