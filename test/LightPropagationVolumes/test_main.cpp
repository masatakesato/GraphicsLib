/*
//#####################################################################################################//
//####### フォームファクタ計算式を使って隣接セルの各面が投影する立体角を計算するテストプログラム ######//
//#####################################################################################################//

#include	<iostream>
#include	<math.h>


#include	<GL/glew.h>
#include	<GL/glut.h>
#pragma comment(lib, "glew32.lib")


#define _USE_MATH_DEFINES
using namespace std;

#include	"GLTexture.h"
#include	"MathLib.h"
// 隣接セルの各面の投影立体角を計算する



#define DIM 100.0// 面の1辺の長さ



// ポリゴンを単位球面上の投影した際の立体角を計算する．graphics gems IIIのフォームファクタ計算方式を利用 
float CalcFormFactor(int numVerts, const Vec3f verts[], const Vec3f p, const Vec3f n)
{
	Vec3f	result_vec = {0.0f, 0.0f, 0.0f};
	float	result = 0.0f;
	
	for(int i=0; i<numVerts; i++)
	{
		int i1 = (i+1)%numVerts;
		
		Vec3f	Ri, Ri1;
		Subtract(Ri, verts[i], p);
		Subtract(Ri1, verts[i1], p);

		Normalize(Ri);
		Normalize(Ri1);

		Vec3f R_Cross;
		CrossProduct(R_Cross, Ri, Ri1);
		Normalize(R_Cross);

		AddScaled(result_vec, result_vec, acos(DotProduct(Ri, Ri1)), R_Cross);
	}

	return	Length(result_vec);//DotProduct(result_vec, n);
}

int main(int argc, char **argv)
{	
	
//====================== 立体角の計算 =====================//
	Vec3f	R[] =
	{
		// 上面
		{-DIM, -DIM, +DIM},
		{+DIM, -DIM, +DIM},
		{+DIM, +DIM, +DIM},
		{-DIM, +DIM, +DIM},

		// 底面
		{-DIM, -DIM, -DIM},
		{+DIM, -DIM, -DIM},
		{+DIM, +DIM, -DIM},
		{-DIM, +DIM, -DIM},

		// 側面
		{+DIM, -DIM, -DIM},
		{+DIM, +DIM, -DIM},
		{+DIM, +DIM, +DIM},
		{+DIM, -DIM, +DIM},
	};

	Vec3f Origin = {0.0f, 0.0f, -2.0f*DIM};
	Vec3f	N = {0,0,1};
	
	float	result = CalcFormFactor(4, &R[8], Origin, N);

	cout << result  << endl;
	

	return 0;
}
*/



//#####################################################################################################//
//############ 球面調和関数(2-bandのみ)を使って隣接セルに放射束を伝播させるテストプログラム ###########//
//#####################################################################################################//

// TODO: 関数の入力/出力，グローバル変数の整理
#include <iostream>
#include <math.h>
#include <stdlib.h>

#include <GL/glut.h>

#define _USE_MATH_DEFINES
using namespace std;


#include "Camera.h"


int		mx = 0;
int		my = 0;
bool	LeftMouseButtonPressed = false;


Camera	cam;


float		g_Angle = 0.5 * M_PI;
Vec3f	g_Direction = {1,0,0};
Vec4f	g_SHCoeff_Center = {0,0,0,0};
Vec4f	g_SHCoeff_right = {0,0,0,0};



// 起点セル中心点から見た隣接セル面の立体角
const float FrontFacingArea	= 0.400669;// / (4.0f * M_PI);	// 正面
const float SideFacingArea	= 0.423431;// / (4.0f * M_PI);	// 側面

// 起点セルの中心点から，隣接セルの各面への単位ベクトル(四錐台の垂線方向)
const Vec3f	g_PropagationVector[5*6] =
{
	// +x方向
	{1.0,		0.0,		0.0},		// +x面
	{0.860111,	0.0,		0.510106},	// +z面
	{0.860111,	0.0,		-0.510106},	// -z面
	{0.860111,	0.510106,	0.0},		// +y面
	{0.860111,	-0.510106,	0.0},		// -y面

	// -x方向
	{-1.0,		0.0,		0.0},		// -x面
	{-0.860111,	0.0,		0.510106},	// +z面
	{-0.860111,	0.0,		-0.510106},	// -z面
	{-0.860111,	0.510106,	0.0},		// +y面
	{-0.860111,	-0.510106,	0.0},		// -y面

	// +y方向
	{0.0,		1.0,		0.0},		// +y面
	{0.510106,	0.860111,	0.0},		// +x面
	{-0.510106,	0.860111,	0.0},		// -x面
	{0.0,		0.860111,	0.510106},	// +z面
	{0.0,		0.860111,	-0.510106},	// +z面
	
	// -y方向
	{0.0,		-1.0,		0.0},		// +y面
	{0.510106,	-0.860111,	0.0},		// +x面
	{-0.510106,	-0.860111,	0.0},		// -x面
	{0.0,		-0.860111,	0.510106},	// +z面
	{0.0,		-0.860111,	-0.510106},	// +z面

	// +z方向
	{0.0,		0.0,		1.0},		// +z面
	{0.510106,	0.0,		0.860111},	// +x面
	{-0.510106,	0.0,		0.860111},	// -x面
	{0.0,		0.510106,	0.860111},	// +y面
	{0.0,		-0.510106,	0.860111},	// -y面

	// -z方向
	{0.0,		0.0,		-1.0},		// +z面
	{0.510106,	0.0,		-0.860111},	// +x面
	{-0.510106,	0.0,		-0.860111},	// -x面
	{0.0,		0.510106,	-0.860111},	// +y面
	{0.0,		-0.510106,	-0.860111}	// -y面

};

// 起点セルから伝播させた放射束を，隣接セルに再投影する際のベクトル
const Vec3f g_ReprojectionVector[5] =
{
	{1.0, 0.0, 0.0},
	{0.0, 0.0, 1.0},
	{0.0, 0.0, -1.0},
	{0.0, 1.0, 0.0},
	{0.0, -1.0, 0.0}
};

// 球面調和関数の正規直交基底(2-bandまで)
const float SHBasis[4] =
{
	0.282094,	// (l=0, m=0)...  0.5 / sqrt(pi)
	0.488602,	// (l=1, m=-1)... 0.5 * sqrt(3.0/pi)
	0.488602,	// (l=1, m=0)... 0.5 * sqrt(3.0/pi)
	0.488602	// (l=1, m=+1)... 0.5 * sqrt(3.0/pi)
};

// Clamped cosine lobeのSH係数(2-bandまで)
const Vec4f SHClampedCosineLobe =
{
	0.5f*sqrt(M_PI),//0.0,//		// (l=0, m=0)... 0.0 * 0.5f*sqrt(pi)
	sqrt(M_PI/3.0),//2.046653,//	// (l=1, m=-1)... 2.0 * sqrt(M_PI/3.0)
	sqrt(M_PI/3.0),//2.046653,//	// (l=1, m=0)... 2.0 * sqrt(M_PI/3.0)
	sqrt(M_PI/3.0)//2.046653//	// (l=1, m=+1)... 2.0 * sqrt(M_PI/3.0)
};

// ベクトルvのSH係数を計算する
Vec4f CalcSHCoeff(const Vec3f &v)
{
	Vec4f result = { SHBasis[0],
						SHBasis[1]*v.y,
						SHBasis[1]*v.z,
						SHBasis[1]*v.x };
	return result;
}

// ベクトル(x,y,z)のSH係数を計算する
Vec4f CalcSHCoeff(float x, float y, float z)
{
	Vec4f result = { SHBasis[0],
						SHBasis[1]*y,
						SHBasis[1]*z,
						SHBasis[1]*x };
	return result;
}

// v軸まわりのコサインローブSH係数を計算する
Vec4f CalcSHCoeff_ClampedCosineLobe(const Vec3f &v)
{
	Vec4f result = { SHClampedCosineLobe.x,
						SHClampedCosineLobe.y*v.y,
						SHClampedCosineLobe.z*v.z,
						SHClampedCosineLobe.w*v.x };
	return result;
}

// dir軸まわりのコサインローブSHを計算する. SH基底とZH係数を使う
Vec4f SHClampedCosineLobe2(const Vec3f &dir)
{
	Vec4f result;
	
	// ZHを使って係数倍したSH基底関数
	Vec4f	ZHCoeffs =
	{
		0.5,	// l=0, m=0
		0.75f,	// l=1, m=0
		0.75f,	// l=1, m=0
		0.75f	// l=1, m=0
	};

	result.x = ZHCoeffs.x;
	result.y = ZHCoeffs.y*dir.y;
	result.z = ZHCoeffs.z*dir.z;
	result.w = ZHCoeffs.w*dir.x;

	result.x *= 4.0f;
	result.y *= 4.0f;
	result.z *= 4.0f;
	result.w *= 4.0f;
	
	return result;
}







// dir方向に角度angle*2の広がりを持つコーンの球面調和関数近似(SH係数)を計算する
Vec4f CalcSHCone(const Vec3f &dir, float angle)
{
	// 帯域調和関数の基底関数(L=1まで)
	Vector2D	ZHCoeffs =
	{
		-sqrt(M_PI) * (-1.0f + cos(angle)),					// l=0, m=0
		0.5f * sqrt(3.0f)*sqrt(M_PI) * pow(sin(angle), 2.0f)// l=1, m=0
	};

	Vec4f	result;
	/*
	float cosine_theta, sine_theta, cosine_phi, sine_phi;

	cosine_theta = dir.z;
	sine_theta = sqrt(1.0f - dir.z * dir.z);

	Vector2D dir_xy = {dir.x, dir.y};
	Normalize(dir_xy);
	cosine_phi = dir_xy.x;
	sine_phi = dir_xy.y;
	*/
	result.x = ZHCoeffs.x * SHBasis[0];
	result.y = ZHCoeffs.y * SHBasis[1] * dir.y;	// y
	result.z = ZHCoeffs.y * SHBasis[2] * dir.z;	// z
	result.w = ZHCoeffs.y * SHBasis[3] * dir.x;	// x

	return result;
}



void DrawSH(Vec4f SHcoeff)
{
	const int DIV = 256;

	for(int iphi=0; iphi<DIV; ++iphi)
	{
		glBegin(GL_TRIANGLE_STRIP);

		for(int itheta=0; itheta<=DIV/2; ++itheta)
		{
			float theta = (float)itheta * (2 * M_PI/DIV);
			
			for(int i=0; i<2; i++)
			{
				float phi = (float)(iphi+i) * (2 * M_PI/DIV);
			
				// 単位球面上の頂点を計算する
				float x = sin(theta) * cos(phi);
				float y = sin(theta) * sin(phi);
				float z = cos(theta);

				Vec4f dirSH = CalcSHCoeff(x,y,z);
				float Y = DotProduct(SHcoeff, dirSH);

				if(Y >=0)	glColor4f(0, Y, 0, 1);
				else		glColor4f(Y, 0, 0, 1);
			
				float s = max(Y, 0.0f);
				x*=s;
				y*=s;
				z*=s;

				glVertex3d(x, y, z);
			}
			
		}
		glEnd();
	}
}



Vec3f OcclusionNormal = {1,0,0};


// 隣のセルに伝播させる(+x方向の隣接セル)
void Propagate()
{
	int i;
	float		FaceArea;
	float		Flux = 0.0f;
	Vec4f	SH_PropVec;	// 隣接セルへの放射束伝播ベクトルのSH係数
	Vec4f	SH_ReprojVec;// 再投影ベクトルのSH係数
	float		Iwc;		// 放射強度


	Vec3f InvDir = {1, 0.0f, 0.0f};
	Vec4f SH_InvDir = CalcSHCoeff(g_PropagationVector[0]);
	Vec4f SH_OccCosineLobe = CalcSHCoeff_ClampedCosineLobe(InvDir);//SHClampedCosineLobe2(InvDir);//CalcSHCone(InvDir, 0.5*M_PI);//
	SCALE_V4(SH_OccCosineLobe, SH_OccCosineLobe, 2.0/*FrontFacingArea*/);


	for(i=0; i<5; i++)
	{
		//=================== 受光面の面積の取得 ===================//
		if(i==0)	FaceArea = FrontFacingArea / M_PI;
		else		FaceArea = SideFacingArea / M_PI;

		//=============== 伝播ベクトルの基底関数を取得 ==============//
		SH_PropVec = CalcSHCoeff(g_PropagationVector[i]);

		//=================== 放射強度の計算 =======================//
		Iwc = max(DotProduct(g_SHCoeff_Center, SH_PropVec), 0.0f);

		//============ 受光面の大きさを掛けてFluxに変換 ============//
		Flux = FaceArea * Iwc;

		//===================== Occlusionの計算 ====================//
		//Normalize(OcclusionNormal);
		//Vec4f occlusionsh = CalcSHCoeff_ClampedCosineLobe(OcclusionNormal);
		//float occlusion = 1.0 - DotProduct(occlusionsh, -SH_PropVec);
		
		float scale = DotProduct(SH_OccCosineLobe, SH_PropVec);
		Flux *= scale;

		//=================隣接セル中心点への再投影 ================//
		// 受光面法線方向のコサインローブSH係数を取得
		SH_ReprojVec = CalcSHCoeff_ClampedCosineLobe(g_ReprojectionVector[i]);
		// Flux倍して累積
		//AddScaled(g_SHCoeff_right, g_SHCoeff_right, Flux*SH_OccCosineLobe, SH_ReprojVec);

		g_SHCoeff_right.x += Flux * SH_ReprojVec.x;//SH_OccCosineLobe.x;//
		g_SHCoeff_right.y += Flux * SH_ReprojVec.y;//SH_OccCosineLobe.y;//
		g_SHCoeff_right.z += Flux * SH_ReprojVec.z;//SH_OccCosineLobe.z;//
		g_SHCoeff_right.w += Flux * SH_ReprojVec.w;//SH_OccCosineLobe.w;//

#ifdef _DEBUG

	float magnitude = sqrt(g_SHCoeff_Center.x*g_SHCoeff_Center.x + g_SHCoeff_Center.y*g_SHCoeff_Center.y + g_SHCoeff_Center.z*g_SHCoeff_Center.z + g_SHCoeff_Center.w*g_SHCoeff_Center.w);
	cout << "CenterSH magnitude = " << magnitude << endl;

	magnitude = sqrt(g_SHCoeff_right.x*g_SHCoeff_right.x + g_SHCoeff_right.y*g_SHCoeff_right.y + g_SHCoeff_right.z*g_SHCoeff_right.z + g_SHCoeff_right.w*g_SHCoeff_right.w);
	cout << "RightSH magnitude = " << magnitude << endl;
#endif
	}// end of i loop


	//=================== 伝播してきた方向に逆伝播しないようブロックしたい =================//
	/*
	g_SHCoeff_right.x *= scale;
	g_SHCoeff_right.y *=scale;
	g_SHCoeff_right.z *= scale;
	g_SHCoeff_right.w *= scale;
	*/
}




// 隣のセルに伝播させる(+x方向の隣接セル)
void Propagate2()
{
	float		Flux = 0.0f;
	Vec4f	SH_PropVec;	// 隣接セルへの放射束伝播ベクトルのSH係数
	Vec4f	SH_ReprojVec;// 再投影ベクトルのSH係数


	//=============== 伝播ベクトルの基底関数を取得 ==============//
	SH_PropVec = CalcSHCone(g_PropagationVector[0], 0.25*M_PI);//CalcSHCoeff(g_PropagationVector[0]);//
	SH_PropVec.x *= 2.0*M_PI;
	SH_PropVec.y *= 2.0*M_PI;
	SH_PropVec.z *= 2.0*M_PI;
	SH_PropVec.w *= 2.0*M_PI;

	//=================== 放射束の取得 =======================//
	Flux = max(DotProduct(g_SHCoeff_Center, SH_PropVec), 0.0f) / 6.0;

	//=================== 投影SHの計算
	SH_ReprojVec = CalcSHCone(g_PropagationVector[0], 0.5*M_PI);
	
	
	g_SHCoeff_right.x += Flux * SH_ReprojVec.x;
	g_SHCoeff_right.y += Flux * SH_ReprojVec.y;
	g_SHCoeff_right.z += Flux * SH_ReprojVec.z;
	g_SHCoeff_right.w += Flux * SH_ReprojVec.w;


#ifdef _DEBUG

	float magnitude = sqrt(g_SHCoeff_Center.x*g_SHCoeff_Center.x + g_SHCoeff_Center.y*g_SHCoeff_Center.y + g_SHCoeff_Center.z*g_SHCoeff_Center.z + g_SHCoeff_Center.w*g_SHCoeff_Center.w);
	cout << "CenterSH magnitude = " << magnitude << endl;

	magnitude = sqrt(g_SHCoeff_right.x*g_SHCoeff_right.x + g_SHCoeff_right.y*g_SHCoeff_right.y + g_SHCoeff_right.z*g_SHCoeff_right.z + g_SHCoeff_right.w*g_SHCoeff_right.w);
	cout << "RightSH magnitude = " << magnitude << endl;
#endif

}





// 隣のセルに伝播させる(+x方向の隣接セル)
void Propagate3()
{
	float		Flux = 0.0f;
	Vec4f	SH_PropVec;	// 隣接セルへの放射束伝播ベクトルのSH係数
	Vec4f	SH_ReprojVec;// 再投影ベクトルのSH係数


	//=============== 伝播ベクトルの基底関数を取得 ==============//
	SH_PropVec = CalcSHCoeff(g_PropagationVector[0]);

	//=================== 放射束の取得 =======================//
	Flux = max(DotProduct(g_SHCoeff_Center, SH_PropVec), 0.0f) * 4.0f * M_PI / 6.0;

	//=================== 投影SHの計算
	SH_ReprojVec = CalcSHCone(g_PropagationVector[0], 0.25*M_PI);

	g_SHCoeff_right.x += Flux * SH_ReprojVec.x;
	g_SHCoeff_right.y += Flux * SH_ReprojVec.y;
	g_SHCoeff_right.z += Flux * SH_ReprojVec.z;
	g_SHCoeff_right.w += Flux * SH_ReprojVec.w;


#ifdef _DEBUG

	float magnitude = sqrt(g_SHCoeff_Center.x*g_SHCoeff_Center.x + g_SHCoeff_Center.y*g_SHCoeff_Center.y + g_SHCoeff_Center.z*g_SHCoeff_Center.z + g_SHCoeff_Center.w*g_SHCoeff_Center.w);
	cout << "CenterSH magnitude = " << magnitude << endl;

	magnitude = sqrt(g_SHCoeff_right.x*g_SHCoeff_right.x + g_SHCoeff_right.y*g_SHCoeff_right.y + g_SHCoeff_right.z*g_SHCoeff_right.z + g_SHCoeff_right.w*g_SHCoeff_right.w);
	cout << "RightSH magnitude = " << magnitude << endl;
#endif

}






// 座標軸の描画
void DRAW_XYZ(const float scale=1.0f)
{
	glBegin(GL_LINES);
		glColor3f(1,0,0);// x
		glVertex3f(0,0,0);
		glVertex3f(scale,0,0);

		glColor3f(0,1,0);// y
		glVertex3f(0,0,0);
		glVertex3f(0,scale,0);

		glColor3f(0,0,1);// z
		glVertex3f(0,0,0);
		glVertex3f(0,0,scale);
	glEnd();
}


void Initialize()
{
	glClearColor(0.4, 0.4, 0.4, 1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);

	cam = Camera(0.75,0,2, 0,0,-1, 1,0,0);
}


void display()
{
	const Vec3f	*pos = cam.GetPosition(),
					*dir = cam.GetForward(),
					*up =cam.GetVertical();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(	pos->x, pos->y, pos->z,
				pos->x + dir->x, pos->y + dir->y, pos->z + dir->z,
				up->x, up->y, up->z);

	glColor3f(0.5, 0.5, 0);


	//============== 中心セルの向きを計算 ===========//
	static float theta = 0.5f * M_PI, phi = 0.0f;//0.5f * M_PI;
	float x = sin(theta) * cos(phi);
	float y = sin(theta) * sin(phi);
	float z = cos(theta);

	InitVec(g_Direction, 0, 0, 1);
	//Normalize(g_Direction);

	// 中心セルの放射束を設定
	g_SHCoeff_Center = CalcSHCoeff_ClampedCosineLobe(g_Direction);//CalcSHCone(g_Direction, 0.5*M_PI);//SHClampedCosineLobe2(g_Direction);//
	//SCALE_V4(g_SHCoeff_Center, g_SHCoeff_Center, M_PI);


	// 隣接セルへの伝播
	Propagate3();

	
	//================== 結果の描画 ===================//
	glPushMatrix();
	DrawSH(g_SHCoeff_Center);
	glPopMatrix();
	
	
	glPushMatrix();
	glTranslatef(1.0, 0.0, 0.0);
	DrawSH(g_SHCoeff_right);
	glPopMatrix();
	

	DRAW_XYZ();

	/*
	glBegin(GL_LINES);
	
	for(int i=25; i<30; i++)
	{
		glColor3f(g_PropagationVector[i].x, g_PropagationVector[i].y, g_PropagationVector[i].z);
		glVertex3f(0,0,0);
		glVertex3fv(g_PropagationVector[i].xyz);
	}
	glEnd();
	*/
	//================== パラメータ更新 ==============//
	//theta += 1.0e-2f;
	//phi -= 2.0e-2f;

	InitVec(g_SHCoeff_right, 0,0,0,0);

	glutSwapBuffers();
}



void keyboard(unsigned char c, int w, int h)
{
	switch(c)
	{
	case 27: exit(0);

	case 'w':	cam.Transrate(+1.0e-1,0,0);	break;
	case 's':	cam.Transrate(-1.0e-1,0,0);	break;
	case 'a':	cam.Transrate(0,+1.0e-1,0);	break;
	case 'd':	cam.Transrate(0,-1.0e-1,0);	break;
	case 'q':	cam.Roll(-1.0e-1);			break;
	case 'e':	cam.Roll(+1.0e-1);			break;
	
	default:			break;
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
	gluPerspective(45, 1.25, 1.0e-2, 1000);
	glMatrixMode(GL_MODELVIEW);
}


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
	int dx = mx - x;
	int dy = my - y;
	//cout << "dv = (" << dx << ", " << dy << ")" << endl;

	if(LeftMouseButtonPressed ==true)// 左ボタンをクリックしたときだけ回転
	{
		cam.Rotate((float)dx*0.01f,(float)dy*0.01f);
	}

    mx = x;
    my = y;
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
	glutReshapeFunc(reshape);
	glutIdleFunc(animate);

	glutKeyboardFunc(keyboard);
	glutMouseFunc(MouseCallback);
	glutMotionFunc(MotionCallback);

	
	
	Initialize();
	glutMainLoop();

	return 0;
}