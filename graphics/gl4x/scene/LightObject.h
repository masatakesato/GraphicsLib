#ifndef LIGHT_OBJECT_H
#define	LIGHT_OBJECT_H

#include	"MovableObject.h"


namespace OreOreLib
{

	// TODO: NodeにアタッチされてたらTransformを継承する 2013.07.06

	// 光源の拡大縮小/
	struct LightObject : MovableObject
	{
	public:

		LightObject();
		LightObject( LightAttribute *lattr, Frustum *lfrustum=NULL );
		~LightObject();

		void Init( LightAttribute *lattr, Frustum *lfrustum=NULL );
		void Release();

		void BindFrustum( Frustum* frustum );
		void UnbindFrustum();

		inline LightAttribute *GetLightAttrib() const	{ return (LightAttribute *)&m_LightAttrib; }
		inline Frustum *GetFrustum() const				{ return m_refLightFrustum; }
		inline ViewTransformMatrix *GetMatrix() const	{ return m_refLightFrustum->GetTransformMatrix(); }
		inline void GetMatrix( ViewTransformMatrix* cammat ) const	{ m_refLightFrustum->GetTransformMatrix(); }

		// Get LightStatus Flag
		inline int LightType() const			{ return m_LightAttrib.LightType; }
		inline bool EnableLighting() const		{ return m_LightAttrib.Enable; }
		inline bool EmitDiffuse() const			{ return m_LightAttrib.EmitDiffuse; }
		inline bool EmitSpecular() const		{ return m_LightAttrib.EmitSpecular; }
		inline bool CastShadow()  const			{ return m_LightAttrib.CastShadow; }


		// Set Light Status
		inline void SetLightType( LIGHT_TYPE type )	{ m_LightAttrib.LightType = type; }
		inline void SetEnableLighting( bool flag )	{ m_LightAttrib.Enable = flag; }
		inline void SetEmitDiffuse( bool flag )		{ m_LightAttrib.EmitDiffuse = flag; }
		inline void SetEmitSpecular( bool flag )	{ m_LightAttrib.EmitSpecular = flag; }
		inline void SetCastShadow( bool flag )		{ m_LightAttrib.CastShadow = flag; }


		// Set Light Params
		inline void SetColor( const Vec3f &color )							{ m_LightAttrib.Color	= color; }
		inline void SetColor( float color_r, float color_g, float color_b )	{ InitVec( m_LightAttrib.Color, color_r, color_g, color_b ); }
		inline void SetIntensity( float intensity )							{ m_LightAttrib.Intensity	= intensity; }
		inline void SetConstantAttenuation( float attn )					{ m_LightAttrib.ConstantAttenuation = attn; }
		inline void SetLinearAttenuation( float attn )						{ m_LightAttrib.LinearAttenuation	= attn; }
		inline void SetQuadraticAttenuation( float attn )					{ m_LightAttrib.QuadraticAttenuation	= attn; }
		inline void SetConeAngle( float angle )								{ m_LightAttrib.ConeAngle	= angle; m_refLightFrustum->SetFov( angle ); }
		inline void SetDropoff( float dropoff )								{ m_LightAttrib.Dropoff	= dropoff; }
		inline void SetRadius( float radius )								{ m_LightAttrib.Radius = radius; m_refLightFrustum->SetZFar( radius ); }
		inline void SetNear( float znear )									{ m_refLightFrustum->SetZNear( znear ); }

		//inline void SetScale( const Vec3f &scale )							{ m_LocalSpace.SetScale( scale ); }	// Scaling Light Geometry. Arealight Only
		//inline void SetScale( float x, float y, float z )					{ m_LocalSpace.SetScale( x, y, z ); }// Scaling Light Geometry. Arealight Only

// TODO: 不要? 2015.012.06
		//inline void SetPosition( const Vec3f &pos )
		//{
		//	// フラスタムのパラメータを設定する
		//	m_refLightFrustum->SetPosition( pos );

		//	// Nodeに設定を反映する
		//	SetTranslation( pos );
		//}


		inline void SetDirection( const Vec3f &dir, const Vec3f &up )
		{
			m_LocalSpace.SetDirection( dir, up );
		}

		inline void SetViewParams( float posx, float posy, float posz, float dirx, float diry, float dirz, float upx, float upy, float upz )
		{
			m_LocalSpace.SetOrigin( posx, posy, posz );
			m_LocalSpace.SetDirection( dirx, diry, dirz, upx, upy, upz );
		}

		inline void SetViewParams( const Vec3f &pos, const Vec3f &dir, const Vec3f &up )
		{
			m_LocalSpace.SetOrigin( pos );
			m_LocalSpace.SetDirection( dir, up );
		}

		inline void SetViewParams( const Vec3f &pos, const Vec3f &dir, const Vec3f &up, const Vec3f &hor )
		{
			m_LocalSpace.SetOrigin( pos );
			m_LocalSpace.SetDirection( dir, up, hor );
		}

		inline void Rotate( float dh, float dv )
		{
			m_LocalSpace.Rotate( dh, dv );
		}

		//	inline void Yaw(float Angle);
		//	inline void Pitch(float Angle);
		inline void Roll( float Angle )
		{
			m_LocalSpace.Roll( Angle );
		}


		virtual void Update();// Update Light Object

		virtual void Info();

		// 点光源の減衰 http://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/	// TODO: シェーダーの点光源処理へ移動する
		//float F_att( float d ) const	{ return 1.0f / ( m_Attrib.ConstantAttenuation + m_Attrib.LinearAttenuation*d + m_Attrib.QuadraticAttenuation*d*d ); };


	private:

		LightAttribute	m_LightAttrib;		// light attribute
		Frustum			*m_refLightFrustum;	// reference to light frustum
		//LightGeometry		m_LightGeom;// light geometry


		friend class RenderScene;
	};


}// end of namespace



#endif // !LIGHT_OBJECT_H
