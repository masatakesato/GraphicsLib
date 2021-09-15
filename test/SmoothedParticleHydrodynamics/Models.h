//--------------- Models.h --------------//
// 作者：	佐藤晶威
// 作成日：	2005.10.5
// 内容：	OpenGLを用いた図形の描画
// 更新履歴：
//			ファイル説明コメントの追加...2008.1.14
//			直方体を描く関数の追加...2008.1.14
//---------------------------------------//



#ifndef MODELS_H
#define MODELS_H



#include <math.h>
#include <GL/glut.h>



void Grid()
{

	glPushMatrix();

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

	glPopMatrix();
}




void DrawBox(double min[3], double max[3])
{

	glPushMatrix();
	glBegin(GL_LINE_STRIP);

		glVertex3d(min[0], min[1], min[2]);
		glVertex3d(min[0], min[1], max[2]);
		glVertex3d(min[0], max[1], max[2]);
		glVertex3d(min[0], max[1], min[2]);

		glVertex3d(max[0], min[1], min[2]);
		glVertex3d(min[0], min[1], min[2]);
		glVertex3d(min[0], max[1], min[2]);
		glVertex3d(max[0], max[1], min[2]);

		glVertex3d(max[0], min[1], max[2]);
		glVertex3d(max[0], min[1], min[2]);
		glVertex3d(max[0], max[1], min[2]);
		glVertex3d(max[0], max[1], max[2]);

		glVertex3d(min[0], min[1], max[2]);
		glVertex3d(max[0], min[1], max[2]);
		glVertex3d(max[0], max[1], max[2]);
		glVertex3d(min[0], max[1], max[2]);

		glVertex3d(max[0], max[1], max[2]);
		glVertex3d(max[0], max[1], min[2]);
		glVertex3d(min[0], max[1], min[2]);
		glVertex3d(min[0], max[1], max[2]);

		glVertex3d(min[0], min[1], min[2]);
		glVertex3d(max[0], min[1], min[2]);
		glVertex3d(max[0], min[1], max[2]);
		glVertex3d(min[0], min[1], max[2]);

	glEnd();
	glPopMatrix();

}


#endif /* MODELS_H */