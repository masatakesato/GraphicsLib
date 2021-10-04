/***********************************************************************
　　Main.cpp

　　DDS Texture Mapping

　　Date : Nov 01, 2007
　　Version : 1.0
　　Author : Pocol
　　Memo :
*************************************************************************/

//
//　include
//
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "DDSLoader.h"
using namespace std;

//
//　global
//
int WindowPositionX = 100;
int WindowPositionY = 100;
int WindowWidth = 512;
int WindowHeight = 512;
char WindowTitle[] = "Texture Mapping (4) - DDS File -";
DDSImage texture;

//
//　prototype
//
void Initialize();
void Display();
void Idle();
void Reshape(int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void PassiveMotion(int x, int y);
void Keyboard(unsigned char key, int x, int y);
void Special(int key, int x, int y);


//----------------------------------------------------------------------------------------------------
//　　main
//　　Desc : メインエントリーポイント
//----------------------------------------------------------------------------------------------------
int main( int argc, char **argv )
{
	glutInit(&argc, argv);
	glutInitWindowPosition(WindowPositionX, WindowPositionY);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow(WindowTitle);
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutIdleFunc(Idle);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutPassiveMotionFunc(PassiveMotion);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);
	
	Initialize();
	
	glutMainLoop();
	return 0;
}


//----------------------------------------------------------------------------------------------------
//　　Initialize
//　　Desc : 初期化処理
//----------------------------------------------------------------------------------------------------
void Initialize()
{
	glClearColor(0.3, 0.3, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);

	//　テクスチャのロード
	texture.Load("sample.dds");

}

//---------------------------------------------------------------------------------------------------
//　　Idle
//　　Desc : アイドリング時の処理
//---------------------------------------------------------------------------------------------------
void Idle()
{
	glutPostRedisplay();
}

//---------------------------------------------------------------------------------------------------
//　　Reshape
//　　Desc : サイズ変更
//---------------------------------------------------------------------------------------------------
void Reshape(int x, int y)
{
	WindowWidth = x;
	WindowHeight = y;
	if ( WindowWidth < 1 ) WindowWidth = 1;
	if ( WindowHeight < 1 ) WindowHeight = 1;
}

//---------------------------------------------------------------------------------------------------
//　　Display
//　　Desc : ウィンドウへの描画
//---------------------------------------------------------------------------------------------------
void Display()
{
	double size = 0.5;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//　テクスチャマッピング有効化
	glEnable(GL_TEXTURE_2D);
	//　テクスチャをバインド
	glBindTexture(GL_TEXTURE_2D, texture.ID);
	//　色の指定
	glColor4f(1.0, 1.0, 1.0, 1.0);
	//　四角形をテクスチャ座標つきで描画
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);	glVertex3d(-size, -size, 0.0);
	glTexCoord2d(0.0, -1.0);	glVertex3d(-size, size, 0.0);
	glTexCoord2d(1.0, -1.0);	glVertex3d(size, size, 0.0);
	glTexCoord2d(1.0, 0.0);	glVertex3d(size, -size, 0.0);
	glEnd();
	//　
	glBindTexture(GL_TEXTURE_2D, 0);
	//　テクスチャマッピング無効化
	glDisable(GL_TEXTURE_2D);

	glutSwapBuffers();
}

//---------------------------------------------------------------------------------------------------
//　　Mouse
//　　Desc : マウス処理
//---------------------------------------------------------------------------------------------------
void Mouse(int button, int state, int x, int y)
{
	switch ( button )
	{
	case GLUT_LEFT_BUTTON:
		switch ( state )
		{
		case GLUT_DOWN:
			break;

		case GLUT_UP:
			break;
		}
		break;

	case GLUT_MIDDLE_BUTTON:
		switch ( state )
		{
		case GLUT_DOWN:
			break;

		case GLUT_UP:
			break;
		}
		break;

	case GLUT_RIGHT_BUTTON:
		switch ( state )
		{
		case GLUT_DOWN:
			break;

		case GLUT_UP:
			break;
		}
		break;

	default:
		break;
	}
	

}

//--------------------------------------------------------------------------------------------------
//　　Motion
//　　Desc : マウスドラッグ時
//-------------------------------------------------------------------------------------------------
void Motion(int x, int y)
{
}

//--------------------------------------------------------------------------------------------------
//　　PassiveMotion
//　　Desc : マウス移動時
//--------------------------------------------------------------------------------------------------
void PassiveMotion(int x, int y)
{
}

//--------------------------------------------------------------------------------------------------
//　　Keyboard
//　　Desc : キーボード処理
//--------------------------------------------------------------------------------------------------
void Keyboard(unsigned char key, int x, int y)
{
	switch ( key )
	{
	case '\033':
		exit(0);
		break;

	default:
		break;
	}
}

//--------------------------------------------------------------------------------------------------
//　　Special
//　　Desc : 特殊キー処理
//--------------------------------------------------------------------------------------------------
void Special(int key, int x, int y)
{
	switch ( key )
	{
	case GLUT_KEY_F1:
		break;

	case GLUT_KEY_F2:
		break;

	case GLUT_KEY_F3:
		break;

	case GLUT_KEY_F4:
		break;

	case GLUT_KEY_F5:
		break;

	case GLUT_KEY_F6:
		break;

	case GLUT_KEY_F7:
		break;

	case GLUT_KEY_F8:
		break;

	case GLUT_KEY_F9:
		break;

	case GLUT_KEY_F10:
		break;

	case GLUT_KEY_F11:
		break;

	case GLUT_KEY_F12:
		break;

	case GLUT_KEY_LEFT:
		break;

	case GLUT_KEY_RIGHT:
		break;

	case GLUT_KEY_UP:
		break;

	case GLUT_KEY_DOWN:
		break;

	case GLUT_KEY_PAGE_UP:
		break;

	case GLUT_KEY_PAGE_DOWN:
		break;

	case GLUT_KEY_HOME:
		break;

	case GLUT_KEY_END:
		break;

	case GLUT_KEY_INSERT:
		break;
	}
}