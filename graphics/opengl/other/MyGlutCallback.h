#include "Camera.h"


namespace MyGlutCallback
{
	#define	PI 3.141592f

	// マウス設定
	int		mx = 0;
	int		my = 0;
	bool	LeftMouseButtonPressed = false;

	// キーボード設定
	#define MAX_KEYS 256
	bool	gKeys[MAX_KEYS];

	// カメラ設定
	Camera *cam;
	float	gDeltaT = 1.0f / 60.0f;
	float	CamRotSpeed = 5.0f;
	float	CamMovSpeed = 5.0f;


	//###########################################################################//
	//####################### キー入力検出コールバック関数 ######################//
	//###########################################################################//

	void ProcessCameraKeys()
	{
		int	i;

		for(i=0; i<MAX_KEYS; i++)
		{
			if(gKeys[i]==false)	continue;

			switch(i)
			{
			case 'w' : { cam->Transrate(+CamMovSpeed * gDeltaT, 0, 0);	break; }
			case 's' : { cam->Transrate(-CamMovSpeed * gDeltaT, 0, 0);	break; }
			case 'a' : { cam->Transrate(0, +CamMovSpeed * gDeltaT, 0);	break; }
			case 'd' : { cam->Transrate(0, -CamMovSpeed * gDeltaT, 0);	break; }
			case 'q' : { cam->Roll(-CamRotSpeed * gDeltaT);			break; }
			case 'e' : { cam->Roll(+CamRotSpeed * gDeltaT);			break; }
			case 'r' : { cam->Transrate(0, 0, +CamMovSpeed * gDeltaT);	break;}
			case 'f' : { cam->Transrate(0, 0, -CamMovSpeed * gDeltaT);	break;}
			default : {	break; }
			}
		}
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



	//###########################################################################//
	//###################### マウス入力検出コールバック関数 #####################//
	//###########################################################################//


	// マウス状態の監視
	void MouseCallback(int button, int state, int x, int y)
	{
		mx = x;
		my = y;

		if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)// 右ボタン押し込んだ状態
		{
			//Pick(x, y);
			// this prevents from only grabbing it when the mouse moves
			//MotionCallback(x, y);
		}
		else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)// 左ボタンを押し込んだ状態
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
		float	dx = float(mx - x) * 2.0f*PI / 800;
		float	dy = float(my - y) * 2.0f*PI / 600;
		//cout << "dv = (" << dx << ", " << dy << ")" << endl;

		if(LeftMouseButtonPressed ==true)// 左ボタンをクリックしたときだけ回転
		{
			cam->Rotate(dx,dy);
		}

		mx = x;
		my = y;
	}



	void Reshape(int w, int h)
	{
		if(w >=h)
		{
			glViewport(0, 0, (int)(h*4/3), h);
			//printf("w =%d\n", (int)(h*4/3) );
			//printf("h =%d\n", h);
		}
		else
		{
			glViewport(0, 0, (int)(w*4/3), w);
		}
		
	}


	void Animate(void){ glutPostRedisplay(); }




};