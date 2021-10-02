#include	"SimpleDeferredShader.h"

#include	<graphics/gl4x/scene/ViewTransformBuffer.h>
#include	<graphics/gl4x/scene/LightAttributeBuffer.h>
#include	<graphics/gl4x/resource/ShadowBuffer.h>



namespace OreOreLib
{
	// Default Constructor
	SimpleDeferredShader::SimpleDeferredShader()
	{
#ifdef	_DEBUG
		tcout << "SimpleDeferredShader()..." << tendl;
#endif // _DEBUG
		m_pShader				= NULL;
		m_ulTexDiffuse			= -1;
		m_ulTexPosition			= -1;
		m_ulTexDepth			= -1;
		m_ulTexNormal			= -1;
		m_ulTexIrradiance		= -1;
		m_ulShadowMapArray		= -1;
		m_ulUseIrradianceMap	= -1;

		m_bUseIrradianceMap		= false;
	
	}



	// Constructor
	SimpleDeferredShader::SimpleDeferredShader( const TCHAR *filepath, GLSL_VERSION version )
	{
#ifdef _DEBUG
		tcout << "SimpleDeferredShader( const TCHAR *filepath, GLSL_VERSION version )..." << tendl;
#endif // _DEBUG
		m_pShader				= NULL;
		m_ulTexDiffuse			= -1;
		m_ulTexPosition			= -1;
		m_ulTexDepth			= -1;
		m_ulTexNormal			= -1;
		m_ulTexIrradiance		= -1;
		m_ulShadowMapArray		= -1;
		m_ulUseIrradianceMap	= -1;

		m_bUseIrradianceMap		= false;

		if( filepath )
			InitShader( filepath, version );
	}



	// destructor
	SimpleDeferredShader::~SimpleDeferredShader()
	{
#ifdef	_DEBUG
		tcout << "~SimpleDeferredShader()..." << tendl;
#endif // _DEBUG
		Release();
	}



	void SimpleDeferredShader::Release()
	{
		SafeDelete( m_pShader );
		m_ulTexDiffuse			= -1;
		m_ulTexPosition			= -1;
		m_ulTexDepth			= -1;
		m_ulTexNormal			= -1;
		m_ulTexIrradiance		= -1;
		m_ulShadowMapArray		= -1;
		m_ulUseIrradianceMap	= -1;

		m_bUseIrradianceMap		= false;
	}



	// init shader
	void SimpleDeferredShader::InitShader( const TCHAR *filepath, GLSL_VERSION version )
	{
		// Create Shader
		m_pShader	= new GLShader();
		m_pShader->Init( filepath, version );

		// Init Attribute Locations
		GL_SAFE_CALL( glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );

		// Init Uniform Locations	
		m_ulTexDiffuse			= GL_SAFE_CALL( glGetUniformLocation( m_pShader->ID(), "g_TexDiffuse" ) );
		m_ulTexPosition			= GL_SAFE_CALL( glGetUniformLocation( m_pShader->ID(), "g_TexPosition" ) );
		m_ulTexDepth			= GL_SAFE_CALL( glGetUniformLocation( m_pShader->ID(), "g_TexDepth" ) );
		m_ulTexNormal			= GL_SAFE_CALL( glGetUniformLocation( m_pShader->ID(), "g_TexNormal" ) );
		m_ulTexIrradiance		= GL_SAFE_CALL( glGetUniformLocation( m_pShader->ID(), "g_TexIrradiance" ) );
		m_ulShadowMapArray		= GL_SAFE_CALL( glGetUniformLocation( m_pShader->ID(), "g_ShadowMapArray" ) );
		m_ulUseIrradianceMap	= GL_SAFE_CALL( glGetUniformLocation( m_pShader->ID(), "g_UseIrradianceMap" ) );


		// link shader
		m_pShader->Link();

		// その他
		// glBindFragDataLocation( program_id, channel, "FragColor0" );
		// サンプル
	}



	void SimpleDeferredShader::BindBufferObject( const IBufferObject* pbufferobj )
	{
		const type_info& id = typeid(*pbufferobj);

		if( id == typeid(ViewTransformBuffer) )
		{
			((ViewTransformBuffer *)pbufferobj)->BindToShader( m_pShader->ID() );
		}
		else if( id == typeid(LightAttributeBuffer) )
		{
			((LightAttributeBuffer *)pbufferobj)->BindToShader( m_pShader->ID() );
		}
		else if( id == typeid(ShadowBuffer) )
		{
			ShadowBuffer *pshadowbuf	= (ShadowBuffer *)pbufferobj;

			// Bind Uniform Buffer Objects
			pshadowbuf->BindToShader( m_pShader->ID() );

			// Bind ShadowmapArray Texture(シェーダーバインドするたびにテクスチャバインドしなおし?)
			m_pShader->Bind();

			glActiveTexture( GL_TEXTURE6 );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D_ARRAY, pshadowbuf->GetTexture()->texID ) );
			glUniform1i( m_ulShadowMapArray, 6 );

			m_pShader->Unbind();
		}


	}



	void SimpleDeferredShader::Render(	const Texture2D *texDiff,
										const Texture2D *texPos,
										const Texture2D *texDepth,
										const Texture2D *texNormal,
										const Texture2D *texIrrad )
	{
		if( !m_refScreenSpaceQuad )	return;

		m_pShader->Bind();
		{
			// Bind textures
			glActiveTexture( GL_TEXTURE0 );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, texDiff->texID ) );
			glUniform1i( m_ulTexDiffuse, 0 );

			glActiveTexture( GL_TEXTURE1 );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, texPos->texID ) );
			glUniform1i( m_ulTexPosition, 1 );

			glActiveTexture( GL_TEXTURE2 );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, texDepth->texID ) );
			glUniform1i( m_ulTexDepth, 2 );

			glActiveTexture( GL_TEXTURE3 );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, texNormal->texID ) );
			glUniform1i( m_ulTexNormal, 3 );
		
			glActiveTexture( GL_TEXTURE4 );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, texIrrad->texID ) );
			glUniform1i( m_ulTexIrradiance, 4 );

			// Set Uniform Variables
			GL_SAFE_CALL( glUniform1i( m_ulUseIrradianceMap, m_bUseIrradianceMap ) );


			// 描画
			m_refScreenSpaceQuad->Draw();

			// Unbind Textures
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, 0 );

			glActiveTexture( GL_TEXTURE1 );
			glBindTexture( GL_TEXTURE_2D, 0 );

			glActiveTexture( GL_TEXTURE2 );
			glBindTexture( GL_TEXTURE_2D, 0 );
		
			glActiveTexture( GL_TEXTURE3 );
			glBindTexture( GL_TEXTURE_2D, 0 );

			glActiveTexture( GL_TEXTURE4 );
			glBindTexture( GL_TEXTURE_2D, 0 );
		}
		m_pShader->Unbind();
	

	}



}// end of namespace