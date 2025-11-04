#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <Windows.h>
#include <random>

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_real_distribution<GLclampf> rd_color(0.0f, 1.0f), rd_place(-0.95f, 0.95f);

GLfloat camera_move[3]{ 0.0f,0.0f,1.0f };
struct Ball {
    GLfloat x, y, z, speed;
    int dir = -1;
    bool check;
};
Ball ball[5] = {};

GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid KeyBoard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
GLvoid TimerFunc(int vlaue);

void InitBuffer();
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
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
};//정육면체, 축,정사면체 벡터들
float vertexColor[] = {
   1,1,1,
   1,1,1,
   1,1,1,
   1,1,1,

   1,1,1,
   1,1,1,
   1,1,1,
   1,1,1,

   1,1,1,
   1,1,1,
   1,1,1,
   1,1,1,

   1,1,1,
   1,1,1,
   1,1,1,
   1,1,1,

   1,1,1,
   1,1,1,
   1,1,1,
   1,1,1,

   1,1,1,
   1,1,1,
   1,1,1,
   1,1,1
};//정육면체, 축,정사면체 색깔들

GLchar* vertexSource, * fragmentSource;
GLuint shaderID;
GLuint vertexShader;
GLuint fragmentShader;

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("CG_01_22");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
        std::cout << "GLEW Initialized\n";

    make_vertexShaders();
    make_fragmentShaders();
    shaderID = make_shaderProgram();
    InitBuffer();
    glutKeyboardFunc(KeyBoard);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutTimerFunc(10, TimerFunc, 1);
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMainLoop();
}
void make_vertexShaders()
{
    vertexSource = filetobuf("vertexShaderSource.glsl");
    //--- 버텍스 세이더 객체 만들기
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //--- 세이더 코드를 세이더 객체에 넣기
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
    //--- 버텍스 세이더 컴파일하기
    glCompileShader(vertexShader);
    //--- 컴파일이 제대로 되지 않은 경우: 에러 체크
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
        return;
    }
}
void make_fragmentShaders()
{
    fragmentSource = filetobuf("fragmentShaderSource.glsl");
    //--- 프래그먼트 세이더 객체 만들기
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //--- 세이더 코드를 세이더 객체에 넣기
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
    //--- 프래그먼트 세이더 컴파일
    glCompileShader(fragmentShader);
    //--- 컴파일이 제대로 되지 않은 경우: 컴파일 에러 체크
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
        return;
    }
}
GLuint make_shaderProgram()
{
    GLuint ShaderProgramID;

    ShaderProgramID = glCreateProgram();
    glAttachShader(ShaderProgramID, vertexShader);
    glAttachShader(ShaderProgramID, fragmentShader);
    glLinkProgram(ShaderProgramID);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint result;
    GLchar errorLog[512];

    glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(ShaderProgramID, 512, NULL, errorLog);
        std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
        exit(-1);
    }
    glUseProgram(ShaderProgramID);

    return ShaderProgramID;
}
GLuint cube, vbo[2];
void InitBuffer()
{
    glGenVertexArrays(1, &cube); //--- VAO 를 지정하고 할당하기
    glGenBuffers(2, vbo); //--- 2개의 VBO를 지정하고 할당하기
    glBindVertexArray(cube); //--- VAO를 바인드하기

    //--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    //--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
    //--- triShape 배열의 사이즈: 9 * float
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexPosition), vertexPosition, GL_STATIC_DRAW);
    //--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //--- attribute 인덱스 0번을 사용가능하게 함
    glEnableVertexAttribArray(0);

    //--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    //--- 변수 colors에서 버텍스 색상을 복사한다.
    //--- colors 배열의 사이즈: 9 *float 
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexColor), vertexColor, GL_STATIC_DRAW);
    //--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //--- attribute 인덱스 1번을 사용 가능하게 함.
    glEnableVertexAttribArray(1);
}

int y_dir, under, before_mouse;
bool l_button, open;
GLfloat background_r = 0.2f, background_g = 0.2f, background_b = 0.2f, y_radian, z_radian, z_move, open_radian1, open_radian2, object_move;

GLvoid drawScene() 
{
    glUseProgram(shaderID);

    glClearColor(background_r, background_g, background_b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//와이어 객체
    glEnable(GL_CULL_FACE);

    unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform");//월드 변환 행렬값을 셰이더의 uniform mat4 modelTransform에게 넘겨줌
    unsigned int viewLocation = glGetUniformLocation(shaderID, "viewTransform");//위와 동일
    unsigned int projectionLocation = glGetUniformLocation(shaderID, "projectionTransform");//위와 동일
    unsigned int colorLocation = glGetUniformLocation(shaderID, "objectColor");//위와 동일

    glm::mat4 white = glm::mat4(1.0f);
    white = glm::scale(white, glm::vec3(1.0, 1.0, 1.0));
    glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(white));

    //원근 투영
    glm::mat4 kTransform = glm::mat4(1.0f);
    kTransform = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
    kTransform = glm::translate(kTransform, glm::vec3(0.0, 0.0, -4.0f));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &kTransform[0][0]);

    //뷰잉 변환 
    glm::mat4 vTransform = glm::mat4(1.0f);
    glm::vec3 cameraPos = glm::vec3(camera_move[0], camera_move[1], camera_move[2] - z_move); //--- 카메라 위치
    glm::vec3 cameraDirection = glm::vec3(camera_move[0], camera_move[1], -1.0f + camera_move[2] - z_move); //--- 카메라 바라보는 방향
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향

    vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);


    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //축
    glm::mat4 axisTransForm = glm::mat4(1.0f);//변환 행렬 생성 T
    axisTransForm = glm::rotate(axisTransForm, glm::radians(z_radian), glm::vec3(0.0, 0.0, 1.0));
    axisTransForm = glm::rotate(axisTransForm, glm::radians(y_radian), glm::vec3(0.0, 1.0, 0.0));
    /*안쪽이 보이는 직육면체*/ {
        glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
        glm::mat4 cube = glm::mat4(1.0f);//변환 행렬 생성 T
        cube = glm::translate(cube, glm::vec3(0.0f, 0.0f, 0.0f));
        cube = glm::scale(cube, glm::vec3(1.0, 1.0, 1.0));
        model = axisTransForm * cube;
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
        glCullFace(GL_FRONT);
         {
            glm::mat4 set_color = glm::mat4(1.0f);
            set_color = glm::scale(set_color, glm::vec3(0.5, 0.5, 0.5));
            glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4); //정육면체
        } {
            glm::mat4 set_color = glm::mat4(1.0f);
            set_color = glm::scale(set_color, glm::vec3(0.4, 0.4, 0.4));
            glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
            glDrawArrays(GL_TRIANGLE_FAN, 4, 4); //정육면체
        } {
            glm::mat4 set_color = glm::mat4(1.0f);
            set_color = glm::scale(set_color, glm::vec3(0.3, 0.3, 0.3));
            glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
            glDrawArrays(GL_TRIANGLE_FAN, 8, 4); //정육면체
        } {
            glm::mat4 set_color = glm::mat4(1.0f);
            set_color = glm::scale(set_color, glm::vec3(0.5, 0.5, 0.5));
            glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
            glDrawArrays(GL_TRIANGLE_FAN, 12, 4); //정육면체
        } {
            glm::mat4 set_color = glm::mat4(1.0f);
            set_color = glm::scale(set_color, glm::vec3(0.3, 0.3, 0.3));
            glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
            glDrawArrays(GL_TRIANGLE_FAN, 20, 4); //정육면체
        } { // 아래1
            glm::mat4 set_color = glm::mat4(1.0f);
            set_color = glm::scale(set_color, glm::vec3(0.4, 0.4, 0.4));
            glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));

            glm::mat4 open = glm::mat4(1.0f);
            open = glm::translate(open, glm::vec3(1.0f, -1.0f, 0.0f));
            open = glm::rotate(open, glm::radians(open_radian1), glm::vec3(0.0, 0.0, 1.0));
            open = glm::translate(open, glm::vec3(-0.5f, 1.0f, 0.0f));
            open = glm::scale(open, glm::vec3(0.5f, 1.0f, 1.0f));
            model = axisTransForm * open;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLE_FAN, 16, 4); //정육면체
        } { // 아래2
            glm::mat4 set_color = glm::mat4(1.0f);
            set_color = glm::scale(set_color, glm::vec3(0.4, 0.4, 0.4));
            glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));

            glm::mat4 open = glm::mat4(1.0f);
            open = glm::translate(open, glm::vec3(-1.0f, -1.0f, 0.0f));
            open = glm::rotate(open, glm::radians(-open_radian2), glm::vec3(0.0, 0.0, 1.0));
            open = glm::translate(open, glm::vec3(0.5f, 1.0f, 0.0f));
            open = glm::scale(open, glm::vec3(0.5f, 1.0f, 1.0f));
            model = axisTransForm * open;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLE_FAN, 16, 4); //정육면체
        } /*크기가 다른 육면체 3개*/
    }/*3개의 정육면체*/ {
        glm::mat4 set_color = glm::mat4(1.0f);
        set_color = glm::scale(set_color, glm::vec3(0.8, 0.1, 0.1));
        glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
        glDisable(GL_CULL_FACE); {
            glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
            glm::mat4 cube = glm::mat4(1.0f);//변환 행렬 생성 T
            if (object_move < 0.0f)
                cube = glm::translate(cube, glm::vec3(object_move * cos(glm::radians(y_radian)), 0.0f, object_move * sin(glm::radians(y_radian)))),
                cube = glm::translate(cube, glm::vec3(0.15 * cos(glm::radians(y_radian)), 0.0, 0.3 * sin(glm::radians(y_radian))));
            else if (object_move > 0.0f)
                cube = glm::translate(cube, glm::vec3(object_move * cos(glm::radians(y_radian)), 0.0f, object_move * sin(glm::radians(y_radian)))),
                cube = glm::translate(cube, glm::vec3(-0.15 * cos(glm::radians(y_radian)), 0.0, -0.3 * sin(glm::radians(y_radian))));
            cube = glm::translate(cube, glm::vec3(0.0f, -1.0f + 0.15f, -0.15f));
            cube = glm::scale(cube, glm::vec3(0.15, 0.15, 0.15));
            model = axisTransForm * cube;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_QUADS, 0, 24); //정육면체
        } {
            glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
            glm::mat4 cube = glm::mat4(1.0f);//변환 행렬 생성 T
            if (object_move < 0.0f)
                cube = glm::translate(cube, glm::vec3(object_move * cos(glm::radians(y_radian)), 0.0f, object_move * sin(glm::radians(y_radian)))),
                cube = glm::translate(cube, glm::vec3(0.1 * cos(glm::radians(y_radian)), 0.0, 0.3 * sin(glm::radians(y_radian))));
            else if (object_move > 0.0f)
                cube = glm::translate(cube, glm::vec3(object_move * cos(glm::radians(y_radian)), 0.0f, object_move * sin(glm::radians(y_radian)))),
                cube = glm::translate(cube, glm::vec3(-0.1 * cos(glm::radians(y_radian)), 0.0, -0.3 * sin(glm::radians(y_radian))));
            cube = glm::translate(cube, glm::vec3(0.0f, -1.0f + 0.1f, 0.1f));
            cube = glm::scale(cube, glm::vec3(0.1, 0.1, 0.1));
            model = axisTransForm * cube;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_QUADS, 0, 24); //정육면체
        } {
            glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
            glm::mat4 cube = glm::mat4(1.0f);//변환 행렬 생성 T
            if (object_move < 0.0f)
                cube = glm::translate(cube, glm::vec3(object_move * cos(glm::radians(y_radian)), 0.0f, object_move * sin(glm::radians(y_radian)))),
                cube = glm::translate(cube, glm::vec3(0.05 * cos(glm::radians(y_radian)), 0.0, 0.3 * sin(glm::radians(y_radian))));
            else if (object_move > 0.0f)
                cube = glm::translate(cube, glm::vec3(object_move * cos(glm::radians(y_radian)), 0.0f, object_move * sin(glm::radians(y_radian)))),
                cube = glm::translate(cube, glm::vec3(-0.05 * cos(glm::radians(y_radian)), 0.0, -0.3 * sin(glm::radians(y_radian))));
            cube = glm::translate(cube, glm::vec3(0.0f, -1.0f + 0.05f, 0.25f));
            cube = glm::scale(cube, glm::vec3(0.05, 0.05, 0.05));
            model = axisTransForm * cube;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_QUADS, 0, 24); //정육면체
        }
    } /*공*/ {
        glm::mat4 set_color = glm::mat4(1.0f);
        set_color = glm::scale(set_color, glm::vec3(0.1, 0.1, 0.8));
        glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
        for (int i = 0; i < 5; ++i) {
            if (ball[i].check) {
                GLUquadricObj* qobj;
                qobj = gluNewQuadric();
                gluQuadricNormals(qobj, GLU_SMOOTH); // 생략 가능
                gluQuadricOrientation(qobj, GLU_OUTSIDE); // 생략 가능
                glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
                glm::mat4 move = glm::mat4(1.0f);//변환 행렬 생성 T
                move = glm::translate(move, glm::vec3(ball[i].x, ball[i].y, ball[i].z));
                model = move * axisTransForm;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                gluSphere(qobj, 0.05f, 20, 20);
            }
        }
    }
    glutSwapBuffers();
}
GLvoid Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

GLvoid KeyBoard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'o':
        open = true;
        break;
    case 'O':
        open = false;
        break;
    case 'z':
        if(z_move < 3.0f)
        z_move += 0.1f;
        break;
    case 'Z':
        z_move -= 0.1f;
        break;
    case 'y':
        if (y_dir == 1)
            y_dir = 0;
        else
            y_dir = 1;
        break;
    case 'Y':
        if (y_dir == -1)
            y_dir = 0;
        else
            y_dir = -1;
        break;
    case 'B':
        for (int i = 0; i < 5; ++i) {
            if (!ball[i].check) {
                ball[i].check = true;
                ball[i].x = rd_place(dre), ball[i].z = rd_place(dre);
                break;
            }
        }
        break;
    case 'q':
        glutLeaveMainLoop();
        break;
    default:
        break;
    }
    glutPostRedisplay();
}
GLvoid Mouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN && button == GLUT_LEFT) l_button = true, before_mouse = x / 10;
    if (l_button && state == GLUT_UP) l_button = false;
}
GLvoid Motion(int x, int y)
{
    if (l_button) {
        z_radian += x / 10 - before_mouse;
        if (z_radian > 60.0f)
            z_radian = 60.0f;
        else if (z_radian < -60.0f)
            z_radian = -60.0f;
        before_mouse = x / 10;
    }
}

GLvoid TimerFunc(int value)
{
    if (open) {
        open_radian1 += 1.0f;
        if (open_radian1 > 90.0 - z_radian)
            open_radian1 = 90.0 - z_radian;
        open_radian2 += 1.0f;
        if (open_radian2 > 90.0 + z_radian)
            open_radian2 = 90.0 + z_radian;
    }
    else {
        open_radian1 -= 1.0f;
        if (open_radian1 < 0.0f)
            open_radian1 = 0.0f;
        open_radian2 -= 1.0f;
        if (open_radian2 < 0.0f)
            open_radian2 = 0.0f;
    }
    object_move -= z_radian / 100;
    if (object_move < -1.0)
        object_move = -1.0;
    if (object_move > 1.0)
        object_move = 1.0;
    y_radian += y_dir;
    if (int(y_radian) % 90 == 0)
        y_dir = 0;
    for (int i = 0; i < 5; ++i) {
        if (ball[i].check) {
            ball[i].speed += 0.005;
            ball[i].y += ball[i].dir * ball[i].speed;
            bool out_side = true;
            GLfloat real_x = ball[i].x * cos(glm::radians(z_radian)) * cos(glm::radians(y_radian)), real_z = ball[i].z * sin(glm::radians(y_radian)), ceiling_up = 0.0f, ceiling_down = 0.0f;
            if (real_x < 0)
                real_x = 1 + real_x;
            else
                real_x = 1 - real_x;
            if (real_z < 0)
                real_z = 1 + real_z;
                else
                real_z = 1 - real_z;
            if (-1.0 * real_x < -1.0 * real_z)
                ceiling_down = -1.0 * real_x;
            else
                ceiling_down = -1.0 * real_z;
            if (1.0f * real_x > 1.0 * real_z)
                ceiling_up = 1.0f * real_x;
            else
                ceiling_up = 1.0 * real_z;
            if (!open && ball[i].y < ceiling_down + 0.05f)
                ball[i].y = ceiling_down + 0.05f, ball[i].dir *= -1, ball[i].speed = 0;
            else if (ball[i].y > ceiling_up - 0.05f)
                ball[i].y = ceiling_up - 0.05f, ball[i].dir *= -1, ball[i].speed = 0;
            if (open && ball[i].y < -3.0)
                ball[i].check = false, ball[i].y = 0.95;
        }
    }
    
    glutTimerFunc(10, TimerFunc, 1);
    glutPostRedisplay();
}

