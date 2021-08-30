#ifndef CAMERA_OBJECT_H
#define	CAMERA_OBJECT_H

#include	"MovableObject.h"


namespace OreOreLib
{

	struct CameraObject : MovableObject
	{
	public:

		CameraObject( Frustum& frustum );
		~CameraObject();

		void BindFrustum( Frustum* frustum );
		//void UnbindFrustum();

		void SetAttribute( FrustumAttribute& attrib );
		void SetViewParams( float posx, float posy, float posz, float dirx, float diry, float dirz, float upx, float upy, float upz );
		void SetViewParams( const Vec3f &pos, const Vec3f &dir, const Vec3f &up );
		void SetProjectionParams( float fov, float aspect, float znear, float zfar );
		void SetFov( float fov );
		void SetAspectRatio( float aspect );
		void SetZNear( float znear );
		void SetZFar( float zfar );

		void Rotate( float Angle, float x, float y, float z );	// 任意軸周りにカメラを回転する
		void Rotate( float dh, float dv );						// 視点移動でカメラを回転する
		void Yaw( float Angle );
		void Pitch( float Angle );
		void Roll( float Angle );
		void Translate( float df, float dh, float dv );		// カメラを移動する

		inline Frustum *GetFrustum() const		{ return m_refViewFrustum; }
		//inline ViewTransformMatrix *GetMatrix() const	{ return m_refViewFrustum->GetMatrix(); }//{ return &(m_refViewFrustum->m_CameraMat); }
		inline ViewTransformMatrix *GetMatrix()  { return m_refViewFrustum->GetTransformMatrix(); }
		

		virtual void Update();	// Update Camea Params
		virtual void Info();


	private:

		CameraObject();
		Frustum		*m_refViewFrustum;
		//		ViewTransformMatrix	m_MatLight;

		// DoFのパラメータも追加予定

	};

}// end of namespace

#endif // !CAMERA_OBJECT_H
