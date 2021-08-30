#ifndef	FRUSTUM_H
#define	FRUSTUM_H

#include	<oreore/mathlib/GraphicsMath.h>

#include	"../resource/BufferLayout.h"
#include	"../scene/TransformAttribute.h"



namespace OreOreLib
{

	// Frustum class
	class Frustum
	{
	public:

		Frustum();
		~Frustum();

		void SetAttribute( const FrustumAttribute &cam_attr );

		void SetViewParams( float posx, float posy, float posz, float dirx, float diry, float dirz, float upx, float upy, float upz );
		void SetViewParams( const Vec3f &pos, const Vec3f &dir, const Vec3f &up );
		void SetViewParams( float posx, float posy, float posz, float dirx, float diry, float dirz, float upx, float upy, float upz, float horx, float hory, float horz );		// Set View Parameter
		void SetViewParams( const Vec3f &pos, const Vec3f &dir, const Vec3f &up, const Vec3f &hor );
		
		void SetPosition( float posx, float posy, float posz );
		void SetPosition( const Vec3f &pos );

		void SetDirection( float dirx, float diry, float dirz, float upx, float upy, float upz );
		void SetDirection( const Vec3f &dir, const Vec3f &up );
		void SetDirection( float dirx, float diry, float dirz, float upx, float upy, float upz, float horx, float hory, float horz );
		void SetDirection( const Vec3f &dir, const Vec3f &up, const Vec3f &hor );

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

		FrustumAttribute *GetAttribute() const	{ return (FrustumAttribute *)&m_FrustumAttrib; }
		Vec3f *GetPosition() const				{ return (Vec3f *)&m_FrustumAttrib.m_Position; }
		Vec3f *GetDirection() const				{ return (Vec3f *)&m_FrustumAttrib.m_Forward; }
		Vec3f *GetVertical() const				{ return (Vec3f *)&m_FrustumAttrib.m_Vertical; }
		Vec3f *GetHorizontal() const			{ return (Vec3f *)&m_FrustumAttrib.m_Horizontal; }
		float *GetFov() const					{ return (float *)&m_FrustumAttrib.m_Fov; }
		float *GetAspectRatio() const			{ return (float *)&m_FrustumAttrib.m_AspectRatio; }
		float *GetZNear() const					{ return (float *)&m_FrustumAttrib.m_zNear; }
		float *GetZFar() const					{ return (float *)&m_FrustumAttrib.m_zFar; }
		float GetDepth() const					{ return m_FrustumAttrib.m_zFar - m_FrustumAttrib.m_zNear; }
		bool IsOrthographic() const				{ return m_FrustumAttrib.m_Fov<1.0e-6f; }

		ViewTransformMatrix *GetTransformMatrix() const	{ return (ViewTransformMatrix *)&m_Transform; }
		Mat4f *GetViewMatrix() const					{ return (Mat4f *)&m_Transform.matView; }
		Mat4f *GetProjectionMatrix() const				{ return (Mat4f *)&m_Transform.matProjection; }
		Mat4f *GetViewProjectionMatrix() const			{ return (Mat4f *)&m_Transform.matViewProjection; }
		Mat4f *GetInvViewProjectionMatrix() const		{ return (Mat4f *)&m_Transform.matInvViewProjection; }

		void GetCornerPoints( Vec3f* corner_points ) const	{ memcpy( corner_points, m_FrustumCornerPoints, sizeof(Vec3f)*8 ); }
		
		void GetSlicePoints( Vec3f* slice_points, float slicedepth ) const;

		bool PointInFrustum( const Vec3f& p ) const;						// フラスタム内に頂点が存在するかどうか調べる
		bool SphereInFrustum( const Vec3f& center, float radius ) const;	// フラスタム内に球が存在するかどうか調べる
		bool AABBInFrustum( const Vec3f& boxMin, const Vec3f& boxMax ) const;// フラスタム内にAABB(ワールド空間)が存在するかどうか調べる

		void UpdateMatrix( uint32 mode );


	protected:
		
		FrustumAttribute		m_FrustumAttrib;	// view frtustum attribute
		ViewTransformMatrix		m_Transform;

		Quatf					m_Quaternion;		// Rotation
		
		Vec4f					m_FrustumPlane[6];	// フラスタム平面情報. xyz: 面法線ベクトル、w:原点からの距離
		Vec3f					m_FrustumCornerPoints[8];


		void UpdateBasis();


	private:

		void NormalizePlane( Vec4f* frustumPlane );
		void CalculateFrustumPlane( const Mat4f& matViewProj );
		void CalculateFrustumCornerPoints( Vec3f* corner_points ) const;

		friend struct CameraObject;
		friend struct LightObject;
	};


}// end of namespace


#endif	// FRUSTUM_H //