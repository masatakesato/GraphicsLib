
#include<math.h>
#include<stdio.h>
#include<time.h>
#include"valiables.h"
#include"Models.h"
//#include"getclock.h"
#include<GL/freeglut.h>


void initialize(void){
 glClearColor(0, 0, 0, 1);
 glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
 glEnable(GL_DEPTH_TEST);

}

void display(void){

 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
 if(AutoCamera==1)
 gluLookAt(scale*cos(PI*(a/180.0))*cos(PI*(b/180.0)), scale*sin(PI*(b/180.0)), scale*sin(PI*(a/180.0))*cos(PI*(b/180.0)), 
	                 (fish+i)->x, (fish+i)->y, (fish+i)->z, 0, 1, 0);
 else
 gluLookAt(scale*cos(PI*(a/180.0))*cos(PI*(b/180.0)), scale*sin(PI*(b/180.0)), scale*sin(PI*(a/180.0))*cos(PI*(b/180.0)), 
	                 0, 0, 0, 0, 1, 0);
/*******************アニメーションさせるオブジェクトをここに書く********************/ 
if(pause==false){

MoveDestination(fd,NumFD,interval);

InitForce(f,Num);
InGroundEffect(f,Num,Height_Limit,InGroundK);
ForceToDest(f,Num,fd,NumFD);
EvasiveAction(f,Num,Distance_Limit,FishK);
EvadeObject(f,Num,o,NumObj);
RefreshPosition(f,Num,interval);

}
/**************************** Render Object ************************************/
DrawFish(f,Num,i,AutoCamera);
DrawObject(o,NumObj);
Grid();
/***********************************************************************************/ 

if(AutoCamera==1)  a+=0.1;
if(a>=360)a=0;

glutSwapBuffers();
}



void keyboard(unsigned char c, int w, int h){

 switch(c){	
  case 27: exit(0);
/********* view control **********/ 
  case '4': a+=2; break;
  case '6': a-=2; break;
  case '8': b+=2; break;
  case '2': b-=2; break;
  case '+': scale-=1.0; break;
  case '-': scale+=1.0; break;
  case ',': i-=1; if(i<0)i=0; break;
  case '.': i+=1; if(i>99)i=99; break;
  case 'm': if(AutoCamera==false) AutoCamera=true; 
	        else AutoCamera=false;
			break;
/********** stop/start ***********/ 
  case 'p': if(pause==false){ pause=true; AutoCamera=false;}
	  else pause=false; 
			break;
/********* Interval change ********/
  case 'a': interval+=0.01;
	          printf("interval=%lf\n", interval);
			  break;
  case 'z': if(interval<=0) interval=0;
	        else interval-=0.01;
			printf("interval=%lf\n", interval);
			break;

  default: ;	
 }


}

void reshape(int w, int h){

    if(w >=h) {glViewport(0, 0, (int)(h*4/3), h);}
	//printf("w =%d\n", (int)(h*4/3) );
	//printf("h =%d\n", h);}
	else glViewport(0, 0, (int)(w*4/3), w);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1.25, 1, 1000);
    glMatrixMode(GL_MODELVIEW);
}

void animate(void){
  glutPostRedisplay();
}

int main(int argc, char *argv[]){
  
  InitFishPosition(f,Num,NumFD);//define the first position randomly
  interval=1.0e-1;//36.0/(GetCPUClock()/1000000);
  
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
