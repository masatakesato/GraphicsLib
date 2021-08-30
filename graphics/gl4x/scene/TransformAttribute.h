#ifndef TRANSFORM_ATTRIBUTE_H
#define	TRANSFORM_ATTRIBUTE_H

#include	<oreore/mathlib/GraphicsMath.h>

#include	"../resource/BufferLayout.h"



namespace OreOreLib
{

	class TransformAttribute
	{
	public:

		TransformAttribute();
		//TransformAttribute( BASIS_TYPE basis=BASIS_ZYX );
		~TransformAttribute(){}

		void SetOrigin( const Vec3f& origin )		{ m_Origin = origin; }
		void SetOrigin( float x, float y, float z )	{ InitVec( m_Origin, x, y, z ); }

		void SetBasis( const Vec3f& vecX, const Vec3f& vecY, const Vec3f& vecZ );
		void SetBasis( float vecX_x, float vecX_y, float vecX_z, float vecY_x, float vecY_y, float vecY_z, float vecZ_x, float vecZ_y, float vecZ_z );

		void SetOrientation( const Quatf& quat );
		void SetScale( const Vec3f &scale )			{ m_Scale = scale; }
		void SetScale( float x, float y, float z )	{ InitVec( m_Scale, x, y, z ); }
	
		void SetDirection( float dirx, float diry, float dirz, float upx, float upy, float upz );
		void SetDirection( const Vec3f &dir, const Vec3f &up );
		void SetDirection( float dirx, float diry, float dirz, float upx, float upy, float upz, float horx, float hory, float horz );
		void SetDirection( const Vec3f &dir, const Vec3f &up, const Vec3f &hor );
		
		void Rotate( float Angle, float x, float y, float z );	// 任意軸周りにカメラを回転する
		void Rotate( float dh, float dv );						// 視点移動でカメラを回転する

		void Yaw( float Angle );
		void Pitch( float Angle );
		void Roll( float Angle );
		void Translate( float df, float dh, float dv );		// カメラを移動する

		void Transform( const Mat4f& matTransform );

		Vec3f* GetOrigin() const				{ return (Vec3f *)&m_Origin; }
		Vec3f *GetForwardAxis() const			{ return (Vec3f *)&m_TransformedBasis[Z_SPACE]; }
		Vec3f *GetVerticalAxis() const			{ return (Vec3f *)&m_TransformedBasis[Y_SPACE]; }
		Vec3f *GetHorizontalAxis() const		{ return (Vec3f *)&m_TransformedBasis[X_SPACE]; }
		Vec3f* GetScale() const					{ return (Vec3f *)&m_Scale; }
		Quatf* GetQuaternion() const			{ return (Quatf *)&m_Quaternion; }


	private:

		Vec3f	m_Origin;
		Vec3f	m_Scale;
		Quatf	m_Quaternion;

//int		m_DirU, m_DirV, m_DirN;	// U:horizontal, V: vertical, N: forward
		Vec3f	m_InitialBasis[3];	// 初期化時の基底ベクトル
		Vec3f	m_TransformedBasis[3];	// 回転を作用させた基底ベクトル

		enum AXIS{ X_SPACE, Y_SPACE, Z_SPACE, NUM_SPACE };



		void UpdateBasis();


		friend struct SceneNode;
	};


}// end of namespace





#endif // !TRANSFORM_ATTRIBUTE_H
