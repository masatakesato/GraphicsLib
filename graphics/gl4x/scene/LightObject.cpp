#include	"LightObject.h"


namespace OreOreLib
{

	LightObject::LightObject()
	{
		m_refLightFrustum					= NULL;
		m_LightAttrib.LightType				= LIGHT_TYPE_UNKNOWN;
		m_LightAttrib.Enable				= true;
		m_LightAttrib.EmitDiffuse			= true;
		m_LightAttrib.EmitSpecular			= true;
		m_LightAttrib.CastShadow			= false;

		InitVec( m_LightAttrib.Color, 1.0f, 1.0f, 1.0f );
		m_LightAttrib.Intensity				= 1.0f;
		m_LightAttrib.Dropoff				= 0.0f;
		m_LightAttrib.ConeAngle				= 0.0f;
		m_LightAttrib.ConstantAttenuation	= 0.0f;
		m_LightAttrib.LinearAttenuation		= 0.0f;
		m_LightAttrib.QuadraticAttenuation	= 0.0f;
		m_LightAttrib.Radius				= 0.0f;
	}



	LightObject::LightObject( LightAttribute *lattr, Frustum *lfrustum/*=NULL*/ )
	{
		Init( lattr, lfrustum );
	}


	LightObject::~LightObject()
	{
		Release();
	}


	void LightObject::Init( LightAttribute *lattr, Frustum *lfrustum/*=NULL*/ )
	{
		//m_NodeType		= NODE_LIGHT;

		//================= Attach LightAttribute ==================//
		if( lattr )
		{
			m_LightAttrib	= *lattr;
		}
		else
		{
			m_LightAttrib.LightType				= LIGHT_TYPE_UNKNOWN;
			m_LightAttrib.Enable				= true;
			m_LightAttrib.EmitDiffuse			= true;
			m_LightAttrib.EmitSpecular			= true;
			m_LightAttrib.CastShadow			= false;

			InitZero( m_LightAttrib.Color );
			m_LightAttrib.Intensity				= 0.0f;
			m_LightAttrib.Dropoff				= 0.0f;
			m_LightAttrib.ConeAngle				= 0.0f;
			m_LightAttrib.ConstantAttenuation	= 0.0f;
			m_LightAttrib.LinearAttenuation		= 0.0f;
			m_LightAttrib.QuadraticAttenuation	= 0.0f;
			m_LightAttrib.Radius				= 100.0f;
		}

		//================= Attach Lightview Camera ================//
		m_refLightFrustum	= lfrustum;

		if( m_refLightFrustum )
		{
			m_refLightFrustum->SetFov( m_LightAttrib.ConeAngle );
			m_refLightFrustum->SetZNear( 0.1f );
			m_refLightFrustum->SetZFar( m_LightAttrib.Radius );
		}
	}


	void LightObject::Release()
	{
		m_refLightFrustum	= NULL;

		m_LightAttrib.LightType				= LIGHT_TYPE_UNKNOWN;
		m_LightAttrib.Enable				= true;
		m_LightAttrib.EmitDiffuse			= true;
		m_LightAttrib.EmitSpecular			= true;
		m_LightAttrib.CastShadow			= false;

		InitZero( m_LightAttrib.Color );
		m_LightAttrib.Intensity				= 0.0f;
		m_LightAttrib.Dropoff				= 0.0f;
		m_LightAttrib.ConeAngle				= 0.0f;
		m_LightAttrib.ConstantAttenuation	= 0.0f;
		m_LightAttrib.LinearAttenuation		= 0.0f;
		m_LightAttrib.QuadraticAttenuation	= 0.0f;
		m_LightAttrib.Radius				= 0.0f;
	}


	void LightObject::BindFrustum( Frustum* frustum )
	{
		if( !frustum )	return;
		m_refLightFrustum	= frustum;
	}


	void LightObject::UnbindFrustum()
	{
		m_refLightFrustum	= NULL;
	}


	void LightObject::Info()
	{

	}


	void LightObject::Update()
	{
		//tcout << "LightObject::Update()..." << endl;
		
		// TODO: 親ノードからのトランスフォームを継承したい. matWorldかければいい? 2015.02.05
		//m_refLightFrustum->SetPosition( *m_LocalSpace.GetOrigin() );
		//m_refLightFrustum->SetDirection( *m_LocalSpace.GetForwardAxis(), *m_LocalSpace.GetVerticalAxis(), *m_LocalSpace.GetHorizontalAxis() );

		m_refLightFrustum->SetViewParams( *m_DerivedSpace.GetOrigin(), *m_DerivedSpace.GetForwardAxis(), *m_DerivedSpace.GetVerticalAxis(), *m_DerivedSpace.GetHorizontalAxis() );
		//m_refLightFrustum->SetPosition( *m_DerivedSpace.GetOrigin() );
		//m_refLightFrustum->SetDirection( *m_DerivedSpace.GetForwardAxis(), *m_DerivedSpace.GetVerticalAxis(), *m_DerivedSpace.GetHorizontalAxis() );

		m_refLightFrustum->UpdateMatrix( FRUSTUM_UPDATE_VIEW |/* FRUSTUM_UPDATE_PROJECTION |*/ FRUSTUM_UPDATE_FRUSTUMPLANE );
	}

}// end of namespace