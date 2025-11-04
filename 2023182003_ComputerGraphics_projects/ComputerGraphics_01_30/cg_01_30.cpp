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
std::uniform_real_distribution<GLclampf> rd_color(0.0f, 1.0f), rd_speed(0.01f, 0.05f), rd_pos(-3.0f, 3.0f), rd_spped2(1.0f, 3.0f), rd_place(-3.0f, 3.0f);

struct Snow {
    GLfloat speed = rd_speed(dre), pos[3]{ rd_pos(dre), 5.5f, rd_pos(dre) };
};
Snow snow[50];
struct Planet {
    GLuint vao;
    GLfloat speed = rd_spped2(dre), cos = rd_color(dre) * 180.0f, radian, rgb[3]{ rd_color(dre),rd_color(dre), rd_color(dre) };
};
Planet moon[3];
struct Building {
    GLfloat pos[3]{ 0.0f, 0.5f, 0.0f }, color[3]{ rd_color(dre), rd_color(dre), rd_color(dre) };
};
Building building[5];

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
     1.0f,1.0f,-1.0f,//24

   2.0f,0,0, //x,y,z 축
   -2.0f,0,0,
   0,2.0f,0,
   0,-2.0f,0,
   0,0,2.0f,
   0,0,-2.0f, //30

   -2.0f,0.0f,2.0f, //바닥
   -2.0f,0.0f,-2.0f,
   2.0f,0.0f,-2.0f,
   2.0f,0.0f,2.0f,// 34

   //사면체
     0.0f,  1.0f,  0.0f,
    -1.0f, -0.0f,  1.0f,
     1.0f, -0.0f,  1.0f,

     0.0f,  1.0f,  0.0f,
    -1.0f, -0.0f, -1.0f,
    -1.0f, -0.0f,  1.0f,

     0.0f,  1.0f,  0.0f,
     1.0f, -0.0f,  1.0f,
     1.0f, -0.0f, -1.0f,

     0.0f,  1.0f,  0.0f,
     1.0f, -0.0f, -1.0f,
    -1.0f, -0.0f, -1.0f,

    -1.0f, -0.0f,  1.0f,
    -1.0f, -0.0f, -1.0f,
     1.0f, -0.0f, -1.0f,
     1.0f, -0.0f,  1.0f
};//정육면체, 축,정사면체 벡터들
float vertexNormal[] = {
    0.0f, 0.0f, 1.0f,//앞면
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,

    0.0f, 1.0f, 0.0f,//윗면
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,

   -1.0f, 0.0f, 0.0f,//왼면
   -1.0f, 0.0f, 0.0f,
   -1.0f, 0.0f, 0.0f,
   -1.0f, 0.0f, 0.0f,

    0.0f, 0.0f,-1.0f,//뒷면
    0.0f, 0.0f,-1.0f,
    0.0f, 0.0f,-1.0f,
    0.0f, 0.0f,-1.0f,

    0.0f,-1.0f, 0.0f,//아래
    0.0f,-1.0f, 0.0f,
    0.0f,-1.0f, 0.0f,
    0.0f,-1.0f, 0.0f,

    1.0f, 0.0f, 0.0f,//오른쪽
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,

    //선
    1.0,0.0,0.0,
    -1.0,0.0,0.0,
    0.0,1.0,0.0,
    0.0,-1.0,0.0,
    0.0,0.0,1.0,
    0.0,0.0,-1.0,

    0.0f,1.0f,0.0f,//아래
    0.0f,1.0f,0.0f,
    0.0f,1.0f,0.0f,
    0.0f,1.0f,0.0f,

    //정사면체
      0.0f,  1.0f,  1.0f,//앞면
      0.0f,  1.0f,  1.0f,
      0.0f,  1.0f,  1.0f,

     -1.0f,  1.0f,  0.0f,//왼
     -1.0f,  1.0f,  0.0f,
     -1.0f,  1.0f,  0.0f,

      1.0f,  1.0f,  0.0f,//오른
      1.0f,  1.0f,  0.0f,
      1.0f,  1.0f,  0.0f,

      0.0f,  1.0f, -1.0f,//뒷면
      0.0f,  1.0f, -1.0f,
      0.0f,  1.0f, -1.0f,

      0.0f, -1.0f,  0.0f,//아래
      0.0f, -1.0f,  0.0f,
      0.0f, -1.0f,  0.0f,
      0.0f, -1.0f,  0.0f
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
    glutCreateWindow("CG_01_26");

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
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexNormal), vertexNormal, GL_STATIC_DRAW);
    //--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //--- attribute 인덱스 1번을 사용 가능하게 함.
    glEnableVertexAttribArray(1);

    for (int i = 0; i < 3; ++i) {
        GLfloat spots[180][3]{};
        GLfloat colors[180][3]{};
        for (int j = 0; j < 180; ++j) {
            spots[j][0] = 2.0f * glm::cos(glm::radians(2.0f * j));
            spots[j][1] = 0.0f;
            spots[j][2] = 2.0f * glm::sin(glm::radians(2.0f * j));
            colors[j][0] = moon[i].rgb[0];
            colors[j][1] = moon[i].rgb[1];
            colors[j][2] = moon[i].rgb[2];
        }
        glGenVertexArrays(1, &moon[i].vao); //--- VAO 를 지정하고 할당하기
        glGenBuffers(2, VBO); //--- 2개의 VBO를 지정하고 할당하기

        glBindVertexArray(moon[i].vao); //--- VAO를 바인드하기
        //--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        //--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
        //--- triShape 배열의 사이즈: 9 * float
        glBufferData(GL_ARRAY_BUFFER, sizeof(spots), spots, GL_STATIC_DRAW);
        //--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //--- attribute 인덱스 0번을 사용가능하게 함
        glEnableVertexAttribArray(0);

        //--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        //--- 변수 colors에서 버텍스 색상을 복사한다.
        //--- colors 배열의 사이즈: 9 *float 
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
        //--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //--- attribute 인덱스 1번을 사용 가능하게 함.
        glEnableVertexAttribArray(1);
    }
    for (int i = 0; i < 5; ++i) {
        if (i < 3) {
            building[i].pos[0] += -1.5f;
            building[i].pos[2] += -1.5f + 1.5f * (2 - i);
        }
        else {
            building[i].pos[0] += -1.5f + 1.5f * (i - 2);
            building[i].pos[2] += -1.5f;
        }
        building[i].color[0] = 0.2f * i;
        building[i].color[1] = 0.2f * i;
        building[i].color[2] = 0.2f * i;
    }
}

bool r_switch, y_switch, m_switch, s_switch;

float r_radian, y_radian, level = 0.5f;
float camera_move[3]{ 0.0f, 0.0f, 1.0f }, light_color[3]{ rd_color(dre), rd_color(dre), rd_color(dre) }, dist, light_y = 45.0f;

int triangle_level;

GLvoid drawScene()
{
    glUseProgram(shaderID);
    glBindVertexArray(VAO);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//와이어 객체

    unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform");//월드 변환 행렬값을 셰이더의 uniform mat4 modelTransform에게 넘겨줌
    unsigned int viewLocation = glGetUniformLocation(shaderID, "viewTransform");//위와 동일
    unsigned int projectionLocation = glGetUniformLocation(shaderID, "projectionTransform");//위와 동일

    //원근 투영
    glm::mat4 kTransform = glm::mat4(1.0f);
    kTransform = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
    kTransform = glm::translate(kTransform, glm::vec3(0.0, 0.0, -8.0f));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &kTransform[0][0]);

    //뷰잉 변환
    glm::mat4 vTransform = glm::mat4(1.0f);
    glm::vec3 cameraPos = glm::vec3(camera_move[0], camera_move[1], camera_move[2]); //--- 카메라 위치
    glm::vec3 cameraDirection = glm::vec3(camera_move[0], camera_move[1], camera_move[2] - 1.0f); //--- 카메라 바라보는 방향
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향

    vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);

    //축
    glm::mat4 axisTransForm = glm::mat4(1.0f);//변환 행렬 생성 T
    axisTransForm = glm::rotate(axisTransForm, glm::radians(30.0f), glm::vec3(1.0, 0.0, 0.0));//x축에 대하여 30도 회전
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(axisTransForm));//변환 행렬을 셰이더에 전달

    unsigned int lightLevelLocation = glGetUniformLocation(shaderID, "lightLevel"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
    unsigned int viewPosLocation = glGetUniformLocation(shaderID, "viewPos");
    unsigned int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
    unsigned int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
    unsigned int objColorLocation = glGetUniformLocation(shaderID, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
    glUniform1f(lightLevelLocation, level);
    glUniform3f(viewPosLocation, camera_move[0], camera_move[1], camera_move[2]);
    glm::vec4 light_pos = { dist, 0.0f, 0.0f, 1.0f };
    glm::mat4 light_move = glm::mat4(1.0f);
    light_move = glm::rotate(light_move, glm::radians(r_radian), glm::vec3(0.0, 1.0, 0.0));//x축에 대하여 30도 회전
    light_move = glm::rotate(light_move, glm::radians(light_y), glm::vec3(0.0, 0.0, 1.0));//x축에 대하여 30도 회전
    light_pos = axisTransForm * light_move * light_pos;
    glUniform3f(lightPosLocation, light_pos[0], light_pos[1], light_pos[2]);
    if (m_switch)
        glUniform3f(lightColorLocation, light_color[0], light_color[1], light_color[2]);
    else
        glUniform3f(lightColorLocation, 0.0f, 0.0f, 0.0f);

    glUniform3f(objColorLocation, 0.151, 0.315, 0.124);
    glDrawArrays(GL_LINES, 24, 6);//저장해둔 배열의 24번째 값부터 6개 사용
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(y_radian), glm::vec3(0.0, 1.0, 0.0));
        model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
        model = axisTransForm * model;
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

        glUniform3f(objColorLocation, 0.5, 0.2, 0.05);
        glDrawArrays(GL_QUADS, 30, 4);
    }
    //triangle 0.5사이즈...
    glUniform3f(objColorLocation, 0.2f, 0.8f, 0.3f);
    for (int i = 0; i < pow(5, triangle_level); ++i) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(y_radian), glm::vec3(0.0, 1.0, 0.0));
        for (int j = 0; j < triangle_level; ++j) {
            int number = pow(5, j);
            number = (i / number) % 5;
            switch (number) {
            case 0:
                model = glm::translate(model, glm::vec3(0.0f, 1.0f / pow(2, triangle_level - j), 0.0f));
                break;
            case 1:
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f / pow(2, triangle_level - j)));
                model = glm::translate(model, glm::vec3(1.0f / pow(2, triangle_level - j), 0.0f, 0.0f));
                break;
            case 2:
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f / pow(2, triangle_level - j)));
                model = glm::translate(model, glm::vec3(-1.0f / pow(2, triangle_level - j), 0.0f, 0.0f));
                break;
            case 3:
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f / pow(2, triangle_level - j)));
                model = glm::translate(model, glm::vec3(1.0f / pow(2, triangle_level - j), 0.0f, 0.0f));
                break;
            case 4:
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f / pow(2, triangle_level - j)));
                model = glm::translate(model, glm::vec3(-1.0f / pow(2, triangle_level - j), 0.0f, 0.0f));
                break;
            default:
                std::cout << "읭?" << std::endl;
                break;
            }
        }
        model = glm::scale(model, glm::vec3(1.0f / pow(2, triangle_level), 1.0f / pow(2, triangle_level), 1.0f / pow(2, triangle_level)));
        model = axisTransForm * model;
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

        glDrawArrays(GL_TRIANGLES, 34, 12);
        glDrawArrays(GL_QUADS, 46, 4);
    }
    //light object
    {
        GLUquadricObj* qobj;
        qobj = gluNewQuadric();
        gluQuadricNormals(qobj, GLU_SMOOTH); // 생략 가능
        gluQuadricOrientation(qobj, GLU_OUTSIDE); // 생략 가능

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(light_pos[0], light_pos[1], light_pos[2]));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        model = model;
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

        glUniform3f(objColorLocation, 1.0f, 1.0f, 1.0f);
        gluSphere(qobj, 1.0f, 5, 5);
    }
    if (m_switch)
        glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);
    else
        glUniform3f(lightColorLocation, 0.0f, 0.0f, 0.0f);
    //building
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    for (int i = 0; i < 5; ++i) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(y_radian), glm::vec3(0.0, 1.0, 0.0));
        model = glm::translate(model, glm::vec3(building[i].pos[0], building[i].pos[1], building[i].pos[2]));
        model = glm::scale(model, glm::vec3(0.25f, 0.5f, 0.25f));
        model = axisTransForm * model;
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

        glUniform3f(objColorLocation, building[i].color[0], building[i].color[1], building[i].color[2]);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDrawArrays(GL_QUADS, 0, 24);
    }
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    /*
    //planet
    for (int i = 0; i < 3; ++i) {
        glBindVertexArray(moon[i].vao);
        glUniform3f(objColorLocation, moon[i].rgb[0], moon[i].rgb[1], moon[i].rgb[2]);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(moon[i].cos), glm::vec3(0.0, 0.0, 1.0));//x축에 대하여 30도 회전
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(axisTransForm * model));

        glDrawArrays(GL_LINE_STRIP, 0, 180);

        GLUquadricObj* qobj;
        qobj = gluNewQuadric();
        gluQuadricNormals(qobj, GLU_SMOOTH); // 생략 가능
        gluQuadricOrientation(qobj, GLU_OUTSIDE); // 생략 가능

        model = glm::rotate(model, glm::radians(moon[i].radian), glm::vec3(0.0, 1.0, 0.0));//x축에 대하여 30도 회전
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 2.0f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(axisTransForm * model));

        gluSphere(qobj, 0.1f, 10, 10);
    }*/
    //snow
    for (int i = 0; i < 50; ++i) {
        GLUquadricObj* qobj;
        qobj = gluNewQuadric();
        gluQuadricNormals(qobj, GLU_SMOOTH); // 생략 가능
        gluQuadricOrientation(qobj, GLU_OUTSIDE); // 생략 가능

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(y_radian), glm::vec3(0.0, 1.0, 0.0));
        model = glm::translate(model, glm::vec3(snow[i].pos[0], snow[i].pos[1], snow[i].pos[2]));
        model = axisTransForm * model;
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

        glUniform3f(objColorLocation, 1.0f, 1.0f, 1.0f);
        gluSphere(qobj, 0.015f, 20, 20);
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
    case 's':
        s_switch = !s_switch;
        break;
    case '0':
        triangle_level = 0;
        break;
    case '1':
        triangle_level = 1;
        break;
    case '2':
        triangle_level = 2;
        break;
    case '3':
        triangle_level = 3;
        break;
    case '4':
        triangle_level = 4;
        break;
    case '5':
        triangle_level = 5;
        break;
    case 'r':
        if (dist == 0)
            dist = 3.0f;
        r_switch = !r_switch;
        break;
    case 'y':
        y_switch = !y_switch;
        break;
    case 'm':
        m_switch = !m_switch;
        break;
    case 'c':
        light_color[0] = rd_color(dre), light_color[1] = rd_color(dre), light_color[2] = rd_color(dre);
        break;
    case 'n':
        if (dist > 0)
            dist -= 0.1f;
        break;
    case 'f':
        dist += 0.1f;
        break;
    case '+':
        if (level < 2.0f)
            level += 0.1f;
        break;
    case '-':
        if (level > -1.0f)
            level -= 0.1f;
        break;
    case 'p':
        if (dist < 1.0f)
            dist = 1.0f;
        if (int(r_radian + 45) % 90 != 0)
            r_radian += 90 - int(r_radian + 45) % 90;
        else
            r_radian += 90.0f;
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
    if (s_switch) {
        for (int i = 0; i < 50; ++i) {
            snow[i].pos[1] -= snow[i].speed;
            if (snow[i].pos[1] < 0.0f)
                snow[i].pos[1] = 5.5f;
        }
    }
    if (y_switch)
        y_radian += 0.2f;
    if (r_switch)
        r_radian += 0.5f;
    for (int i = 0; i < 3; ++i)
        moon[i].radian += moon[i].speed;
    glutPostRedisplay();
    glutTimerFunc(10, TimerFunc, 1);
}