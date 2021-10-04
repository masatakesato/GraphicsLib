#include <iostream>


#include	<oreore/mathlib/GraphicsMath.h>

#include "Models.h"
#include "RegularGrid.h"





#include <math.h>


#include <GL/freeglut.h>



float phi=90.f, theta=60.f, dist=90.f;


Vec3d bb[2];
RegularGrid rg;



void initialize(void)
{
	glClearColor(0, 0, 0, 1);
	glPointSize(10.0);
	glLineWidth(2.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//glEnable(GL_BLEND);
	
	InitVec( bb[0], -9.0, -9.0, -9.0 );
	InitVec( bb[1], 9.0, 9.0, 9.0 );
	
	RegularGrid_Init(&rg, bb, 0.5);
		

	Vec3d pos;
	InitVec( pos, 0.0, 2.0, 6.0 );

	int o;

	for(o=0; o<10; o++)
	RegularGrid_InsertID(&rg, 3, pos);


	RegularGrid_Info(&rg);
	
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(dist*cos(M_PI*(phi/180.0))*cos(M_PI*(theta/180.0)),dist*sin(M_PI*(theta/180.0)),dist*sin(M_PI*(phi/180.0))*cos(M_PI*(theta/180.0)),0,0,0,0,1,0);

	
	int i,j,k;

	for(i=0; i<rg.Size[0]; i++)
		for(j=0; j<rg.Size[1]; j++)
			for(k=0; k<rg.Size[2]; k++)
			{
				if(rg.Elms[rg.Size[1]*rg.Size[2]*i + rg.Size[2]*j + k] >0)
				{
					RegularGrid_RenderGL(&rg, i,j,k);
					std::cout << rg.Elms[rg.Size[1]*rg.Size[2]*i + rg.Size[2]*j + k] << endl;
				}
				
			}

	Grid();
	
	glutSwapBuffers();
	
}

void keyboard(unsigned char c, int w, int h)
{

	switch(c)
	{
	case 27: exit(0);
	case '6': phi-=2;   break;
	case '4': phi+=2;   break;
	case '2': theta-=2; break;
	case '8': theta+=2; break;
	case '+': dist-=1;  break;
	case '-': dist+=1;  break;
	default:			break;
	}

	if(phi>=360 || phi<=-360) phi=0;

}

void reshape(int w, int h)
{
	if(w >=h) {glViewport(0, 0, (int)(h*4/3), h);
	printf("w =%d\n", (int)(h*4/3) );
	printf("h =%d\n", h);}
	else glViewport(0, 0, (int)(w*4/3), w);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1.25, 1, 1000);
	glMatrixMode(GL_MODELVIEW);

}

void animate(void){ glutPostRedisplay(); }

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow(argv[0]);

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutIdleFunc(animate);
	
	initialize();
	glutMainLoop();

	return 0;
}







/*
#include <cg/cg.h>
#include <cg/cgGL.h>

#include <iostream>

void init()
{
// コンテキストの作成
CGcontext context = cgCreateContext();

// プロファイルの作成
CGprofile profile = cgGLGetLatestProfile(CG_GL_VERTEX);

//++++++++++++++++++ プログラムの作成 +++++++++++++++++++//

const char* filename = "test.cg";// プログラムを記述したファイル名
const char* funcname = "foo";// ファイル内関数名




CGprogram program = cgCreateProgramFromFile(context, CG_SOURCE, filename, profile, funcname, NULL);

// プログラムのロード
cgGLLoadProgram(program);

// プログラムのバインド
cgGLBindProgram(program);

// プロファイルの有効化
cgGLEnableProfile(profile);

// レンダリングを行う
//doSomeRendering();


// プロファイルの無効化
cgGLDisableProfile(profile);

// プログラムの破棄
cgDestroyProgram(program);

// コンテキストの破棄
cgDestroyContext(context);
}

int main(int argc, char **argv)
{



}
*/