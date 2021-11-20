#ifndef	MODEL_GL_H
#define	MODEL_GL_H


#include	<windows.h>
#include	<iostream>

#include	<graphics/gl4x/shader/GLShader.h>
#include	<graphics/gl4x/scene/Camera.h>
#include	<graphics/gl4x/scene/MeshLoader.h>




class ModelLoadObj
{
private:

	OreOreLib::GLShader	*m_Shader;// シェーダ
	Camera	*m_Camera;// カメラ

	//=============== シェーダ変数 =============//
	GLint	vertLoc;
	GLint	normLoc;
	GLint	texcLoc;
	GLint	m_Tangent;
	GLint	m_Binormal;

	GLint	mvpLoc;
	GLint	g_epLightDir;
	GLint	g_epEyePos;
/*
	GLint	m_NormalSampler;
	GLint	m_HeightSampler;
	GLint	m_DiffuseSampler;

	GLuint	m_NormalTexture;
	GLuint	m_HeightTexture;
	GLuint	m_DiffuseTexture;
*/

	MeshLoader m_MeshLoader;


public:

	ModelLoadObj();
	~ModelLoadObj();

	void Init();
	void LoadShader( const char *vsFile, const char *gsFile, const char *fsFile );
	
	void InitCamera(const Vec3f &pos, const Vec3f &dir, const Vec3f &up,
					float aspect_ratio, float fovy, float znear, float zfar);
	void SetCameraView(const Vec3f &pos, const Vec3f &dir, const Vec3f &up);
	void SetCameraProjection(float aspect_ratio, float fovy, float znear, float zfar);
	
	void TranslateCamera(float df, float dh, float dv)	{ m_Camera->Transrate(df, dh, dv); };
	void RollCamera(float Angle)						{ m_Camera->Roll(Angle); };
	void RotateCamera(float dh, float dv)				{ m_Camera->Rotate(dh, dv); };
	
	void SetViewport(int x, int y, int w, int h)		{ glViewport(x, y, w, h); };

	void Draw();

};




#endif	// MODEL_GL_H //