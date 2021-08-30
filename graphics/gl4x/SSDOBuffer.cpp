#include	"SSDOBuffer.h"


namespace OreOreLib
{

#define DEFAULT_OCCL_RADIUS		0.279710f
#define DEFAULT_MAX_OCCL_RADIUS	0.639419f


	SSDOBuffer::SSDOBuffer()
	{
		m_Param.OcclusionRadius			= DEFAULT_OCCL_RADIUS;
		m_Param.MaxOcclusionDistance	= DEFAULT_MAX_OCCL_RADIUS;
		InitVec( m_Param.TexSize, 0.0f, 0.0f );
//		InitVec( m_Param.ViewPosition, 0.0f, 0.0f, 0.0f );	// いずれいらなくなる
	}



	SSDOBuffer::SSDOBuffer( int texwidth, int texheight )
	{
		Init( texwidth, texheight );
	}



	SSDOBuffer::~SSDOBuffer()
	{
		Release();
	}

	

	void SSDOBuffer::Init( int texwidth, int texheight )
	{
		m_Param.OcclusionRadius			= DEFAULT_OCCL_RADIUS;
		m_Param.MaxOcclusionDistance	= DEFAULT_MAX_OCCL_RADIUS;
		InitVec( m_Param.TexSize, float(texwidth), float(texheight) );
		//InitVec( m_Param.ViewPosition, 0.0f, 0.0f, 0.0f );	// いずれいらなくなる

		m_TexOcclusionSH.Allocate( texwidth, texheight, 0, 0, DATA_FORMAT::FORMAT_R16G16B16A16_FLOAT );
		m_TexOcclusionSH.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );
		m_TexOcclusionSH.GenHardwareTexture();

		m_TexBlurBuffer.Allocate( texwidth, texheight, 0, 0, DATA_FORMAT::FORMAT_R16G16B16A16_FLOAT );
		m_TexBlurBuffer.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );
		m_TexBlurBuffer.GenHardwareTexture();

		m_TexFinalOcclusion.Allocate( texwidth, texheight, 0, 0, DATA_FORMAT::FORMAT_R16G16B16A16_FLOAT );
		m_TexFinalOcclusion.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );
		m_TexFinalOcclusion.GenHardwareTexture();
	}
	


	void SSDOBuffer::Release()
	{
		//SSDOParams	m_Param;
		m_TexOcclusionSH.Release();
		m_TexBlurBuffer.Release();
		m_TexFinalOcclusion.Release();
	}



	//void SSDOBuffer::BindToShader( GLuint program_id )
	//{
	//
	//
	//}


	//void SSDOBuffer::UpdateSubresource()
	//{
	//	m_uboTransformMatrix.BindToShaderByName( program_id, m_TransformUniformBlockNames[0] );
	//	m_uboAttributes.BindToShaderByName( program_id, m_TransformUniformBlockNames[1] );
	//}



}// end of namespace