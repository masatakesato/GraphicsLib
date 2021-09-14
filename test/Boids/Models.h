#include<GL/glut.h>
#include<stdio.h>


void Grid(){

glBegin(GL_LINES);
  
 glColor3d(0.05, 0.05, 0.05);//z direction
    glVertex3d(-45, 0, -50); glVertex3d(-45, 0, 50);
    glVertex3d(-35, 0, -50); glVertex3d(-35, 0, 50);
	glVertex3d(-25, 0, -50); glVertex3d(-25, 0, 50);
	glVertex3d(-15, 0, -50); glVertex3d(-15, 0, 50);
	glVertex3d(-5, 0, -50); glVertex3d(-5, 0, 50);
	glVertex3d(5, 0, -50); glVertex3d(5, 0, 50);
	glVertex3d(15, 0, -50); glVertex3d(15, 0, 50);
	glVertex3d(25, 0, -50); glVertex3d(25, 0, 50);
	glVertex3d(35, 0, -50); glVertex3d(35, 0, 50);
	glVertex3d(45, 0, -50); glVertex3d(45, 0, 50);

	glVertex3d(-50, 0, -45); glVertex3d(50, 0, -45);
    glVertex3d(-50, 0, -35); glVertex3d(50, 0, -35);
	glVertex3d(-50, 0, -25); glVertex3d(50, 0, -25);
	glVertex3d(-50, 0, -15); glVertex3d(50, 0, -15);
	glVertex3d(-50, 0, -5); glVertex3d(50, 0, -5);
	glVertex3d(-50, 0, 5); glVertex3d(50, 0, 5);
	glVertex3d(-50, 0, 15); glVertex3d(50, 0, 15);
	glVertex3d(-50, 0, 25); glVertex3d(50, 0, 25);
	glVertex3d(-50, 0, 35); glVertex3d(50, 0, 35);
	glVertex3d(-50, 0, 45); glVertex3d(50, 0, 45);

 glColor3d(0.2, 0.2, 0.2);
    glVertex3d(-50, 0, -50); glVertex3d(-50, 0, 50);
    glVertex3d(-40, 0, -50); glVertex3d(-40, 0, 50);
	glVertex3d(-30, 0, -50); glVertex3d(-30, 0, 50);
	glVertex3d(-20, 0, -50); glVertex3d(-20, 0, 50);
	glVertex3d(-10, 0, -50); glVertex3d(-10, 0, 50);	
	glVertex3d(10, 0, -50); glVertex3d(10, 0, 50);
	glVertex3d(20, 0, -50); glVertex3d(20, 0, 50);
	glVertex3d(30, 0, -50); glVertex3d(30, 0, 50);
	glVertex3d(40, 0, -50); glVertex3d(40, 0, 50);
    glVertex3d(50, 0, -50); glVertex3d(50, 0, 50);

	glVertex3d(-50, 0, -50); glVertex3d(50, 0, -50);
    glVertex3d(-50, 0, -40); glVertex3d(50, 0, -40);
	glVertex3d(-50, 0, -30); glVertex3d(50, 0, -30);
	glVertex3d(-50, 0, -20); glVertex3d(50, 0, -20);
	glVertex3d(-50, 0, -10); glVertex3d(50, 0, -10);
	glVertex3d(-50, 0, 10); glVertex3d(50, 0, 10);
	glVertex3d(-50, 0, 20); glVertex3d(50, 0, 20);
	glVertex3d(-50, 0, 30); glVertex3d(50, 0, 30);
	glVertex3d(-50, 0, 40); glVertex3d(50, 0, 40);
    glVertex3d(-50, 0, 50); glVertex3d(50, 0, 50);

 glColor3d(0.5, 0.5, 0.5);
    glVertex3d(0, 0, -50); glVertex3d(0, 0, 0);
    glVertex3d(-50, 0, 0); glVertex3d(0, 0, 0);

 glColor3d(1,0,0);
    glVertex3d(0, 0, 0); glVertex3d(50, 0, 0);//x axis
 glColor3d(0,1,0);
    glVertex3d(0, 0, 0); glVertex3d(0, 50, 0);//y axis
 glColor3d(0,0,1);
    glVertex3d(0, 0, 0); glVertex3d(0, 0, 50);//z axis

glEnd();
 
}//end of DrawShimen 