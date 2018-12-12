#include "stdafx.h"
#include <math.h>
#include <gl/glut.h>
#include <mmsystem.h> //导入声音头文件库
#pragma comment(lib,"winmm.lib")//导入声音的链接库
#define PI 3.14159

float theta = -90.0;  //rotating angle
int inner = 10, outer = 80;   //torus's inner & outer radius
float s = outer + 4 * inner + 50;
float eyex = 0, eyey = 0, eyez = s;  //eye point initial position
float atx = 0, aty = 0, atz = 0;  //at point initial position
int ww, hh;  //variable for windows's width 
bool flag = true;
float angle = 0; //for angle between view-line and z axis
float r = s; //for distance between eye and at
float step = r / 10;  //for forward and backword step;
int colorflag = 1;

void Display(void);
void Reshape(GLsizei w, GLsizei h);
void mytime(int value);
void drawground();
void drawsphere();
void drawwall();
void init();
void mymenu(int value);
void mykeyboard(unsigned char key, int x, int y);

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
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(1024 / 2 - 250, 768 / 2 - 250);
	glutCreateWindow("Rotating 3D World");  //创建窗口，标题为“Rotating 3D World”；
	glutReshapeFunc(Reshape);
	init();
	glutDisplayFunc(Display);  //用于绘制当前窗口；
	glutKeyboardFunc(mykeyboard);
	glutTimerFunc(100, mytime, 10);
	glutMainLoop();   //表示开始运行程序，用于程序的结尾；
	return 0;
}

void init()
{
	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	mciSendString(TEXT("open 在りし日のふたり.mp3 alias mysong"), NULL, 0, 0);//载入音乐

	glutCreateMenu(mymenu);  //创建菜单
	glutAddMenuEntry("LineModel", 1);  //第一个菜单名“线框模型”，设置1为该菜单参数
	glutAddMenuEntry("FullModel", 2);  //第二个菜单名“实体模型”，设置2为该菜单参数
	glutAddMenuEntry("MusicPlay", 3);  //第三个菜单名“音乐播放”，设置3为该菜单参数
	glutAddMenuEntry("MusicStop", 4);   //第四个菜单名“音乐停止”，设置4为该菜单参数
	glutAttachMenu(GLUT_RIGHT_BUTTON);  //菜单附到鼠标右键


	//光照示例代码
	//define light position1 
	GLfloat light_position1[] = { -outer,outer,outer + 4 * inner + 50,0.0 };
	//	GLfloat light_position1[]={0,0,0,0.0};
	GLfloat light_position2[] = { +outer,-outer,outer + inner,0.0 };

	//GLfloat light color1
	GLfloat light_ambient1[] = { 1.0,1.0,1.0,1.0 };
	GLfloat light_diffuse1[] = { 1.0,1.0,1.0,1.0 };
	GLfloat light_specular1[] = { 1.0,1.0,1.0,1.0 };

	//GLfloat light color2
	GLfloat light_ambient2[] = { 0.8,0.8,0.8,1.0 };
	GLfloat light_diffuse2[] = { 0.8,0.8,0.8,1.0 };
	GLfloat light_specular2[] = { 0.8,0.8,0.8,1.0 };

	// light model- global light	
	GLfloat lmodel_ambient[] = { 0.8,0.2,0.2,1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	// set light source location
	glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position2);

	// set light source color;
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular1);

	// set light source color;
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse2);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular2);

	// set material let material fits color	
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	//make light source enable;
	glEnable(GL_LIGHTING);  //启用光源
	glEnable(GL_LIGHT0);  //打开第一盏灯
	glEnable(GL_LIGHT1); //打开第二盏灯

	//Color shade mode
	glShadeModel(GL_SMOOTH);
	// glShadeModel(GL_FLAT);  

	//Material parameter setting
	GLfloat mat_specular1[] = { 1.0,1.0,1.0,1.0 };
	GLfloat mat_shininess1[] = { 80.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular1);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess1);

}

void mymenu(int value)
{
	//如果菜单参数为1，则执行线框模式
	if (value == 1) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//如果菜单参数为2，则执行实心模式
	if (value == 2) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//如果菜单参数为3，则播放音乐
	if (value == 3) mciSendString(TEXT("play MySong"), NULL, 0, 0);
	//如果菜单参数为2，则音乐停止
	if (value == 4) mciSendString(TEXT("stop MySong"), NULL, 0, 0);
}


void mykeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'W':
	case 'w':// 向前直走
	   //your code Here
		eyex = eyex - step * sin(angle*PI / 180.0);
		eyez = eyez - step * cos(angle*PI / 180.0);
		atx = atx - step * sin(angle*PI / 180.0);
		atz = atz - step * cos(angle*PI / 180.0);
		break;
	case 'S':
	case 's'://向后退
	   //your code Here
		eyex = eyex + step * sin(angle*PI / 180.0);
		eyez = eyez + step * cos(angle*PI / 180.0);
		atx = atx + step * sin(angle*PI / 180.0);
		atz = atz + step * cos(angle*PI / 180.0);
		break;
	case 'A':
	case 'a'://左看
		//your code Here
		angle = angle + 1;
		atx = eyex - r * sin(angle*PI / 180.0);
		atz = eyez - r * cos(angle*PI / 180.0);
		break;
	case 'D':
	case 'd'://右看
		//your code Here
		angle = angle - 1;
		atx = eyex - r * sin(angle*PI / 180.0);
		atz = eyez - r * cos(angle*PI / 180.0);
		break;
	}
	glutPostRedisplay();//参数修改后调用重画函数，屏幕图形将发生改变
}

void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyex, eyey, eyez, atx, aty, atz, 0, 1, 0);
	glPushMatrix();
	glColor3f(0.0, 0.0, 1.0);
	drawwall();
	glColor3f(1.0, 0, 0);
	drawground();
	drawsphere();
	glPopMatrix();
	glutSwapBuffers();
}

void drawsphere()
{
	float tr;
	tr = (outer + 3 * inner);
	glRotatef(theta, 0, 1, 0);
	glPushMatrix();
	glPushMatrix();
	glColor3f(1.0, 0, 1.0);
	glutSolidTorus(inner, outer, 30, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(outer, 0, 0);
	glRotatef(theta, 0, 1, 0);
	glTranslatef(-outer, 0, 0);

	glPushMatrix();
	glTranslatef(tr, 0, 0);
	glRotatef(-45, 1, 0, 0);
	glColor3f(0.0, 1.0, 0);
	glutSolidSphere(inner, 20, 20);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
}

void drawground()
{
	//ground
	for (int i = -outer - 4 * inner; i < outer + 4 * inner; i += 2 * inner)
		for (int j = -outer - 4 * inner; j < outer + 4 * inner; j += 2 * inner)
		{
			glBegin(GL_QUADS);
			glVertex3d(j, -outer - 4 * inner, i);
			glVertex3d(j, -outer - 4 * inner, i + 2 * inner);
			glVertex3d(j + 2 * inner, -outer - 4 * inner, i + 2 * inner);
			glVertex3d(j + 2 * inner, -outer - 4 * inner, i);
			glEnd();
		}

	//top
	for (int i = -outer - 4 * inner; i < outer + 4 * inner; i += 2 * inner)
		for (int j = -outer - 4 * inner; j < outer + 4 * inner; j += 2 * inner)
		{
			glBegin(GL_QUADS);
			glVertex3d(j, outer + 4 * inner, i);
			glVertex3d(j, outer + 4 * inner, i + 2 * inner);
			glVertex3d(j + 2 * inner, outer + 4 * inner, i + 2 * inner);
			glVertex3d(j + 2 * inner, outer + 4 * inner, i);
			glEnd();
		}
}

void drawwall()
{
	int i, j;

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//left
	for (i = -outer - 4 * inner; i < outer + 4 * inner; i += 2 * inner)
		for (j = -outer - 4 * inner; j < outer + 4 * inner; j += 2 * inner)
		{
			colorflag = -colorflag;
			if (colorflag == 1)
				glColor3f(1, 1, 0);
			else
				glColor3f(1, 1, 1);

			glBegin(GL_QUADS);
			glVertex3d(-outer - 4 * inner, j, i);
			glVertex3d(-outer - 4 * inner, j + 2 * inner, i);
			glVertex3d(-outer - 4 * inner, j + 2 * inner, i + 2 * inner);
			glVertex3d(-outer - 4 * inner, j, i + 2 * inner);
			glEnd();
		}

	//right
	for (i = -outer - 4 * inner; i <= outer + 4 * inner - 2 * inner; i += 2 * inner)   //for z
		for (j = -outer - 4 * inner; j <= outer + 4 * inner - 2 * inner; j += 2 * inner)   //for y
		{
			glBegin(GL_QUADS);
			glVertex3f(outer + 4 * inner, j, i);
			glVertex3f(outer + 4 * inner, j + 2 * inner, i);
			glVertex3f(outer + 4 * inner, j + 2 * inner, i + 2 * inner);
			glVertex3f(outer + 4 * inner, j, i + 2 * inner);
			glEnd();
		}

	glColor3f(1.0, 1.0, 0.0);
	//front
	for (i = -outer - 4 * inner; i <= outer + 4 * inner - 2 * inner; i += 2 * inner)   //for z
		for (j = -outer - 4 * inner; j <= outer + 4 * inner - 2 * inner; j += 2 * inner)   //for y
		{
			glBegin(GL_QUADS);
			glVertex3f(j, i, -outer - 4 * inner);
			glVertex3f(j + 2 * inner, i, -outer - 4 * inner);
			glVertex3f(j + 2 * inner, i + 2 * inner, -outer - 4 * inner);
			glVertex3f(j, i + 2 * inner, -outer - 4 * inner);
			glEnd();
		}
}

void mytime(int value)
{
	theta += 0.5;
	if (theta >= 360.0) theta -= 360.0;
	glutPostRedisplay();
	glutTimerFunc(100, mytime, 10);
}

void Reshape(GLsizei w, GLsizei h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, w / h, 10, 2 * outer + 8 * inner + 250);
	glViewport(0, 0, w, h);
	glMatrixMode(GL_MODELVIEW);
	ww = w;
	hh = h;
}
