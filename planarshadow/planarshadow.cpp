/************************************************************************       
\link   www.twinklingstar.cn
\author twinklingstar
\date   2015/03/15
\file   planarshadow.cpp
****************************************************************************/
#include <stdlib.h>
#include <gl/glut.h>
#include <string.h>
#include <stdio.h>

//Camera & light positions
float cameraPosition[]={-4.0f, 6.0f,-4.0f};
float lightPosition[]={1.0f,2.0f,-1.0f};


//window size
int		windowWidth;
int		windowHeight;


float white[]={1.0f,1.0f,1.0f,1.0f};
float black[]={0.0f,0.0f,0.0f,0.0f};
float plane[]={0.0f,1.0f,0.0f,0.0f};

float dot3x3(float* v1,float* v2)
{
	return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

void cross3x3(float* v1,float* v2,float* result)
{
	result[0] = v1[1]*v2[2] - v1[2]*v2[1];
	result[1] = v1[2]*v2[0] - v1[0]*v2[2];
	result[2] = v1[0]*v2[2] - v1[2]*v2[0];
}

void setMatrix(float* plane,float* light,float *result)
{
	float delta = dot3x3(plane,light) + plane[3];
	result[0] = delta - plane[0]*light[0];
	result[4] = -plane[1]*light[0];
	result[8] = -plane[2]*light[0];
	result[12] = -plane[3]*light[0];

	result[1] = -plane[0]*light[1];
	result[5] = delta - plane[1]*light[1];
	result[9] = -plane[2]*light[1];
	result[13] = -plane[3]*light[1];

	result[2] = -plane[0]*light[2];
	result[6] = -plane[1]*light[2];
	result[10] = delta - plane[2]*light[2];
	result[14] = -plane[3]*light[2];

	result[3] = -plane[0];
	result[7] = -plane[1];
	result[11] = -plane[2];
	result[15] = dot3x3(plane,light);
}


void drawPlane()
{
	int size = 4;
	float height = 0.0;

	glColor3f(0.0f,1.0f,0.0f);
	glPushMatrix();
	glBegin(GL_QUADS);
	glVertex3f(size,height,size);
	glVertex3f(-size,height,size);
	glVertex3f(-size,height,-size);
	glVertex3f(size,height,-size);
	glEnd();
	glPopMatrix();
}

void drawLight()
{
	glColor3f(1.0f,1.0f,1.0f);
	glPushMatrix();
	glTranslatef(lightPosition[0],lightPosition[1],lightPosition[2]);
	glutSolidSphere(0.05,32,32);
	glPopMatrix();
}

void drawScene( bool isShadow = false)
{
	if( isShadow )
		glColor3f(1.0f, 0.0f, 0.0f);
	else
		glColor3f(0.0f,0.0f,1.0f);

	static float a = 0.0f;
	glPushMatrix();
	glTranslatef(0.0f, 0.5f, 0.0f);
	glRotatef(a, 1.0f, 1.0f, 0.0f);
	glutSolidTorus(0.2, 0.5, 24, 48);
	glPopMatrix();

	//draw false shadow
	glPushMatrix();
	glTranslatef(0.0f, -4.0f, 0.0f);
	glRotatef(a, 1.0f, 0.0f, 1.0f);
	glutSolidTorus(0.2, 0.5, 24, 48);
	glPopMatrix();

	//draw anti-shadow
	glPushMatrix();
	glTranslatef(1.0f, 3.0f, -1.0f);
	glRotatef(a, 0.0f, 1.0f, 1.0f);
	glutSolidTorus(0.2, 0.5, 24, 48);
	glPopMatrix();

	a += 0.05f;
}

bool init()
{
	//Shading states
	glShadeModel(GL_SMOOTH);
	//Clearing color of the color buffer.
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//Clearing depth of the depth buffer
	glClearDepth(1.0f);
	//Depth test.
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	//We use glScale when drawing the scene
	glEnable(GL_NORMALIZE);
	//Use the color as the ambient and diffuse material
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	//White specular material color, shininess 16
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMaterialf(GL_FRONT, GL_SHININESS, 16.0f);
	return true;
}

void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cameraPosition[0], cameraPosition[1], cameraPosition[2], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glViewport(0, 0, windowWidth, windowHeight);

	//draw the plane.
	//Use dim light to represent shadowed areas
	float lightColor[]={0.2f,0.2f,0.2f,1.0f};
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightColor);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT1, GL_SPECULAR, white);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	drawPlane();

	//draw the shadows.
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	float matrix[16];
	setMatrix(plane,lightPosition,matrix);
	glPushMatrix();
	glMultMatrixf(matrix);
	drawScene(true);
	glPopMatrix();

	//draw the scene.
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	drawScene(false);

	//draw the light
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	drawLight();

	glutSwapBuffers();
	glutPostRedisplay();
}

void myReshape(GLsizei w,GLsizei h)
{
	windowWidth	=	w;
	windowHeight=	h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)windowWidth/windowHeight, 1.0f, 1000.0f);

}

int main(int argc,char ** argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(400,400);
	glutCreateWindow("Planar Projection Shadows");
	if( !init() )
		return 0;
	glutReshapeFunc(myReshape);
	glutDisplayFunc(myDisplay);

	glutMainLoop();
	return(0);
}