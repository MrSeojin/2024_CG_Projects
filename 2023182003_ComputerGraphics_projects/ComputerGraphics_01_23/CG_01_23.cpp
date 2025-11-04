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
std::uniform_real_distribution<GLclampf> rd_color(0.0f, 1.0f), rd_place(-0.8f, 0.8f);

GLfloat camera_move[3]{ 0.0f,0.0f,1.0f }, hexa_move[3], robot_move[3]{0.0f,0.05f,0.0f}, leg_radian, speed = 0.05, o_move, y_radian, tower[4][2] = { {0.9, 0.9},{0.9, -0.9},{-0.9, 0.9},{-0.9,-0.9} },
block[3][3]{ {rd_place(dre), 0.1f ,rd_place(dre)},{rd_place(dre),0.1f ,rd_place(dre)},{rd_place(dre),0.1f, rd_place(dre)} },
small_move[3][3], small_dir[3], delay[30][4];

GLvoid drawScene();
GLvoid KeyBoard(unsigned char key, int x, int y);
GLvoid Reshape(int w, int h);
GLvoid TimerFunc(int x);
BOOL robot_to_block();

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
    glutCreateWindow("CG_01_23");

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
    if (robot_to_block())
        robot_move[1] = 0.1f;
    glutKeyboardFunc(KeyBoard);
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
GLuint hexa[6], cube, vbo[2];
void InitBuffer()
{
    // 6면체		
    const GLfloat spot_8[8][3]{ {1.0f,1.0f,1.0f}, {-1.0f,1.0f,1.0f}, {1.0f,1.0f,-1.0f}, {-1.0f,1.0f,-1.0f}, {-1.0f,-1.0f,-1.0f}, {-1.0f,-1.0f,1.0f}, {1.0f,-1.0f,-1.0f}, {1.0f,-1.0f,1.0f} };
    const int triangles[6][4]{ {0,1,2,3},{2,0,6,7},{0,1,7,5},{1,3,5,4},{3,2,4,6},{5,4,7,6} };
    for (int i = 0; i < 6; ++i) {
        const GLfloat color[3] = { rd_color(dre), rd_color(dre), rd_color(dre) };
        GLfloat spots[4][3]{
            { spot_8[triangles[i][0]][0],spot_8[triangles[i][0]][1],spot_8[triangles[i][0]][2] },
            { spot_8[triangles[i][1]][0],spot_8[triangles[i][1]][1],spot_8[triangles[i][1]][2] },
            { spot_8[triangles[i][2]][0],spot_8[triangles[i][2]][1],spot_8[triangles[i][2]][2] },
            { spot_8[triangles[i][3]][0],spot_8[triangles[i][3]][1],spot_8[triangles[i][3]][2] } },
            colors[4][3]{
            { color[0],color[1],color[2] },
            { color[0],color[1],color[2] },
            { color[0],color[1],color[2] },
            { color[0],color[1],color[2] } };
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
    // cube
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
    for (int i = 0; i < 3; ++i) {
        small_move[i][2] -= 0.1 * (1 + i);
    }
}

GLfloat background_r = 0.2f, background_g = 0.2f, background_b = 0.2f;

bool j_switch, leg_dir, arm_dir, t_switch, f_switch;
int x_dir, y_dir, z_dir, o_dir, camera_dir;

GLfloat direction = 0.0f;

GLvoid drawScene()
{
    glUseProgram(shaderID);

    glClearColor(background_r, background_g, background_b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//와이어 객체

    unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform");//월드 변환 행렬값을 셰이더의 uniform mat4 modelTransform에게 넘겨줌
    unsigned int viewLocation = glGetUniformLocation(shaderID, "viewTransform");//위와 동일
    unsigned int projectionLocation = glGetUniformLocation(shaderID, "projectionTransform");//위와 동일
    unsigned int colorLocation = glGetUniformLocation(shaderID, "vColor");//위와 동일

    glm::mat4 white = glm::mat4(1.0f);
    white = glm::scale(white, glm::vec3(1.0, 1.0, 1.0));
    glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(white));

    //원근 투영
    glm::mat4 kTransform = glm::mat4(1.0f);
    kTransform = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
    kTransform = glm::translate(kTransform, glm::vec3(0.0, 0.0, -3.0f));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &kTransform[0][0]);

    //뷰잉 변환 
    glm::mat4 vTransform = glm::mat4(1.0f);
    glm::vec3 cameraPos = glm::vec3(camera_move[0], camera_move[1], camera_move[2]); //--- 카메라 위치
    glm::vec3 cameraDirection = glm::vec3(camera_move[0], camera_move[1], -1.0f + camera_move[2]); //--- 카메라 바라보는 방향
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향

    vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);

    //축
    glm::mat4 axisTransForm = glm::mat4(1.0f);//변환 행렬 생성 T
    axisTransForm = glm::rotate(axisTransForm, glm::radians(y_radian), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(axisTransForm));//변환 행렬을 셰이더에 전달

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    {   // 무대/**/
        glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
        {
            {	// 윗면
                model = axisTransForm;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
                glBindVertexArray(hexa[0]);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            } {	// 오른
                model = axisTransForm;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
                glBindVertexArray(hexa[1]);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            } {	// 앞면1
                glm::mat4 open = glm::mat4(1.0f);
                open = glm::scale(open, glm::vec3(0.5f, 1.0f, 1.0f));
                open = glm::translate(open, glm::vec3(o_move + 1.0f, 0.0f, 0.0f));
                model = axisTransForm * open;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
                glBindVertexArray(hexa[2]);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            } {  // 앞면 2
                glm::mat4 open = glm::mat4(1.0f);
                open = glm::scale(open, glm::vec3(0.5f, 1.0f, 1.0f));
                open = glm::translate(open, glm::vec3(-o_move - 1.0f, 0.0f, 0.0f));
                model = axisTransForm * open;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            } {	// 왼면
                model = axisTransForm;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
                glBindVertexArray(hexa[3]);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            } {	// 뒷면
                model = axisTransForm;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
                glBindVertexArray(hexa[4]);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            }
        }
        glBindVertexArray(cube); //--- VAO를 바인드하기
    } /*로봇*/ {
        glm::mat4 shapeTransForm = glm::mat4(1.0f);//변환 행렬 생성 T
        shapeTransForm = glm::rotate(shapeTransForm, glm::radians(direction), glm::vec3(0.0f, 1.0f, 0.0f));
        {   //머리
            glm::mat4 set_color = glm::mat4(1.0f);
            set_color = glm::scale(set_color, glm::vec3(0.1, 0.1, 0.6));
            glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
            glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
            glm::mat4 cube = glm::mat4(1.0f);//변환 행렬 생성 T
            cube = glm::translate(cube, glm::vec3(robot_move[0], robot_move[1] + 0.55f - 1.0f, robot_move[2]));
            cube = glm::scale(cube, glm::vec3(0.05, 0.05, 0.05));
            model = axisTransForm * cube * shapeTransForm;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_QUADS, 0, 24); //정육면체
        } { //몸통
            glm::mat4 set_color = glm::mat4(1.0f);
            set_color = glm::scale(set_color, glm::vec3(0.1, 0.1, 0.6));
            glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
            glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
            glm::mat4 cube = glm::mat4(1.0f);//변환 행렬 생성 T
            cube = glm::translate(cube, glm::vec3(robot_move[0], robot_move[1], robot_move[2]));
            cube = glm::rotate(cube, glm::radians(direction), glm::vec3(0.0f, 1.0f, 0.0f));
            cube = glm::translate(cube, glm::vec3(0.0f, 0.35f - 1.0f, 0.0f));
            cube = glm::scale(cube, glm::vec3(0.1, 0.15, 0.05));
            model = axisTransForm * cube * shapeTransForm;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_QUADS, 0, 24); //정육면체
        } { //팔
            glm::mat4 set_color = glm::mat4(1.0f);
            set_color = glm::scale(set_color, glm::vec3(0.6, 0.0, 0.6));
            glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
            glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
            glm::mat4 cube = glm::mat4(1.0f);//변환 행렬 생성 T
            cube = glm::translate(cube, glm::vec3(robot_move[0], robot_move[1], robot_move[2]));
            cube = glm::rotate(cube, glm::radians(direction), glm::vec3(0.0f, 1.0f, 0.0f));
            cube = glm::translate(cube, glm::vec3(0.125f, 0.5f - 1.0f, 0.0f));
            cube = glm::rotate(cube, glm::radians(leg_radian), glm::vec3(1.0f, 0.0f, 0.0f));
            cube = glm::translate(cube, glm::vec3(0.0f, -0.13f, 0.0f));
            cube = glm::scale(cube, glm::vec3(0.025, 0.13, 0.05));
            model = axisTransForm * cube * shapeTransForm;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_QUADS, 0, 24); //정육면체
        } {
            glm::mat4 set_color = glm::mat4(1.0f);
            set_color = glm::scale(set_color, glm::vec3(0.6, 0.0, 0.6));
            glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
            glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
            glm::mat4 cube = glm::mat4(1.0f);//변환 행렬 생성 T
            cube = glm::translate(cube, glm::vec3(robot_move[0], robot_move[1], robot_move[2]));
            cube = glm::rotate(cube, glm::radians(direction), glm::vec3(0.0f, 1.0f, 0.0f));
            cube = glm::translate(cube, glm::vec3(-0.125f, 0.5f - 1.0f, 0.0f));
            cube = glm::rotate(cube, glm::radians(-leg_radian), glm::vec3(1.0f, 0.0f, 0.0f));
            cube = glm::translate(cube, glm::vec3(0.0f, -0.13f, 0.0f));
            cube = glm::scale(cube, glm::vec3(0.025, 0.13, 0.05));
            model = axisTransForm * cube * shapeTransForm;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_QUADS, 0, 24); //정육면체
        } { //다리
            glm::mat4 set_color = glm::mat4(1.0f);
            set_color = glm::scale(set_color, glm::vec3(0.0, 0.6, 0.6));
            glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
            glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
            glm::mat4 cube = glm::mat4(1.0f);//변환 행렬 생성 T
            cube = glm::translate(cube, glm::vec3(robot_move[0], robot_move[1], robot_move[2]));
            cube = glm::rotate(cube, glm::radians(direction), glm::vec3(0.0f, 1.0f, 0.0f));
            cube = glm::translate(cube, glm::vec3(0.05f, 0.2f - 1.0f, 0.0f));
            cube = glm::rotate(cube, glm::radians(leg_radian), glm::vec3(1.0f, 0.0f, 0.0f));
            cube = glm::translate(cube, glm::vec3(0.0f, -0.1f, 0.0f));
            cube = glm::scale(cube, glm::vec3(0.05, 0.1, 0.05));
            model = axisTransForm * cube * shapeTransForm;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_QUADS, 0, 24); //정육면체
        } {
            glm::mat4 set_color = glm::mat4(1.0f);
            set_color = glm::scale(set_color, glm::vec3(0.0, 0.6, 0.6));
            glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
            glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
            glm::mat4 cube = glm::mat4(1.0f);//변환 행렬 생성 T
            cube = glm::translate(cube, glm::vec3(robot_move[0], robot_move[1], robot_move[2]));
            cube = glm::rotate(cube, glm::radians(direction), glm::vec3(0.0f, 1.0f, 0.0f));
            cube = glm::translate(cube, glm::vec3(-0.05f, 0.2f - 1.0f, 0.0f));
            cube = glm::rotate(cube, glm::radians(-leg_radian), glm::vec3(1.0f, 0.0f, 0.0f));
            cube = glm::translate(cube, glm::vec3(0.0f, -0.1f, 0.0f));
            cube = glm::scale(cube, glm::vec3(0.05, 0.1, 0.05));
            model = axisTransForm * cube * shapeTransForm;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_QUADS, 0, 24); //정육면체
        } { //코
            glm::mat4 set_color = glm::mat4(1.0f);
            set_color = glm::scale(set_color, glm::vec3(0.6, 0.6, 0.0));
            glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
            glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
            glm::mat4 cube = glm::mat4(1.0f);//변환 행렬 생성 T
            cube = glm::translate(cube, glm::vec3(robot_move[0], robot_move[1], robot_move[2]));
            cube = glm::rotate(cube, glm::radians(direction), glm::vec3(0.0f, 1.0f, 0.0f));
            cube = glm::translate(cube, glm::vec3(0.0f, +0.5375f - 1.0f, +0.1f));
            cube = glm::scale(cube, glm::vec3(0.0125, 0.0125, 0.1));
            model = axisTransForm * cube * shapeTransForm;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_QUADS, 0, 24); //정육면체
        }
    } /*작은 로봇*/ if (t_switch) {
        for (int i = 0; i < 3; ++i) {
            glm::mat4 shapeTransForm = glm::mat4(1.0f);//변환 행렬 생성 T
            shapeTransForm = glm::rotate(shapeTransForm, glm::radians(small_dir[i]), glm::vec3(0.0f, 1.0f, 0.0f));
            shapeTransForm = glm::scale(shapeTransForm, glm::vec3(0.5, 0.5, 0.5));
            {   //머리
                glm::mat4 set_color = glm::mat4(1.0f);
                set_color = glm::scale(set_color, glm::vec3(0.1, 0.1, 0.6));
                glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
                glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
                glm::mat4 cube = glm::mat4(1.0f);//변환 행렬 생성 T
                cube = glm::translate(cube, glm::vec3(small_move[i][0], small_move[i][1] + 0.275f - 1.0f, small_move[i][2]));
                cube = glm::scale(cube, glm::vec3(0.05, 0.05, 0.05));
                model = axisTransForm * cube * shapeTransForm;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glDrawArrays(GL_QUADS, 0, 24); //정육면체
            } { //몸통
                glm::mat4 set_color = glm::mat4(1.0f);
                set_color = glm::scale(set_color, glm::vec3(0.1, 0.1, 0.6));
                glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
                glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
                glm::mat4 cube = glm::mat4(1.0f);//변환 행렬 생성 T
                cube = glm::translate(cube, glm::vec3(small_move[i][0], small_move[i][1], small_move[i][2]));
                cube = glm::rotate(cube, glm::radians(small_dir[i]), glm::vec3(0.0f, 1.0f, 0.0f));
                cube = glm::translate(cube, glm::vec3(0.0f, 0.175f - 1.0f, 0.0f));
                cube = glm::scale(cube, glm::vec3(0.1, 0.15, 0.05));
                model = axisTransForm * cube * shapeTransForm;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glDrawArrays(GL_QUADS, 0, 24); //정육면체
            } { //팔
                glm::mat4 set_color = glm::mat4(1.0f);
                set_color = glm::scale(set_color, glm::vec3(0.6, 0.0, 0.6));
                glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
                glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
                glm::mat4 cube = glm::mat4(1.0f);//변환 행렬 생성 T
                cube = glm::translate(cube, glm::vec3(small_move[i][0], small_move[i][1], small_move[i][2]));
                cube = glm::rotate(cube, glm::radians(small_dir[i]), glm::vec3(0.0f, 1.0f, 0.0f));
                cube = glm::translate(cube, glm::vec3(0.0625f, 0.25f - 1.0f, 0.0f));
                cube = glm::rotate(cube, glm::radians(leg_radian), glm::vec3(1.0f, 0.0f, 0.0f));
                cube = glm::translate(cube, glm::vec3(0.0f, -0.065f, 0.0f));
                cube = glm::scale(cube, glm::vec3(0.025, 0.13, 0.05));
                model = axisTransForm * cube * shapeTransForm;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glDrawArrays(GL_QUADS, 0, 24); //정육면체
            } {
                glm::mat4 set_color = glm::mat4(1.0f);
                set_color = glm::scale(set_color, glm::vec3(0.6, 0.0, 0.6));
                glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
                glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
                glm::mat4 cube = glm::mat4(1.0f);//변환 행렬 생성 T
                cube = glm::translate(cube, glm::vec3(small_move[i][0], small_move[i][1], small_move[i][2]));
                cube = glm::rotate(cube, glm::radians(small_dir[i]), glm::vec3(0.0f, 1.0f, 0.0f));
                cube = glm::translate(cube, glm::vec3(-0.0625f, 0.25f - 1.0f, 0.0f));
                cube = glm::rotate(cube, glm::radians(-leg_radian), glm::vec3(1.0f, 0.0f, 0.0f));
                cube = glm::translate(cube, glm::vec3(0.0f, -0.065f, 0.0f));
                cube = glm::scale(cube, glm::vec3(0.025, 0.13, 0.05));
                model = axisTransForm * cube * shapeTransForm;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glDrawArrays(GL_QUADS, 0, 24); //정육면체
            } { //다리
                glm::mat4 set_color = glm::mat4(1.0f);
                set_color = glm::scale(set_color, glm::vec3(0.0, 0.6, 0.6));
                glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
                glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
                glm::mat4 cube = glm::mat4(1.0f);//변환 행렬 생성 T
                cube = glm::translate(cube, glm::vec3(small_move[i][0], small_move[i][1], small_move[i][2]));
                cube = glm::rotate(cube, glm::radians(small_dir[i]), glm::vec3(0.0f, 1.0f, 0.0f));
                cube = glm::translate(cube, glm::vec3(0.025f, 0.1f - 1.0f, 0.0f));
                cube = glm::rotate(cube, glm::radians(leg_radian), glm::vec3(1.0f, 0.0f, 0.0f));
                cube = glm::translate(cube, glm::vec3(0.0f, -0.05f, 0.0f));
                cube = glm::scale(cube, glm::vec3(0.05, 0.1, 0.05));
                model = axisTransForm * cube * shapeTransForm;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glDrawArrays(GL_QUADS, 0, 24); //정육면체
            } {
                glm::mat4 set_color = glm::mat4(1.0f);
                set_color = glm::scale(set_color, glm::vec3(0.0, 0.6, 0.6));
                glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
                glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
                glm::mat4 cube = glm::mat4(1.0f);//변환 행렬 생성 T
                cube = glm::translate(cube, glm::vec3(small_move[i][0], small_move[i][1], small_move[i][2]));
                cube = glm::rotate(cube, glm::radians(small_dir[i]), glm::vec3(0.0f, 1.0f, 0.0f));
                cube = glm::translate(cube, glm::vec3(-0.025f, 0.1f - 1.0f, 0.0f));
                cube = glm::rotate(cube, glm::radians(-leg_radian), glm::vec3(1.0f, 0.0f, 0.0f));
                cube = glm::translate(cube, glm::vec3(0.0f, -0.05f, 0.0f));
                cube = glm::scale(cube, glm::vec3(0.05, 0.1, 0.05));
                model = axisTransForm * cube * shapeTransForm;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glDrawArrays(GL_QUADS, 0, 24); //정육면체
            } { //코
                glm::mat4 set_color = glm::mat4(1.0f);
                set_color = glm::scale(set_color, glm::vec3(0.6, 0.6, 0.0));
                glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
                glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
                glm::mat4 cube = glm::mat4(1.0f);//변환 행렬 생성 T
                cube = glm::translate(cube, glm::vec3(small_move[i][0], small_move[i][1], small_move[i][2]));
                cube = glm::rotate(cube, glm::radians(small_dir[i]), glm::vec3(0.0f, 1.0f, 0.0f));
                cube = glm::translate(cube, glm::vec3(0.0f, 0.26875 - 1.0f, 0.05f));
                cube = glm::scale(cube, glm::vec3(0.0125, 0.00625, 0.1));
                model = axisTransForm * cube * shapeTransForm;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glDrawArrays(GL_QUADS, 0, 24); //정육면체
            }
        } /*장애물*/
    } /*원기둥*/ for (int i = 0; i < 4; ++i) {
        glm::mat4 set_color = glm::mat4(1.0f);
        set_color = glm::scale(set_color, glm::vec3(0.5, 0.5, 0.5));
        glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));

        glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
        glm::mat4 move = glm::mat4(1.0f);//변환 행렬 생성 T
        move = glm::translate(move, glm::vec3(tower[i][0], -1.0f, tower[i][1]));
        move = glm::rotate(move, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = axisTransForm * move;
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

        GLUquadricObj* qobj = gluNewQuadric();
        gluQuadricDrawStyle(qobj, GLU_LINE); // 도형 스타일
        gluQuadricNormals(qobj, GLU_SMOOTH); // 생략 가능
        gluQuadricOrientation(qobj, GLU_OUTSIDE); // 생략 가능
        gluCylinder(qobj, 0.1, 0.1, 0.8, 20, 1);
    } /*Π*/ {
        glm::mat4 set_color = glm::mat4(1.0f);
        set_color = glm::scale(set_color, glm::vec3(0.5, 0.5, 0.5));
        glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
        {
            glm::mat4 shapeTransForm = glm::mat4(1.0f);//변환 행렬 생성 T
            glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
            shapeTransForm = glm::translate(shapeTransForm, glm::vec3(0.0f, 0.0f, -0.05f));
            shapeTransForm = glm::scale(shapeTransForm, glm::vec3(0.6, 0.1, 0.1));
            model = axisTransForm * shapeTransForm;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_QUADS, 0, 24); //정육면체
        } {
            glm::mat4 shapeTransForm = glm::mat4(1.0f);//변환 행렬 생성 T
            glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
            shapeTransForm = glm::translate(shapeTransForm, glm::vec3(-0.5f, -0.6f, -0.05f));
            shapeTransForm = glm::scale(shapeTransForm, glm::vec3(0.1, 0.6, 0.1));
            model = axisTransForm * shapeTransForm;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_QUADS, 0, 24); //정육면체
        } {
            glm::mat4 shapeTransForm = glm::mat4(1.0f);//변환 행렬 생성 T
            glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
            shapeTransForm = glm::translate(shapeTransForm, glm::vec3(0.5f, -0.6f, -0.05f));
            shapeTransForm = glm::scale(shapeTransForm, glm::vec3(0.1, 0.6, 0.1));
            model = axisTransForm * shapeTransForm;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_QUADS, 0, 24); //정육면체
        }
    }
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            glm::mat4 set_color = glm::mat4(1.0f);
            if ((i + j) % 2)
                set_color = glm::scale(set_color, glm::vec3(0.3, 0.3, 0.3));
            else
                set_color = glm::scale(set_color, glm::vec3(0.7, 0.7, 0.7));
            glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
            glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
            glm::mat4 cube = glm::mat4(1.0f);//변환 행렬 생성 T
            cube = glm::translate(cube, glm::vec3(-0.9f + 0.2f * i, -1.0f, -0.9f + 0.2f * j));
            cube = glm::scale(cube, glm::vec3(0.1f, 0.0f, 0.1f));
            model = axisTransForm * cube;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_QUADS, 0, 24); //정육면체
        }
    } /*장애물*/ {
        glm::mat4 set_color = glm::mat4(1.0f);
        set_color = glm::scale(set_color, glm::vec3(0.1, 0.1, 0.));
        glUniformMatrix4fv(colorLocation, 1, GL_FALSE, glm::value_ptr(set_color));
        for (int i = 0; i < 3; ++i) {
            glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
            glm::mat4 cube = glm::mat4(1.0f);//변환 행렬 생성 T
            cube = glm::translate(cube, glm::vec3(block[i][0], -1.0 + block[i][1]/2, block[i][2]));
            cube = glm::scale(cube, glm::vec3(0.1, block[i][1]/2, 0.1));
            model = axisTransForm * cube;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_QUADS, 0, 24); //정육면체
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
    case 't':
        t_switch = !t_switch;
        break;
    case 'f':
        f_switch = !f_switch;
        break;
    case 'o':
        o_dir = 1;
        break;
    case 'O':
        o_dir = -1;
        break;
    case 'w':
        x_dir = 0, z_dir = -1;
        break;
    case 'a':
        x_dir = -1, z_dir = 0;
        break;
    case 's':
        x_dir = 0, z_dir = 1;
        break;
    case 'd':
        x_dir = 1, z_dir = 0;
        break;
    case '+':
        if (speed < 0.1f)
            speed += 0.01f;
        break;
    case '-':
        if (speed > 0.01f)
            speed -= 0.01f;
        break;
    case 'j':
        j_switch = true;
        y_dir = 1;
        break;
    case 'i':
        j_switch = false;
        x_dir = 0, y_dir = 0, z_dir = 0;
        camera_move[0] = 0.0f, camera_move[1] = 0.0f, camera_move[2] = 1.0f;
        hexa_move[0] = 0.0f, hexa_move[1] = 0.0f, hexa_move[2] = 0.0f;
        robot_move[0] = 0.0f, robot_move[1] = 0.0f, robot_move[2] = 0.0f;
        leg_radian = 0.0f, speed = 0.05f, direction = 0.0f;
        o_move = 0.0f, o_dir = 0;
        break;
    case 'z':
        camera_move[2] += 0.1f;
        break;
    case 'Z':
        camera_move[2] -= 0.1f;
        break;
    case 'x':
        camera_move[0] += 0.1f;
        break;
    case 'X':
        camera_move[0] -= 0.1f;
        break;
    case 'y':
        if (camera_dir == 1)
            camera_dir = 0;
        else camera_dir = 1;
        break;
    case 'Y':
        if (camera_dir == -1)
            camera_dir = 0;
        else camera_dir = -1;
        break;
    case 'q':
        glutLeaveMainLoop();
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

GLvoid TimerFunc(int x)
{
    y_radian += camera_dir;
    if (o_dir == 1) {
        o_move += 0.1f;
        if (o_move >= 2.0f)
            o_move = 2.0f, o_dir = 0;
    }
    else if (o_dir == -1) {
        o_move -= 0.1f;
        if (o_move <= 0.0f)
            o_move = 0.0f, o_dir = 0;
    }
    if (x_dir != 0 || y_dir != 0 || z_dir != 0) {
        if (leg_dir && leg_radian < 30.0f * (speed * 25 + 1))
            leg_radian += speed * 25 + 1;
        else if (!leg_dir && leg_radian > -30.0f * (speed * 25 + 1))
            leg_radian -= speed * 25 + 1;
        else
            leg_dir = !leg_dir;

        for (int i = 29; i >= 0; --i) {
            for (int j = 0; j < 4; ++j) {
                if (j != 2) {
                    if (i == 0) {
                        if (j < 3)
                            delay[i][j] = robot_move[j];
                        else delay[i][j] = direction;
                    }
                    else
                        delay[i][j] = delay[i - 1][j];
                }
            }
        }
        small_move[2][0] = delay[29][0], small_move[2][2] = delay[29][2], small_dir[2] = delay[29][3];
        small_move[1][0] = delay[19][0], small_move[1][2] = delay[19][2], small_dir[1] = delay[19][3];
        small_move[0][0] = delay[9][0], small_move[0][2] = delay[9][2], small_dir[0] = delay[9][3];

        robot_move[0] += speed * x_dir;
        robot_move[2] += speed * z_dir;
    }
    for (int i = 29; i >= 0; --i) {
        if (i == 0) {
            delay[i][2] = robot_move[2];
        }
        else
            delay[i][2] = delay[i - 1][2];
    }
    small_move[2][1] = delay[29][1];
    small_move[1][1] = delay[19][1];
    small_move[0][1] = delay[9][1];
    if (j_switch) {

        robot_move[1] += speed * y_dir;
        if (robot_move[1] >= 0.5f)
            y_dir = -1;
        if (robot_move[1] < 0.0f)
            robot_move[1] = 0.0f;
        if (robot_move[1] == 0.0 || robot_to_block())
            j_switch = false;
    }
    else if (!robot_to_block()) {
        if (robot_move[1] != 0.0f)
            robot_move[1] -= speed;
        if (robot_move[1] < 0.0f)
            robot_move[1] = 0.0f;
    }
    GLfloat robot_left = -0.05 + robot_move[0], robot_right = 0.05 + robot_move[0], robot_back = -0.05 + robot_move[2], robot_front = 0.05 + robot_move[2];
    if (robot_left <= -1.0f)
        x_dir *= -1, robot_move[0] = -1.0f + 0.05;
    if (robot_right >= 1.0f)
        x_dir *= -1, robot_move[0] = 1.0f - 0.05;
    if (robot_back <= -1.0f)
        z_dir *= -1, robot_move[2] = -1.0f + 0.05;
    if (robot_front >= 1.0f)
        z_dir *= -1, robot_move[2] = 1.0f - 0.05;

    if (x_dir == -1)
        direction = -90.0f;
    else if (x_dir == 1)
        direction = 90.0f;
    else if (z_dir == -1)
        direction = 180.0f;
    else
        direction = 0.0f;

    glutTimerFunc(10, TimerFunc, 1);
    glutPostRedisplay();
}
BOOL robot_to_block() {
    GLfloat robot_left = -0.1 + robot_move[0], robot_right = 0.1 + robot_move[0], robot_bottom = -1.0f + robot_move[1], robot_top = -0.3 + robot_move[1], robot_back = -0.1 + robot_move[2], robot_front = 0.1 + robot_move[2];
    for (int i = 0; i < 4; ++i) {
        bool check = true;
        GLfloat block_left = tower[i][0] - 0.1f, block_right = tower[i][0] + 0.1f, block_back = tower[i][1] - 0.1f, block_front = tower[i][1] + 0.1f;
        if (block_left > robot_right)
            check = false;
        if (block_right < robot_left)
            check = false;
        if (block_back > robot_front)
            check = false;
        if (block_front < robot_back)
            check = false;
        if (check == true) {
            if (x_dir == 1)
                x_dir *= -1, robot_move[0] = block_left - 0.1;
            else if (x_dir == -1)
                x_dir *= -1, robot_move[0] = block_right + 0.1;
            else if (z_dir == 1)
                z_dir *= -1, robot_move[2] = block_back - 0.1;
            else if (z_dir == -1)
                z_dir *= -1, robot_move[2] = block_front + 0.1;
            return true;
        }
    }
{
        bool check = true;
        GLfloat block_left = -0.6, block_right = -0.4f, block_bottom = -1.0f, block_top = 0.2f, block_back = -0.15f, block_front = 0.05f;
        if (block_top < robot_bottom)
            check = false;
        else {
            if (block_left > robot_right)
                check = false;
            if (block_right < robot_left)
                check = false;
            if (block_back > robot_front)
                check = false;
            if (block_front < robot_back)
                check = false;
            if (check == true) {
                if (x_dir == 1)
                    x_dir *= -1, robot_move[0] = block_left - 0.1;
                else if (x_dir == -1)
                    x_dir *= -1, robot_move[0] = block_right + 0.1;
                else if (z_dir == 1)
                    z_dir *= -1, robot_move[2] = block_back - 0.1;
                else if (z_dir == -1)
                    z_dir *= -1, robot_move[2] = block_front + 0.1;
                return true;
            }
        }
    } {
        bool check = true;
        GLfloat block_left = 0.4, block_right = 0.6f, block_bottom = -1.0f, block_top = 0.2f, block_back = -0.15f, block_front = 0.05f;
        if (block_top < robot_bottom)
            check = false;
        else {
            if (block_left > robot_right)
                check = false;
            if (block_right < robot_left)
                check = false;
            if (block_back > robot_front)
                check = false;
            if (block_front < robot_back)
                check = false;
            if (check == true) {
                if (x_dir == 1)
                    x_dir *= -1, robot_move[0] = block_left - 0.1;
                else if (x_dir == -1)
                    x_dir *= -1, robot_move[0] = block_right + 0.1;
                else if (z_dir == 1)
                    z_dir *= -1, robot_move[2] = block_back - 0.1;
                else if (z_dir == -1)
                    z_dir *= -1, robot_move[2] = block_front + 0.1;
                return true;
            }
        }
    } {
        bool check = true;
        GLfloat block_left = -0.6, block_right = 0.6f, block_bottom = 0.0f, block_top = 0.1f, block_back = -0.15f, block_front = -0.05f;
        if (block_bottom > robot_top)
            check = false;
        else {
            if (block_left > robot_right)
                check = false;
            if (block_right < robot_left)
                check = false;
            if (block_back > robot_front)
                check = false;
            if (block_front < robot_back)
                check = false;
            if (check == true) {
                if (x_dir == 1)
                    x_dir *= -1, robot_move[0] = block_left - 0.1;
                else if (x_dir == -1)
                    x_dir *= -1, robot_move[0] = block_right + 0.1;
                else if (z_dir == 1)
                    z_dir *= -1, robot_move[2] = block_back - 0.1;
                else if (z_dir == -1)
                    z_dir *= -1, robot_move[2] = block_front + 0.1;
                else if (j_switch)
                    robot_move[1] = 0.3f;
                return true;
            }
        }
    }
    for (int i = 0; i < 3; ++i) {
        bool check = true;
        GLfloat block_left = -0.1 + block[i][0], block_right = 0.1 + block[i][0], block_bottom = -1.0f, block_top = -1.0 + block[i][1], block_back = -0.1 + block[i][2], block_front = 0.1 + block[i][2];
        if (block_top < robot_bottom)
            check = false;
        else {
            if (block_left > robot_right)
                check = false;
            if (block_right < robot_left)
                check = false;
            if (block_back > robot_front)
                check = false;
            if (block_front < robot_back)
                check = false;
            if (check == true) {
                if (j_switch) {
                    block[i][1] -= 0.01f;
                    robot_move[1] = block[i][1];
                }
                else if (robot_move[1] == 0.0f && x_dir == 1)
                    x_dir *= -1, robot_move[0] = block_left - 0.1;
                else if (robot_move[1] == 0.0f && x_dir == -1)
                    x_dir *= -1, robot_move[0] = block_right + 0.1;
                else if (robot_move[1] == 0.0f && z_dir == 1)
                    z_dir *= -1, robot_move[2] = block_back - 0.1;
                else if (robot_move[1] == 0.0f && z_dir == -1)
                    z_dir *= -1, robot_move[2] = block_front + 0.1;
                return true;
            }
        }
    }
    return false;
}
