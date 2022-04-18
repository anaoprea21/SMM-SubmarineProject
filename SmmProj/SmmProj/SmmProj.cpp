#include<GL/freeglut.h>
#include <stdio.h>


float xt = 1.0, yt = 1.0;            //for keyboard
float x = 1.0, y = 1.0, z = 1.0;    // for movement
float angle = 0;  				// for animation
float Autorun = 300;			// For Movement Autorun

void animation(void)
{
	if (angle >= 0 && angle < 10)
		angle = angle + 0.5;
	else angle = 0;
	glutPostRedisplay();
}
void Auto(void)
{
	if (Autorun <= 300 && Autorun > -350)
		Autorun = Autorun - 0.05;
	else Autorun = 300;
	glutPostRedisplay();
}
void settings(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glPointSize(5.0);
	glLineWidth(1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 400.0, 0.0, 400.0);
}
void Display(void)
{

	//background color
	GLclampf Red = 0.0f, Green = 0.0f, Blue = 1.0f, Alpha = 0.0f;
	glClearColor(Red, Green, Blue, Alpha);
	glClear(GL_COLOR_BUFFER_BIT);

	//the submarine
	glPushMatrix();
	glPushMatrix();
	glRotatef(angle, 0.0, 0.0, 0.0);
	glTranslatef(Autorun, 0.0, 0.0);
	glTranslatef(xt, yt, 0.0);			//for move 

	//body
	glBegin(GL_POLYGON);
	glColor3f(0.0, 1.0, 0.0);
	glVertex2i(20, 200);//most right
	glVertex2i(10, 280);//most upper right
	glVertex2i(100, 300);//middle up
	glVertex2i(320, 250); //most left up
	glVertex2i(320, 200); //most left down
	glVertex2i(100, 160);//middle down
	glEnd();

	glPushMatrix();
	glRotatef(angle, 0.0, 0.0, 0.0);


	//propeller
	glBegin(GL_POLYGON);
	glColor3f(0.0, 1.0, 0.0);
	glVertex2i(320, 225);
	glVertex2i(360, 265);
	glVertex2i(340, 225);
	glVertex2i(360, 185);
	glVertex2i(320, 225);
	glEnd();

	glPopMatrix();
	glPopMatrix();
	glutSwapBuffers();
	glFlush();
}
void keyboard(GLubyte key, int x, int y)
{
	switch (key)
	{
	case 'd':
		xt += 2.0;
		glColor3f(0.0, 1.0, 0.0);
		glutPostRedisplay();
		break;

	case 'a':
		xt -= 2.0;
		glColor3f(1.0, 0.0, 0.0);
		glutPostRedisplay();
		break;

	case 's':
		yt -= 2.0;
		glColor3f(0.0, 0.0, 0.0);
		glutPostRedisplay();
		break;

	case 'w':
		yt += 2.0;
		glColor3f(0.0, 0.0, 1.0);
		glutPostRedisplay();
		break;

	case 'e':
		xt += 2.0;
		yt += 2.0;
		glColor3f(1.0, 0.0, 1.0);
		glutPostRedisplay();
		break;

	case 'q':
		xt -= 2.0;
		yt += 2.0;
		glColor3f(0.0, 1.0, 1.0);
		glutPostRedisplay();
		break;
	case 'c':
		xt += 2.0;
		yt -= 2.0;
		glColor3f(1.0, 0.0, 1.0);
		glutPostRedisplay();
		break;
	case 'z':
		xt -= 2.0;
		yt -= 2.0;
		glColor3f(0.0, 0.0, 1.0);
		glutPostRedisplay();
		break;

	default:
		break;
	}
}
void main(int arg1, char** arg2)
{
	//initialize
	glutInit(&arg1, arg2);

	//single for single buffer, single window
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	//window settings
	glutInitWindowPosition(200, 50);   //200 Lelt 50 Height
	glutInitWindowSize(600, 600);
	glutCreateWindow("Submarine --- first try");//window title

	settings();
	glutDisplayFunc(Display);

	//animation
	glutIdleFunc(animation);
	glutIdleFunc(Auto);

	glutKeyboardFunc(keyboard);
	glutMainLoop();
}