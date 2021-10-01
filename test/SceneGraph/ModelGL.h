#ifndef	MODEL_GL_H
#define	MODEL_GL_H

#include	<windows.h>
#include	<iostream>

#include	<graphics/gl4x/scene/SceneGraph.h>
#include	<graphics/gl4x/scene/RenderScene.h>
using namespace OreOreLib;



class ModelGL
{
private:
	
	SceneGraph		*m_pSceneGraph;
	RenderScene		*m_pRenderScene;
	

public:

	ModelGL();
	~ModelGL();

	void Init();
	void Release();

	void ExecuteKeyDownEvent( int key );
	void ExecuteKeyUpEvent( int key );
	void ExecuteKeyPressEvent( int numkeys, const bool keys[] );

	void InitCamera(const Vec3f &pos, const Vec3f &dir, const Vec3f &up, float aspect_ratio, float fovy, float znear, float zfar );
	void SetCameraView(const Vec3f &pos, const Vec3f &dir, const Vec3f &up );
	void SetCameraProjection( float aspect_ratio, float fovy, float znear, float zfar );
	void SetCameraAspectRatio( float aspect_ratio )		{ m_pSceneGraph->GetCurrentCameraObject()->SetAspectRatio( aspect_ratio ); }
	void TranslateCamera(float df, float dh, float dv)	{ m_pSceneGraph->GetCurrentCameraObject()->Translate( df, dh, dv ); }
	void RollCamera(float Angle)						{ m_pSceneGraph->GetCurrentCameraObject()->Roll( Angle ); }
	void RotateCamera(float dh, float dv)				{ m_pSceneGraph->GetCurrentCameraObject()->Rotate( dh, dv ); }
	


	void SetViewport(int x, int y, int w, int h)
	{
		glViewport(x, y, w, h); 
		m_pRenderScene->InitOffScreenBuffers( w, h ); 
		//m_pRenderScene->AllocateAmbientOcclusionBuffer( w/2, h/2 );
	};

	void Draw();

};




#endif	// MODEL_GL_H //