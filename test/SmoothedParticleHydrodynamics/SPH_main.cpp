#include <iostream>
#include <GL/freeglut.h>
#include <math.h>
#include <malloc.h>

#include "Models.h"

#include "SPHSolver.h"



float phi=80,theta=20,dist=110;
static bool swtch = false;

SPH *sph;


//-8.67334 : -10.6333
//-8.17334 : -10.1333
//-9.63334 : -11.5933

void initialize(void)
{
	sph = (SPH *) malloc (sizeof(SPH));
	SPH_Init(sph);

	//NeighborList_GetNeighbor(&mps->NList, mps->Particles, 0, mps->pNum-1, &mps->Grid, 0.5);

	//NeighborList_Info(&mps->NList, mps->pNum);

	glClearColor(1, 1, 1, 1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glEnable(GL_DEPTH_TEST);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(dist*cos(M_PI*(phi/180.0))*cos(M_PI*(theta/180.0)),dist*sin(M_PI*(theta/180.0)),dist*sin(M_PI*(phi/180.0))*cos(M_PI*(theta/180.0)),0,0,0,0,1,0);
	
	if(swtch == true)
		SPH_Run(sph);

	glScaled(100,100,100);

	SPH_RenderBoundary(sph);
	SPH_RenderParticles(sph);

	//RegularGrid_RenderGL(&sph->Grid, 0,0,0);

	glScaled(1,1,1);
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
	case 'p': 
		if(swtch==true){
			swtch=false;
		}else{
			swtch=true;
		}
						break;

	default:			break;
	}
	if(phi>=360 || phi<=-360) phi=0;
}

/*
void mouse(int button, int state, int x, int y)
{
　　switch (button) {
　　　　case GLUT_LEFT_BUTTON:
　　　　　　pos_x += 0.1;
　　　　　　glutPostRedisplay();
　　　　　　break;
　　　　case GLUT_RIGHT_BUTTON:
　　　　　　pos_x -= 0.1;
　　　　　　glutPostRedisplay();
　　　　　　break;
　　　　default:
　　　　　　break;
　　}
}
*/




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