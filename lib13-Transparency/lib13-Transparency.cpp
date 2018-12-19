#include "stdafx.h"
#include <math.h>
#include <GL/glut.h>

#define PI 3.14159

float theta = -90.0; //圆环旋转角
float angle = 10; //左右场景每次旋转角
float sightangle = -90;
float s = 10; //前后直走步长
float R = 100;
int inner = 10, outer = 80;

float eyex = 0, eyey = 0, eyez = outer + 4 * inner + 50;  //初始视点位置
float atx = 0, aty = 0, atz = 0;     //初始目标点位置
float atx1, atz1, eyex1, eyez1;

float tt = 0, tt2 = 0;

// void specialkeyboard(int key, int x, int y);
void mykeyboard(unsigned char key, int x, int y);
void Display(void);
void Reshape(int w, int h);
void myidle();
void drawground();
void drawsphere();
void drawwall();
void init();

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
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);   //设置显示模式；（缓冲，颜色类型）
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(1024 / 2 - 250, 768 / 2 - 250);
	glutCreateWindow("Rotating 3D World");  //创建窗口，标题为“Rotating 3D World”；
	glutReshapeFunc(Reshape);
	init();
	glutDisplayFunc(Display);  //用于绘制当前窗口；
	glutKeyboardFunc(mykeyboard);
	glutIdleFunc(myidle);
	glutMainLoop();   //表示开始运行程序，用于程序的结尾；

	return 0;
}



void init()
{


	//define light position1
	GLfloat light_position1[] = { -outer,outer,outer + inner,0.0 };
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
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	//clear color
	glClearColor(0.0, 0.0, 0.0, 0.0);

	//Color shade mode
	glShadeModel(GL_SMOOTH);

	// polygon mode   
	glPolygonMode(GL_FRONT, GL_FILL);

	// Anti-aliasing 
  /* glEnable(GL_POINT_SMOOTH);
   glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
   glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	 glEnable(GL_POLYGON_SMOOTH);
	 glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);*/

	 // glEnable(GLUT_MULTISAMPLE);  
	 // depth detecting
	glEnable(GL_DEPTH_TEST);

}

void Display(void)
{


	glClearColor(1, 1, 1, 1);
	//	glClear(GL_COLOR_BUFFER_BIT);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	//	gluLookAt(0,-10,350,0,0,0,0,1,0);

	gluLookAt(eyex, eyey, eyez, atx, aty, atz, 0, 1, 0);




	//about ground
	glPushMatrix();
	glFrontFace(GL_CW);
	glTranslatef(0, -outer - 4 * inner, 0);
	glScalef(1.0f, -1.0f, 1.0f);
	glTranslatef(0, outer + 4 * inner, 0);
	drawsphere();
	glFrontFace(GL_CCW);
	glPopMatrix();

	//about top 
	glPushMatrix();
	glFrontFace(GL_CW);
	glTranslatef(0, +outer + 4 * inner, 0);
	glScalef(1.0f, -1.0f, 1.0f);
	glTranslatef(0, -outer - 4 * inner, 0);
	drawsphere();
	glFrontFace(GL_CCW);
	glPopMatrix();

	//about right wall
	glPushMatrix();
	glFrontFace(GL_CW);
	glTranslatef(+outer + 4 * inner, 0, 0);
	glScalef(-1.0f, 1.0f, 1.0f);
	glTranslatef(-outer - 4 * inner, 0, 0);
	drawsphere();
	glFrontFace(GL_CCW);
	glPopMatrix();

	//about left wall
	glPushMatrix();
	glFrontFace(GL_CW);
	glTranslatef(-outer - 4 * inner, 0, 0);
	glScalef(-1.0f, 1.0f, 1.0f);
	glTranslatef(+outer + 4 * inner, 0, 0);
	drawsphere();
	glFrontFace(GL_CCW);
	glPopMatrix();

	//about front wall
	glPushMatrix();
	glFrontFace(GL_CW);
	glTranslatef(0, 0, -outer - 4 * inner);
	glScalef(1.0f, 1.0f, -1.0f);
	glTranslatef(0, 0, outer + 4 * inner);
	drawsphere();
	glFrontFace(GL_CCW);
	glPopMatrix();

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	drawground();
	drawwall();
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);




	drawsphere();


	glutSwapBuffers();
	//	glFlush();
}


void drawsphere()
{

	float tr;
	tr = (outer + 3 * inner);

	glRotatef(theta, 0, 1, 0);

	glPushMatrix();
	glPushMatrix();
	glColor3f(1.0, 0, 0.0);
	glutSolidTorus(inner, outer, 50, 80);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(outer, 0, 0);
	glRotatef(theta, 0, 1, 0);
	glTranslatef(-outer, 0, 0);

	glPushMatrix();
	glTranslatef(tr, 0, 0);
	glRotatef(-45, 1, 0, 0);
	glColor3f(0.0, 1.0, 0);
	glutSolidSphere(inner, 40, 40);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();

}

void drawground()
{

	int colorflag = 1;

	GLfloat mat_specular1[] = { 1.0,1.0,1.0,1.0 };
	GLfloat mat_shininess1[] = { 80.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular1);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess1);

	glNormal3f(0, 1, 0);
	for (int i = -outer - 4 * inner; i < outer + 4 * inner; i += 2 * inner)
	{
		if (colorflag > 0)    glColor4f(1.0, 1.0, 0.0, 0.8);
		else         glColor4f(1.0, 1.0, 1.0, 0.8);

		colorflag = -colorflag;

		for (int j = -outer - 4 * inner; j < outer + 4 * inner; j += 2 * inner)
		{
			if (colorflag > 0)    glColor4f(1.0, 1.0, 0.0, 0.8);
			else         glColor4f(1.0, 1.0, 1.0, 0.8);

			colorflag = -colorflag;
			glBegin(GL_QUADS);
			glVertex3d(j, -outer - 4 * inner, i);
			glVertex3d(j, -outer - 4 * inner, i + 2 * inner);
			glVertex3d(j + 2 * inner, -outer - 4 * inner, i + 2 * inner);
			glVertex3d(j + 2 * inner, -outer - 4 * inner, i);
			glEnd();
		}
	}


	glNormal3f(0, -1, 0);
	colorflag = 1;

	for (int i = -outer - 4 * inner; i < outer + 4 * inner; i += 2 * inner)
	{
		if (colorflag > 0)    glColor4f(1.0, 1.0, 0.0, 0.8);
		else         glColor4f(1.0, 1.0, 1.0, 0.8);

		colorflag = -colorflag;

		for (int j = -outer - 4 * inner; j < outer + 4 * inner; j += 2 * inner)
		{
			if (colorflag > 0)    glColor4f(1.0, 1.0, 0.0, 0.8);
			else         glColor4f(1.0, 1.0, 1.0, 0.8);

			colorflag = -colorflag;
			glBegin(GL_QUADS);
			glVertex3d(j, outer + 4 * inner, i);
			glVertex3d(j, outer + 4 * inner, i + 2 * inner);
			glVertex3d(j + 2 * inner, outer + 4 * inner, i + 2 * inner);
			glVertex3d(j + 2 * inner, outer + 4 * inner, i);
			glEnd();
		}
	}


}


void drawwall()
{
	int i, j;

	glNormal3f(1, 0, 0);

	int colorflag = 1;

	//left
	for (i = -outer - 4 * inner; i < outer + 4 * inner; i += 2 * inner)
	{
		if (colorflag > 0)    glColor4f(1.0, 1.0, 0.0, 0.8);
		else         glColor4f(1.0, 1.0, 1.0, 0.8);

		colorflag = -colorflag;

		for (j = -outer - 4 * inner; j < outer + 4 * inner; j += 2 * inner)
		{
			if (colorflag > 0)    glColor4f(1.0, 1.0, 0.0, 0.8);
			else         glColor4f(1.0, 1.0, 1.0, 0.8);

			colorflag = -colorflag;

			glBegin(GL_QUADS);
			glVertex3d(-outer - 4 * inner, j, i);
			glVertex3d(-outer - 4 * inner, j + 2 * inner, i);
			glVertex3d(-outer - 4 * inner, j + 2 * inner, i + 2 * inner);
			glVertex3d(-outer - 4 * inner, j, i + 2 * inner);
			glEnd();
		}
	}


	colorflag = 1;

	glNormal3f(0, -1, 0);

	//right
	for (i = -outer - 4 * inner; i <= outer + 4 * inner - 2 * inner; i += 2 * inner)   //for z
	{
		if (colorflag > 0)    glColor4f(1.0, 1.0, 0.0, 0.8);
		else         glColor4f(1.0, 1.0, 1.0, 0.8);

		colorflag = -colorflag;

		for (j = -outer - 4 * inner; j <= outer + 4 * inner - 2 * inner; j += 2 * inner)   //for y
		{
			if (colorflag > 0)    glColor4f(1.0, 1.0, 0.0, 0.8);
			else         glColor4f(1.0, 1.0, 1.0, 0.8);

			colorflag = -colorflag;

			glBegin(GL_QUADS);
			glVertex3f(outer + 4 * inner, j, i);
			glVertex3f(outer + 4 * inner, j + 2 * inner, i);
			glVertex3f(outer + 4 * inner, j + 2 * inner, i + 2 * inner);
			glVertex3f(outer + 4 * inner, j, i + 2 * inner);
			glEnd();

		}
	}

	colorflag = 1;

	glNormal3f(0, 0, 1);

	//front
	for (i = -outer - 4 * inner; i <= outer + 4 * inner - 2 * inner; i += 2 * inner)   //for z
	{
		if (colorflag > 0)    glColor4f(1.0, 1.0, 0.0, 0.8);
		else         glColor4f(1.0, 1.0, 1.0, 0.8);

		colorflag = -colorflag;

		for (j = -outer - 4 * inner; j <= outer + 4 * inner - 2 * inner; j += 2 * inner)   //for y
		{
			if (colorflag > 0)    glColor4f(1.0, 1.0, 0.0, 0.8);
			else         glColor4f(1.0, 1.0, 1.0, 0.8);

			colorflag = -colorflag;

			glBegin(GL_QUADS);
			glVertex3f(j, i, -outer - 4 * inner);
			glVertex3f(j + 2 * inner, i, -outer - 4 * inner);
			glVertex3f(j + 2 * inner, i + 2 * inner, -outer - 4 * inner);
			glVertex3f(j, i + 2 * inner, -outer - 4 * inner);
			glEnd();

		}
	}


}

void myidle()
{

	theta += 0.5;
	//	if (theta>=360.0) theta-=360.0;    
	glutPostRedisplay();
}

void Reshape(GLsizei w, GLsizei h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//	 glOrtho(-outer-6*inner,outer+6*inner,-outer-4*inner,outer+4*inner,20,2*outer+8*inner+50);
	gluPerspective(90, w / h, 10, 2 * outer + 8 * inner + 250);
	glViewport(0, 0, w, h);
	glMatrixMode(GL_MODELVIEW);


}

void mykeyboard(unsigned char key, int x, int y)
{

	switch (key)
	{
	case 'W':
	case 'w':// 向前走

		eyex1 = eyex - s * sin(sightangle * 2 * PI / 360 - PI / 2);
		eyez1 = eyez - s * cos(sightangle * 2 * PI / 360 - PI / 2);

		atx1 = atx - s * sin(sightangle * 2 * PI / 360 - PI / 2);
		atz1 = atz - s * cos(sightangle * 2 * PI / 360 - PI / 2);

		eyex = eyex1;
		eyez = eyez1;

		atz = atz1;
		atx = atx1;

		break;
	case 'S':
	case 's'://向后走
		eyex1 = eyex + s * sin(sightangle * 2 * PI / 360 - PI / 2);
		eyez1 = eyez + s * cos(sightangle * 2 * PI / 360 - PI / 2);

		atx1 = atx + s * sin(sightangle * 2 * PI / 360 - PI / 2);
		atz1 = atz + s * cos(sightangle * 2 * PI / 360 - PI / 2);

		eyex = eyex1;
		eyez = eyez1;

		atz = atz1;
		atx = atx1;
		break;
	case 'A':
	case 'a'://左转
		/* atx1=eyex+(atx-eyex)*cos(angle*2*PI/360.0)+(eyez-atz)*sin(angle*2*PI/360.0);
		 atz1=eyez-(eyez-atz)*sin(angle*2*PI/360.0)+(atx-eyex)*cos(angle*2*PI/360.0);*/

		atx1 = eyex + (atx - eyex)*cos(angle * 2 * PI / 360.0) + (eyez - atz)*sin(angle * 2 * PI / 360.0);
		atz1 = eyez - (eyez - atz)*cos(angle * 2 * PI / 360.0) - (-atx + eyex)*sin(angle * 2 * PI / 360.0);

		atx = atx1;
		atz = atz1;

		sightangle = sightangle + angle;

		break;
	case 'D':
	case 'd'://右转
	   /*atx1=eyex+(atx-eyex)*cos(-angle*2*PI/360.0)+(eyez-atz)*sin(-angle*2*PI/360.0);
		 atz1=eyez-(eyez-atz)*sin(-angle*2*PI/360.0)+(atx-eyex)*cos(-angle*2*PI/360.0);*/

		atx1 = eyex + (atx - eyex)*cos(angle * 2 * PI / 360.0) - (eyez - atz)*sin(angle * 2 * PI / 360.0);
		atz1 = eyez - (eyez - atz)*cos(angle * 2 * PI / 360.0) + (-atx + eyex)*sin(angle * 2 * PI / 360.0);

		atx = atx1;
		atz = atz1;

		sightangle = sightangle - angle;

		break;
	}
	//参数修改后调用重画函数，屏幕图形将发生改变
	glutPostRedisplay();


}