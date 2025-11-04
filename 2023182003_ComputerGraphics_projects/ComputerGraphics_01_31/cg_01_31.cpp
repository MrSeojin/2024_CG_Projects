#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <random>
#include <gl\glew.h>
#include <gl\freeglut.h>
#include <gl\freeglut_ext.h>
#include <gl\glm\glm\glm.hpp>
#include <gl\glm\glm\ext.hpp>
#include <gl\glm\glm\gtc\matrix_transform.hpp>

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_real_distribution<GLclampf> rd_color(0.0f, 1.0f), rd_point(-1.0f, 1.0f);

GLuint coord_vao[3], hexa_vao, penta_vao, background_vao, vbo[3];
GLfloat background_r = 0.2f, background_g = 0.2f, background_b = 0.2f, radian_x, radian_y, move_x, move_y;
GLint width, height;
GLuint shaderProgramID;	//셰이더 프로그램 이름
GLuint vertexShader;	// 버텍스 셰이더 객체
GLuint fragmentShader;	// 프래그먼트 세이더 객체
unsigned int texture_index[7];
BITMAPINFO* bmp;
int shape_type, x_dir, y_dir;
BOOL h_switch, fill = true;

char* filetobuf(const char* file);
GLubyte* LoadDIBitmap(const char* filename, BITMAPINFO** info);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
void InitBuffer();
void InitTextures();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid SpecialKeyboard(int key, int x, int y);
GLvoid animation(int value);
void main(int argc, char** argv)	// 윈도우 생성
{
	width = 800, height = 800;
	glutInit(&argc, argv);							// glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);	// 디스플레이 모드 설정
	glutInitWindowPosition(50, 50);					// 윈도우의 위치 지정
	glutInitWindowSize(width, height);					// 윈도우의 크기 지정
	glutCreateWindow("project_01_29");	// 윈도우 생성 (윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)						// glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	make_shaderProgram();
	InitBuffer();
	InitTextures();

	glutTimerFunc(100, animation, 1);

	glutDisplayFunc(drawScene);						// 출력 함수의 지정
	glutReshapeFunc(Reshape);						// 다시 그리기 함수 지정
	glutKeyboardFunc(Keyboard);		// 키보드 입력 콜백함수 지정
	glutSpecialFunc(SpecialKeyboard);

	glutMainLoop();									// 이벤트 처리 시작
}

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
GLubyte* LoadDIBitmap(const char* filename, BITMAPINFO** info)
{
	FILE* fp;
	GLubyte* bits;
	int bitsize{}, infosize{};
	BITMAPFILEHEADER header;
	//--- 바이너리 읽기 모드로 파일을 연다
	if ((fp = fopen(filename, "rb")) == NULL)
		return NULL;
	//--- 비트맵 파일 헤더를 읽는다.
	if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1) {
		fclose(fp); return NULL;
	}
	//--- 파일이 BMP 파일인지 확인한다.
	if (header.bfType != 'MB') {
		fclose(fp); return NULL;
	}
	//--- BITMAPINFOHEADER 위치로 간다.
	infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);
	//--- 비트맵 이미지 데이터를 넣을 메모리 할당을 한다.
	if ((*info = (BITMAPINFO*)malloc(infosize)) == NULL) {
		fclose(fp); return NULL;
	}
	//--- 비트맵 인포 헤더를 읽는다.
	if (fread(*info, 1, infosize, fp) < (unsigned int)infosize) {
		free(*info);
		fclose(fp); return NULL;
	}
	//--- 비트맵의 크기 설정
	if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
		bitsize = ((*info)->bmiHeader.biWidth *
			(*info)->bmiHeader.biBitCount + 7) / 8 *
		abs((*info)->bmiHeader.biHeight);
	//--- 비트맵의 크기만큼 메모리를 할당한다.
	if ((bits = (GLubyte*)malloc(bitsize)) == NULL) {
		free(*info);
		fclose(fp); return NULL;
	}
	//--- 비트맵 데이터를 bit(GLubyte 타입)에 저장한다.
	if (fread(bits, 1, bitsize, fp) < (unsigned int)bitsize) {
		free(*info); free(bits);
		fclose(fp); return NULL;
	}
	fclose(fp);
	return bits;
}
void make_vertexShaders()
{
	GLchar* vertexSource;

	vertexSource = filetobuf("vertex.glsl");	//text파일
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, NULL);	//vertexShader하나 만들기
	glCompileShader(vertexShader);

	// 만들어지지 않으면 오류 출력
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader error\n" << errorLog << std::endl;
		return;
	}
}
void make_fragmentShaders()
{
	GLchar* fragmentSource;

	fragmentSource = filetobuf("fragment.glsl");
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, NULL);
	glCompileShader(fragmentShader);

	// 만들어지지 않으면 오류 출력
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader error\n" << errorLog << std::endl;
		return;
	}
}
GLuint make_shaderProgram()
{
	make_vertexShaders();
	make_fragmentShaders();

	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);

	glLinkProgram(shaderProgramID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// 연결되지 않으면 오류 출력
	GLint result;
	GLchar errorLog[512];
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderProgramID, 512, NULL, errorLog);
		std::cerr << "ERROR:shader program 연결 실패\n" << errorLog << std::endl;
		return false;
	}
	return true;
}
void InitBuffer()
{
	{	// 축
		//x
		glGenVertexArrays(1, &coord_vao[0]);
		glBindVertexArray(coord_vao[0]);
		glGenBuffers(2, vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		const GLfloat x_coord[2][3]{ {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), x_coord, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		const GLfloat x_color[2][3]{ { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } };
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), x_color, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		//y
		glGenVertexArrays(1, &coord_vao[1]);
		glBindVertexArray(coord_vao[1]);
		glGenBuffers(3, vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		const GLfloat y_coord[2][3]{ {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f} };
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), y_coord, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		const GLfloat y_color[2][3]{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } };
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), y_color, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
		//z
		glGenVertexArrays(1, &coord_vao[2]);
		glBindVertexArray(coord_vao[2]);
		glGenBuffers(3, vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		const GLfloat z_coord[2][3]{ {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 1.0f} };
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), z_coord, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		const GLfloat z_color[2][3]{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } };
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), z_color, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}
	{	// 5
		GLfloat spots[]{
			//사면체
			 0, 0.5f, 0,
			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,

			 0, 0.5f, 0,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,

			 0, 0.5f, 0,
			 0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f, -0.5f,

			 0, 0.5f, 0,
			 0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f
		};
		GLfloat color[]{
			//정사면체
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
		};
		GLfloat Texture[]{
			//앞면
			0.5, 1.0,
			0.0, 0.0,
			1.0, 0.0,

			//왼면
			0.5, 1.0,
			0.0, 0.0,
			1.0, 0.0,

			//오른
			0.5, 1.0,
			0.0, 0.0,
			1.0, 0.0,

			//뒷면
			0.5, 1.0,
			0.0, 0.0,
			1.0, 0.0,

			//아래
			1.0, 1.0,
			1.0, 0.0,
			0.0, 0.0,
			0.0, 1.0
		};
		glGenVertexArrays(1, &penta_vao);
		glBindVertexArray(penta_vao);
		glGenBuffers(3, vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(spots), spots, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Texture), Texture, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);
	}
	//8	
	{
		GLfloat spots[]{
			-1.0f, 1.0f, 1.0f, //앞면
			-1.0f,-1.0f, 1.0f,
			 1.0f,-1.0f, 1.0f,
			 1.0f, 1.0f, 1.0f,

			-1.0f, 1.0f,-1.0f, //윗면
			-1.0f, 1.0f, 1.0f,
			 1.0f, 1.0f, 1.0f,
			 1.0f, 1.0f,-1.0f,

			-1.0f,1.0f,-1.0f, //왼쪽옆
			-1.0f,-1.0f,-1.0f,
			-1.0f,-1.0f,1.0f,
			-1.0f,1.0f,1.0f,

			 1.0f, 1.0f,-1.0f, //뒷면
			 1.0f,-1.0f,-1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f, 1.0f, -1.0f,

				-1.0f, -1.0f, 1.0f, //아랫면
				-1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, 1.0f,

				1.0f, 1.0f, 1.0f, //오른쪽 옆
				1.0f, -1.0f, 1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f, 1.0f, -1.0f,//24
		};
		GLfloat color[]{
			//정사면체
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
		};
		GLfloat Texture[]{
			//앞면
			1.0, 1.0,
			1.0, 0.0,
			0.0, 0.0,
			0.0, 1.0,

			//윗면
			1.0, 1.0,
			1.0, 0.0,
			0.0, 0.0,
			0.0, 1.0,

			//왼면
			1.0, 1.0,
			1.0, 0.0,
			0.0, 0.0,
			0.0, 1.0,

			//뒷면
			1.0, 1.0,
			1.0, 0.0,
			0.0, 0.0,
			0.0, 1.0,

			//아래
			1.0, 1.0,
			1.0, 0.0,
			0.0, 0.0,
			0.0, 1.0,

			//오른
			1.0, 1.0,
			1.0, 0.0,
			0.0, 0.0,
			0.0, 1.0
		};
		glGenVertexArrays(1, &hexa_vao);
		glBindVertexArray(hexa_vao);
		glGenBuffers(3, vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(spots), spots, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Texture), Texture, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);
	}
	//background
	{
		GLfloat spots[]{
			-5.0f, 5.0f,-5.0f, //앞면
			-5.0f,-5.0f,-5.0f,
			 5.0f,-5.0f,-5.0f,
			 5.0f, 5.0f,-5.0f,
		};
		GLfloat color[]{
			1,1,1,
			1,1,1,
			1,1,1,
			1,1,1
		};
		GLfloat Texture[]{
			1.0, 1.0,
			1.0, 0.0,
			0.0, 0.0,
			0.0, 1.0
		};
		glGenVertexArrays(1, &background_vao);
		glBindVertexArray(background_vao);
		glGenBuffers(3, vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(spots), spots, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Texture), Texture, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);
	}
}
void InitTextures() 
{
	glGenTextures(7, texture_index);
	glUseProgram(shaderProgramID);

	//--- texture[0]
	int tLocation1 = glGetUniformLocation(shaderProgramID, "outTexture1"); //--- outTexture1 유니폼 샘플러의 위치를 가져옴
	glUniform1i(tLocation1, 0); //--- 샘플러를 0번 유닛으로 설정
	glBindTexture(GL_TEXTURE_2D, texture_index[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data1 = LoadDIBitmap("front.bmp", &bmp);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 1000, 1000, 0, GL_BGR, GL_UNSIGNED_BYTE, data1);

	//--- texture[1]
	int tLocation2 = glGetUniformLocation(shaderProgramID, "outTexture2"); //--- outTexture2 유니폼 샘플러의 위치를 가져옴
	glUniform1i(tLocation2, 1);
	glBindTexture(GL_TEXTURE_2D, texture_index[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data2 = LoadDIBitmap("up.bmp", &bmp);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 1000, 1000, 0, GL_BGR, GL_UNSIGNED_BYTE, data2);

	//--- texture[2]
	int tLocation3 = glGetUniformLocation(shaderProgramID, "outTexture3"); //--- outTexture3 유니폼 샘플러의 위치를 가져옴
	glUniform1i(tLocation3, 2);
	glBindTexture(GL_TEXTURE_2D, texture_index[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data3 = LoadDIBitmap("left.bmp", &bmp);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 1000, 1000, 0, GL_BGR, GL_UNSIGNED_BYTE, data3);

	//--- texture[3]
	int tLocation4 = glGetUniformLocation(shaderProgramID, "outTexture4"); //--- outTexture4 유니폼 샘플러의 위치를 가져옴
	glUniform1i(tLocation4, 3);
	glBindTexture(GL_TEXTURE_2D, texture_index[3]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data4 = LoadDIBitmap("back.bmp", &bmp);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 1000, 1000, 0, GL_BGR, GL_UNSIGNED_BYTE, data4);

	//--- texture[4]
	int tLocation5 = glGetUniformLocation(shaderProgramID, "outTexture5"); //--- outTexture5 유니폼 샘플러의 위치를 가져옴
	glUniform1i(tLocation5, 4);
	glBindTexture(GL_TEXTURE_2D, texture_index[4]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data5 = LoadDIBitmap("bottom.bmp", &bmp);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 1000, 1000, 0, GL_BGR, GL_UNSIGNED_BYTE, data5);

	//--- texture[5]
	int tLocation6 = glGetUniformLocation(shaderProgramID, "outTexture6"); //--- outTexture6 유니폼 샘플러의 위치를 가져옴
	glUniform1i(tLocation6, 5);
	glBindTexture(GL_TEXTURE_2D, texture_index[5]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data6 = LoadDIBitmap("right.bmp", &bmp);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 1000, 1000, 0, GL_BGR, GL_UNSIGNED_BYTE, data6);

	//--- texture[6] 배경
	int tLocation7 = glGetUniformLocation(shaderProgramID, "outTexture7"); //--- outTexture7 유니폼 샘플러의 위치를 가져옴
	glUniform1i(tLocation7, 6);
	glBindTexture(GL_TEXTURE_2D, texture_index[6]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data7 = LoadDIBitmap("sky.bmp", &bmp);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 1000, 1000, 0, GL_BGR, GL_UNSIGNED_BYTE, data7);
}
GLvoid drawScene()
{
	glClearColor(background_r, background_g, background_b, 1.0f);			// 바탕색을 파랑색으로 지정
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);					// 설정된 색으로 전체를 칠하기
	//shader 그리기----------------------
	glUseProgram(shaderProgramID);	// 활성화 한 번만 해주면 된다.
	if (h_switch)
		glEnable(GL_DEPTH_TEST);	// 깊이 검사
	else
		glDisable(GL_DEPTH_TEST);	// 깊이 검사 

	unsigned int indexLocation = glGetUniformLocation(shaderProgramID, "index");//위와 동일
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");//월드 변환 행렬값을 셰이더의 uniform mat4 modelTransform에게 넘겨줌
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");//위와 동일
	unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");//위와 동일

	//원근 투영
	glm::mat4 kTransform = glm::mat4(1.0f);
	kTransform = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
	kTransform = glm::translate(kTransform, glm::vec3(0.0, 0.0, -5.0f));
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &kTransform[0][0]);

	//뷰잉 변환
	glm::mat4 vTransform = glm::mat4(1.0f);
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f); //--- 카메라 위치
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향

	vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);

	// x, y축 30도씩
	glm::mat4 Rx = glm::mat4(1.0f);
	glm::mat4 Ry = glm::mat4(1.0f);
	glm::mat4 R = glm::mat4(1.0f);
	Rx = glm::rotate(Rx, glm::radians(30.0f), glm::vec3(1.0, 0.0, 0.0));
	Ry = glm::rotate(Ry, glm::radians(30.0f), glm::vec3(0.0, 1.0, 0.0));
	R = Rx * Ry;
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(R));

	glLineWidth(2);
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glUniform1i(indexLocation, 0);
		//x축
		glBindVertexArray(coord_vao[0]);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
		//y축
		glBindVertexArray(coord_vao[1]);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
		//z축
		glBindVertexArray(coord_vao[2]);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
	}
	/**/ {	//background
		glBindVertexArray(background_vao);
		glActiveTexture(GL_TEXTURE0); //--- 유닛 0을 활성화
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glUniform1i(indexLocation, 1);
		glBindTexture(GL_TEXTURE_2D, texture_index[6]);

		glm::mat4 shapeTransform = glm::mat4(1.0f);
		shapeTransform = glm::translate(shapeTransform, glm::vec3(0.0f, 0.0f, 0.0f));
		//shapeTransform = glm::scale(shapeTransform, glm::vec3(1.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(shapeTransform));

		glDrawArrays(GL_QUADS, 0, 4);
	}
	// 객체 이동, 회전
	{
		glm::mat4 model_Rx = glm::mat4(1.0f);
		glm::mat4 model_Ry = glm::mat4(1.0f);
		glm::mat4 T = glm::mat4(1.0f);
		glm::mat4 TR = glm::mat4(1.0f);
		T = glm::translate(T, glm::vec3(move_x, move_y, 0.0));
		model_Rx = glm::rotate(model_Rx, glm::radians(radian_x), glm::vec3(1.0, 0.0, 0.0));
		model_Ry = glm::rotate(model_Ry, glm::radians(radian_y), glm::vec3(0.0, 1.0, 0.0));
		TR = R * T * model_Rx * model_Ry;
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
	}
	if (fill)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (shape_type) {
		glActiveTexture(GL_TEXTURE0); //--- 유닛 0을 활성화
		glUniform1i(indexLocation, 1);
		glBindVertexArray(penta_vao);

		glBindTexture(GL_TEXTURE_2D, texture_index[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindTexture(GL_TEXTURE_2D, texture_index[2]);
		glDrawArrays(GL_TRIANGLES, 3, 3);

		glBindTexture(GL_TEXTURE_2D, texture_index[5]);
		glDrawArrays(GL_TRIANGLES, 6, 3);

		glBindTexture(GL_TEXTURE_2D, texture_index[3]);
		glDrawArrays(GL_TRIANGLES, 9, 3);

		glBindTexture(GL_TEXTURE_2D, texture_index[4]);
		glDrawArrays(GL_QUADS, 12, 4);
	}
	else {
		glActiveTexture(GL_TEXTURE0); //--- 유닛 0을 활성화
		glUniform1i(indexLocation, 1);
		glBindVertexArray(hexa_vao);

		glBindTexture(GL_TEXTURE_2D, texture_index[0]);
		glDrawArrays(GL_QUADS, 0, 4);

		glBindTexture(GL_TEXTURE_2D, texture_index[1]);
		glDrawArrays(GL_QUADS, 4, 4);

		glBindTexture(GL_TEXTURE_2D, texture_index[2]);
		glDrawArrays(GL_QUADS, 8, 4);

		glBindTexture(GL_TEXTURE_2D, texture_index[3]);
		glDrawArrays(GL_QUADS, 12, 4);

		glBindTexture(GL_TEXTURE_2D, texture_index[4]);
		glDrawArrays(GL_QUADS, 16, 4);

		glBindTexture(GL_TEXTURE_2D, texture_index[5]);
		glDrawArrays(GL_QUADS, 20, 4);
	}
	//-----------------------------------
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
		shape_type = 0;
		break;
	case 'p':
		shape_type = 1;
		break;
	case 'h':
		if (h_switch)
			h_switch = false;
		else
			h_switch = true;
		break;
	case 'w':
		fill = false;
		break;
	case 'W':
		fill = true;
		break;
	case 'x':
		if (x_dir == 1)
			x_dir = 0;
		else
			x_dir = 1;
		break;
	case 'X':
		if (x_dir == -1)
			x_dir = 0;
		else
			x_dir = -1;
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
	case 's':
		x_dir = 0;
		y_dir = 0;
		radian_x = 0.0f;
		radian_y = 0.0f;
		move_x = 0.0f;
		move_y = 0.0f;
		shape_type = 0;
		fill = true;
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}
GLvoid SpecialKeyboard(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT) {
		move_x -= 0.05f;
		if (move_x < -0.5f)
			move_x = -0.5f;
	}
	else if (key == GLUT_KEY_RIGHT) {
		move_x += 0.05f;
		if (move_x > 0.5f)
			move_x = 0.5f;
	}
	else if (key == GLUT_KEY_UP) {
		move_y += 0.05f;
		if (move_y > 0.5f)
			move_y = 0.5f;
	}
	else if (key == GLUT_KEY_DOWN) {
		move_y -= 0.05f;
		if (move_y < -0.5f)
			move_y = -0.5f;
	}
	glutPostRedisplay();
}
GLvoid animation(int value)
{
	radian_x += x_dir;
	radian_y += y_dir;
	glutPostRedisplay();
	glutTimerFunc(100, animation, value);
}
