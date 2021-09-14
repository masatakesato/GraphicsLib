#include<GL/glut.h>
#include<stdio.h>

void DrawShimen(double x, double y, double z, double scalex, double scaley, double scalez,
				double r,double g, double b){

   double Vertex[4][3] ={
	{0, 0.816496580927726032732428024901964, 0},//v0
	{-0.5, 0, 0.288675134594812882254574390250979},//v1
	{0.5, 0, 0.288675134594812882254574390250979},//v2
	{0, 0, -0.577350269189625764509148780501957}//v3
	};

glColor3d(r,g,b);
glBegin(GL_TRIANGLES);
  glVertex3d(Vertex[0][0]*scalex+x, Vertex[0][1]*scaley+y, Vertex[0][2]*scalez+z);
  glVertex3d(Vertex[1][0]*scalex+x, Vertex[1][1]*scaley+y, Vertex[1][2]*scalez+z);
  glVertex3d(Vertex[2][0]*scalex+x, Vertex[2][1]*scaley+y, Vertex[2][2]*scalez+z);
  
  glVertex3d(Vertex[0][0]*scalex+x, Vertex[0][1]*scaley+y, Vertex[0][2]*scalez+z);
  glVertex3d(Vertex[2][0]*scalex+x, Vertex[2][1]*scaley+y, Vertex[2][2]*scalez+z);
  glVertex3d(Vertex[3][0]*scalex+x, Vertex[3][1]*scaley+y, Vertex[3][2]*scalez+z);

  glVertex3d(Vertex[0][0]*scalex+x, Vertex[0][1]*scaley+y, Vertex[0][2]*scalez+z);
  glVertex3d(Vertex[3][0]*scalex+x, Vertex[3][1]*scaley+y, Vertex[3][2]*scalez+z);
  glVertex3d(Vertex[1][0]*scalex+x, Vertex[1][1]*scaley+y, Vertex[1][2]*scalez+z);

  glVertex3d(Vertex[1][0]*scalex+x, Vertex[1][1]*scaley+y, Vertex[1][2]*scalez+z);
  glVertex3d(Vertex[2][0]*scalex+x, Vertex[2][1]*scaley+y, Vertex[2][2]*scalez+z);
  glVertex3d(Vertex[3][0]*scalex+x, Vertex[3][1]*scaley+y, Vertex[3][2]*scalez+z);
glEnd();
}//end of DrawShimen 

void DrawColumn(double x, double y, double z, double scalex, double scaley, double scalez,
	double r,double g, double b){

glPushMatrix();
glTranslated(x,y,z);
glScaled(scalex, scaley, scalez);
glColor3d(r,g,b);
 glBegin(GL_QUAD_STRIP);
   glVertex3d(1,0,0);            glVertex3d(1,1,0);
   glVertex3d(0.866025,0,0.5);   glVertex3d(0.866025,1,0.5);  
   glVertex3d(0.5,0,0.866025);   glVertex3d(0.5,1,0.866025);

   glVertex3d(0,0,1);            glVertex3d(0,1,1); 
   glVertex3d(-0.5,0,0.866025);  glVertex3d(-0.5,1,0.866025);
   glVertex3d(-0.866025,0,0.5);  glVertex3d(-0.866025,1,0.5);

   glVertex3d(-1,0,0);           glVertex3d(-1,1,0);
   glVertex3d(-0.866025,0,-0.5); glVertex3d(-0.866025,1,-0.5);  
   glVertex3d(-0.5,0,-0.866025); glVertex3d(-0.5,1,-0.866025);

   glVertex3d(0,0,-1);           glVertex3d(0,1,-1);
   glVertex3d(0.5,0,-0.866025);  glVertex3d(0.5,1,-0.866025);
   glVertex3d(0.866025,0,-0.5);  glVertex3d(0.866025,1,-0.5);

   glVertex3d(1,0,0);            glVertex3d(1,1,0);
 glEnd();
glPopMatrix();
}