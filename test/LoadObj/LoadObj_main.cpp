#include <iostream>
#include <math.h>
#include <stdlib.h>


#include	<GL/glew.h>
#include	<GL/freeglut.h>

#include	<oreore/common/Utility.h>
#include	<oreore/mathlib/GraphicsMath.h>

#include	<graphics/gl4x/scene/Camera.h>
#include	<graphics/gl4x/scene/MeshData.h>


#pragma comment (lib, "glew32.lib")


//using namespace OreOreLib;
using namespace std;


MeshData g_Mesh;

int numVertAttrs = 0;
VertexLayout *vertexlist = NULL;
int numIndices = 0;
int *Indices = 0;



#define	PI 3.141592f


Camera		*cam;


#define MAX_KEYS 256
bool	gKeys[MAX_KEYS];
int		mx = 0;
int		my = 0;
bool	LeftMouseButtonPressed = false;

float	gDeltaT = 1.0f;// / 60.0f;
float	CamRotSpeed = 0.1f;




void DRAW_XYZ()
{
	glBegin(GL_LINES);
		glColor3f(1,0,0);// x
		glVertex3f(0,0,0);
		glVertex3f(1,0,0);
		
		glColor3f(0,1,0);// y
		glVertex3f(0,0,0);
		glVertex3f(0,1,0);
		
		glColor3f(0,0,1);// z
		glVertex3f(0,0,0);
		glVertex3f(0,0,1);
	glEnd();
}


void ProcessCameraKeys()
{
	int	i;

	for(i=0; i<MAX_KEYS; i++)
	{
		if(gKeys[i]==false)	continue;
		
		switch(i)
		{
			case 'w' :{	cam->Transrate(+CamRotSpeed * gDeltaT, 0, 0);	break; }
			case 's' :{	cam->Transrate(-CamRotSpeed * gDeltaT, 0, 0);	break; }
			case 'a' :{	cam->Transrate(0, +CamRotSpeed * gDeltaT, 0);	break; }
			case 'd' :{	cam->Transrate(0, -CamRotSpeed * gDeltaT, 0);	break; }
			case 'r' :{	cam->Transrate(0, 0, +CamRotSpeed * gDeltaT);	break; }
			case 'f' :{	cam->Transrate(0, 0, -CamRotSpeed * gDeltaT);	break; }
			case 'q' :{	cam->Roll(-CamRotSpeed * gDeltaT);			break; }
			case 'e' :{	cam->Roll(+CamRotSpeed * gDeltaT);			break; }
			default : {	break; }
		}// end of switch
		
	}// end of i loop

}



void Initialize()
{
	glClearColor(0,0,0,1);
	glPointSize(10.0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	
	cam = new Camera(5,5,5, -1,-1,-1, 0,0,1);



	TCHAR currdir[ MAX_PATH ];
	GetCurrentDirectory( MAX_PATH, currdir );
	SetCurrentDirectory( _T( "../../../assets/scene/obj" ) );

	g_Mesh.Load( "testscene.obj" );

	SetCurrentDirectory( currdir );

	g_Mesh.Information();
	g_Mesh.GetGroupInfo(1);


	g_Mesh.GenVertexList(numVertAttrs, &vertexlist, numIndices, &Indices);

}

void display()
{
	
	ProcessCameraKeys();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//====================================== 座標軸とかレンダリング =======================================//
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	const Vec3f &pos = *cam->GetPosition();
	const Vec3f &dir = *cam->GetForward();
	const Vec3f &up	= *cam->GetVertical();

	gluLookAt( pos.x, pos.y, pos.z, pos.x + dir.x, pos.y + dir.y, pos.z + dir.z, up.x, up.y, up.z );
	
	DRAW_XYZ();// 座標軸の描画
	

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glColor3f( 0.6f, 0.6f, 0.6f );

	glEnableClientState(GL_VERTEX_ARRAY);
//	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//glTexCoordPointer(2, GL_FLOAT, 0, TexCoordArray);
	glVertexPointer( 3, GL_FLOAT, sizeof(VertexLayout), &(vertexlist->Position) );

	glDrawElements( GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, Indices );

	glDisableClientState(GL_VERTEX_ARRAY);
//	glDisableClientState(GL_TEXTURE_COORD_ARRAY);



	glutSwapBuffers();
}


// キーを押し込んだのを検出する
void KeyboardCallback(unsigned char key, int w, int h)
{
	gKeys[key] = true;
}


// キーを離したのを検出する
void KeyboardUpCallback(unsigned char key, int x, int y)
{
	gKeys[key] = false;

	switch (key)
	{
	case 27:
		{
			exit(0);
			break;
		}
		default : { break; }
	}
}



void reshape(int w, int h)
{
	if(w >=h) {glViewport(0, 0, (int)(h*4/3), h);
	printf("w =%d\n", (int)(h*4/3) );
	printf("h =%d\n", h);}
	else glViewport(0, 0, (int)(w*4/3), w);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1.25, 0.001, 10000);
	glMatrixMode(GL_MODELVIEW);
}

// マウス状態の監視
void MouseCallback(int button, int state, int x, int y)
{
	mx = x;
	my = y;
	
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)// 左ボタンを押し込んだ状態
	{
		LeftMouseButtonPressed = true;
		//cout << "Mouse Position = (" << x << ", " << y << ")" << endl;
	}
	if (state == GLUT_UP)
	{
		LeftMouseButtonPressed = false;
	}

	//cout << "Mouse LB state = " << LeftMouseButtonPressed << endl;
}


// マウス動作の監視 
void MotionCallback(int x, int y)
{
	float dx = float(mx - x) * 2.0f*PI / 800;
	float dy = float(my - y) * 2.0f*PI / 600;
	//cout << "dv = (" << dx << ", " << dy << ")" << endl;

	if(LeftMouseButtonPressed ==true)// 左ボタンをクリックしたときだけ回転
	{
		cam->Rotate(dx, dy);
	}

    mx = x;
    my = y;
}


void animate(void)
{
	glutPostRedisplay();
}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow(argv[0]);
	
	glewInit();
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(animate);

	glutKeyboardFunc(KeyboardCallback);
	glutKeyboardUpFunc(KeyboardUpCallback);
	glutMouseFunc(MouseCallback);
	glutMotionFunc(MotionCallback);
	MotionCallback(0,0);

	Initialize();
	glutMainLoop();

	SafeDelete( cam );

	return 0;
}
