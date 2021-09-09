#include	"Camera.h"


#include <iostream>
using namespace std;
#include <math.h>
/*
// クォータニオンの掛け算 r ← p*q
static void qmul(float r[], const float p[], const float q[])
{
	r[0] = p[0] * q[0] - p[1] * q[1] - p[2] * q[2] - p[3] * q[3];
	r[1] = p[0] * q[1] + p[1] * q[0] + p[2] * q[3] - p[3] * q[2];
	r[2] = p[0] * q[2] - p[1] * q[3] + p[2] * q[0] + p[3] * q[1];
	r[3] = p[0] * q[3] + p[1] * q[2] - p[2] * q[1] + p[3] * q[0];
}



// 回転の変換行列 r <- クォータニオン q
static void qrot(float r[], float q[])
{
	float x2 = q[1] * q[1] * 2.0f;
	float y2 = q[2] * q[2] * 2.0f;
	float z2 = q[3] * q[3] * 2.0f;
	float xy = q[1] * q[2] * 2.0f;
	float yz = q[2] * q[3] * 2.0f;
	float zx = q[3] * q[1] * 2.0f;
	float xw = q[1] * q[0] * 2.0f;
	float yw = q[2] * q[0] * 2.0f;
	float zw = q[3] * q[0] * 2.0f;

	r[ 0] = 1.0f - y2 - z2;
	r[ 1] = xy + zw;
	r[ 2] = zx - yw;
	r[ 4] = xy - zw;
	r[ 5] = 1.0f - z2 - x2;
	r[ 6] = yz + xw;
	r[ 8] = zx + yw;
	r[ 9] = yz - xw;
	r[10] = 1.0f - x2 - y2;
	r[ 3] = r[ 7] = r[11] = r[12] = r[13] = r[14] = 0.0f;
	r[15] = 1.0f;
}
*/


Camera::Camera()
{
tcout << _T("Camera::Camera()...") << tendl;
	InitVec(m_Position,			0.0f, 0.0f, 0.0f);
	InitVec(m_Forward,		1.0f, 0.0f, 0.0f);
	InitVec(m_Horizontal,	0.0f, 1.0f, 0.0f);
	InitVec(m_Vertical,	0.0f, 0.0f, 1.0f);
}



Camera::Camera(	float pos_x, float pos_y, float pos_z,
				float dir_x, float dir_y, float dir_z,
				float up_x, float up_y, float up_z)
{
	//============== 初期位置の設定 ===============//
	InitVec(m_Position, pos_x, pos_y, pos_z);
	
	//============== 初期姿勢の設定 ===============//
	InitVec(m_Forward,	dir_x, dir_y, dir_z);// 方向
	Normalize(m_Forward);
	
	InitVec(m_Vertical, up_x, up_y, up_z);// 水平軸計算用に仮に値を入れる
	Normalize(m_Vertical);
	CrossProduct(m_Horizontal, m_Vertical, m_Forward);//　水平
	Normalize(m_Horizontal);
	
	CrossProduct(m_Vertical, m_Forward, m_Horizontal);// 垂直

	//============ ビュー変換行列を作成する ==========//
	MatViewGL(m_matView, m_Horizontal, m_Vertical, m_Forward, m_Position);
	Multiply(m_ViewProjection, m_matProj, m_matView);
}

// 任意軸まわりの回転
void Camera::Rotate(float Angle, float x, float y, float z)
{
	Vec4f	temp;
	Vec4f	quat_view;
	Vec4f	result;
	
	//=============== 回転クォータニオン設定 ==============//
	temp.x = x * sin(Angle*0.5f);
	temp.y = y * sin(Angle*0.5f);
	temp.z = z * sin(Angle*0.5f);
	temp.w = cos(Angle*0.5f);


	//=============== m_Forwardの回転 =================//
	quat_view.x = m_Forward.x;
	quat_view.y = m_Forward.y;
	quat_view.z = m_Forward.z;
	quat_view.w = 0;

	result = mult(mult(temp, quat_view), conjugate(temp));

	m_Forward.x = result.x;
	m_Forward.y = result.y;
	m_Forward.z = result.z;


	//=============== m_Verticalの回転 ================//
	quat_view.x = m_Vertical.x;
	quat_view.y = m_Vertical.y;
	quat_view.z = m_Vertical.z;
	quat_view.w = 0;

	result = mult(mult(temp, quat_view), conjugate(temp));

	m_Vertical.x = result.x;
	m_Vertical.y = result.y;
	m_Vertical.z = result.z;


	//============== m_Horizontalの回転 ==============//
	quat_view.x = m_Horizontal.x;
	quat_view.y = m_Horizontal.y;
	quat_view.z = m_Horizontal.z;
	quat_view.w = 0;

	result = mult(mult(temp, quat_view), conjugate(temp));

	m_Horizontal.x = result.x;
	m_Horizontal.y = result.y;
	m_Horizontal.z = result.z;

	//============ ビュー変換行列を作成する ==========//
	MatViewGL(m_matView, m_Horizontal, m_Vertical, m_Forward, m_Position);
	Multiply(m_ViewProjection, m_matProj, m_matView);
}


// ビュー平面上の移動ベクトルに応じた回転
void Camera::Rotate(float dh, float dv)
{
	
	Vec3f	vert_scaled, hol_scaled;
	Vec3f	rot_axis;
	float	angle;
	
	if(dh == 0.0f && dv == 0.0f) return;
	
	//=============== 回転軸と角度を決める ===============//
	Scale(hol_scaled, m_Horizontal, dv);// 画面横方向の動きでカメラ垂直軸の回転を制御する
	Scale(vert_scaled, m_Vertical, dh);// 画面たて方向の動きでカメラ水平軸の回転を制御する
	
	Subtract(rot_axis, vert_scaled, hol_scaled);
	//SET(rot_axis, vert_scaled);
	angle = Length(rot_axis);	
	Scale(rot_axis, 1.0f/angle);

	//================== 軸周りに回転 ====================//
	Rotate(angle, rot_axis.x, rot_axis.y, rot_axis.z);


	//============ ビュー変換行列を作成する ==========//
	MatViewGL(m_matView, m_Horizontal, m_Vertical, m_Forward, m_Position);
	Multiply(m_ViewProjection, m_matProj, m_matView);
}



void Camera::Roll(float Angle)
{
	Vec4f	temp;
	Vec4f	quat_view;
	Vec4f	result;
	
	//=============== 回転クォータニオン設定 ==============//
	temp.x = m_Forward.x * sin(Angle*0.5f);
	temp.y = m_Forward.y * sin(Angle*0.5f);
	temp.z = m_Forward.z * sin(Angle*0.5f);
	temp.w = cos(Angle*0.5f);

/*
	//=============== m_Forwardの回転 =================//
	quat_view.x = m_Forward.x;
	quat_view.y = m_Forward.y;
	quat_view.z = m_Forward.z;
	quat_view.w = 0;

	result = mult(mult(temp, quat_view), conjugate(temp));

	m_Forward[0] = result.x;
	m_Forward[1] = result.y;
	m_Forward[2] = result.z;
*/

	//=============== m_Verticalの回転 ================//
	quat_view.x = m_Vertical.x;
	quat_view.y = m_Vertical.y;
	quat_view.z = m_Vertical.z;
	quat_view.w = 0;

	result = mult(mult(temp, quat_view), conjugate(temp));

	InitVec(m_Vertical, result.x, result.y, result.z);


	//============== m_Horizontalの回転 ==============//
	quat_view.x = m_Horizontal.x;
	quat_view.y = m_Horizontal.y;
	quat_view.z = m_Horizontal.z;
	quat_view.w = 0;

	result = mult(mult(temp, quat_view), conjugate(temp));

	m_Horizontal.x = result.x;
	m_Horizontal.y = result.y;
	m_Horizontal.z = result.z;


	//============ ビュー変換行列を作成する ==========//
	MatViewGL(m_matView, m_Horizontal, m_Vertical, m_Forward, m_Position);
	Multiply(m_ViewProjection, m_matProj, m_matView);
}



void Camera::Transrate(float df, float dh, float dv)
{
	//============ カメラの位置を移動する ============//
	m_Position.x += df * m_Forward.x + dh * m_Horizontal.x + dv * m_Vertical.x;
	m_Position.y += df * m_Forward.y + dh * m_Horizontal.y + dv * m_Vertical.y;
	m_Position.z += df * m_Forward.z + dh * m_Horizontal.z + dv * m_Vertical.z;

	//============ ビュー変換行列を作成する ==========//
	MatViewGL(m_matView, m_Horizontal, m_Vertical, m_Forward, m_Position);
	Multiply(m_ViewProjection, m_matProj, m_matView);
}


void Camera::Information()
{
	tcout << GetForward()->x << _T(", ") << GetForward()->y << _T(", ") << GetForward()->z << tendl;
	tcout << m_Horizontal.x << _T(", ") << m_Horizontal.y << _T(", ") << m_Horizontal.z << tendl;
	tcout << GetVertical()->x << _T(", ") << GetVertical()->y << _T(", ") << GetVertical()->z << tendl;
}



namespace OreOreLib
{


CameraStandAlone::CameraStandAlone() : CameraObject( this->m_ShadowCaster )
{


}


CameraStandAlone::~CameraStandAlone()
{



}




}// end of namespace