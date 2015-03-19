/************************************************************************		
\link	www.twinklingstar.cn
\author Twinkling Star
\date	2014/03/17
****************************************************************************/
#include <stdlib.h>
#include <gl/glew.h>
#include <gl/glut.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#pragma comment(lib,"glew32.lib")
//Camera & light positions

float cameraPosition[]={-2.5f, 3.5f,-2.5f};
float lightPosition[]={2.0f,3.0f,-2.0f};


//shadow width and height
int		gShadowWidth;
int		gShadowHeight;

//window size
int		gWinWidth;
int		gWinHeight;

//Textures
GLuint gShadowTex;

float gCameraMV[16];
float gLightPM[16];
float gLightVM[16];


float white[]={1.0f,1.0f,1.0f,1.0f};
float black[]={0.0f,0.0f,0.0f,0.0f};


bool isExtensionSupported( char* szTargetExtension )
{
	const unsigned char *pszExtensions = NULL;
	const unsigned char *pszStart;
	unsigned char *pszWhere, *pszTerminator;

	// Extension names should not have spaces
	pszWhere = (unsigned char *) strchr( szTargetExtension, ' ' );
	if( pszWhere || *szTargetExtension == '\0' )
		return false;

	// Get Extensions String
	pszExtensions = glGetString( GL_EXTENSIONS );

	// Search The Extensions String For An Exact Copy
	pszStart = pszExtensions;
	for(;;)
	{
		pszWhere = (unsigned char *) strstr( (const char *) pszStart, szTargetExtension );
		if( !pszWhere )
			break;
		pszTerminator = pszWhere + strlen( szTargetExtension );
		if( pszWhere == pszStart || *( pszWhere - 1 ) == ' ' )
			if( *pszTerminator == ' ' || *pszTerminator == '\0' )
				return true;
		pszStart = pszTerminator;
	}
	return false;
}


void drawScene(float angle)
{
	//Display lists for objects
	static GLuint spheresList=0, torusList=0, baseList=0;

	//Create spheres list if necessary
	if(!spheresList)
	{
		spheresList=glGenLists(1);
		glNewList(spheresList, GL_COMPILE);
		{
			glColor3f(0.0f, 1.0f, 0.0f);
			glPushMatrix();

			glTranslatef(0.45f, 1.0f, 0.45f);
			glutSolidSphere(0.2, 24, 24);

			glTranslatef(-0.9f, 0.0f, 0.0f);
			glutSolidSphere(0.2, 24, 24);

			glTranslatef(0.0f, 0.0f,-0.9f);
			glutSolidSphere(0.2, 24, 24);

			glTranslatef(0.9f, 0.0f, 0.0f);
			glutSolidSphere(0.2, 24, 24);

			glPopMatrix();
		}
		glEndList();
	}

	//Create torus if necessary
	if(!torusList)
	{
		torusList=glGenLists(1);
		glNewList(torusList, GL_COMPILE);
		{
			glColor3f(1.0f, 0.0f, 0.0f);
			glPushMatrix();

			glTranslatef(0.0f, 0.5f, 0.0f);
			glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			glutSolidTorus(0.2, 0.5, 24, 48);

			glPopMatrix();
		}
		glEndList();
	}

	//Create base if necessary
	if(!baseList)
	{
		baseList=glGenLists(1);
		glNewList(baseList, GL_COMPILE);
		{
			glColor3f(0.0f, 0.0f, 1.0f);
			glPushMatrix();

			glScalef(1.0f, 0.05f, 1.0f);
			glutSolidCube(3.0f);

			glPopMatrix();
		}
		glEndList();
	}


	//Draw objects
	glCallList(baseList);
	glCallList(torusList);

	glPushMatrix();
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	glCallList(spheresList);
	glPopMatrix();
}

bool init()
{
	if( !isExtensionSupported("GL_ARB_depth_texture") ||
		!isExtensionSupported("GL_ARB_shadow") ||
		!isExtensionSupported("GL_ARB_texture_non_power_of_two"))
	{
		printf("Don't support!\n");
		return false;
	}

	//Shading states
	glShadeModel(GL_SMOOTH);

	//Clearing color of the color buffer.
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//Clearing depth of the depth buffer
	glClearDepth(1.0f);
	//Depth test.
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	//We use glScale when drawing the scene
	glEnable(GL_NORMALIZE);

	//Create the shadow map texture
	glGenTextures(1, &gShadowTex);
	glBindTexture(GL_TEXTURE_2D, gShadowTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, gShadowWidth, gShadowHeight, 0,GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	//Use the color as the ambient and diffuse material
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	//White specular material color, shininess 16
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMaterialf(GL_FRONT, GL_SHININESS, 16.0f);
	//glCullFace(GL_FRONT);

	//Calculate & save matrices
	//Load identity modelview
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(cameraPosition[0], cameraPosition[1], cameraPosition[2], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, gCameraMV);

	glLoadIdentity();
	gluPerspective(45.0f, 1.0f, 1.0f, 8.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, gLightPM);

	glLoadIdentity();
	gluLookAt(lightPosition[0], lightPosition[1], lightPosition[2],0.0f, 0.0f, 0.0f,0.0f, 1.0f, 0.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, gLightVM);
	glPopMatrix();

	return true;
}

void multiplyMatrix4(float* matrix1,float* matrix2,float* result)
{
	result[0] = matrix1[0]*matrix2[0] + matrix1[1]*matrix2[4] + matrix1[2]*matrix2[8] + matrix1[3]*matrix2[12];
	result[1] = matrix1[0]*matrix2[1] + matrix1[1]*matrix2[5] + matrix1[2]*matrix2[9] + matrix1[3]*matrix2[13];
	result[2] = matrix1[0]*matrix2[2] + matrix1[1]*matrix2[6] + matrix1[2]*matrix2[10] + matrix1[3]*matrix2[14];
	result[3] = matrix1[0]*matrix2[3] + matrix1[1]*matrix2[7] + matrix1[2]*matrix2[11] + matrix1[3]*matrix2[15];

	result[4] = matrix1[4]*matrix2[0] + matrix1[5]*matrix2[4] + matrix1[6]*matrix2[8] + matrix1[7]*matrix2[12];
	result[5] = matrix1[4]*matrix2[1] + matrix1[5]*matrix2[5] + matrix1[6]*matrix2[9] + matrix1[7]*matrix2[13];
	result[6] = matrix1[4]*matrix2[2] + matrix1[5]*matrix2[6] + matrix1[6]*matrix2[10] + matrix1[7]*matrix2[14];
	result[7] = matrix1[4]*matrix2[3] + matrix1[5]*matrix2[7] + matrix1[6]*matrix2[11] + matrix1[7]*matrix2[15];

	result[8] = matrix1[8]*matrix2[0] + matrix1[9]*matrix2[4] + matrix1[10]*matrix2[8] + matrix1[11]*matrix2[12];
	result[9] = matrix1[8]*matrix2[1] + matrix1[9]*matrix2[5] + matrix1[10]*matrix2[9] + matrix1[11]*matrix2[13];
	result[10] = matrix1[8]*matrix2[2] + matrix1[9]*matrix2[6] + matrix1[10]*matrix2[10] + matrix1[11]*matrix2[14];
	result[11] = matrix1[8]*matrix2[3] + matrix1[9]*matrix2[7] + matrix1[10]*matrix2[11] + matrix1[11]*matrix2[15];

	result[12] = matrix1[12]*matrix2[0] + matrix1[13]*matrix2[4] + matrix1[14]*matrix2[8] + matrix1[15]*matrix2[12];
	result[13] = matrix1[12]*matrix2[1] + matrix1[13]*matrix2[5] + matrix1[14]*matrix2[9] + matrix1[15]*matrix2[13];
	result[14] = matrix1[12]*matrix2[2] + matrix1[13]*matrix2[6] + matrix1[14]*matrix2[10] + matrix1[15]*matrix2[14];
	result[15] = matrix1[12]*matrix2[3] + matrix1[13]*matrix2[7] + matrix1[14]*matrix2[11] + matrix1[15]*matrix2[15];
}


void createDepthTexture()
{
	
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(gLightPM);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(gLightVM);
	//Use viewport the same size as the shadow map
	glViewport(0, 0, gShadowWidth, gShadowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Draw back faces into the shadow map
	glCullFace(GL_FRONT);
	//Disable color writes
	glColorMask(0, 0, 0, 0);
	//Draw the scene
	drawScene(0);
	//Read the depth buffer into the shadow map texture
	glBindTexture(GL_TEXTURE_2D, gShadowTex);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, gShadowWidth, gShadowHeight);
	//restore states
	glCullFace(GL_BACK);
	glColorMask(1, 1, 1, 1);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void coordTransforms()
{
	//用环境光渲染渲染场景
	float lightColor[]={0.2f,0.2f,0.2f,1.0f};
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightColor);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, black);
	glLightfv(GL_LIGHT1, GL_SPECULAR, black);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);
	drawScene(0);

	float bias[]={0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f};
	float texM[16], tpM[16];
	multiplyMatrix4(gLightPM,bias,tpM);
	multiplyMatrix4(gLightVM,tpM,texM);
	//设置纹理坐标的生成参数
	float column1[4],column2[4],column3[4],column4[4];
	column1[0] = texM[0];
	column1[1] = texM[4];
	column1[2] = texM[8];
	column1[3] = texM[12];
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_S, GL_EYE_PLANE, column1);
	glEnable(GL_TEXTURE_GEN_S);

	column2[0] = texM[1];
	column2[1] = texM[5];
	column2[2] = texM[9];
	column2[3] = texM[13];
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_T, GL_EYE_PLANE, column2);
	glEnable(GL_TEXTURE_GEN_T);

	column3[0] = texM[2];
	column3[1] = texM[6];
	column3[2] = texM[10];
	column3[3] = texM[14];
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_R, GL_EYE_PLANE, column3);
	glEnable(GL_TEXTURE_GEN_R);

	column4[0] = texM[3];
	column4[1] = texM[7];
	column4[2] = texM[11];
	column4[3] = texM[15];
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_Q, GL_EYE_PLANE, column4);
	glEnable(GL_TEXTURE_GEN_Q);
}

void setupCamera()
{
	glViewport(0, 0, gWinWidth, gWinHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)gWinWidth/gWinHeight, 1.0f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(gCameraMV);
}

void redraw(void)
{
	//创建深度图纹理对象
	createDepthTexture();
	
	//设置常规摄像机的视角
	setupCamera();
	
	//坐标变换
	coordTransforms();

	//设置光源
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT1, GL_SPECULAR, white);

	//开启深度图纹理
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gShadowTex);
	
	//启动与深度图的对比
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	//只有深度小于深度图中对应纹理的数值，会给alpha通道赋值为1；否则，赋值为0
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_ALPHA);

	//启动Alpha测试，大于1的像素会通过测试
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, 0.99f);
	
	drawScene(0);

	//Disable textures and texgen
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);

	//Restore other states
	glDisable(GL_LIGHTING);
	glDisable(GL_ALPHA_TEST);

	glFinish();
	glutSwapBuffers();
	glutPostRedisplay();

}


void reshape(GLsizei w,GLsizei h)
{
	gShadowWidth	=	gWinWidth	=	w;
	gShadowHeight	=	gWinHeight  =	h;

	glBindTexture(GL_TEXTURE_2D, gShadowTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, gShadowWidth, gShadowHeight, 0,GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
}

int main(int argc,char ** argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glewInit();
	glutInitWindowSize(400,400);

	glutCreateWindow("Shadow Mapping");

	assert(init());

	glutReshapeFunc(reshape);
	glutDisplayFunc(redraw);

	glutMainLoop();
	return(0);
}