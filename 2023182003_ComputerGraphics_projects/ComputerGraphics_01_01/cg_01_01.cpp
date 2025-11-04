#include <iostream>
#include <random>
#include <gl\glew.h>
#include <gl\freeglut.h>
#include <gl\freeglut_ext.h>

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_real_distribution<GLclampf> uid(0.0f, 1.0f);
GLclampf backGround_r = 0.0f, backGround_g = 0.0f, backGround_b = 1.0f, backGround_a = 1.0f;
int to_out_change_color = 0;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void change_random_color(int value);

void main(int argc, char** argv)	// 윈도우 생성
{
	glutInit(&argc, argv);							// glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);	// 디스플레이 모드 설정
	glutInitWindowPosition(0, 0);					// 윈도우의 위치 지정
	glutInitWindowSize(800, 600);					// 윈도우의 크기 지정
	glutCreateWindow("project_01_01");				// 윈도우 생성 (윈도우 이름)
	
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
	glutMainLoop();									// 이벤트 처리 시작
}

GLvoid drawScene()
{
	glClearColor(backGround_r, backGround_g, backGround_b, backGround_a);			// 바탕색을 파랑색으로 지정
	glClear(GL_COLOR_BUFFER_BIT);					// 설정된 색으로 전체를 칠하기
	// 그리기 부분 구현: 그리기 관련 부분이 여기에 포함된다
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
		backGround_r = 0.0f;
		backGround_g = 1.0f;
		backGround_b = 1.0f;
		break; //--- 배경색을 청록색으로 설정
	case 'm':
		backGround_r = 1.0f;
		backGround_g = 0.0f;
		backGround_b = 1.0f;
		break; //--- 배경색을 자홍색으로 설정
	case 'y':
		backGround_r = 1.0f;
		backGround_g = 1.0f;
		backGround_b = 0.0f;
		break; //--- 배경색을 노랑색으로 설정
	case 'a':
		backGround_r = uid(dre);
		backGround_g = uid(dre);
		backGround_b = uid(dre);
		break;
	case 'w':
		backGround_r = 1.0f;
		backGround_g = 1.0f;
		backGround_b = 1.0f;
		break;
	case 'k':
		backGround_r = 0.0f;
		backGround_g = 0.0f;
		backGround_b = 0.0f;
		break;
	case 't':
		to_out_change_color = 1;
		glutTimerFunc(100, change_random_color, to_out_change_color); // (밀리세컨, 함수명, 함수에 전달할 값)
		break;
	case 's':
		to_out_change_color = 0;
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}
void change_random_color(int value) 
{
	backGround_r = uid(dre);
	backGround_g = uid(dre);
	backGround_b = uid(dre);
	glutPostRedisplay();
	if (to_out_change_color)
		glutTimerFunc(100, change_random_color, to_out_change_color); // (밀리세컨, 함수명, 함수에 전달할 값)
}