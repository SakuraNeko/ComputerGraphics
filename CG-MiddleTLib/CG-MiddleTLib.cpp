#include "stdafx.h"
#include <mmsystem.h> //导入声音头文件库
#pragma comment(lib,"winmm.lib")//导入声音的链接库

using namespace std;

class Rectangle1
{
public:
	int x;
	int y;
	int width;
	int height;
	Rectangle1(int x, int y, int w, int h)
	{
		this->x = x;
		this->y = y;
		width = w;
		height = h;
	}
};
class Board
{
public:
	int width;
	int height;
	POINT p;
	int colorId;
	Board()
	{
		p = POINT();
	}
	void init()
	{

		p.x = 60 + rand() % 340;//
		p.y = 20 + rand() % 300;//
		width = 70;
		height = 20;
	}

	~Board() { }
};
class MyBoard
{
public:
	int width;
	int height;
	POINT p;
	int colorId;
	int speed;
	MyBoard()
	{
		p = POINT();
		width = 70;
		height = 15;
		speed = 2;
	}
	void init()
	{

		p.x = 200; p.y = 470;
		width = 70;
		height = 15;
		speed = 2;
	}
	void move(int flag)
	{
		p.x += speed * flag;

	}
};
class Ball
{
public:
	int side;
	POINT p;
	int colorId;
	int speed;
	int runx;
	int runy;
	Ball()
	{
		p = POINT();
		p.x = 250; p.y = 400;
		side = 10;
		runx = -5;
		runy = -5;
	}
	void move()
	{
		p.x += runx;
		p.y += runy;
	}
	void change(int x, int y)
	{
		runx = x * runx; runy = y * runy;
	}
};

void Myinit();
void display(void);
void drawWin();
void drawTitle();
void myreshape(GLsizei w, GLsizei h);
void mykeyboard(unsigned char key, int x, int y);
void myMotion(int x, int y);
void myidle();
void mytime(int value);
void collision();
void drawCNString(const char* str);
void selectFont(int size, int charset, const char* face);
void isVictory();

MyBoard myboard = MyBoard();
vector<Board> boards;
vector<Ball> balls;

Rectangle1 gameboundary = Rectangle1(10, 10, 480, 500);

int score = 0;
int pos = 1;
int num = 7;
int flag = 0;
bool gameWin = false;
bool gameMod = false;
GLfloat color[5][3] = { 0,0,1, 1,0,0, 0.5,0.2,0.2, 0,1,0, 0,0.8,0.6 };

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
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);   //设置显示模式；（缓冲，颜色类型）
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(1024 / 2 - 250, 768 / 2 - 250);
	glutCreateWindow("PingPong-Plus（1601010061，罗元）");  //创建窗口；
	//Myinit();
	glutDisplayFunc(display);  //用于绘制当前窗口；
	glutReshapeFunc(myreshape);//注册窗口改变回调函数
	glutKeyboardFunc(mykeyboard);//键盘响应
	glutMotionFunc(myMotion);
	glutPassiveMotionFunc(myMotion);
	glutIdleFunc(myidle);    //注册闲置回调函数
	//glutTimerFunc(200, mytime, 10);
	mciSendString(TEXT("open 1.mp3 alias mysong"), NULL, 0, 0);//加载音乐，将这段代码放到主函数（init）下,将”1.mp3”改为你的音乐文件名（支持中文）
	mciSendString(TEXT("play MySong"), NULL, 0, 0);
	glutMainLoop();   //表示开始运行程序，用于程序的结尾；

	return 0;
}

void Myinit()
{
	score = 0;
	balls.clear();
	boards.clear();
	//重绘
	myboard.init();
	
	Ball btemp = Ball();
	balls.push_back(btemp);
	num = 10;
	for (int i = 0; i < num; i++)
	{
		Board botemp = Board();
		botemp.init();
		boards.push_back(botemp);
	}
	gameWin = false;
	glutTimerFunc(20, mytime, 10);
	glutTimerFunc(200, mytime, 1); 
	
	
}

void drawTitle()//主界面绘制
{
	glColor3f(1, 1, 1);
	selectFont(40, GB2312_CHARSET, "楷体_GB2312"); //设置字体楷体
	glRasterPos2f(120, 200);  //在世界坐标0,0处定位首字位置
	drawCNString("PingPong-Plus");

	glColor3f(1, 1, 0);
	selectFont(18, GB2312_CHARSET, "楷体_GB2312"); //设置字体楷体
	glRasterPos2f(80, 350);  //在世界坐标0,0处定位首字位置
	drawCNString("回车开始。游戏中Z&X切换鼠标和键盘模式");

	glColor3f(1, 1, 1);
	selectFont(20, GB2312_CHARSET, "楷体_GB2312"); //设置字体楷体
	glRasterPos2f(170, 400);  //在世界坐标0,0处定位首字位置
	drawCNString("1601010061 罗元");
}

void drawWin()//胜利界面绘制
{
	glColor3f(1, 1, 0);
	selectFont(30, GB2312_CHARSET, "楷体_GB2312"); //设置字体楷体
	glRasterPos2f(55, 250);  //在世界坐标0,0处定位首字位置
	drawCNString("你赢了!回车重新开始游戏吧！");
	mciSendString(TEXT("stop MySong"), NULL, 0, 0);
}

void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT);   //刷新颜色缓冲区
	//glRectf(x1,y1,x2,y2);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  //设置清屏颜色
	glClear(GL_COLOR_BUFFER_BIT);   //刷新颜色缓冲区；

	if (gameWin == true )
	{
		flag = 2;
		drawWin();
	}
	else if (flag == 0)
	{
		drawTitle();
	}
	else if (flag == 1)
	{
		glColor3f(1, 1, 0);
		glRectf(myboard.p.x, myboard.p.y,
			myboard.p.x + myboard.width, myboard.p.y + myboard.height);
		for (unsigned int i = 0; i < boards.size(); i++)
		{
			int t = i % 5;
			glColor3f(color[t][0], color[t][1], color[t][2]);

			glRectf(boards[i].p.x, boards[i].p.y,
				boards[i].p.x + boards[i].width, boards[i].p.y + boards[i].height);
		}
		for (unsigned int i = 0; i < balls.size(); i++)
		{
			glColor3f(0, 1, 0);
			glBegin(GL_POLYGON);  //开始绘制时钟内圆
			int n = 10, R = balls[i].side / 2;
			for (int j = 0; j < n; j++)
				glVertex2f(balls[i].p.x + R + R * cos(j * 2 * 3.14 / n), balls[i].p.y + R + R * sin(j * 2 * 3.14 / n));
			glEnd();
			/*glRectf(balls[i].p.x, balls[i].p.y,
				balls[i].p.x + balls[i].side, balls[i].p.y + balls[i].side);*/
		}
	}
	glFlush(); //用于刷新命令队列和缓冲区，使所有尚未被执行的OpenGL命令得到执行
}

void myidle()//闲置函数
{
	isVictory();
}

void isVictory()//胜利判断
{
	if (score >= num)
	{
		gameWin = true ;
	}
}

void mytime(int value)
{
	if (value == 10 && gameWin == false ) {
		for (unsigned int i = 0; i < balls.size(); i++)
		{
			balls[i].move();
		}
		collision();
		glutPostRedisplay();//重绘
		glutTimerFunc(50, mytime, 10);
	}
}

void collision()
{
	for (unsigned int i = 0; i < balls.size(); i++)
	{
		for (unsigned int j = 0; j < boards.size(); j++)//左右边相碰撞
		{
			if (balls[i].p.y <= boards[j].p.y + boards[j].height && balls[i].p.y >= boards[j].p.y)
			{
				int t1 = balls[i].p.x + balls[i].side - (boards[j].p.x);
				int t2 = balls[i].p.x - (boards[j].p.x + boards[j].width);
				if (((t1 <= 6) && (t1 >= 0)) || ((t2 >= -6) && (t2 <= 0)))
				{
					boards.erase(boards.begin() + j);
					balls[i].change(-1, 1);
					score++;
					break;
				}
			}

			else if (balls[i].p.x <= boards[j].p.x + boards[j].width && balls[i].p.x >= boards[j].p.x)//上下边碰撞
			{
				int t1 = balls[i].p.y + balls[i].side - (boards[j].p.y);
				int t2 = balls[i].p.y - (boards[j].p.y + boards[j].height);
				if (((t2 <= 6) && (t2 >= 0)) || ((t1 >= -6) && (t1 <= 0)))
				{
					boards.erase(boards.begin() + j);
					balls[i].change(1, -1);
					score++;
					break;
				}
			}
		}

		//游戏边界判定（左上右）
		if (balls[i].p.x < gameboundary.x) 
		{
			balls[i].runx = -balls[i].runx;
		}
		if (balls[i].p.y < gameboundary.y)
		{
			balls[i].runy = -balls[i].runy;
		}
		if (balls[i].p.x + balls[i].side > gameboundary.x + gameboundary.width)
		{
			balls[i].runx = -balls[i].runx;
		}
		if (balls[i].p.y + balls[i].side > gameboundary.y + gameboundary.height)
		{
			balls.erase(balls.begin() + i);
		}

		//与板碰撞
		else if (balls[i].p.x <= myboard.p.x + myboard.width && balls[i].p.x >= myboard.p.x)
		{
			int t1 = balls[i].p.y + balls[i].side - (myboard.p.y);
			int t2 = balls[i].p.y - (myboard.p.y + myboard.height);
			if (((t2 <= 6) && (t2 >= 0)) || ((t1 >= -6) && (t1 <= 0)))
			{
				balls[i].change(1, -1);
			}
		}
	}
}

void mykeyboard(unsigned char key, int x, int y)
{
	if (key == '\r')
	{ 
		flag = 1; 
		Myinit(); 
	}
	if (key == 'Z' || key == 'z')
	{
		gameMod = false;
	}
	if (key == 'X'|| key == 'x')
	{
		gameMod = true;
	}
	if (gameMod == false)
	{
		switch (key)
		{
		case 'A':
		case 'a'://左移：矩形对角坐标变量修改
			myboard.p.x -= 20;
			break;
		case 'D':
		case 'd'://右移：矩形对角坐标变量修改
			myboard.p.x += 20;
			break;
		case '+'://释放多个小球（测试用）
			balls.push_back(Ball());
			break;
		}
	}
	//参数修改后调用重画函数，屏幕图形将发生改变
	glutPostRedisplay();  //*
}

void myMotion(int x, int y)//鼠标坐标采集
{
	if (gameMod == true)
	{
		myboard.p.x = x;
		glutPostRedisplay();
	}
}
void myreshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);   //* 设置视区位置
	glMatrixMode(GL_PROJECTION);//设置投影变换模式
	glLoadIdentity();  //调单位矩阵，清空当前矩阵堆栈
	gluOrtho2D(0, w, h, 0);  //*  
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

void selectFont(int size, int charset, const char* face)
{
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(hOldFont);
}


