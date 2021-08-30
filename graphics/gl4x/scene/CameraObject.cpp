#include	"CameraObject.h"

namespace OreOreLib
{

	//	CameraObject::CameraObject()
	//	{
	////tcout << "CameraObject::CameraObject()" << endl;
	//		//m_NodeType			= NODE_CAMERA;
	//		m_refViewFrustum	= NULL;
	//	}


	CameraObject::CameraObject( Frustum& frustum )
	{
		//tcout << "CameraObject::CameraObject( FrustumAttribute *cam_attr, ViewTransformMatrix *cam_mat )" << endl;
		//m_NodeType			= NODE_CAMERA;
		m_refViewFrustum	= &frustum;
	}


	CameraObject::~CameraObject()
	{
		//tcout << "CameraObject::~CameraObject()" << endl;
		m_refViewFrustum	= NULL;
	}


	void CameraObject::BindFrustum( Frustum* frustum )
	{
		m_refViewFrustum	= frustum;
	}


	//void CameraObject::UnbindFrustum()
	//{
	//	m_refViewFrustum	= NULL;
	//}


	void CameraObject::SetAttribute( FrustumAttribute &cam_attr )
	{
		m_refViewFrustum->SetProjectionParams( cam_attr.m_Fov, cam_attr.m_AspectRatio, cam_attr.m_zNear, cam_attr.m_zFar );
	}


	void CameraObject::SetViewParams( float posx, float posy, float posz, float dirx, float diry, float dirz, float upx, float upy, float upz )
	{
		m_LocalSpace.SetOrigin( posx, posy, posz );
		m_LocalSpace.SetDirection( dirx, diry, dirz, upx, upy, upz );
	}


	void CameraObject::SetViewParams( const Vec3f &pos, const Vec3f &dir, const Vec3f &up )
	{
		m_LocalSpace.SetOrigin( pos );
		m_LocalSpace.SetDirection( dir, up );
	}


	// 射影変換のパラメータを設定する
	void CameraObject::SetProjectionParams( float fov, float aspect, float znear, float zfar )
	{
		m_refViewFrustum->SetProjectionParams( fov, aspect, znear, zfar );
	}


	// Fovを設定する
	void CameraObject::SetFov( float fov )
	{
		m_refViewFrustum->SetFov( fov );
	}


	// アスペクト比を設定する
	void CameraObject::SetAspectRatio( float aspect )
	{
		m_refViewFrustum->SetAspectRatio( aspect );
	}


	void CameraObject::SetZNear( float znear )
	{
		m_refViewFrustum->SetZNear( znear );
	}


	void CameraObject::SetZFar( float zfar )
	{
		m_refViewFrustum->SetZFar( zfar );
	}


	// 任意軸まわりの回転
	void CameraObject::Rotate( float Angle, float x, float y, float z )
	{
		m_LocalSpace.Rotate( Angle, x, y, z );
	}


	// ビュー平面上の移動ベクトルに応じた回転
	void CameraObject::Rotate( float dh, float dv )
	{
		m_LocalSpace.Rotate( dh, dv );
	}


	void CameraObject::Yaw( float Angle )
	{
		m_LocalSpace.Yaw( Angle );
	}


	void CameraObject::Pitch( float Angle )
	{
		m_LocalSpace.Pitch( Angle );
	}


	void CameraObject::Roll( float Angle )
	{
		m_LocalSpace.Roll( Angle );
	}


	void CameraObject::Translate( float df, float dh, float dv )
	{
		m_LocalSpace.Translate( df, dh, dv );
	}



	// TODO: ProjectionMatrixは毎回アップデートしなくてもいいはず、、、
	void CameraObject::Update()
	{
		//tcout << "CameraObject::Update()..." << endl;

		//m_refViewFrustum->SetPosition( *m_DerivedSpace.GetOrigin() );
		//m_refViewFrustum->SetDirection( *m_DerivedSpace.GetForwardAxis(), *m_DerivedSpace.GetVerticalAxis(), *m_DerivedSpace.GetHorizontalAxis() );
		m_refViewFrustum->SetViewParams( *m_DerivedSpace.GetOrigin(), *m_DerivedSpace.GetForwardAxis(), *m_DerivedSpace.GetVerticalAxis(), *m_DerivedSpace.GetHorizontalAxis() );

		// Update Frustum
		m_refViewFrustum->UpdateMatrix( FRUSTUM_UPDATE_VIEW | /*FRUSTUM_UPDATE_PROJECTION |*/ FRUSTUM_UPDATE_FRUSTUMPLANE );
	}


	void CameraObject::Info()
	{


	}


}// end of namespace