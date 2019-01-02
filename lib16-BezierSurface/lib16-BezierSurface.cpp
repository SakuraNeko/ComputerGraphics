//BezierSurfacet.cpp //
#include "stdafx.h"
#include <gl/glut.h>
#include <math.h>
#include "Texture.h"
#include <windows.h>


GLuint textureid[1];
float flag = 0;
float nNumPoints = 4;


GLfloat ctrlpoints[4][4][3] = {
   {{ -1.5, -1.5, 4.0}, { -0.5, -1.5, 2.0},
	{0.5, -1.5, -1.0}, {1.5, -1.5, 2.0}},
   {{ -1.5, -0.5, 1.0}, { -0.5, -0.5, 3.0},
	{0.5, -0.5, 0.0}, {1.5, -0.5, -1.0}},
   {{ -1.5, 0.5, 4.0}, { -0.5, 0.5, 0.0},
	{0.5, 0.5, 3.0}, {1.5, 0.5, 4.0}},
   {{ -1.5, 1.5, -2.0}, { -0.5, 1.5, -2.0},
	{0.5, 1.5, 0.0}, {1.5, 1.5, -1.0}}
};

GLfloat texpts[2][2][2] = { {{0.0, 0.0}, {0.0, 1.0}},
			{{1.0, 0.0}, {1.0, 1.0}} };

float rotatey = 0, rotatex = 0;

void initlights(void);
void reshape(int w, int h);
void DrawPoints(void);
void display(void);
void init();
void makeImage(void);
void keyboard(unsigned char key, int x, int y);
void myidle();
void mymenu(int value);


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
	glutCreateWindow("Bezier Curve");  //创建窗口，标题为“Rotating 3D World”；
	init();;
	glutDisplayFunc(display);  //用于绘制当前窗口；
	glutCreateMenu(mymenu);
	glutAddMenuEntry("Surface Show-Control Points", 0);
	glutAddMenuEntry("Surface Show-Mesh", 1);
	glutAddMenuEntry("Surface Show-Fill Mode under Light & Material", 2);
	glutAddMenuEntry("Surface Show-Design Picture", 3);
	glutAddMenuEntry("Surface Show-Image Texture", 4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutIdleFunc(myidle);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();   //表示开始运行程序，用于程序的结尾；

	return 0;
}

void initlights(void)
{
	GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat position[] = { 0.0, 0.0, 2.0, 1.0 };
	GLfloat mat_diffuse[] = { 0.6, 0.6, 0.6, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}


// This function is used to superimpose the control points over the curve
void DrawPoints(void)
{
	int i, j;	// Counting variables

	// Set point size larger to make more visible
	glPointSize(5.0f);

	// Loop through all control points for this example
	glBegin(GL_POINTS);
	for (i = 0; i < nNumPoints; i++)
		for (j = 0; j < 3; j++)
			glVertex3fv(ctrlpoints[i][j]);
	glEnd();
}



#define	imageWidth 64
#define	imageHeight 64
GLubyte image[3 * imageWidth*imageHeight];

void makeImage(void)
{
	int i, j;
	float ti, tj;

	for (i = 0; i < imageWidth; i++) {
		ti = 2.0*3.14159265*i / imageWidth;
		for (j = 0; j < imageHeight; j++) {
			tj = 2.0*3.14159265*j / imageHeight;

			image[3 * (imageHeight*i + j)] = (GLubyte)127 * (1.0 + sin(ti));
			image[3 * (imageHeight*i + j) + 1] = (GLubyte)127 * (1.0 + cos(2 * tj));
			image[3 * (imageHeight*i + j) + 2] = (GLubyte)127 * (1.0 + cos(ti + tj));
		}
	}
}



void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glRotatef(rotatey, 0, 1, 0);
	glRotatef(rotatex, 1, 0, 0);

	if (flag == 0)
	{
		glDisable(GL_LIGHTING);
		DrawPoints();
	}
	if (flag == 1) {
		glDisable(GL_LIGHTING);
		glEvalMesh2(GL_LINE, 0, 20, 0, 20);
	}
	if (flag == 2)
	{
		initlights();
		// glDisable(GL_TEXTURE_2D);
		glEvalMesh2(GL_FILL, 0, 20, 0, 20);
	}
	if (flag == 3)
	{
		initlights();
		makeImage();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0,
			GL_RGB, GL_UNSIGNED_BYTE, image);
		glEnable(GL_TEXTURE_2D);
		glEvalMesh2(GL_FILL, 0, 20, 0, 20);
	}
	if (flag == 4)
	{

		initlights();
		glBindTexture(GL_TEXTURE_2D, textureid[0]);
		glEnable(GL_TEXTURE_2D);
		glEvalMesh2(GL_FILL, 0, 20, 0, 20);

	}

	glDisable(GL_TEXTURE_2D);

	// glFlush();
	glutSwapBuffers();

}



void init(void)
{

	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4,
		0, 1, 12, 4, &ctrlpoints[0][0][0]);
	glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2,
		0, 1, 4, 2, &texpts[0][0][0]);
	glEnable(GL_MAP2_TEXTURE_COORD_2);
	glEnable(GL_MAP2_VERTEX_3);
	glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);


	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0,
 //                GL_RGB, GL_UNSIGNED_BYTE, image);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	BuildTexture((char*)"Texture.jpg", textureid[0]);

}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glOrtho(-4.0, 4.0, -4.0*(GLfloat)h / (GLfloat)w,
			4.0*(GLfloat)h / (GLfloat)w, -4.0, 4.0);
	else
		glOrtho(-4.0*(GLfloat)w / (GLfloat)h,
			4.0*(GLfloat)w / (GLfloat)h, -4.0, 4.0, -4.0, 4.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(85.0, 1.0, 1.0, 1.0);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	}
}


void myidle()
{
	Sleep(100);

	rotatey++;
	rotatex++;
	glutPostRedisplay();

}

void mymenu(int value)
{

	flag = value;
	glutPostRedisplay();
}
