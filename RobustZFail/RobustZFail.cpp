/************************************************************************		
\link	www.twinklingstar.cn
\author Twinkling Star
\date	2014/03/17
****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include "SrVector3.h"


class Triangle  
{
public:
	Triangle()
	{
		index[0] = index[1] = index[2] = -1;
		lightFacing = false;
	}
	bool	lightFacing;
	int		index[3];
};

class Edge  
{
public:
	Edge()
	{
		vertexIndex[0] = vertexIndex[1] = -1;
		triangleIndex[0] = triangleIndex[1] = -1;
	}
	int vertexIndex[2];
	int triangleIndex[2];
};

class Model
{
public:
	Model()
	{
		triangle	= NULL;
		vertex		= NULL;
		edge		= NULL;
		numVertex	= 0;
		numTriangle = 0;
	}
	~Model()
	{
		if( triangle )
		{
			delete []triangle;
			triangle = NULL;
		}
		if( vertex )
		{
			delete []vertex;
			vertex = NULL;
		}
		if( edge )
		{
			delete []edge;
			edge = NULL;
		}
	}
	Triangle*	triangle;			//The triangles of the model.
	SrVector3*	vertex;
	Edge*		edge;
	int			numVertex;
	int			numTriangle;


private:
	Model(const Model&);
	const Model& operator = (const Model&);
};

class BasicModel
{
public:
	Model		model;
	SrVector3	color;
	SrVector3	position;
	SrVector3	rotate;
	SrVector3	objectSpaceLightPosition;
};

class Light
{
public:
	SrVector3 position;
	SrVector3 color;
};


float		gYAxisAngle				= 0;
float		gXAxisAngle				= 0;
int			gLastMouseX			= 0;
int			gLastMouseY			= 0;
float		gZoom				= 0;
float		gHorizon			= 0;
bool		gMoveScene			= false;
bool		gStopTimer			= false;
bool		gDisplaySide		= false;
bool		gIsZFail			= true;

BasicModel* gModel				= NULL;

SrVector3	gLightPosition		= SrVector3(1.0f,1.0f,-4.0f);
SrVector3	gRotate				= SrVector3(0,0,0);
SrVector3	gBlack				= SrVector3(0,0,0);
SrVector3	gWhite				= SrVector3(1.0f,1.0f,1.0f);

void outputInfo()
{
	printf("***********************************************************\n");
	printf("Up:		Zoom in\n");
	printf("Down:		Zoom out\n");
	printf("Left:		Move left\n");
	printf("Right:		Move Right\n");
	printf("S:		Stop or start rotation\n");
	printf("F:		Display the side of the shadow volume\n");
	printf("C:		Shift Between the Z-Fail and the Z-Pass.\n");
	printf("Default algorithm: Z-Fail\n");
	printf("***********************************************************\n");
}

void setEdges(BasicModel& BasicModel)
{
	Model* model = &BasicModel.model;
	int numEdge = model->numTriangle * 3 / 2;
	model->edge = new Edge[ numEdge ];
	int indxEdge = 0;
	int tri1 , tri2;
	int indx1 , indx2;

	for( tri1=0 ; tri1<model->numTriangle ; tri1++ )
	{
		for( indx1 = 0 ; indx1<3 ; indx1++ )
		{
			int ep1 = model->triangle[tri1].index[indx1];
			int ep2 = model->triangle[tri1].index[(indx1+1)%3];
			for( tri2=tri1+1 ; tri2<model->numTriangle ; tri2++ )
			{
				for( indx2 = 0 ; indx2<3 ; indx2++ )
				{
					int ep3 = model->triangle[tri2].index[indx2];
					int ep4 = model->triangle[tri2].index[(indx2+1)%3];
					if( ep1==ep4 && ep2==ep3 )
					{
						model->edge[ indxEdge ].triangleIndex[0] = tri1;
						model->edge[ indxEdge ].triangleIndex[1] = tri2;
						model->edge[ indxEdge ].vertexIndex[0] = ep1;
						model->edge[ indxEdge ].vertexIndex[1] = ep2;
						indxEdge ++;
					}
				}
			}
		}
	}
}

void buildTetrahedron(BasicModel*& basicModel,
					  const SrVector3& position = SrVector3(0.0f,0.0f,0.0f),
					  const SrVector3& rotate = SrVector3(0.0f,0.0f,0.0f),
					  const SrVector3& color = SrVector3(1.0f,1.0f,1.0f))
{
	basicModel = new BasicModel;
	basicModel->color = color;
	basicModel->position = position;
	basicModel->rotate = rotate;

	Model* model = &basicModel->model;

	model->numVertex = 4;
	model->vertex = new SrVector3[ model->numVertex ];

	model->numTriangle = 4;
	model->triangle = new Triangle[ model->numTriangle ];

	model->vertex[0].set(0,0,0);
	model->vertex[1].set(1,0,0);
	model->vertex[2].set(0,1,0);
	model->vertex[3].set(0,0,1);

	model->triangle[0].index[0] = 0;
	model->triangle[0].index[1] = 1;
	model->triangle[0].index[2] = 3;

	model->triangle[1].index[0] = 0;
	model->triangle[1].index[1] = 2;
	model->triangle[1].index[2] = 1;

	model->triangle[2].index[0] = 2;
	model->triangle[2].index[1] = 0;
	model->triangle[2].index[2] = 3;

	model->triangle[3].index[0] = 1;
	model->triangle[3].index[1] = 2;
	model->triangle[3].index[2] = 3;

	setEdges(*basicModel);
}


void computeLightFace(const BasicModel& basicModel)
{
	int i;
	for( i=0 ; i<basicModel.model.numTriangle ; i++ )
	{
		SrVector3 v0 , v1 , v2;
		v0 = basicModel.model.vertex[basicModel.model.triangle[i].index[0]];
		v1 = basicModel.model.vertex[basicModel.model.triangle[i].index[1]];
		v2 = basicModel.model.vertex[basicModel.model.triangle[i].index[2]];
		SrVector3 normal = (v1 - v0).cross(v2 - v0);
		if( normal.dot(basicModel.objectSpaceLightPosition) - normal.dot(v0) <0 )	
		{// Back Face
			basicModel.model.triangle[i].lightFacing = false;
		}
		else	
		{//Front Face
			basicModel.model.triangle[i].lightFacing = true;
		}
	}
}

void drawBasicModel(const BasicModel& basicModel)
{
	glColor3f(basicModel.color.x,basicModel.color.y,basicModel.color.z);

	glPushMatrix();
		glTranslatef(basicModel.position.x,basicModel.position.y,basicModel.position.z);
		glRotatef(basicModel.rotate.x,1.0f,0,0);
		glRotatef(basicModel.rotate.y,0,1.0f,0);
		glRotatef(basicModel.rotate.z,0,0,1.0f);
		int i;
		SrVector3 normal;
		for( i=0 ; i<basicModel.model.numTriangle ; i++ )
		{
			SrVector3 v0 , v1 , v2;
			v0 = basicModel.model.vertex[basicModel.model.triangle[i].index[0]];
			v1 = basicModel.model.vertex[basicModel.model.triangle[i].index[1]];
			v2 = basicModel.model.vertex[basicModel.model.triangle[i].index[2]];
			normal = (v1 - v0).cross(v2 - v0);
			glBegin(GL_TRIANGLES);
				glNormal3f(normal.x,normal.y,normal.z);
				glVertex3f(v0.x,v0.y,v0.z);
				glVertex3f(v1.x,v1.y,v1.z);
				glVertex3f(v2.x,v2.y,v2.z);
			glEnd();
		}
	glPopMatrix();
}


void drawBox(const SrVector3& minBox, const SrVector3& maxBox,const SrVector3& color = SrVector3(0,1.0f,0))
{
	glColor3f(color.x,color.y,color.z);
	glPushMatrix();
		glBegin(GL_QUADS);
		//back face
		glNormal3f(0.0f,0.0f,1.0f);
		glVertex3f(minBox.x,minBox.y,minBox.z);
		glVertex3f(maxBox.x,minBox.y,minBox.z);
		glVertex3f(maxBox.x,maxBox.y,minBox.z);
		glVertex3f(minBox.x,maxBox.y,minBox.z);

		//front face
		glNormal3f(0.0f,0.0f,-1.0f);
		glVertex3f(minBox.x,minBox.y,maxBox.z);
		glVertex3f(minBox.x,maxBox.y,maxBox.z);
		glVertex3f(maxBox.x,maxBox.y,maxBox.z);
		glVertex3f(maxBox.x,minBox.y,maxBox.z);

		//left face
		glNormal3f(1.0f,0.0f,0.0f);
		glVertex3f(minBox.x,minBox.y,minBox.z);
		glVertex3f(minBox.x,maxBox.y,minBox.z);
		glVertex3f(minBox.x,maxBox.y,maxBox.z);
		glVertex3f(minBox.x,minBox.y,maxBox.z);

		//right face
		glNormal3f(-1.0f,0.0f,0.0f);
		glVertex3f(maxBox.x,minBox.y,minBox.z);
		glVertex3f(maxBox.x,minBox.y,maxBox.z);
		glVertex3f(maxBox.x,maxBox.y,maxBox.z);
		glVertex3f(maxBox.x,maxBox.y,minBox.z);

		//top face
		glNormal3f(0.0f,-1.0f,0.0f);
		glVertex3f(minBox.x,maxBox.y,minBox.z);
		glVertex3f(maxBox.x,maxBox.y,minBox.z);
		glVertex3f(maxBox.x,maxBox.y,maxBox.z);
		glVertex3f(minBox.x,maxBox.y,maxBox.z);

		//bottom face
		glNormal3f(0.0f,1.0f,0.0f);
		glVertex3f(minBox.x,minBox.y,minBox.z);
		glVertex3f(minBox.x,minBox.y,maxBox.z);
		glVertex3f(maxBox.x,minBox.y,maxBox.z);
		glVertex3f(maxBox.x,minBox.y,minBox.z);

		glEnd();
	glPopMatrix();
}


void timerMessage(int id)
{
	gRotate.y += 10;
	gRotate.x += 10;
	glutPostRedisplay();
	if( !gStopTimer )
		glutTimerFunc(200,timerMessage,0);
}

void setNormalLight( const SrVector3& lightPosition = SrVector3(0,0,0))
{
	SrVector3 lightColor = SrVector3(0.5,0.5f,0.5f);
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition.get()); 
	glLightfv(GL_LIGHT1, GL_AMBIENT, (lightColor/5.0f).get());
	glLightfv(GL_LIGHT1, GL_DIFFUSE, (lightColor).get());
	glLightfv(GL_LIGHT1, GL_SPECULAR, gWhite.get());
}


void setAmbient( )
{	
	SrVector3 lightColor = SrVector3(0.2,0.2f,0.2f);
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT1, GL_AMBIENT, (lightColor/2.0f).get());
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor.get());
	glLightfv(GL_LIGHT1, GL_SPECULAR, gBlack.get());
}

void getInverseMatrix(const SrVector3& shift, 
					  const SrVector3& rotate,
					  float* result)
{
	glPushMatrix();
		glLoadIdentity();
		glRotatef(-rotate.z,0,0,1.0f);
		glRotatef(-rotate.y,0,1.0f,0);
		glRotatef(-rotate.x,1.0f,0,0);
		glTranslatef(-shift.x,-shift.y,-shift.z);
		glGetFloatv(GL_MODELVIEW_MATRIX,result);
	glPopMatrix();
}

void getMatrixMultiVector3(float* matrix,const SrVector3& p, SrVector3& result)
{
	result.x = p.x*matrix[0] + p.y*matrix[4] + p.z*matrix[8] + matrix[12];
	result.y = p.x*matrix[1] + p.y*matrix[5] + p.z*matrix[9] + matrix[13];
	result.z = p.x*matrix[2] + p.y*matrix[6] + p.z*matrix[10] + matrix[14];
}

void drawLight()
{
	glColor3f(1.0f,1.0f,1.0f);
	glPushMatrix();
		glTranslatef(gLightPosition.x,gLightPosition.y,gLightPosition.z);
		glutSolidSphere(0.05,32,32);
	glPopMatrix();
}

void drawSilhouetteEdge(const BasicModel& basicModel)
{
	glPushMatrix();
		float color[3] = {1.0f,0,0};
		glTranslatef(basicModel.position.x,basicModel.position.y,basicModel.position.z);
		glRotatef(basicModel.rotate.x,1.0f,0,0);
		glRotatef(basicModel.rotate.y,0,1.0f,0);
		glRotatef(basicModel.rotate.z,0,0,1.0f);
		int numEdge = 0;
		glBegin(GL_QUADS);
			int i;
			for( i=0 ; i<basicModel.model.numTriangle*3/2 ; i++ )
			{
				bool tr1LightFacing = basicModel.model.triangle[basicModel.model.edge[i].triangleIndex[0]].lightFacing;
				bool tr2LightFacing = basicModel.model.triangle[basicModel.model.edge[i].triangleIndex[1]].lightFacing;
				if( tr1LightFacing!=tr2LightFacing )
				{
					SrVector3 v0, v1;
					if( !tr1LightFacing )
					{
						v0 = basicModel.model.vertex[basicModel.model.edge[i].vertexIndex[0]];
						v1 = basicModel.model.vertex[basicModel.model.edge[i].vertexIndex[1]];
					}
					else
					{
						v1 = basicModel.model.vertex[basicModel.model.edge[i].vertexIndex[0]];
						v0 = basicModel.model.vertex[basicModel.model.edge[i].vertexIndex[1]];

					}
					numEdge ++;
					if( numEdge%3==0 )
					{
						color[1] = 1.0f;
					}
					else
					{
						color[2] += 0.2f;
					}
					glColor3f(color[0] , color[1],color[2]);
					glVertex3f(v0.x,v0.y,v0.z);
					glVertex3f(v1.x,v1.y,v1.z);
					glVertex4f(v1.x - basicModel.objectSpaceLightPosition.x, 
								v1.y - basicModel.objectSpaceLightPosition.y, 
								v1.z - basicModel.objectSpaceLightPosition.z, 
								0.0f );
					glVertex4f(v0.x - basicModel.objectSpaceLightPosition.x, 
								v0.y - basicModel.objectSpaceLightPosition.y, 
								v0.z - basicModel.objectSpaceLightPosition.z, 
								0.0f );
				}
			}
		glEnd();

	glPopMatrix();

}

void drawCap(const BasicModel& basicModel)
{
	int i , j;
	glPushMatrix();
		glTranslatef(basicModel.position.x,basicModel.position.y,basicModel.position.z);
		glRotatef(basicModel.rotate.x,1.0f,0,0);
		glRotatef(basicModel.rotate.y,0,1.0f,0);
		glRotatef(basicModel.rotate.z,0,0,1.0f);
		glBegin(GL_TRIANGLES);
			for( i=0 ; i<basicModel.model.numTriangle ; i++ )
			{
				if( !basicModel.model.triangle[i].lightFacing )
				{
					if(gIsZFail)
					{
						for( j=0 ; j<3 ; j++ )
						{
							glVertex4f( basicModel.model.vertex[basicModel.model.triangle[i].index[j]].x - basicModel.objectSpaceLightPosition.x,
								basicModel.model.vertex[basicModel.model.triangle[i].index[j]].y - basicModel.objectSpaceLightPosition.y,
								basicModel.model.vertex[basicModel.model.triangle[i].index[j]].z - basicModel.objectSpaceLightPosition.z,
								0.0f);
						}
					}
				}
				else
				{
					for( j=0 ; j<3 ; j++ )
					{
						glVertex3f( basicModel.model.vertex[basicModel.model.triangle[i].index[j]].x,
									basicModel.model.vertex[basicModel.model.triangle[i].index[j]].y,
									basicModel.model.vertex[basicModel.model.triangle[i].index[j]].z);
					}
				}
			}
		glEnd();
	glPopMatrix();
}



void renderMessage( )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	SrVector3 minBox = SrVector3(-2.0f,-2.0f,-10.0f);
	SrVector3 maxBox = SrVector3( 2.0f, 2.0f,10.0f);
	glTranslatef(gHorizon,0,gZoom);
	glTranslatef(0,0,-4.0f);
	glRotatef(gYAxisAngle,0,1.0f,0);
	glRotatef(gXAxisAngle,1.0f,0,0);
	glTranslatef(0,0,4.0f);

	/*drawBox(minBox,maxBox,SrVector3(0.2f, 0.2f, 1.0f));*/

	gModel->rotate = gRotate;
	/*
		Render the scene with depth testing, back-face culling, and
		all light sources disabled (ambient & emissive illumination only)
	*/
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	setAmbient();

	drawBox(minBox,maxBox,SrVector3(0.2f, 0.2f, 1.0f));
	drawBasicModel(*gModel);
	/*
		Disable depth writes, enable additive blending, and set the 
		global ambient light contribution to zero (and zero any 
		emissive contribution if present)
	*/
	glDepthMask(0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ONE);

	/*
		For each light source:
	*/
	glStencilFunc(GL_ALWAYS,0,0);
	glStencilMask(0xff);
	/*
		Disable color buffer writes and enable stencil testing with
		the always stencil function and writing stencil.
	*/
	glColorMask(false,false,false,false);
	/*
		Clear the stencil buffer to zero.
	*/
	glEnable(GL_STENCIL_TEST);
	glClearStencil(0);
	glClear(GL_STENCIL_BUFFER_BIT);
	/*
		For each occluder:
	*/
	/*
		Determine whether each triangle in the occluder's model is 
		front- or back-facing with respect to the light's position.
	*/
	float invMatrix[16];
	getInverseMatrix(gModel->position,gModel->rotate,invMatrix);
	getMatrixMultiVector3(invMatrix,gLightPosition,gModel->objectSpaceLightPosition);
	computeLightFace(*gModel);

	int pass = 0;
	for( pass = 0 ; pass<2 ; pass ++ )
	{
		if( pass==0 )
		{
			if( gIsZFail )
			{/*
					Configure zfail stencil testing to increment stencil for
					back-facing polygons that fail the depth test.
				*/
				glCullFace(GL_FRONT);
				glStencilOp(GL_KEEP,GL_INCR,GL_KEEP);
			}
			else
			{
				/*
					Configure zpass stencil testing to increment stencil for
					front-facing polygons that succeed the depth test.
				*/
				glCullFace(GL_BACK);
				glStencilOp(GL_KEEP,GL_KEEP,GL_INCR);
			}
		}
		else
		{
			if( gIsZFail )
			{
				/*
					Configure zfail stencil testing to decrement stencil for
					front-facing polygons that fail the depth test.
				*/
				glCullFace(GL_BACK);
				glStencilOp(GL_KEEP,GL_DECR,GL_KEEP);			
			}
			else
			{
				/*
					Configure zpass stencil testing to decrement stencil for
					back-facing polygons that succeed the depth test.
				*/
				glCullFace(GL_FRONT);
				glStencilOp(GL_KEEP,GL_KEEP,GL_DECR);
			}
		}
		/*
			Render all possible silhouette edges as quads that 
			project from the edge away from the light to infinity.
		*/
		drawSilhouetteEdge(*gModel);
		/*
			Specially render all the occluder triangles.Project and 
			render back facing triangles away from the light to infinity. 
			Render front-facing triangles directly.
		*/
		drawCap(*gModel);
	}
	/*
		Position and enable the current light (and otherwise configure 
		the light's attenuation, color, etc.)
	*/
	setNormalLight(gLightPosition);
	/*
		Set stencil testing to render only pixels with a zero stencil value, 
		i.e., visible fragments illuminated by the current light. Use equal
		depth testing to update only the visible fragments, and then, increment
		stencil stencil to avoid double blending. Re-enable color buffer writes again.
	*/
	glStencilFunc(GL_EQUAL,0,0xff);
	glStencilOp(GL_KEEP,GL_KEEP,GL_INCR);
	glDepthFunc(GL_LEQUAL);
	glColorMask(1.0f,1.0f,1.0f,1.0f);
	/*
		Re-draw the scene to add the contribution of the current light to illuminated
		(non-shadowed) regions of the scene.
	*/

	glCullFace(GL_BACK);
	drawBasicModel(*gModel);
	drawBox(minBox,maxBox,SrVector3(0.2f, 0.2f, 1.0f));
	/*
		Disable blending and stencil testing; re-enable depth writes. Restore the depth test.
	*/
	glDisable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_STENCIL_TEST);
	glDepthMask(1);

	drawLight();


	if( gDisplaySide )
	{
		drawSilhouetteEdge(*gModel);
	}

	glFlush();
}




void init()
{
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	float MatAmb[] = {0.4f, 0.4f, 0.4f, 1.0f};				// Material - Ambient Values
	float MatDif[] = {0.2f, 0.6f, 0.9f, 1.0f};				// Material - Diffuse Values
	float MatSpc[] = {0.0f, 0.0f, 0.0f, 1.0f};				// Material - Specular Values
	float MatShn[] = {1.0f};
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_AMBIENT, MatAmb);			// Set Material Ambience
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MatDif);			// Set Material Diffuse
	glMaterialfv(GL_FRONT, GL_SPECULAR, MatSpc);		// Set Material Specular
	glMaterialfv(GL_FRONT, GL_SHININESS, MatShn);		// Set Material Shininess
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);

	if(!gModel )
	{
		buildTetrahedron(gModel,SrVector3(0.0f,0.0f,-6.0f),SrVector3(0,0,0),SrVector3(1.0f,0.0f,0.0f));
	}
}

void reshapeMessage(GLsizei w,GLsizei h)
{
	glViewport(0,0,w,h);
	float nearPlane = 1.0f;
	float farPlane	= 10000.0f;
	float fovy = 45;
	float top = nearPlane*tan(3.1415926*fovy/360.0f);
	float bott = -top;
	float right = top*(float)w/(float)h;
	float left = - right;
	if(gIsZFail)
	{
		float pinf[4][4];
		pinf[0][1] = pinf[0][2] = pinf[0][3] = pinf[1][0] =
			pinf[1][2] = pinf[1][3] = pinf[3][0] = pinf[3][1] =
			pinf[3][3] = 0;
		pinf[0][0] = 2*nearPlane / (right - left);
		pinf[1][1] = 2*nearPlane / (top - bott);
		pinf[2][0] = ( right + left ) / (right - left);
		pinf[2][1] = ( top + bott) / (top - bott);
		pinf[2][2] = pinf[2][3] = -1;
		pinf[3][2] = -2*nearPlane;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glLoadMatrixf(&pinf[0][0]);
		glMatrixMode(GL_MODELVIEW);
	}
	else
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(left, right, bott, top, nearPlane, farPlane);
		glMatrixMode(GL_MODELVIEW);
	}

}



void mouseMotionMessage(int x, int y)
{

	if( gMoveScene )
	{
		float lenX = x - gLastMouseX;
		float lenY = y - gLastMouseY;

		gLastMouseX = x;
		gLastMouseY = y;
		gYAxisAngle += lenX*0.05f;
		gXAxisAngle += lenY*0.05f;

		glutPostRedisplay();

	}
}

void mouseClickMessage(int button, int state, int x, int y)
{
	if( state == GLUT_DOWN )
	{
		gLastMouseX = x;
		gLastMouseY = y;
		gMoveScene	= true;
	}
	else if( state == GLUT_UP )
	{
		gMoveScene = false;
	}
}

void specialKeyMessage(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_UP:
		gZoom += 0.1f;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		gZoom -= 0.1f;
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT:
		gHorizon -= 0.1f;
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		gHorizon += 0.1f;
		glutPostRedisplay();
		break;
	}
}

void keyMessage(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 's':
	case 'S':
		gStopTimer = !gStopTimer;
		if( !gStopTimer )
		{
			glutTimerFunc(200,timerMessage,0);
		}
		break;
	case 'f':
	case 'F':
		gDisplaySide = !gDisplaySide;
		glutPostRedisplay();
		break;
	case 'c':
	case 'C':
		gIsZFail = !gIsZFail;
		if( gIsZFail )
			printf("Current algorithm: Z-Fail\n");
		else
			printf("Current algorithm: Z-Pass\n");
		glutPostRedisplay();
		break;

	}
}

int main(int argc,char ** argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
	glutInitWindowSize(400,400);

	glutCreateWindow("Robust-Z-Fail");

	init();
	outputInfo();
	glutReshapeFunc(reshapeMessage);
	glutDisplayFunc(renderMessage);
	glutTimerFunc(200,timerMessage,0);
	glutMouseFunc(mouseClickMessage);
	glutMotionFunc(mouseMotionMessage);
	glutSpecialFunc(specialKeyMessage);
	glutKeyboardFunc(keyMessage);

	glutMainLoop();
	return(0);
}