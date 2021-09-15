#ifndef	MODEL_GL_H
#define	MODEL_GL_H



// TODO: 
// シーングラフを管理するクラスが必要
// 現状は，本クラスに直接処理を記述(2011.07.14)
// m_LightCam: 光源オブジェクトのカメラ




#include	<windows.h>
#include	<iostream>
#include	<oreore/GLShader.h>
#include	<oreore/Camera.h>

#include	"LightPropagationVolumes.h"



class ModelGL
{
private:

	Camera	*m_Camera;// カメラ
Camera	*m_LightCam;// 光源カメラ

	LightPropagationVolumes	m_LPV;
	LPVTexture	*m_LpvTex;


public:

	ModelGL();
	~ModelGL();

	void Init();
	void LoadShader( const char *vsFile, const char *gsFile, const char *fsFile );
	
	void InitCamera(const Vec3f &pos, const Vec3f &dir, const Vec3f &up,
					float aspect_ratio, float fovy, float znear, float zfar);
	void SetCameraView(const Vec3f &pos, const Vec3f &dir, const Vec3f &up);
	void SetCameraProjection(float aspect_ratio, float fovy, float znear, float zfar);

	void TransrateCamera(float df, float dh, float dv)	{ m_Camera->Transrate(df, dh, dv); };
	void RollCamera(float Angle)						{ m_Camera->Roll(Angle); };
	void RotateCamera(float dh, float dv)				{ m_Camera->Rotate(dh, dv); };
	
	void SetViewport(int x, int y, int w, int h)		{ glViewport(x, y, w, h); };

	void Draw();

	void SetMode(){ m_LPV.SetMode(); }
};




#endif	// MODEL_GL_H //