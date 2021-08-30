#include	"LightAttributeBuffer.h"


#include	"./common/Utility.h"
#include	"Defines.h"
#include	"MathLib.h"
#include	"GLBindPointManager.h"


namespace OreOreLib
{

	//const tstring	LightAttributeBuffer::m_LightSourceUniformBlockNames[UB_PACKED_LIGHT_NUM] =
	//{
	//	_T( "ubPackedPosAttrib" ),
	//	_T( "ubPackedColorDropoff" ),
	//	_T( "ubPackedDirConeAngle" ),
	//	_T( "ubPackedAttenRadius" ),
	//};

	const char	*LightAttributeBuffer::m_LightSourceUniformBlockNames[UB_PACKED_LIGHT_NUM] =
	{
		"ubPackedPosAttrib",
		"ubPackedColorDropoff",
		"ubPackedDirConeAngle",
		"ubPackedAttenRadius",
	};



	GLuint			LightAttributeBuffer::m_BindPoints[UB_PACKED_LIGHT_NUM] = { 0, 0, 0, 0 };



	// default constructor
	LightAttributeBuffer::LightAttributeBuffer()
	{
		m_PackedPosAttrib		= NULL;
		m_PackedColorDropoff	= NULL;
		m_PackedDirConeAngle	= NULL;
		m_PackedAttenRadius		= NULL;

		m_ReferredObjects		= NULL;
		//m_refShadowAttachments	= NULL;
	}



	// destructor
	LightAttributeBuffer::~LightAttributeBuffer()
	{
		Release();
	}


	//// copy constructor
	//LightAttributeBuffer::LightAttributeBuffer( const LightAttributeBuffer& obj )
	//{
	//	m_numMaxLights		= obj.m_numMaxLights;
	//	m_numActiveLights	= obj.m_numActiveLights;

	//	Init( m_numMaxLights );

	//	memcpy( m_PackedPosAttrib, obj.m_PackedPosAttrib, sizeof(PosAttrib) * m_numMaxLights );
	//	memcpy( m_PackedColorDropoff, obj.m_PackedColorDropoff, sizeof(ColorDropoff) * m_numMaxLights );
	//	memcpy( m_PackedDirConeAngle, obj.m_PackedDirConeAngle, sizeof(DirConeAngle) * m_numMaxLights );
	//	memcpy( m_PackedAttenRadius, obj.m_PackedAttenRadius, sizeof(AttenRadius) * m_numMaxLights );
	//}



	void LightAttributeBuffer::Init( int num_max_lights )
	{
		Release();

		m_CacheManager.Init( Min( num_max_lights, (int)MAX_LIGHTS ) );

		m_PackedPosAttrib		= new PosAttrib[ num_max_lights ];
		m_PackedColorDropoff	= new ColorDropoff[ num_max_lights ];
		m_PackedDirConeAngle	= new DirConeAngle[ num_max_lights ];
		m_PackedAttenRadius		= new AttenRadius[ num_max_lights ];

		memset( m_PackedPosAttrib, 0, sizeof(PosAttrib) * num_max_lights );
		memset( m_PackedColorDropoff, 0, sizeof(ColorDropoff) * num_max_lights );
		memset( m_PackedDirConeAngle, 0, sizeof(DirConeAngle) * num_max_lights );
		memset( m_PackedAttenRadius, 0, sizeof(AttenRadius) * num_max_lights );


		for( int i=0; i<UB_PACKED_LIGHT_NUM; ++i )
		{
			if( m_BindPoints[i] == 0 )	m_BindPoints[i]	= GLBindPointManager::Reserve( GL_UNIFORM_BUFFER );
			m_UniformBlocks[i].Init( GL_STREAM_DRAW, m_BindPoints[i] );
		}

		m_ReferredObjects	= new MovableObject*[ num_max_lights ];
		memset( m_ReferredObjects, 0, sizeof(MovableObject*) * num_max_lights );

	}



	void LightAttributeBuffer::Release()
	{
		for( int i=0; i<m_CacheManager.numMaxSlots(); ++i )		m_ReferredObjects[i] = NULL;
		SafeDeleteArray( m_ReferredObjects );

		m_CacheManager.Release();

		SafeDeleteArray( m_PackedPosAttrib );
		SafeDeleteArray( m_PackedColorDropoff );
		SafeDeleteArray( m_PackedDirConeAngle );
		SafeDeleteArray( m_PackedAttenRadius );

		for( int i=0; i<UB_PACKED_LIGHT_NUM; ++i )
			m_UniformBlocks[i].Release();

		//m_refShadowAttachments	= NULL;

	}



	void LightAttributeBuffer::BindToShader( GLuint program_id )
	{
		( m_UniformBlocks + UB_PACKED_LIGHT_POS_ATTRIB )->BindToShaderByName( program_id, m_LightSourceUniformBlockNames[ UB_PACKED_LIGHT_POS_ATTRIB ] );
		( m_UniformBlocks + UB_PACKED_LIGHT_COLOR_DROPOFF )->BindToShaderByName( program_id,  m_LightSourceUniformBlockNames[ UB_PACKED_LIGHT_COLOR_DROPOFF ] );
		( m_UniformBlocks + UB_PACKED_LIGHT_DIR_CONEANGLE )->BindToShaderByName( program_id,  m_LightSourceUniformBlockNames[ UB_PACKED_LIGHT_DIR_CONEANGLE ] );
		( m_UniformBlocks + UB_PACKED_LIGHT_ATTEN_RADIUS )->BindToShaderByName( program_id,  m_LightSourceUniformBlockNames[ UB_PACKED_LIGHT_ATTEN_RADIUS ] );
	}



	bool LightAttributeBuffer::SetLightAttribute( int slot_id, const FrustumAttribute& attribFrustum, const LightAttribute& attribLight, uint8 numshadows, short shadowcaster_idx, short lightmat_idx )
	{
		if( slot_id >= m_CacheManager.numMaxSlots() )	return false;

		SetDirty( slot_id );

		// Pack Positon and lighttype/numshadows/shadowmapid
		( m_PackedPosAttrib + slot_id )->pos			= attribFrustum.m_Position;
		//( m_PackedPosAttrib + slot_id )->attrib			= attribLight.LightType<<24 | numshadows<<16 | shadowcaster_idx;// TODO: 機能拡張
		( m_PackedPosAttrib + slot_id )->attrib			=
			PackLightTypeBits( attribLight.LightType )			|
			PackEmitDiffuseBits( attribLight.EmitDiffuse )		|
			PackEmitSpecularBits( attribLight.EmitSpecular )	|
			PackNumShadowsBits( numshadows )					|
			PackShadowCasterIndexBits( shadowcaster_idx )		|
			PackLightMatIndexBits( lightmat_idx );

		// Pack ColorIntensity and Dropoff
		Scale( ( m_PackedColorDropoff + slot_id )->colorintensity, attribLight.Color, attribLight.Intensity );
		( m_PackedColorDropoff + slot_id )->dropoff		= attribLight.Dropoff;

		// Pack Direction and Cone Angle
		( m_PackedDirConeAngle + slot_id )->dir			= attribFrustum.m_Forward;
		( m_PackedDirConeAngle + slot_id )->coneangle	= attribLight.ConeAngle;

		// Pack Attetunation
		( m_PackedAttenRadius + slot_id )->constatten	= attribLight.ConstantAttenuation;
		( m_PackedAttenRadius + slot_id )->linearAtten	= attribLight.LinearAttenuation;
		( m_PackedAttenRadius + slot_id )->quadratten	= attribLight.QuadraticAttenuation;
		( m_PackedAttenRadius + slot_id )->radius		= attribLight.Radius;

		return true;
	}



	bool LightAttributeBuffer::SetShadowCasterIndex( int slot_id, uint8 numshadows, short shadowcaster_idx )
	{
		if( slot_id >= m_CacheManager.numMaxSlots() )	return false;

		SetDirty( slot_id );

		( m_PackedPosAttrib+slot_id )->attrib	= (m_PackedPosAttrib + slot_id)->attrib | PackShadowCasterIndexBits( shadowcaster_idx );
		
		return true;
	}



	bool LightAttributeBuffer::SetLightMatrixIndex( int slot_id, short lightmat_idx )
	{
		if( slot_id >= m_CacheManager.numMaxSlots() )	return false;

		SetDirty( slot_id );

		uint32	lightmat_idx_sign = lightmat_idx < 0 ? 1 : 0;
		( m_PackedPosAttrib+slot_id )->attrib	= (m_PackedPosAttrib + slot_id)->attrib | PackLightMatIndexBits( lightmat_idx );

		return true;
	}
			


	void LightAttributeBuffer::UpdateSubresource()
	{
		int num_activeslots = m_CacheManager.numMaxSlots();//m_CacheManager.numReservedSlots();

		if( num_activeslots == 0 )	return;

		( m_UniformBlocks + UB_PACKED_LIGHT_POS_ATTRIB )->UpdateSubresource( m_PackedPosAttrib, sizeof(PosAttrib) * num_activeslots );
		( m_UniformBlocks + UB_PACKED_LIGHT_COLOR_DROPOFF )->UpdateSubresource( m_PackedColorDropoff, sizeof(ColorDropoff) * num_activeslots );
		( m_UniformBlocks + UB_PACKED_LIGHT_DIR_CONEANGLE )->UpdateSubresource( m_PackedDirConeAngle, sizeof(DirConeAngle) * num_activeslots );
		( m_UniformBlocks + UB_PACKED_LIGHT_ATTEN_RADIUS )->UpdateSubresource( m_PackedAttenRadius, sizeof(AttenRadius) * num_activeslots );
	}


}// end of namespace