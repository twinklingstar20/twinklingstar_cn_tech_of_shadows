/* Soft shadows using a shadow texture per polygon.  Based on an algorithm   */
/* described by Paul Heckbert and Michael Herf of CMU; see their web site    */
/* http://www.cs.cmu.edu/ph/shadow.html for details.                         */
/*                                                                           */
#include <stdlib.h>
#include <gl/glut.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>


/* list of polygons that have shadow textures */
static GLfloat gPts[][4][3] = {
	/* floor */
	-100.f, -100.f, -320.f,
	-100.f, -100.f, -520.f,
	100.f, -100.f, -320.f,
	100.f, -100.f, -520.f,

	/* left wall */
	-100.f, -100.f, -320.f,
	-100.f,  100.f, -320.f,
	-100.f, -100.f, -520.f,
	-100.f,  100.f, -520.f,

	/* back wall */
	-100.f, -100.f, -520.f,
	-100.f,  100.f, -520.f,
	100.f, -100.f, -520.f,
	100.f,  100.f, -520.f,

	/* right wall */
	100.f, -100.f, -520.f,
	100.f,  100.f, -520.f,
	100.f, -100.f, -320.f,
	100.f,  100.f, -320.f,

	/* ceiling */
	-100.f,  100.f, -520.f,
	-100.f,  100.f, -320.f,
	100.f,  100.f, -520.f,
	100.f,  100.f, -320.f,

	/* blue panel */
	-60.f,  -40.f, -400.f,
	-60.f,   70.f, -400.f,
	-30.f,  -40.f, -480.f,
	-30.f,   70.f, -480.f,

	/* yellow panel */
	-40.f,  -50.f, -400.f,
	-40.f,   50.f, -400.f,
	-10.f,  -50.f, -450.f,
	-10.f,   50.f, -450.f,

	/* red panel */
	-20.f,  -60.f, -400.f,
	-20.f,   30.f, -400.f,
	10.f,  -60.f, -420.f,
	10.f,   30.f, -420.f,

	/* green panel */
	0.f,  -70.f, -400.f,
	0.f,   10.f, -400.f,
	30.f,  -70.f, -395.f,
	30.f,   10.f, -395.f,
};

static GLfloat gMaterials[][4] = {
	1.0f, 1.0f, 1.0f, 1.0f, /* floor        */
	1.0f, 1.0f, 1.0f, 1.0f, /* left wall    */
	1.0f, 1.0f, 1.0f, 1.0f, /* back wall    */
	1.0f, 1.0f, 1.0f, 1.0f, /* right wall   */
	1.0f, 1.0f, 1.0f, 1.0f, /* ceiling      */
	0.2f, 0.5f, 1.0f, 1.0f, /* blue panel   */
	1.0f, 0.6f, 0.0f, 1.0f, /* yellow panel */
	1.0f, 0.2f, 0.2f, 1.0f, /* red panel    */
	0.3f, 0.9f, 0.6f, 1.0f, /* green panel  */
};

/* number of shadow textures to make */
static int gNumShadowTex = sizeof(gPts) / sizeof(gPts[0]);
//window size
static GLuint		gWindowWidth  = 400;
static GLuint		gWindowHeight = 400;

static GLint gTexXSize = 128;
static GLint gTexYSize = 128;

/* texture object names */
const GLuint gFloorTex	 = 1;
const GLuint gShadowTexs = 2;

static GLfloat gOrigin[4]	= { 0.f, 0.f, 0.f, 1.f };
static GLfloat gBlack[4]	= { 0.f, 0.f, 0.f, 1.f };
static GLfloat gAmbient[4]	= { 0.2f, 0.2f, 0.2f, 1.f };

static GLfloat gLightPos[4] = { 70.f, 70.f, -320.f, 1.f };

/* some simple vector utility routines */
void
vcopy(GLfloat a[3], GLfloat b[3])
{
	b[0] = a[0];
	b[1] = a[1];
	b[2] = a[2];
}

void
vnormalize(GLfloat v[3])
{
	float m = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	v[0] /= m;
	v[1] /= m;
	v[2] /= m;
}

void
vadd(GLfloat a[3], GLfloat b[3], GLfloat c[3])
{
	c[0] = a[0] + b[0];
	c[1] = a[1] + b[1];
	c[2] = a[2] + b[2];
}

void
vsub(GLfloat a[3], GLfloat b[3], GLfloat c[3])
{
	c[0] = a[0] - b[0];
	c[1] = a[1] - b[1];
	c[2] = a[2] - b[2];
}

void
vcross(GLfloat a[3], GLfloat b[3], GLfloat c[3])
{
	c[0] = a[1] * b[2] - a[2] * b[1];
	c[1] = -(a[0] * b[2] - a[2] * b[0]);
	c[2] = a[0] * b[1] - a[1] * b[0];
}

float
vdot(GLfloat a[3], GLfloat b[3])
{
	return (a[0]*b[0] + a[1]*b[1] + a[2]*b[2]);
}

void
findNormal(GLfloat pts[][3], GLfloat normal[3]) {
	GLfloat a[3], b[3];

	vsub(pts[1], pts[0], a);
	vsub(pts[2], pts[0], b);
	vcross(b, a, normal);
	vnormalize(normal);
}

/* Make a checkerboard texture for the floor. */
GLfloat *
make_texture(int maxs, int maxt)
{
	GLint s, t;
	static GLfloat *texture;

	texture = (GLfloat *) malloc(maxs * maxt * sizeof(GLfloat));
	for (t = 0; t < maxt; t++) {
		for (s = 0; s < maxs; s++) {
			texture[s + maxs * t] = ((s >> 4) & 0x1) ^ ((t >> 4) & 0x1);
		}
	}
	return texture;
}

void
keyboard(unsigned char key, int x, int y)
{
	if (key == 27)  /* ESC */
		exit(0);
}

void
make_shadow_texture(int index, GLfloat eyept[3])
{
	GLfloat xaxis[3], yaxis[3], zaxis[3];
	GLfloat cov[3]; /* center of view */
	GLfloat pte[3]; /* plane to eye */
	GLfloat eye[3];
	GLfloat tmp[3], dist;
	GLfloat (*qpts)[3] = gPts[index];
	GLfloat left, right, bottom, top;
	GLfloat znear = 10.f, zfar = 600.f;
	GLint n;

	/* For simplicity, we don't compute the transformation matrix described */
	/* in Heckbert and Herf's paper.  The transformation and frustum used   */
	/* here is much simpler.                                                */
	vcopy(eyept, eye);
	vsub(qpts[1], qpts[0], yaxis);
	vsub(qpts[2], qpts[0], xaxis);
	vcross(yaxis, xaxis, zaxis);

	vnormalize(zaxis);
	vnormalize(xaxis); /* x-axis of eye coord system, in object space */
	vnormalize(yaxis); /* y-axis of eye coord system, in object space */

	/* center of view is just eyepoint offset in direction of normal */ 
	vadd(eye, zaxis, cov);

	/* set up viewing matrix */
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(eye[0], eye[1], eye[2],
			  cov[0], cov[1], cov[2],
			  yaxis[0], yaxis[1], yaxis[2]);

	/* compute a frustum that just encloses the polygon */
	vsub(qpts[0], eye, tmp); /* from eye to 0th vertex */
	left = vdot(tmp, xaxis);
	vsub(qpts[2], eye, tmp); /* from eye to 2nd vertex */
	right = vdot(tmp, xaxis);
	vsub(qpts[0], eye, tmp); /* from eye to 0th vertex */
	bottom = vdot(tmp, yaxis);
	vsub(qpts[1], eye, tmp); /* from eye to 1st vertex */
	top = vdot(tmp, yaxis);

	/* scale the frustum values based on the distance to the polygon */
	vsub(qpts[0], eye, pte);
	dist	=  fabs(vdot(zaxis, pte));
	left	*= (znear / dist);
	right	*= (znear / dist);
	bottom	*= (znear / dist);
	top		*= (znear / dist);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glFrustum(left, right, bottom, top, znear, zfar);
	glMatrixMode(GL_MODELVIEW);

	glDisable(GL_LIGHTING);

	glDisable(GL_TEXTURE_2D);

	for (n=0; n < gNumShadowTex; n++) {
		qpts = gPts[n];
	
		if (n == index) {
			/* this poly has full intensity, no occlusion */
			glColor3f(1.f, 1.f, 1.f);
		} else {
			/* all other polys just occlude the light */
			glColor3f(0.f, 0.f, 0.f);
		}
	
		glBegin(GL_TRIANGLE_STRIP);
		glVertex3fv(qpts[0]);
		glVertex3fv(qpts[1]);
		glVertex3fv(qpts[2]);
		glVertex3fv(qpts[3]);
		glEnd();
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void make_all_shadow_textures(float eye[3]) {
	GLint texPerRow;
	GLint n;
	GLfloat x, y;

	texPerRow = (gWindowWidth / gTexXSize);
	for (n=0; n < gNumShadowTex; n++) {
		y = (n / texPerRow) * gTexYSize;
		x = (n % texPerRow) * gTexXSize;
		glViewport(x, y, gTexXSize, gTexYSize);
		make_shadow_texture(n, eye);
	}
	glViewport(0, 0, gWindowWidth, gWindowHeight);
}


void store_all_shadow_textures(void) {
	GLint texPerRow;
	GLint n, x, y;
	GLubyte *texbuf;

	texbuf = (GLubyte *) malloc(gWindowWidth * gTexYSize * sizeof(int));

	/* how many shadow textures can fit in the window */
	texPerRow = (gWindowWidth / gTexXSize);

	for (n=0; n < gNumShadowTex; n++) {
		GLenum format;

		x = (n % texPerRow) * gTexXSize;
		y = (n / texPerRow) * gTexYSize;

		glBindTexture(GL_TEXTURE_2D, gShadowTexs + n);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		format = GL_LUMINANCE;

		glCopyTexImage2D(GL_TEXTURE_2D, 0, format, x, y, gTexXSize, gTexYSize, 0);
	}
	free(texbuf);
}

bool
init()
{
	/* turn on features */
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glCullFace(GL_BACK);
	glLightfv(GL_LIGHT0, GL_AMBIENT, gBlack);
	//Create the floor texture.
	float* tex = make_texture(gTexXSize, gTexYSize);
	glBindTexture(GL_TEXTURE_2D, gFloorTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 1, gTexXSize, gTexYSize, 0, GL_RED, GL_FLOAT, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	free(tex);
	return true;
}

void 
draw(void)
{
	GLint n;
	GLfloat normal[3];
	GLfloat (*qpts)[3];

	glPushMatrix();
	glLoadIdentity();

	glLightfv(GL_LIGHT0, GL_POSITION, gLightPos);

	glPopMatrix();

	/* make shadow textures from just one frame */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	make_all_shadow_textures(gLightPos);
	store_all_shadow_textures();

	
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	glColor3f(1.f, 1.f, 1.f);

	glEnable(GL_TEXTURE_2D);

	/* Unfortunately, using the texture as an occlusion map requires two */
	/* passes: one in which the occlusion map modulates the diffuse      */
	/* lighting, and one in which the ambient lighting is added in. It's */
	/* incorrect to modulate the ambient lighting, but if the result is  */
	/* acceptable to you, you can include it in the first pass and       */
	/* omit the second pass. */

	/* draw only with diffuse light, modulating it with the texture */
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gBlack);
	for (n=0; n < gNumShadowTex; n++) {
		qpts = gPts[n];
		findNormal(qpts, normal);
		glNormal3fv(normal);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gMaterials[n]);
		glBindTexture(GL_TEXTURE_2D, gShadowTexs + n);
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0,0); glVertex3fv(qpts[0]);
		glTexCoord2f(0,1); glVertex3fv(qpts[1]);
		glTexCoord2f(1,0); glVertex3fv(qpts[2]);
		glTexCoord2f(1,1); glVertex3fv(qpts[3]);
		glEnd();
	}

	/* add in the ambient lighting */
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glDepthFunc(GL_LEQUAL);
	for (n=0; n < gNumShadowTex; n++) {
		qpts = gPts[n];
		glColor4f(gAmbient[0] * gMaterials[n][0],
				  gAmbient[1] * gMaterials[n][1],
				  gAmbient[2] * gMaterials[n][2],
				  gAmbient[3] * gMaterials[n][3]);
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0,0); glVertex3fv(qpts[0]);
		glTexCoord2f(0,1); glVertex3fv(qpts[1]);
		glTexCoord2f(1,0); glVertex3fv(qpts[2]);
		glTexCoord2f(1,1); glVertex3fv(qpts[3]);
		glEnd();
	}
	/* restore the ambient colors to their defaults */
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gAmbient);

	/* blend in the checkerboard floor */
	glEnable(GL_BLEND);
	glBlendFunc(GL_ZERO, GL_SRC_COLOR);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gFloorTex);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gMaterials[0]);
	glTranslatef(0.0f, 0.05f, 0.0f);
	glColor3f(1.f, 1.f, 1.f);
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(0.f, 1.f, 0.f);
	glTexCoord2f(0.f, 0.f); glVertex3fv(gPts[0][0]);
	glTexCoord2f(0.f, 1.f); glVertex3fv(gPts[0][1]);
	glTexCoord2f(1.f, 0.f); glVertex3fv(gPts[0][2]);
	glTexCoord2f(1.f, 1.f); glVertex3fv(gPts[0][3]);
	glEnd();

	/* undo some state settings that we did above */
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glDepthFunc(GL_LESS);
	glTranslatef(0.0f, -0.05f, 0.0f);

	glutSwapBuffers();
}

void 
reshape(GLsizei w,GLsizei h)
{
	gWindowWidth	=	w;
	gWindowHeight	=	h;
	/* set up perspective projection */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-30.0f, 30.0f, -30.0f, 30.0f, 100.0f, 640.0f);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc,char ** argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(gWindowWidth, gWindowHeight);
	glutCreateWindow("Heckbert&Herf's Shadows");

	glutReshapeFunc(reshape);
	glutDisplayFunc(draw);
	glutKeyboardFunc(keyboard);

	assert(init());

	glutMainLoop();

	return(0);
}