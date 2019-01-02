#include "stdafx.h"
#include <gl/glut.h>

#define PI 3.14159
#define MAX_ASCII_CHAR 128
#define W_SIZE 768


GLuint textureid[16];// 定义纹理变量
GLuint rocktexture[5];//岩石纹理
GLuint Envirtexture[4];//环境纹理贴图
C3DSModel draw3ds[5];


float theta = -90.0;  //rotating angle
int inner = 10, outer = 80;   //torus's inner & outer radius
float s = outer + 4 * inner;
float eyex = 0, eyey = 0, eyez = s + 100;  //eye point initial position
float atx = 0, aty = 0, atz = 0;  //at point initial position
int ww, hh;  //variable for windows's width 

bool flag = true;
float angle = 0; //for angle between view-line and z axis
float r = s; //for distance between eye and at

float colorflag = 1;
double angle2 = 0;
double anglestep = 1 * 3.14 / 180;
double anglestep2 = 1 * 3.14 / 180;
double step = anglestep * 100;
double step2 = anglestep2 * 100;

float run = 0;	//为场景图片移动
int lighton = 0;
int k = 0;
int start = 0;
int runball = 0;
int runperson = 0;

int level = 1;//关卡
int score = 0;//得分
int rocknum = 5 + level * 2;


static GLint fogMode;
GLTMatrix mShadowMatrix;
GLTVector3 vPoints[3] = { { 0.0f, -outer - 4 * inner, 0.0f },
							 { 10.0f, -outer - 4 * inner, 0.0f },
							 { 5.0f, -outer - 4 * inner, -5.0f } };

bool light1_is_on = false;
bool light2_is_on = false;
bool light3_is_on = false;
bool light4_is_on = false;

class Ball
{
public:
	int x;
	int y;
	int z;
	int r;
	float time;
	bool jump = false;
	int textureid;
	Ball()
	{
		int ran1 = -120 + rand() % 240;
		int ran2 = rand() % 400;
		int ran3 = rand() % 3;
		int ran4 = rand() % 15;
		int ran5 = rand() % 3;
		x = ran1;
		y = -s;
		z = -s - ran2;
		r = 25 + ran4;
		time = 0;
		jump = false;
		textureid = ran3;
		if (ran3 == 0)	//固定某种纹理会跳
		{
			jump = true;
		}
	}
	void init()
	{
		int ran1 = -120 + rand() % 240;
		int ran2 = rand() % 400;
		int ran3 = rand() % 3;
		int ran4 = rand() % 15;
		int ran5 = rand() % 3;
		x = ran1;
		y = -s;
		z = -s - ran2;
		r = 25 + ran4;
		time = 0;
		jump = false;
		textureid = ran3;
		if (ran3 == 0)
		{
			jump = true;
		}
	}
};

vector<Ball> balls;


/*申明部分*/

void init();//初始化
void Display(void);//主显示
void Reshape(GLsizei w, GLsizei h);
void timeController(int value);//时间控制（简单状态机）
void collision();//碰撞判定
void selectFont(int size, int charset, const char* face); //选择字体
void drawString(const char* str);//生成字体函数
void drawCNString(const char* str); //生成（中文）字体函数
void drawMenu(int value);//菜单生成
void drawskyball();//天空球生成
void drawground();//绘制地面
void drawside();//侧面绘制
void drawball(int x, int y, int z, int r, bool jump, int Tid);//障碍球生成
void drawplayer();//玩家生成
void MotionController(int x, int y);//视角响应控制
void keyboardInput(unsigned char key, int x, int y);//键盘输入


/*ProgramStart*/

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	const char *argv[] = { "hello ", " " };
	int argc = 2; // must/should match the number of strings in argv

	glutInit(&argc, (char**)argv);    //初始化GLUT库；
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(W_SIZE, W_SIZE);
	glutInitWindowPosition(1024 / 2 - 250, 768 / 2 - 250);
	glutCreateWindow("奔跑吧~铲车的旅途！（CG BigLib）");  //创建窗口和标题；
	glutReshapeFunc(Reshape);	//调用重绘函数Reshape

	init();//首次初始化

	glutDisplayFunc(Display);  //用于绘制当前窗口；
	glutPassiveMotionFunc(MotionController);
	glutKeyboardFunc(keyboardInput);
	glutTimerFunc(100, timeController, 10);

	glutMainLoop();   //表示开始运行程序，用于程序的结尾；
	return 0;
}


void init()
{
	//音乐设定
	mciSendString(TEXT("open 光の街.mp3 alias mysong"), NULL, 0, 0);//加载音乐
	mciSendString(TEXT("play MySong"), NULL, 0, 0);//播放音乐

	for (unsigned int i = 0; i < rocknum; i++)//设置球的数量
	{
		Ball temp = Ball();
		balls.push_back(temp);
	}

	glClearColor(1, 1, 1, 1);

	BuildTexture((char*)"title.jpg", textureid[0]);//标题
	BuildTexture((char*)"win.jpg", textureid[8]);//胜利
	BuildTexture((char*)"muda.jpg", textureid[9]);//失败

	BuildTexture((char*)"rock1.jpg", rocktexture[0]);
	BuildTexture((char*)"rock2.jpg", rocktexture[1]);
	BuildTexture((char*)"rock3.jpg", rocktexture[2]);

	BuildTexture((char*)"side.jpg", Envirtexture[0]);
	BuildTexture((char*)"road.jpg", Envirtexture[1]);

	BuildTexture((char*)"skybig.jpg", Envirtexture[3]);//天空球

	draw3ds[0].Load((char*)"player.3DS");

	//glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);//像素存储模式
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//define light position1
	GLfloat light_position1[] = { -outer,outer + 100,outer + 4 * inner + 50,0.0 };
	GLfloat light_position2[] = { -outer + 100,outer + 100,outer + 4 * inner + 50,0.0 };
	GLfloat light_position3[] = { -outer,outer,outer + 4 * inner - 50,0.0 };
	GLfloat light_position4[] = { -outer + 100,outer,outer + 4 * inner - 50,0.0 };

	//GLfloat light color1
	GLfloat light_ambient1[] = { 1.0,1.0,0,1.0 };
	GLfloat light_diffuse1[] = { 1.0,1.0,0,1.0 };
	GLfloat light_specular1[] = { 1.0,1.0,0,1.0 };

	//GLfloat light color2
	GLfloat light_ambient2[] = { 0.0,0.8,0.8,1.0 };
	GLfloat light_diffuse2[] = { 0.0,0.8,0.8,1.0 };
	GLfloat light_specular2[] = { 0.0,0.8,0.8,1.0 };

	//GLfloat light color3
	GLfloat light_ambient3[] = { 0.8,0.0,0.8,1.0 };
	GLfloat light_diffuse3[] = { 0.8,0.0,0.8,1.0 };
	GLfloat light_specular3[] = { 0.8,0.0,0.8,1.0 };

	//GLfloat light color4
	GLfloat light_ambient4[] = { 0.0,0.3,0.0,1.0 };
	GLfloat light_diffuse4[] = { 0.0,0.3,0.0,1.0 };
	GLfloat light_specular4[] = { 0.0,0.3,0.0,1.0 };

	// light model- global light	
	GLfloat lmodel_ambient[] = { 0.8,0.2,0.2,1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	// set light source location
	glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
	glLightfv(GL_LIGHT2, GL_POSITION, light_position3);
	glLightfv(GL_LIGHT3, GL_POSITION, light_position4);

	// set light source color;
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular1);

	// set light source color;
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse2);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular2);

	// set light source color;
	glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse3);
	glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular3);

	// set light source color;
	glLightfv(GL_LIGHT3, GL_AMBIENT, light_ambient4);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, light_diffuse4);
	glLightfv(GL_LIGHT3, GL_SPECULAR, light_specular4);


	// set material let material fits color	
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	/*
	//make light source enable;
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	*/

	//clear color
	// glClearColor(0.0,0.0,0.0,0.0);

	//Color shade mode
	glShadeModel(GL_SMOOTH);
	//glShadeModel(GL_FLAT);  

	//Material parameter setting
	GLfloat mat_specular1[] = { 1.0,1.0,1.0,1.0 };
	GLfloat mat_shininess1[] = { 80.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular1);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess1);

	glEnable(GL_DEPTH_TEST);

	//FogColor
	GLfloat fogColor[] = { 1,1,1,1 };

	//雾气生成
	fogMode = GL_EXP;
	glFogi(GL_FOG_MODE, fogMode);
	glFogfv(GL_FOG_COLOR, fogColor);  //fog’s color
	glFogf(GL_FOG_START, 250.0f);  // how far to start
	glFogf(GL_FOG_END, 330.0f); //how far to end
	glFogi(GL_FOG_MODE, GL_LINEAR);  //which mode
	glFogf(GL_FOG_DENSITY, 0.5f);
	glEnable(GL_FOG);

	//// Draw shadows first
	//glDisable(GL_DEPTH_TEST);//影子没有深度
	//glDisable(GL_LIGHTING);//影子不含光照
	//glPushMatrix();
	//glMultMatrixf(mShadowMatrix);
	//drawplayer();
	//glPopMatrix();
	//glEnable(GL_LIGHTING);
	//glEnable(GL_DEPTH_TEST);

	//菜单生成
	glutCreateMenu(drawMenu);
	glutAddMenuEntry("LineModel", 1);
	glutAddMenuEntry("RealModel", 2);
	glutAddMenuEntry("MusicPlay", 3);
	glutAddMenuEntry("MusicStop", 4);
	glutAddMenuEntry("LightOpen", 5);
	glutAddMenuEntry("LightOFF", 6);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	//其他代码
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (start == 0)//开始界面
	{  //绘制启动画面
	   //绘制矩形，使得矩形大小和裁剪窗口大小一致
	   //给该矩形添加贴图
		glClearColor(0, 0, 0, 1);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(90, ww / hh, 10, 2 * outer + 8 * inner + 250);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//绘制3D场景

		gluLookAt(eyex, eyey - 100, eyez, atx, aty, atz, 0, 1, 0);

		static const GLfloat vertex_list[][3] = { -0.5f * 120, -0.5f * 120, -0.5f * 120,
																	  0.5f * 120, -0.5f * 120, -0.5f * 120,
																	 -0.5f * 120,   0.5f * 120, -0.5f * 120,
																	  0.5f * 120,   0.5f * 120, -0.5f * 120,
																	 -0.5f * 120, -0.5f * 120,   0.5f * 120,
																	  0.5f * 120, -0.5f * 120,   0.5f * 120,
																	 -0.5f * 120,   0.5f * 120,   0.5f * 120,
																	  0.5f * 120,   0.5f * 120,   0.5f * 120 };
		// 将要使用的顶点的序号保存到一个数组里面
		static const GLint index_list[][4] = { 0, 2, 3, 1,
																 0, 4, 6, 2,
																 0, 1, 5, 4,
																 4, 5, 7, 6,
																 1, 3, 7, 5,
																 2, 6, 7, 3 };

		int i, j;

		// 绘制的时候代码很简单
		glBegin(GL_QUADS);
		glColor3f(0, 0, 0);
		for (i = 0; i < 6; ++i)          // 有六个面，循环六次
			for (j = 0; j < 4; ++j)      // 每个面有四个顶点，循环四次
			{

				glVertex3fv(vertex_list[index_list[i][j]]);
			}
		glEnd();

		//标题文字
		selectFont(60, ANSI_CHARSET, "Comic Sans MS");
		glColor3f(0, 0, 0);
		glRasterPos3f(-50, 0, 120);
		drawCNString(" 奔跑吧~铲车的旅途！");

		selectFont(30, ANSI_CHARSET, "Comic Sans MS");
		glColor3f(0, 0, 0);
		glRasterPos3f(50, 0, 80);
		drawCNString("三人组");

		selectFont(40, ANSI_CHARSET, "Comic Sans MS");
		glColor3f(0, 0, 0);
		glRasterPos3f(-40, 0, 70);
		drawCNString("回车键开始");
		glEnd();

		lighton++;
		if (lighton == 300)
		{
			drawMenu(5);
		}
		if (lighton >= 500)
		{
			lighton = 0;
		}
	}
	else if (start == 1)//游戏界面
	{
		glClearColor(1, 1, 1, 1);

		//GLfloat light color1
		GLfloat light_ambient1[] = { 1.0,1.0,1.0,1.0 };
		GLfloat light_diffuse1[] = { 1.0,1.0,1.0,1.0 };
		GLfloat light_specular1[] = { 1.0,1.0,1.0,1.0 };

		//GLfloat light color2
		GLfloat light_ambient2[] = { 0.8,0.8,0.8,1.0 };
		GLfloat light_diffuse2[] = { 0.8,0.8,0.8,1.0 };
		GLfloat light_specular2[] = { 0.8,0.8,0.8,1.0 };

		glDisable(GL_LIGHT3);
		glDisable(GL_LIGHT2);
		//glEnable(GL_LIGHT1);
		//glEnable(GL_LIGHT0);

		//切换成透视投影
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(90, ww / hh, 10, 2 * outer + 8 * inner + 250);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//绘制3D场景

		gluLookAt(eyex, eyey, eyez, atx, aty, atz, 0, 1, 0);

		/*
		//1>读取带alpha通道的透明贴图, tga格式：
		//BuildTexture("shu.tga", textureid[4]);
		//// 2>启用纹理
		//glEnable(GL_TEXTURE_2D);
		//// 3>启用透明和调和模式，参考代码如下：
		//glEnable(GL_BLEND);        //ALPHA测试
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glEnable(GL_ALPHA_TEST);
		//glAlphaFunc(GL_GREATER, 0.1);
		////4>绑定纹理ID号，参考代码：
		//glBindTexture(GL_TEXTURE_2D, textureid[4]);
		//// 5>纹理坐标映射
		//glBegin(GL_QUADS);
		//glTexCoord2f(0, 0);
		//glVertex3f(-50, -50, 0);
		//glTexCoord2f(1, 0);
		//glVertex3f(50, -50, 0);
		//glTexCoord2f(1, 1);
		//glVertex3f(50, 50, 0);
		//glTexCoord2f(0, 1);
		//glVertex3f(-50, 50, 0);
		//glEnd();
		////6>关闭纹理
		//glDisable(GL_TEXTURE_2D);


		//selectFont(24, GB2312_CHARSET, "楷体_GB2312"); //设置字体楷体48号字
		//glColor3f(0, 0, 1);
		//glRasterPos3f(-30, 0, 100);  //在世界坐标250,250处定位首字位置
		//drawCNString("我创造的3D世界");  //写字“Hello，大家好”
		*/

		//绘制用户界面
		selectFont(24, ANSI_CHARSET, "Comic Sans MS");
		glColor3f(0, 0, 0);
		glRasterPos3f(50, 0, 100);
		if (level == 1) drawCNString(" Lv1");
		else if (level == 2) drawCNString(" Lv2");
		else if (level == 3) drawCNString(" Lv3");
		else if (level == 4) drawCNString(" Lv4");
		else if (level == 5) drawCNString(" Lv5");
		else if (level == 6) drawCNString(" Lv6");
		else if (level == 7) drawCNString(" Lv7");
		else if (level == 8) drawCNString(" Lv8");
		else if (level == 9) drawCNString(" Lv9");

		drawskyball();//天空球
		glColor3f(0.0, 0.0, 1.0);
		drawside();
		glColor3f(1.0, 0, 0);
		drawground();


		for (unsigned int i = 0; i < balls.size(); i++)
		{
			drawball(balls[i].x, balls[i].y, balls[i].z, balls[i].r, balls[i].jump, balls[i].textureid);
		}

		drawplayer();//生成玩家
		glPopMatrix();

	}
	else if (start == 2)//失败
	{
		glEnable(GL_TEXTURE_2D);  //启用纹理
		glBindTexture(GL_TEXTURE_2D, textureid[9]);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(0, 0);
		glTexCoord2f(1, 0);
		glVertex2f(100, 0);
		glTexCoord2f(1, 1);
		glVertex2f(100, 100);
		glTexCoord2f(0, 1);
		glVertex2f(0, 100);
		glEnd();
		glDisable(GL_TEXTURE_2D);  //启用纹理
	}
	else if (start == 3)//胜利&下一关
	{
		glEnable(GL_TEXTURE_2D);  //启用纹理
		glBindTexture(GL_TEXTURE_2D, textureid[8]);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(0, 0);
		glTexCoord2f(1, 0);
		glVertex2f(100, 0);
		glTexCoord2f(1, 1);
		glVertex2f(100, 100);
		glTexCoord2f(0, 1);
		glVertex2f(0, 100);
		glEnd();
		glDisable(GL_TEXTURE_2D);  //启用纹理
	}
	glutSwapBuffers();
}

void Reshape(GLsizei w, GLsizei h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (start == 0)
		gluOrtho2D(0, 100, 0, 100);
	else if (start == 1)
		gluPerspective(90, w / h, 10, 2 * outer + 8 * inner + 250);
	else if (start == 2)
		gluOrtho2D(0, 100, 0, 100);
	else if (start == 3)
		gluOrtho2D(0, 100, 0, 100);

	glViewport(0, 0, w, h);
	glMatrixMode(GL_MODELVIEW);
	ww = w;
	hh = h;
}

void timeController(int value)
{
	run += 0.01;		//纹理场景移动速度
	if (run >= 0.5) { run = 0; }
	if (start == 1)
	{
		for (unsigned int i = 0; i < balls.size(); i++)//设置球的速度
		{
			balls[i].z += 10;
			if (balls[i].jump == true)
			{
				float s0 = -s, v = 4, a = 10;	//t = 0.1;弹跳
				balls[i].y += (v - a * balls[i].time);
				balls[i].time += 0.1;
				if (balls[i].y < s0)
				{
					balls[i].y = s0;
					balls[i].time = 0;
				}
			}
			if (balls[i].z > 300)
			{
				balls[i].init();
				score++;
				if (score > rocknum * 10)//10轮滚石后胜利
				{
					score = 0;
					start = 3;			//成功界面
					Reshape(ww, hh);	//重绘成功界面
					level++;
				}
			}
		}
		collision();	//判断碰撞

	}


	theta += 0.5;

	if (k < 14)k++;
	else
		k = 5;
	if (theta >= 360.0) theta -= 360.0;
	glutPostRedisplay();
	glutTimerFunc(100, timeController, 10);
}

void collision()
{

	for (unsigned int i = 0; i < balls.size(); i++)//设置球的速度
	{
		int x = runperson;
		int y = 10 - s;
		int z = s - 30;
		//人物的xyz坐标为(0+runperson, r - tr, tr-3*r),r = 10
		if (balls[i].z + balls[i].r / 2 > z - 5 && balls[i].z - balls[i].r / 2 < z + 5)
		{
			if (balls[i].x + balls[i].r / 2 > x - 5 && balls[i].x - balls[i].r / 2 < x + 5)
			{
				start = 2;				//失败画面
				score = 0;
				Reshape(ww, hh);		//重绘失败画面
				for (unsigned int i = 0; i < balls.size(); i++)//设置球的速度
				{
					balls[i].init();

				}
			}
		}
	}
}

void selectFont(int size, int charset, const char* face)
{
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(hOldFont);
}

void drawString(const char* str) {
	static int isFirstCall = 1;
	static GLuint lists;

	if (isFirstCall) {
		// 如果是第一次调用，执行初始化
		// 为每一个ASCII字符产生一个显示列表
		isFirstCall = 0;

		// 申请MAX_CHAR个连续的显示列表编号
		lists = glGenLists(MAX_ASCII_CHAR);

		// 把每个字符的绘制命令都装到对应的显示列表中
		wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_ASCII_CHAR, lists);
	}
	// 调用每个字符对应的显示列表，绘制每个字符
	for (; *str != '\0'; ++str)
		glCallList(lists + *str);
}

void drawCNString(const char* str)
{
	int len, i;
	wchar_t* wstring;
	HDC hDC = wglGetCurrentDC();
	GLuint list = glGenLists(1);

	// 计算字符的个数
	// 如果是双字节字符的（比如中文字符），两个字节才算一个字符
	// 否则一个字节算一个字符
	len = 0;
	for (i = 0; str[i] != '\0'; ++i)
	{
		if (IsDBCSLeadByte(str[i]))
			++i;
		++len;
	}

	// 将混合字符转化为宽字符
	wstring = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, -1, wstring, len);
	wstring[len] = L'\0';

	// 逐个输出字符
	for (i = 0; i < len; ++i)
	{
		wglUseFontBitmapsW(hDC, wstring[i], 1, list);
		glCallList(list);
	}

	// 回收所有临时资源
	free(wstring);
	glDeleteLists(list, 1);
}

void drawMenu(int value)
{
	if (value == 1) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (value == 2) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (value == 3) mciSendString(TEXT("play MySong"), NULL, 0, 0);//播放音乐;
	if (value == 4) mciSendString(TEXT("stop MySong"), NULL, 0, 0);//暂停音乐;
	if (value == 5) //开启光照
	{
		if (start == 0) {
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			glEnable(GL_LIGHT1);
			glEnable(GL_LIGHT2);
			glEnable(GL_LIGHT3);
			light1_is_on = true;
			light2_is_on = true;
			light3_is_on = true;
			light4_is_on = true;
		}
		else if (start == 1 || start == 2 || start == 3) {
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT1);
			glEnable(GL_LIGHT0);
			light1_is_on = true;
			light2_is_on = true;
		}
	}
	if (value == 6) //关闭光照
	{
		if (start == 0) {
			glDisable(GL_LIGHT0);
			glDisable(GL_LIGHT1);
			glDisable(GL_LIGHT2);
			glDisable(GL_LIGHT3);
			light1_is_on = false;
			light2_is_on = false;
			light3_is_on = false;
			light4_is_on = false;
		}
		else if (start == 1 || start == 2 || start == 3) {
			glDisable(GL_LIGHT0);
			glDisable(GL_LIGHT1);
			light1_is_on = false;
			light2_is_on = false;
		}
	}
}

void drawskyball()//绘制天空球
{
	glEnable(GL_TEXTURE_2D);	//曲面纹理
	//1 > 指定纹理，绑定纹理ID号
	glBindTexture(GL_TEXTURE_2D, Envirtexture[3]);  //天空纹理
	glBegin(GL_QUADS);							//动态贴图用k
	//2>启用自动纹理
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	//3> 自动纹理映射
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	glPushMatrix();				//组合
	glColor3f(1, 1, 1);

	glutSolidSphere(500, 30, 30);

	glPopMatrix();

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_2D);
}

void drawground()
{
	//ground
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Envirtexture[1]);  //地板纹理贴图，绑定指定纹理
	glBegin(GL_QUADS);							//动态贴图用k
	glTexCoord2f(0.0f, 0.5f - run);
	glVertex3d(-outer - 4 * inner, -outer - 4 * inner, -outer - 4 * inner);
	glTexCoord2f(1.0f, 0.5f - run);
	glVertex3d(outer + 4 * inner, -outer - 4 * inner, -outer - 4 * inner);
	glTexCoord2f(1.0f, 1.0f - run);
	glVertex3d(outer + 4 * inner, -outer - 4 * inner, 2 * outer + 8 * inner);
	glTexCoord2f(0.0f, 1.0f - run);
	glVertex3d(-outer - 4 * inner, -outer - 4 * inner, 2 * outer + 8 * inner);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	/*
	//top
	//glColor3f(1, 1, 1);
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, Envirtexture[3]);  //天空纹理贴图
	//glBegin(GL_QUADS);
	//glTexCoord2f(0.0f, 0.5f - run);
	//glVertex3d(-outer - 4 * inner, outer + 4 * inner, -outer - 4 * inner);
	//glTexCoord2f(1.0f, 0.5f - run);
	//glVertex3d(outer + 4 * inner, outer + 4 * inner, -outer - 4 * inner);
	//glTexCoord2f(1.0f, 1.0f - run);
	//glVertex3d(outer + 4 * inner, outer + 4 * inner, 2 * outer + 8 * inner);//
	//glTexCoord2f(0.0f, 1.0f - run);
	//glVertex3d(-outer - 4 * inner, outer + 4 * inner, 2 * outer + 8 * inner);//
	//glEnd();
	//glDisable(GL_TEXTURE_2D);
	*/
}

void drawside()
{
	//leftSide
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Envirtexture[0]);  //地板纹理贴图，绑定指定纹理
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f + run, 0.0f);
	glVertex3d(-outer - 4 * inner, -outer - 4 * inner, 2 * outer + 8 * inner);
	glTexCoord2f(0.5f + run, 0.0f);
	glVertex3d(-outer - 4 * inner, -outer - 4 * inner, -outer - 4 * inner);
	glTexCoord2f(0.5f + run, 1.0f);
	glVertex3d(-outer - 4 * inner, outer + 4 * inner, -outer - 4 * inner);
	glTexCoord2f(0.0f + run, 1.0f);
	glVertex3d(-outer - 4 * inner, outer + 4 * inner, 2 * outer + 8 * inner);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	//rightSide
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Envirtexture[0]);  //地板纹理贴图，绑定指定纹理
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f + run, 0.0f);
	glVertex3d(outer + 4 * inner, -outer - 4 * inner, 2 * outer + 8 * inner);
	glTexCoord2f(0.5f + run, 0.0f);
	glVertex3d(outer + 4 * inner, -outer - 4 * inner, -outer - 4 * inner);
	glTexCoord2f(0.5f + run, 1.0f);
	glVertex3d(outer + 4 * inner, outer + 4 * inner, -outer - 4 * inner);
	glTexCoord2f(0.0f + run, 1.0f);
	glVertex3d(outer + 4 * inner, outer + 4 * inner, 2 * outer + 8 * inner);

	/*
	//front
	//glColor3f(1, 1, 1);
	//glEnable(GL_TEXTURE_2D);				//
	//glBindTexture(GL_TEXTURE_2D, textureid[10]);  //地板纹理贴图，绑定指定纹理
	//glBegin(GL_QUADS);
	//glTexCoord2f(0.0f, 0.0f + run);
	//glVertex3d(-outer - 4 * inner, -outer - 4 * inner, -outer - 4 * inner);
	//glTexCoord2f(1.0f, 0.0f + run);
	//glVertex3d(outer + 4 * inner, -outer - 4 * inner, -outer - 4 * inner);
	//glTexCoord2f(1.0f, 1.0f + run);
	//glVertex3d(outer + 4 * inner, outer + 4 * inner, -outer - 4 * inner);
	//glTexCoord2f(0.0f, 1.0f + run);
	//glVertex3d(-outer - 4 * inner, outer + 4 * inner, -outer - 4 * inner);
	//glEnd();
	//glDisable(GL_TEXTURE_2D);
	*/

	glEnd();
	glDisable(GL_TEXTURE_2D);

}

void drawball(int x, int y, int z, int r, bool jump, int Tid)
{
	float tr;
	/*
	//颜色材质绘球环
	tr=(outer+3*inner);
	glRotatef(theta,0,1,0);
	glPushMatrix();
		glPushMatrix();
			glColor3f(1.0,0,0.0);
		  glutSolidTorus(inner,outer,30,50);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(outer,0,0);
			glRotatef(theta,0,1,0);
			glTranslatef(-outer,0,0);

			glPushMatrix();
				glTranslatef(tr,0,0);
				glRotatef(-45,1,0,0);
					glColor3f(0.0,1.0,0);
				glutSolidSphere(inner,20,20);
			glPopMatrix();
		glPopMatrix();
	  glPopMatrix();*/

	glEnable(GL_TEXTURE_2D);	//曲面纹理
	//指定纹理，绑定纹理ID号
	glBindTexture(GL_TEXTURE_2D, rocktexture[Tid]);
	//启用自动纹理
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	//自动纹理映射
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	/*	tr=(outer+3*inner);
	   glRotatef(theta,0,1,0);
	   glPushMatrix();					//1
		   glPushMatrix();				//11
			   glColor3f(1.0,0,0.0);
			   //glutSolidTorus(inner,outer,30,50); //画环
		   glPopMatrix();				//11
		   glPushMatrix();				//12
			   glTranslatef(outer,0,0);
			   glRotatef(theta,0,1,0);
			   glTranslatef(-outer,0,0);

				glBindTexture(GL_TEXTURE_2D, textureid[2]);
			   glPushMatrix();		    //121
				   glTranslatef(tr,0,0);
				   glRotatef(-45,1,0,0);
					   glColor3f(1,1.0,1);
				   glutSolidSphere(40,30,30);		//画球体
			   glPopMatrix();			//121
		   glPopMatrix();				//12
		 glPopMatrix();				//1

		 glDisable(GL_TEXTURE_GEN_S);
   glDisable(GL_TEXTURE_GEN_T);
   glDisable(GL_TEXTURE_2D);


   */
	tr = (outer + 4 * inner);

	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(x, r + y, z);//球的位置
	glRotatef(theta * 10, 1, 0, 0);//球绕着X轴旋转
	glutSolidSphere(r, 30, 30);		//画球体
	glPopMatrix();

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_2D);

}

void drawplayer()
{
	/*
	glRotatef(90, 0, 1, 0);
	glEnable(GL_TEXTURE_2D);  //启用纹理
	glScalef(0.5, 0.5, 0.5);
	draw3ds[0].Render();  //
	glDisable(GL_TEXTURE_2D);   //使用后关闭纹理
	*/
	float tr;
	int r = 10;
	tr = (outer + 4 * inner);
	glPushMatrix();
	glColor3f(1, 0, 1);
	glTranslatef(0 + runperson, r - tr, tr - 3 * r);//人物的x,y,z坐标

	//glRotatef(theta * 10, 1, 0, 0);//球绕着X轴旋转
	//glutSolidSphere(r, 30, 30);		//画球体

	glRotatef(90, 0, 1, 0);
	glScalef(0.3, 0.3, 0.3);//模型大小
	draw3ds[0].Render();  //显示铲车模型
	glPopMatrix();
}

void keyboardInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 13://回车
		start = 1;
		break;
	case 'Z':
	case 'z'://左方向键
		if (runperson > -s + 20)runperson -= 10;
		break;
	case 'C':
	case 'c'://右方向键
		if (runperson < s - 20)runperson += 10;
		break;
	case 'v':
	case 'V':
		if (light1_is_on)
		{
			glDisable(GL_LIGHT0);
			light1_is_on = false;
		}
		else
		{
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			light1_is_on = true;
		}
		break;
	case 'b':
	case 'B':
		if (light2_is_on)
		{
			glDisable(GL_LIGHT1);
			light2_is_on = false;
		}
		else
		{
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT1);
			light2_is_on = true;
		}
		break;
	case 'n':
	case 'N':
		if (light3_is_on)
		{
			glDisable(GL_LIGHT2);
			light3_is_on = false;
		}
		else
		{
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT2);
			light3_is_on = true;
		}
		break;
	case 'm':
	case 'M':
		if (light4_is_on)
		{
			glDisable(GL_LIGHT3);
			light4_is_on = false;
		}
		else
		{
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT3);
			light4_is_on = true;
		}
		break;
	}
	glutPostRedisplay();//参数修改后调用重画函数，屏幕图形将发生改变
}

void MotionController(int x, int y)								//按下中途坐标
{					//执行画矩形
	angle = (ww / 2 - x)*anglestep * 30 / (ww / 2);			//左右移动视觉最多30度
	angle2 = (y - hh / 2)*anglestep2 * 30 / (hh / 2);
	atx = eyex - s * sin(angle)*cos(angle2);//your code Here
	atz = eyez - s * cos(angle)*cos(angle2);
	aty = eyey - s * sin(angle2);
	glutPostRedisplay();
}
/*ProgramEnd*/