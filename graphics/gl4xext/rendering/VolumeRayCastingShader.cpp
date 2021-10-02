#include	"VolumeRayCastingShader.h"

#include	<oreore/common/TString.h>

#include	<graphics/gl4x/resource/ShadowBuffer.h>

#include	<graphics/gl4x/scene/Frustum.h>
#include	<graphics/gl4x/scene/ViewTransformBuffer.h>
#include	<graphics/gl4x/scene/LightAttributeBuffer.h>



namespace  OreOreLib
{

	// Default Constructor
	VolumeRayCastingShader::VolumeRayCastingShader()
	{
		m_ulMatVoxelSpace	= -1;
		m_ulShadowMapArray	= -1;
		m_ulMatLight		= -1;
	}



	// Constructor
	VolumeRayCastingShader::VolumeRayCastingShader( const TCHAR *filepath, GLSL_VERSION version )
	{
#ifdef _DEBUG		
		tcout << _T( "VolumeRayCastingShader( const TCHAR *filepath, GLSL_VERSION version )..." ) << tendl;
#endif	
		m_ulMatVoxelSpace	= -1;
		m_ulShadowMapArray	= -1;
		m_ulMatLight		= -1;

		if( filepath )
			InitShader( filepath, version );
	}


	// Destructor
	VolumeRayCastingShader::~VolumeRayCastingShader()
	{
#ifdef	_DEBUG
		tcout << _T( "~VolumeRayCastingShader()..." ) << tendl;
#endif
		Release();
	}


	void VolumeRayCastingShader::InitShader( const TCHAR *filepath, GLSL_VERSION version )
	{
		GLuint program_id;
		
		// Create Shader
		m_Shader.Init( filepath, version );
		program_id		= m_Shader.ID();

		// Init Attribute Locations
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );

		// Init Uniform Locations
		m_ulMatVoxelToWorld	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_MatWorld" ) );
		m_ulMatVoxelSpace	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_MatVoxelSpace" ) );
		m_ulShadowMapArray	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_ShadowMapArray" ) );
		m_ulMatLight		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_MatLight" ) );

		// Bind FragData Locations
		//glBindFragDataLocation( program_id,	RsmPosition,	"fPosition");	// ワールド座標
		//glBindFragDataLocation( program_id,	RsmNormal,		"fNormal");		// 法線
		//glBindFragDataLocation( program_id,	RsmFlux,		"fFlux");		// 放射束
		//glBindFragDataLocation( program_id,	RsmDepth,		"fDepth");		// 奥行き

		// Link Shader
		m_Shader.Link();
	}



	void VolumeRayCastingShader::Release()
	{
		m_Shader.Release();

		m_ulMatVoxelSpace	= -1;
		m_ulShadowMapArray	= -1;
		m_ulMatLight		= -1;
	}



	void VolumeRayCastingShader::BindBufferObject( const IBufferObject* pbufferobj )
	{
		const type_info &buffer_type	= typeid( *pbufferobj );
		//tcout << _T("class name: ") << buffer_type.name() << tendl;// .name()使うと、コンパイラが間違えてメモリリーク報告してくる@vs2010. vs2013だと治ってる. 2015.07.25


		if( buffer_type == typeid( ViewTransformBuffer ) )	// ビュー行列だった場合
		{
			( (ViewTransformBuffer *)pbufferobj )->BindToShader( m_Shader.ID() );
		}
		else if( buffer_type == typeid( LightAttributeBuffer ) )// 光源だった場合
		{
			( (LightAttributeBuffer *)pbufferobj )->BindToShader( m_Shader.ID() );
		}
		else if( buffer_type == typeid( ShadowBuffer ) )	// シャドウバッファの場合
		{
			ShadowBuffer *pshadowbuf = (ShadowBuffer *)pbufferobj;

			// Bind UniformBuffer to Shader
			pshadowbuf->BindToShader( m_Shader.ID() );

			// Bind Shadowmap texture
			m_Shader.Bind();
			{
				GL_SAFE_CALL( glActiveTexture( GL_TEXTURE0 ) );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D_ARRAY, pshadowbuf->GetTexture()->texID ) );
				GL_SAFE_CALL( glUniform1i( m_ulShadowMapArray, 0 ) );
			}
			m_Shader.Unbind();
		}
		else
		{
		}

	}



	void VolumeRayCastingShader::Render( Texture3D *pTexVolume, const Mat4f* pMatVoxlSpace, const Mat4f* pMatVoxlToWorldSpace )
	{
		if( !m_refUnitCube )
			return;

		m_Shader.Bind();
		{
			glUniformMatrix4fv( m_ulMatVoxelToWorld, 1, GL_FALSE, pMatVoxlToWorldSpace->m );
			glUniformMatrix4fv( m_ulMatVoxelSpace, 1, GL_FALSE, pMatVoxlSpace->m );

			m_refUnitCube->Draw();
		}
		m_Shader.Unbind();
	}


}