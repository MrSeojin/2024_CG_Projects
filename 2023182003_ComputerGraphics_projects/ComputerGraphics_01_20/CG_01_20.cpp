#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <Windows.h>
#include <time.h>

GLvoid drawScene();
GLvoid KeyBoard(unsigned char key, int x, int y);
GLvoid Reshape(int w, int h);
GLvoid TimerFunc(int x);

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
    -0.5f,0.5f,0.5f, //앞면
    -0.5f,0.0f,0.5f,
     0.5f,0.0f,0.5f,
     0.5f,0.5f,0.5f,

   -0.5f,0.5f,-0.5f, //윗면
  -0.5f,0.5f,0.5f,
   0.5f,0.5f,0.5f,
   0.5f,0.5f,-0.5f,

   -0.5f,0.5f,-0.5f, //왼쪽옆
   -0.5f,0.0f,-0.5f,
   -0.5f,0.0f,0.5f,
   -0.5f,0.5f,0.5f,

   0.5f,0.5f,-0.5f, //뒷면
   0.5f,0.0f,-0.5f,
   -0.5f,0.0f,-0.5f,
   -0.5f,0.5f,-0.5f,

   -0.5f,0.0f,0.5f, //아랫면
   -0.5f,0.0f,-0.5f,
   0.5f,0.0f,-0.5f,
   0.5f,0.0f,0.5f,

   0.5f,0.5f,0.5f, //오른쪽 옆
   0.5f,0.0f,0.5f,
   0.5f,0.0f,-0.5f,
   0.5f,0.5f,-0.5f,//24

   2.0f,0,0, //x,y,z 축
   -2.0f,0,0,
   0,2.0f,0,
   0,-2.0f,0,
   0,0,4.0f,
   0,0,-4.0f, //30

   -2.0f,0.0f,2.0f, //바닥
   -2.0f,0.0f,-2.0f,
   2.0f,0.0f,-2.0f,
   2.0f,0.0f,2.0f,// 34

   // 중앙
    -0.3f,0.2f,0.3f, //앞면
    -0.3f,0.0f,0.3f,
     0.3f,0.0f,0.3f,
     0.3f,0.2f,0.3f,

   -0.3f,0.2f,-0.3f, //윗면
  -0.3f,0.2f,0.3f,
   0.3f,0.2f,0.3f,
   0.3f,0.2f,-0.3f,

   -0.3f,0.2f,-0.3f, //왼쪽옆
   -0.3f,0.0f,-0.3f,
   -0.3f,0.0f,0.3f,
   -0.3f,0.2f,0.3f,

   0.3f,0.2f,-0.3f, //뒷면
   0.3f,0.0f,-0.3f,
   -0.3f,0.0f,-0.3f,
   -0.3f,0.2f,-0.3f,

   -0.3f,0.0f,0.3f, //아랫면
   -0.3f,0.0f,-0.3f,
   0.3f,0.0f,-0.3f,
   0.3f,0.0f,0.3f,

   0.3f,0.2f,0.3f, //오른쪽 옆
   0.3f,0.0f,0.3f,
   0.3f,0.0f,-0.3f,
   0.3f,0.2f,-0.3f,//58

   // 위1
    -0.05f,0.3f,0.05f, //앞면
    -0.05f,0.0f,0.05f,
     0.05f,0.0f,0.05f,
     0.05f,0.3f,0.05f,

   -0.05f,0.3f,-0.05f, //윗면
  -0.05f,0.3f,0.05f,
   0.05f,0.3f,0.05f,
   0.05f,0.3f,-0.05f,

   -0.05f,0.3f,-0.05f, //왼쪽옆
   -0.05f,0.0f,-0.05f,
   -0.05f,0.0f,0.05f,
   -0.05f,0.3f,0.05f,

   0.05f,0.3f,-0.05f, //뒷면
   0.05f,0.0f,-0.05f,
   -0.05f,0.0f,-0.05f,
   -0.05f,0.3f,-0.05f,

   -0.05f,0.0f,0.05f, //아랫면
   -0.05f,0.0f,-0.05f,
   0.05f,0.0f,-0.05f,
   0.05f,0.0f,0.05f,

   0.05f,0.3f,0.05f, //오른쪽 옆
   0.05f,0.0f, 0.05f,
   0.05f,0.0f,-0.05f,
   0.05f,0.3f,-0.05f,//82
   //위2
         -0.05f, 0.3f, 0.05f, //앞면
         -0.05f, 0.0f, 0.05f,
         0.05f, 0.0f, 0.05f,
         0.05f, 0.3f, 0.05f,

         -0.05f, 0.3f, -0.05f, //윗면
         -0.05f, 0.3f, 0.05f,
         0.05f, 0.3f, 0.05f,
         0.05f, 0.3f, -0.05f,

         -0.05f, 0.3f, -0.05f, //왼쪽옆
         -0.05f, 0.0f, -0.05f,
         -0.05f, 0.0f, 0.05f,
         -0.05f, 0.3f, 0.05f,

         0.05f, 0.3f, -0.05f, //뒷면
         0.05f, 0.0f, -0.05f,
         -0.05f, 0.0f, -0.05f,
         -0.05f, 0.3f, -0.05f,

         -0.05f, 0.0f, 0.05f, //아랫면
         -0.05f, 0.0f, -0.05f,
         0.05f, 0.0f, -0.05f,
         0.05f, 0.0f, 0.05f,

         0.05f, 0.3f, 0.05f, //오른쪽 옆
         0.05f, 0.0f, 0.05f,
         0.05f, 0.0f, -0.05f,
         0.05f, 0.3f, -0.05f,//106

         // 앞 1
            -0.05f, 0.3f, 0.05f, //앞면
            -0.05f, 0.0f, 0.05f,
            0.05f, 0.0f, 0.05f,
            0.05f, 0.3f, 0.05f,

            -0.05f, 0.3f, -0.05f, //윗면
            -0.05f, 0.3f, 0.05f,
            0.05f, 0.3f, 0.05f,
            0.05f, 0.3f, -0.05f,

            -0.05f, 0.3f, -0.05f, //왼쪽옆
            -0.05f, 0.0f, -0.05f,
            -0.05f, 0.0f, 0.05f,
            -0.05f, 0.3f, 0.05f,

            0.05f, 0.3f, -0.05f, //뒷면
            0.05f, 0.0f, -0.05f,
            -0.05f, 0.0f, -0.05f,
            -0.05f, 0.3f, -0.05f,

            -0.05f, 0.0f, 0.05f, //아랫면
            -0.05f, 0.0f, -0.05f,
            0.05f, 0.0f, -0.05f,
            0.05f, 0.0f, 0.05f,

            0.05f, 0.3f, 0.05f, //오른쪽 옆
            0.05f, 0.0f, 0.05f,
            0.05f, 0.0f, -0.05f,
            0.05f, 0.3f, -0.05f,//130
            //앞 2
            -0.05f, 0.3f, 0.05f, //앞면
            -0.05f, 0.0f, 0.05f,
            0.05f, 0.0f, 0.05f,
            0.05f, 0.3f, 0.05f,

            -0.05f, 0.3f, -0.05f, //윗면
            -0.05f, 0.3f, 0.05f,
            0.05f, 0.3f, 0.05f,
            0.05f, 0.3f, -0.05f,

            -0.05f, 0.3f, -0.05f, //왼쪽옆
            -0.05f, 0.0f, -0.05f,
            -0.05f, 0.0f, 0.05f,
            -0.05f, 0.3f, 0.05f,

            0.05f, 0.3f, -0.05f, //뒷면
            0.05f, 0.0f, -0.05f,
            -0.05f, 0.0f, -0.05f,
            -0.05f, 0.3f, -0.05f,

            -0.05f, 0.0f, 0.05f, //아랫면
            -0.05f, 0.0f, -0.05f,
            0.05f, 0.0f, -0.05f,
            0.05f, 0.0f, 0.05f,

            0.05f, 0.3f, 0.05f, //오른쪽 옆
            0.05f, 0.0f, 0.05f,
            0.05f, 0.0f, -0.05f,
            0.05f, 0.3f, -0.05f,//154end
};//정육면체, 축,정사면체 벡터들
float vertexColor[] = {
   0,1,1,
   0,0,1,
   1,0,1,
   1,1,1,

   0,1,0,
   0,1,1,
   1,1,1,
   1,1,0,

   0,1,0,
   0,0,0,
   0,0,1,
   0,1,1,

   1,1,0,
   1,0,0,
   0,0,0,
   0,1,0,

   0,0,1,
   0,0,0,
   1,0,0,
   1,0,1,

   1,1,1,
   1,0,1,
   1,0,0,
   1,1,0,


   //선
   1,0,0,
   0,0,0,
   0,1,0,
   0,0,0,
   0,0,1,
   0,0,0,

   0,1,1,
   0,0,1,
   1,0,1,
   1,1,1,

   // 중앙
   0,1,1,
   0,0,1,
   1,0,1,
   1,1,1,

   0,1,0,
   0,1,1,
   1,1,1,
   1,1,0,

   0,1,0,
   0,0,0,
   0,0,1,
   0,1,1,

   1,1,0,
   1,0,0,
   0,0,0,
   0,1,0,

   0,0,1,
   0,0,0,
   1,0,0,
   1,0,1,

   1,1,1,
   1,0,1,
   1,0,0,
   1,1,0,
   // 위1
   0,1,1,
   0,0,1,
   1,0,1,
   1,1,1,

   0,1,0,
   0,1,1,
   1,1,1,
   1,1,0,

   0,1,0,
   0,0,0,
   0,0,1,
   0,1,1,

   1,1,0,
   1,0,0,
   0,0,0,
   0,1,0,

   0,0,1,
   0,0,0,
   1,0,0,
   1,0,1,

   1,1,1,
   1,0,1,
   1,0,0,
   1,1,0,
   //위2
   0, 1, 1,
   0, 0, 1,
   1, 0, 1,
   1, 1, 1,

   0, 1, 0,
   0, 1, 1,
   1, 1, 1,
   1, 1, 0,

   0, 1, 0,
   0, 0, 0,
   0, 0, 1,
   0, 1, 1,

   1, 1, 0,
   1, 0, 0,
   0, 0, 0,
   0, 1, 0,

   0, 0, 1,
   0, 0, 0,
   1, 0, 0,
   1, 0, 1,

   1, 1, 1,
   1, 0, 1,
   1, 0, 0,
   1, 1, 0,
   //앞1
   0, 1, 1,
       0, 0, 1,
       1, 0, 1,
       1, 1, 1,

       0, 1, 0,
       0, 1, 1,
       1, 1, 1,
       1, 1, 0,

       0, 1, 0,
       0, 0, 0,
       0, 0, 1,
       0, 1, 1,

       1, 1, 0,
       1, 0, 0,
       0, 0, 0,
       0, 1, 0,

       0, 0, 1,
       0, 0, 0,
       1, 0, 0,
       1, 0, 1,

       1, 1, 1,
       1, 0, 1,
       1, 0, 0,
       1, 1, 0,
       //앞2
       0, 1, 1,
       0, 0, 1,
       1, 0, 1,
       1, 1, 1,

       0, 1, 0,
       0, 1, 1,
       1, 1, 1,
       1, 1, 0,

       0, 1, 0,
       0, 0, 0,
       0, 0, 1,
       0, 1, 1,

       1, 1, 0,
       1, 0, 0,
       0, 0, 0,
       0, 1, 0,

       0, 0, 1,
       0, 0, 0,
       1, 0, 0,
       1, 0, 1,

       1, 1, 1,
       1, 0, 1,
       1, 0, 0,
       1, 1, 0,
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
    glutCreateWindow("CG_01_20");

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
GLuint VAO, VBO[2];
void InitBuffer()
{
    glGenVertexArrays(1, &VAO); //--- VAO 를 지정하고 할당하기
    glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기


    glBindVertexArray(VAO); //--- VAO를 바인드하기
    //--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    //--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
    //--- triShape 배열의 사이즈: 9 * float
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexPosition), vertexPosition, GL_STATIC_DRAW);


    //--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //--- attribute 인덱스 0번을 사용가능하게 함
    glEnableVertexAttribArray(0);
    //--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    //--- 변수 colors에서 버텍스 색상을 복사한다.
    //--- colors 배열의 사이즈: 9 *float 
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexColor), vertexColor, GL_STATIC_DRAW);
    //--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //--- attribute 인덱스 1번을 사용 가능하게 함.
    glEnableVertexAttribArray(1);
}

bool stop = true;

float b_move[3]{}, e_move[2][3]{ {-0.3f,0.2f,0.5f},{0.3f,0.2f,0.5f} };
float m_radian, t_radian, f_radian, y_radian = 0.0f, a_radian, r_radian;

float camera_move[3]{ 0.0f,0.0f,1.0f };

int b_dir, y_dir, m_dir, f_dir, e_dir, t_dir, a_dir, r_dir;

GLvoid drawScene() {
    glUseProgram(shaderID);
    glBindVertexArray(VAO);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//와이어 객체

    /*정면*/ {
        glViewport(0, 0, 800, 800);

        unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform");//월드 변환 행렬값을 셰이더의 uniform mat4 modelTransform에게 넘겨줌
        unsigned int viewLocation = glGetUniformLocation(shaderID, "viewTransform");//위와 동일
        unsigned int projectionLocation = glGetUniformLocation(shaderID, "projectionTransform");//위와 동일

        //원근 투영
        glm::mat4 kTransform = glm::mat4(1.0f);
        kTransform = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
        kTransform = glm::translate(kTransform, glm::vec3(0.0, 0.0, -5.0f));
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &kTransform[0][0]);

        //뷰잉 변환 
        glm::mat4 vTransform = glm::mat4(1.0f);
        glm::vec3 cameraPos = glm::vec3(camera_move[0], camera_move[1], camera_move[2]); //--- 카메라 위치
        glm::vec3 cameraDirection = glm::vec3(camera_move[0], 0.0f, 1.0f - camera_move[2]); //--- 카메라 바라보는 방향
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향

        vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);

        glm::mat4 y_rotate = glm::mat4(1.0f);//변환 행렬 생성 T
        y_rotate = glm::translate(y_rotate, glm::vec3(camera_move[0], camera_move[1], camera_move[2] + 3.0f));
        y_rotate = glm::rotate(y_rotate, glm::radians(y_radian), glm::vec3(0.0, 1.0, 0.0));//ㅛ축에 대하여 회전
        y_rotate = glm::translate(y_rotate, glm::vec3(-camera_move[0], -camera_move[1], -camera_move[2] - 3.0f));

        glm::mat4 floor = glm::mat4(1.0f);//변환 행렬 생성 T
        //축
        glm::mat4 axisTransForm = glm::mat4(1.0f);//변환 행렬 생성 T
        axisTransForm = glm::rotate(axisTransForm, glm::radians(30.f), glm::vec3(1.0, 0.0, 0.0));//x축에 대하여 30도 회전
        axisTransForm = glm::rotate(axisTransForm, glm::radians(r_radian), glm::vec3(0.0, 1.0, 0.0));//x축에 대하여 30도 회전
        axisTransForm = glm::rotate(axisTransForm, glm::radians(a_radian), glm::vec3(1.0, 0.0, 0.0));//x축에 대하여 30도 회전
        axisTransForm = glm::rotate(axisTransForm, glm::radians(a_radian), glm::vec3(0.0, 1.0, 0.0));//x축에 대하여 30도 회전
        axisTransForm = glm::rotate(axisTransForm, glm::radians(a_radian), glm::vec3(0.0, 0.0, 1.0));//x축에 대하여 30도 회전
        floor = axisTransForm * y_rotate;
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(floor));//변환 행렬을 셰이더에 전달
        glDrawArrays(GL_LINES, 24, 6);//저장해둔 배열의 24번째 값부터 6개 사용
        glDrawArrays(GL_QUADS, 30, 4);


        glm::mat4 shapeTransForm = glm::mat4(1.0f);//변환 행렬 생성 T
        glm::mat4 center_R = glm::mat4(1.0f);//변환 행렬 생성 T

        {   // 본체
            glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
            shapeTransForm = glm::translate(shapeTransForm, glm::vec3(b_move[0], b_move[1], b_move[2]));
            model = axisTransForm * y_rotate * shapeTransForm;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_QUADS, 0, 24); //정육면체
        } { // 중앙
            glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
            glm::mat4 T = glm::mat4(1.0f);
            T = glm::translate(T, glm::vec3(0.0f, 0.5f, 0.0f));
            center_R = glm::rotate(center_R, glm::radians(m_radian), glm::vec3(0.0, 1.0, 0.0));
            model = axisTransForm * y_rotate * shapeTransForm * T * center_R;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_QUADS, 34, 24); //정육면체
        } { // 위1
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 T = glm::mat4(1.0f);
            glm::mat4 R = glm::mat4(1.0f);
            T = glm::translate(T, glm::vec3(-0.15f, 0.7f, 0.0f));
            R = glm::rotate(R, glm::radians(t_radian), glm::vec3(0.0, 1.0, 0.0));//x축 회전
            model = axisTransForm * y_rotate * shapeTransForm * center_R * R * T;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_QUADS, 58, 24); //정육면체
        } { // 위2
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 T = glm::mat4(1.0f);
            T = glm::translate(T, glm::vec3(0.15f, 0.7f, 0.0f));
            glm::mat4 R = glm::mat4(1.0f);
            R = glm::rotate(R, glm::radians(t_radian), glm::vec3(0.0, 1.0, 0.0));
            model = axisTransForm * y_rotate * shapeTransForm * center_R * R * T;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_QUADS, 82, 24); //정육면체
        } { // 앞1
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 T = glm::mat4(1.0f);
            glm::mat4 R = glm::mat4(1.0f);
            T = glm::translate(T, glm::vec3(e_move[0][0], e_move[0][1], e_move[0][2]));
            R = glm::rotate(R, glm::radians(90.0f + f_radian), glm::vec3(1.0, 0.0, 0.0));
            model = axisTransForm * y_rotate * shapeTransForm * T * R;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_QUADS, 106, 24); //정육면체
        } { // 앞2
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 T = glm::mat4(1.0f);
            glm::mat4 R = glm::mat4(1.0f);
            T = glm::translate(T, glm::vec3(e_move[1][0], e_move[1][1], e_move[1][2]));
            R = glm::rotate(R, glm::radians(90.0f - f_radian), glm::vec3(1.0, 0.0, 0.0));
            model = axisTransForm * y_rotate * shapeTransForm * T * R;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_QUADS, 130, 24); //정육면체
        }
    }/*상단*/ {
        glViewport(10, 590, 200, 200);

        unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform");//월드 변환 행렬값을 셰이더의 uniform mat4 modelTransform에게 넘겨줌
        unsigned int viewLocation = glGetUniformLocation(shaderID, "viewTransform");//위와 동일
        unsigned int projectionLocation = glGetUniformLocation(shaderID, "projectionTransform");//위와 동일

        //원근 투영
        glm::mat4 kTransform = glm::mat4(1.0f); 
        kTransform = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &kTransform[0][0]);

        //뷰잉 변환
        glm::mat4 vTransform = glm::mat4(1.0f);
        glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위치
        glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
        glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, -1.0f); //--- 카메라 위쪽 방향

        vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);

        glm::mat4 floor = glm::mat4(1.0f);//변환 행렬 생성 T
        //축
        glm::mat4 axisTransForm = glm::mat4(1.0f);//변환 행렬 생성 T
        axisTransForm = glm::rotate(axisTransForm, glm::radians(0.f), glm::vec3(1.0, 1.0, 1.0));//x축에 대하여 30도 회전
        floor = axisTransForm;
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(floor));//변환 행렬을 셰이더에 전달
        glDrawArrays(GL_LINES, 24, 6);//저장해둔 배열의 24번째 값부터 6개 사용
        glDrawArrays(GL_QUADS, 30, 4);


        glm::mat4 shapeTransForm = glm::mat4(1.0f);//변환 행렬 생성 T
        glm::mat4 center_R = glm::mat4(1.0f);//변환 행렬 생성 T

        {   // 본체
            glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
            shapeTransForm = glm::translate(shapeTransForm, glm::vec3(b_move[0], b_move[1], b_move[2]));
            model = axisTransForm  * shapeTransForm;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_QUADS, 0, 24); //정육면체
        } { // 중앙
            glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
            glm::mat4 T = glm::mat4(1.0f);
            T = glm::translate(T, glm::vec3(0.0f, 0.5f, 0.0f));
            center_R = glm::rotate(center_R, glm::radians(m_radian), glm::vec3(0.0, 1.0, 0.0));
            model = axisTransForm * shapeTransForm * T * center_R;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_QUADS, 34, 24); //정육면체
        } { // 위1
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 T = glm::mat4(1.0f);
            glm::mat4 R = glm::mat4(1.0f);
            T = glm::translate(T, glm::vec3(-0.15f, 0.7f, 0.0f));
            R = glm::rotate(R, glm::radians(t_radian), glm::vec3(0.0, 1.0, 0.0));//x축 회전
            model = axisTransForm * shapeTransForm * center_R * R * T;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_QUADS, 58, 24); //정육면체
        } { // 위2
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 T = glm::mat4(1.0f);
            T = glm::translate(T, glm::vec3(0.15f, 0.7f, 0.0f));
            glm::mat4 R = glm::mat4(1.0f);
            R = glm::rotate(R, glm::radians(t_radian), glm::vec3(0.0, 1.0, 0.0));
            model = axisTransForm * shapeTransForm * center_R * R * T;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_QUADS, 82, 24); //정육면체
        } { // 앞1
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 T = glm::mat4(1.0f);
            glm::mat4 R = glm::mat4(1.0f);
            T = glm::translate(T, glm::vec3(e_move[0][0], e_move[0][1], e_move[0][2]));
            R = glm::rotate(R, glm::radians(90.0f + f_radian), glm::vec3(1.0, 0.0, 0.0));
            model = axisTransForm * shapeTransForm * T * R;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_QUADS, 106, 24); //정육면체
        } { // 앞2
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 T = glm::mat4(1.0f);
            glm::mat4 R = glm::mat4(1.0f);
            T = glm::translate(T, glm::vec3(e_move[1][0], e_move[1][1], e_move[1][2]));
            R = glm::rotate(R, glm::radians(90.0f - f_radian), glm::vec3(1.0, 0.0, 0.0));
            model = axisTransForm * shapeTransForm * T * R;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_QUADS, 130, 24); //정육면체
        }
    }/*옆*/{
        glViewport(590, 590, 200, 200);

        unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform");//월드 변환 행렬값을 셰이더의 uniform mat4 modelTransform에게 넘겨줌
        unsigned int viewLocation = glGetUniformLocation(shaderID, "viewTransform");//위와 동일
        unsigned int projectionLocation = glGetUniformLocation(shaderID, "projectionTransform");//위와 동일

        //원근 투영
        glm::mat4 kTransform = glm::mat4(1.0f);
        kTransform = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &kTransform[0][0]);

        //뷰잉 변환
        glm::mat4 vTransform = glm::mat4(1.0f);
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f); //--- 카메라 위치
        glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향

        vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);

        glm::mat4 floor = glm::mat4(1.0f);//변환 행렬 생성 T
        //축
        glm::mat4 axisTransForm = glm::mat4(1.0f);//변환 행렬 생성 T
        axisTransForm = glm::rotate(axisTransForm, glm::radians(0.0f), glm::vec3(1.0, 1.0, 1.0));//x축에 대하여 30도 회전
        floor = axisTransForm;
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(floor));//변환 행렬을 셰이더에 전달
        glDrawArrays(GL_LINES, 24, 6);//저장해둔 배열의 24번째 값부터 6개 사용
        glDrawArrays(GL_QUADS, 30, 4);


        glm::mat4 shapeTransForm = glm::mat4(1.0f);//변환 행렬 생성 T
        glm::mat4 center_R = glm::mat4(1.0f);//변환 행렬 생성 T

        {   // 본체
            glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
            shapeTransForm = glm::translate(shapeTransForm, glm::vec3(b_move[0], b_move[1], b_move[2]));
            model = axisTransForm * shapeTransForm;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_QUADS, 0, 24); //정육면체
        } { // 중앙
            glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
            glm::mat4 T = glm::mat4(1.0f);
            T = glm::translate(T, glm::vec3(0.0f, 0.5f, 0.0f));
            center_R = glm::rotate(center_R, glm::radians(m_radian), glm::vec3(0.0, 1.0, 0.0));
            model = axisTransForm * shapeTransForm * T * center_R;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_QUADS, 34, 24); //정육면체
        } { // 위1
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 T = glm::mat4(1.0f);
            glm::mat4 R = glm::mat4(1.0f);
            T = glm::translate(T, glm::vec3(-0.15f, 0.7f, 0.0f));
            R = glm::rotate(R, glm::radians(t_radian), glm::vec3(0.0, 1.0, 0.0));//x축 회전
            model = axisTransForm * shapeTransForm * center_R * R * T;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_QUADS, 58, 24); //정육면체
        } { // 위2
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 T = glm::mat4(1.0f);
            T = glm::translate(T, glm::vec3(0.15f, 0.7f, 0.0f));
            glm::mat4 R = glm::mat4(1.0f);
            R = glm::rotate(R, glm::radians(t_radian), glm::vec3(0.0, 1.0, 0.0));
            model = axisTransForm * shapeTransForm * center_R * R * T;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_QUADS, 82, 24); //정육면체
        } { // 앞1
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 T = glm::mat4(1.0f);
            glm::mat4 R = glm::mat4(1.0f);
            T = glm::translate(T, glm::vec3(e_move[0][0], e_move[0][1], e_move[0][2]));
            R = glm::rotate(R, glm::radians(90.0f + f_radian), glm::vec3(1.0, 0.0, 0.0));
            model = axisTransForm * shapeTransForm * T * R;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_QUADS, 106, 24); //정육면체
        } { // 앞2
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 T = glm::mat4(1.0f);
            glm::mat4 R = glm::mat4(1.0f);
            T = glm::translate(T, glm::vec3(e_move[1][0], e_move[1][1], e_move[1][2]));
            R = glm::rotate(R, glm::radians(90.0f - f_radian), glm::vec3(1.0, 0.0, 0.0));
            model = axisTransForm * shapeTransForm * T * R;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_QUADS, 130, 24); //정육면체
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
    case 'b':
        stop = false;
        if (b_dir == 0)
            b_dir = 1;
        else
            b_dir = 0;
        break;
    case 'B':
        stop = false;
        if (b_dir == 0)
            b_dir = -1;
        else
            b_dir = 0;
        break;
    case 'm':
        stop = false;
        if (m_dir == 1)
            m_dir = 0;
        else
            m_dir += 1;
        break;
    case 'M':
        stop = false;
        if (m_dir == -1)
            m_dir = 0;
        else
            m_dir -= 1;
        break;
    case 'x':
        camera_move[0] += 0.1f;
        break;
    case 'X':
        camera_move[0] -= 0.1f;
        break;
    case 'z':
        camera_move[2] += 0.1f;
        break;
    case 'Z':
        camera_move[2] -= 0.1f;
        break;
    case 'y':
        stop = false;
        if (y_dir == 1)
            y_dir = 0;
        else
            y_dir += 1;
        break;
    case 'Y':
        stop = false;
        if (y_dir == -1)
            y_dir = 0;
        else
            y_dir -= 1;
        break;
    case 'r':
        stop = false;
        if (r_dir == 1)
            r_dir = 0;
        else
            r_dir += 1;
        break;
    case 'R':
        stop = false;
        if (r_dir == -1)
            r_dir = 0;
        else
            r_dir -= 1;
        break;
    case 't':
        stop = false;
        if (t_dir == 0)
            t_dir = 1;
        else
            t_dir = 0;
        break;
    case 'T':
        stop = false;
        if (t_dir == 0)
            t_dir = -1;
        else
            t_dir = 0;
        break;
    case 'f':
        stop = false;
        if (f_dir == 0)
            f_dir = 1;
        else
            f_dir = 0;
        break;
    case 'F':
        stop = false;
        if (f_dir == 0)
            f_dir = -1;
        else
            f_dir = 0;
        break;
    case 'e':
        stop = false;
        if (e_dir == 0)
            e_dir = 1;
        else
            e_dir = 0;
        break;
    case 'E':
        stop = false;
        if (e_dir == 0)
            e_dir = -1;
        else
            e_dir = 0;
        break;
    case 'a':
        stop = false;
        if (a_dir == 0)
            a_dir = 1;
        else
            a_dir = 0;
        break;
    case 'A':
        stop = false;
        if (a_dir == 0)
            a_dir = -1;
        else
            a_dir = 0;
        break;
    case 's':
        stop = true;
        break;
    case 'c':
        stop = true;

        b_move[0] = 0.0f;
        b_move[1] = 0.0f;
        b_move[2] = 0.0f;

        e_move[0][0] = -0.3f;
        e_move[0][1] = 0.2f;
        e_move[0][2] = 0.5f;
        e_move[1][0] = 0.3f;
        e_move[1][1] = 0.2f;
        e_move[1][2] = 0.5f;

        m_radian = 0.0f, t_radian = 0.0f, f_radian = 0.0f, y_radian = 0.0f, a_radian = 0.0f, r_radian = 0.0f;

        camera_move[0] = 0.0f;
        camera_move[1] = 0.0f;
        camera_move[2] = 1.0f;

        b_dir = 0, y_dir = 0, m_dir, f_dir = 0, e_dir = 0, t_dir = 0, a_dir = 0, r_dir = 0;
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
    if (!stop) {
        b_move[0] += 0.1f * b_dir;
        if (b_move[0] < -1.5f || b_move[0] > 1.5f)
            b_dir *= -1;

        y_radian += y_dir;


        a_radian += a_dir;

        r_radian += r_dir;

        m_radian += 1.0f * m_dir;

        f_radian += 1.0f * f_dir;
        if (f_radian > 90.0f || f_radian < -90.0f)
            f_dir *= -1;
        if (e_dir == 1) {
            if (e_move[0][0] != e_move[1][0]) {
                if (e_move[0][0] < 0)
                    e_move[0][0] += 0.01f;
                else if (e_move[0][0] == 0);
                else
                    e_move[0][0] = 0.0f;
                if (e_move[1][0] > 0)
                    e_move[1][0] -= 0.01f;
                else if (e_move[1][0] == 0)
                    e_dir = -1;
                else
                    e_move[1][0] = 0.0f;
            }
            else
                e_dir = -1;
        }
        else if (e_dir == -1) {
            if (e_move[0][0] >= -0.3f)
                e_move[0][0] -= 0.01f;
            else if (e_move[0][0] == -0.3f);
            else
                e_move[0][0] = -0.3f;
            if (e_move[1][0] <= 0.3f)
                e_move[1][0] += 0.01f;
            else if (e_move[1][0] == 0.3f);
            else
                e_move[1][0] = 0.3f;
            if (e_move[1][0] == 0.3f && e_move[0][0] == -0.3f)
                e_dir = 1;
        }
        t_radian += 1.0f * t_dir;
    }
    glutTimerFunc(10, TimerFunc, 1);
    glutPostRedisplay();
}