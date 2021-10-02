#include	"SSDOShader.h"

#include	<graphics/gl4x/resource/GLBindPointManager.h>
#include	<graphics/gl4x/scene/ViewTransformBuffer.h>



namespace OreOreLib
{

#define DEFAULT_NOISE_TEX_SIZE	4
	

	GLuint	SSDOShader::m_BindPoint	= 0;


	SSDOShader::SSDOShader()
	{
#ifdef _DEBUG
		tcout << _T( "SSDOShader()..." ) << tendl;
#endif	
		m_pShader			= NULL;

		m_ulBlurDir			= -1;
		m_ulTexPosition		= -1;
		m_ulTexNormal		= -1;
		m_ulTexNoise		= -1;
		m_ulTexOcclusion	= -1;
		m_ulTexBlur			= -1;
		m_ulTexFinal		= -1;

		m_refTexPosition	= NULL;
		m_refTexNormal		= NULL;
		m_refSSDOBuffer		= NULL;
	}


	// Constructor
	SSDOShader::SSDOShader( const TCHAR *filepath, GLSL_VERSION version )
	{
#ifdef _DEBUG
		tcout << _T( "SSDOShader( const TCHAR *filepath, GLSL_VERSION version )..." ) << tendl;
#endif
		//const type_info &id	= typeid(*this);
		//tcout << "class name: " << id.name() << endl;// .name()使うと、コンパイラが間違えてメモリリーク報告してくる. 2013.08.05

		m_pShader			= NULL;

		m_ulBlurDir			= -1;
		m_ulTexPosition		= -1;
		m_ulTexNormal		= -1;
		m_ulTexNoise		= -1;
		m_ulTexOcclusion	= -1;
		m_ulTexBlur			= -1;
		m_ulTexFinal		= -1;

		m_refTexPosition	= NULL;
		m_refTexNormal		= NULL;
		m_refSSDOBuffer		= NULL;

		if( filepath )
			InitShader( filepath, version );
	}


	// Destructor
	SSDOShader::~SSDOShader()
	{
#ifdef _DEBUG
		tcout << _T("~SSDOShader()...") << tendl;
#endif
		Release();
	}



	void SSDOShader::InitShader( const TCHAR *filepath, GLSL_VERSION version )
	{		
		// create shader
		m_pShader	= new GLShader();
		m_pShader->Init( filepath, version/*, Defines*/ );
		GLuint program_id	= m_pShader->ID();
		
		// initialize attribute location
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );

		// initialize uniform location
		m_ulTexFinal		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_TexFinal" ) );

		// initialize output fragment location
		GL_SAFE_CALL( glBindFragDataLocation( program_id, FRAG_LOCATION_COLOR, "outColor" ) );

		// link shader
		m_pShader->Link();


		//========================= Initialize Noise Texture =============================//
		InitNoiseTexture( DEFAULT_NOISE_TEX_SIZE, DEFAULT_NOISE_TEX_SIZE );


		//====================== Initialize Uniform Buffer Object ========================//
		if( m_BindPoint==0 )	m_BindPoint	= GLBindPointManager::Reserve( GL_UNIFORM_BUFFER );
		m_uboParams.Init( GL_STATIC_DRAW, m_BindPoint );
		m_uboParams.BindToShaderByName( program_id, "ubSSDOParams" );


		//========================= Initialize Occlusion_SH pass =========================//
		GL_SHADER_MACRO Defines[] =	{ GL_SHADER_MACRO( _T(""), _T("") ), GL_SHADER_MACRO( _T(""), _T("") ), };

		// create shader
		Defines[0].Name	= _T("PASS_OCCLUSION_SH");
		m_Pass[SHADER_PASS_OCCLUSION_SH].Init( filepath, version, Defines );
		program_id	= m_Pass[ SHADER_PASS_OCCLUSION_SH ].ID();

		// initialize attribute location
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );

		// initialize uniform location
		m_ulTexPosition		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_TexPosition" ) );
		m_ulTexNormal		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_TexNormal" ) );
		m_ulTexNoise		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_TexNoise" ) );

		// initialize output fragment location
		GL_SAFE_CALL( glBindFragDataLocation( program_id, FRAG_LOCATION_COLOR, "outColor" ) );

		// link shader
		m_Pass[ SHADER_PASS_OCCLUSION_SH ].Link();

		m_uboParams.BindToShaderByName( program_id, "ubSSDOParams" );


		//======================= Initialize Horizontal Blur pass ========================//
		Defines[0].Name = _T("PASS_BLUR");
		// create shader
		m_Pass[SHADER_PASS_BLUR].Init( filepath, version, Defines );
		program_id			= m_Pass[ SHADER_PASS_BLUR ].ID();

		// initialize attribute location
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );

		// initialize uniform location
		m_ulBlurDir			= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_BlurDir" ) );
		m_ulTexOcclusion	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_TexOcclusion" ) );

		// initialize output fragment location
		GL_SAFE_CALL( glBindFragDataLocation( program_id, FRAG_LOCATION_COLOR, "outColor" ) );

		// link shader
		m_Pass[ SHADER_PASS_BLUR ].Link();

		m_uboParams.BindToShaderByName( program_id, "ubSSDOParams" );

		/*
		//======================= Initialize Vertical Blur pass ========================//
		// create shader
		m_Pass[ SHADER_PASS_BLUR_VERTICAL ].Init( filepath, &GL_SHADER_MACRO( "PASS_BLUR_VERTICAL", "" ) );
		program_id			= m_Pass[ SHADER_PASS_BLUR_VERTICAL ].ID();

		// initialize attribute location
		glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" );
		glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" );

		// initialize uniform location
		m_ulTexBlur			= glGetUniformLocation( program_id, "g_TexBlur" );

		// initialize output fragment location
		glBindFragDataLocation( program_id, FRAG_LOCATION_COLOR, "outColor" );

		// link shader
		m_Pass[ SHADER_PASS_BLUR_VERTICAL ].Link();

		m_uboParams.BindToShaderByName( program_id, "ubSSDOParams" );
		*/

	}



	void SSDOShader::BindBufferObject( const IBufferObject* pbufferobj )
	{
		const type_info &buffer_type	= typeid(*pbufferobj);
		
		if( buffer_type == typeid(ViewTransformBuffer) )	// カメラだった場合
		{
			((ViewTransformBuffer *)pbufferobj)->BindToShader( m_pShader->ID() ); 

			((ViewTransformBuffer *)pbufferobj)->BindToShader( m_Pass[SHADER_PASS_OCCLUSION_SH].ID() ); 

		}
	
	
	}



	void SSDOShader::Release()
	{
		SafeDelete( m_pShader );

		m_uboParams.Release();
		m_TexNoise.Release();
		
		m_RenderTarget.Release();

		m_ulTexPosition		= -1;
		m_ulTexNormal		= -1;
		m_ulTexNoise		= -1;
		m_ulTexOcclusion	= -1;
		
		m_refTexPosition	= NULL;
		m_refTexNormal		= NULL;
		m_refSSDOBuffer		= NULL;

		for( int i=0; i<NUM_SHADER_PASS; ++i )	m_Pass[i].Release();
	}



	void SSDOShader::BindFramebuffer( const Texture2D& tex_pos, const Texture2D& tex_normal, const Texture2D& tex_diffuse )
	{
		m_refTexPosition	= (Texture2D *)&tex_pos;
		m_refTexNormal		= (Texture2D *)&tex_normal;
	}
	

	
	void SSDOShader::UnbindFramebuffer()
	{
		m_refTexPosition	= NULL;
		m_refTexNormal		= NULL;
	}



	void SSDOShader::BindSSDOBuffer( SSDOBuffer* buffer )
	{
		if( m_refSSDOBuffer	== buffer )
			return;

		m_refSSDOBuffer	= buffer;

		Vec2f &texSize = m_refSSDOBuffer->GetParam()->TexSize;
		const uint32 texids[]	= { m_refSSDOBuffer->GetTexOcclusionSH()->texID, m_refSSDOBuffer->GetTexBlur()->texID, m_refSSDOBuffer->GetResult()->texID };//{ m_TexOcclusionSH.texID, m_TexBlurBuffer.texID };//{ texocc->texID, texblur->texID };//

		m_RenderTarget.Init( int( texSize.x ), int( texSize.y ), false );
		m_RenderTarget.BindTextures( ArraySize( texids ), g_DefaultColorAttachments, texids );

		m_uboParams.UpdateSubresource( m_refSSDOBuffer->GetParam(), sizeof( SSDOParams ) );
	}



	void SSDOShader::UnbindSSDOBuffer()
	{
		m_RenderTarget.UnbindTextures();
		m_refSSDOBuffer	= NULL;
	}


	
	void SSDOShader::Render( uint32 texid )
	{
		if( !m_refScreenSpaceQuad || !m_refTexPosition || !m_refTexNormal || !m_refSSDOBuffer )
			return;
		

		const Vec2f& texSize	= m_refSSDOBuffer->GetParam()->TexSize;
		glViewport( 0, 0, int(texSize.x), int(texSize.y) );


		//============= 1st pass: Accumulate Occlusion SH ==============//

		m_Pass[ SHADER_PASS_OCCLUSION_SH ].Bind();
		{
			m_RenderTarget.Bind( 1, FBO_SLOT_OCCLUSION_SH );
			{
				// Clear Buffer
				glClear( GL_COLOR_BUFFER_BIT /*| GL_DEPTH_BUFFER_BIT*/ );

				//glUniform1i( m_ulPass, 0 );

				glActiveTexture( g_DefaultTextureUnits[ TEX_SLOT_POSITION ] );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_refTexPosition->texID ) );
				GL_SAFE_CALL( glUniform1i( m_ulTexPosition, TEX_SLOT_POSITION ) );

				glActiveTexture( g_DefaultTextureUnits[ TEX_SLOT_NORMAL ] );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_refTexNormal->texID ) );
				GL_SAFE_CALL( glUniform1i( m_ulTexNormal, TEX_SLOT_NORMAL ) );

				glActiveTexture( g_DefaultTextureUnits[ TEX_SLOT_NOISE ] );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_TexNoise.texID ) );
				GL_SAFE_CALL( glUniform1i( m_ulTexNoise, TEX_SLOT_NOISE ) );

				m_refScreenSpaceQuad->Draw();

			}
			//m_Fbo.Unbind();
		}
		//m_Pass[ SHADER_PASS_OCCLUSION_SH ].Unbind();

		
		m_Pass[ SHADER_PASS_BLUR ].Bind();
		{

			//================ 2nd pass: Horizontal Blur ===================//
			m_RenderTarget.Bind( 1, FBO_SLOT_BLUR );
			{
				glClear( GL_COLOR_BUFFER_BIT /*| GL_DEPTH_BUFFER_BIT*/ );

				GL_SAFE_CALL( glUniform1i( m_ulBlurDir, 0 ) );

				glActiveTexture( g_DefaultTextureUnits[ TEX_SLOT_OCCLUSION_SH ] );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_refSSDOBuffer->GetTexOcclusionSH()->texID ) );
				GL_SAFE_CALL( glUniform1i( m_ulTexOcclusion, TEX_SLOT_OCCLUSION_SH ) );

				m_refScreenSpaceQuad->Draw();
			}
			

			//================= 3rd pass: Vertical Blur ====================//
			m_RenderTarget.Bind( 1, FBO_SLOT_FINAL );
			{
				glClear( GL_COLOR_BUFFER_BIT /*| GL_DEPTH_BUFFER_BIT*/ );

				GL_SAFE_CALL( glUniform1i( m_ulBlurDir, 1 ) );

				glActiveTexture( g_DefaultTextureUnits[ TEX_SLOT_OCCLUSION_SH ] );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_refSSDOBuffer->GetTexBlur()->texID ) );
				GL_SAFE_CALL( glUniform1i( m_ulTexOcclusion, TEX_SLOT_OCCLUSION_SH ) );

				m_refScreenSpaceQuad->Draw();
			}
			m_RenderTarget.Unbind();
		}
		m_Pass[ SHADER_PASS_BLUR ].Unbind();
		
		
		m_pShader->Bind();
		{
			//================== 4th pass: Composit ========================//
			glViewport( 0, 0, int(texSize.x)*2, int(texSize.y)*2 );

			glActiveTexture( g_DefaultTextureUnits[ TEX_SLOT_FINAL ] );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_refSSDOBuffer->GetResult()->texID ) );
			GL_SAFE_CALL( glUniform1i( m_ulTexFinal, TEX_SLOT_FINAL ) );

			m_refScreenSpaceQuad->Draw();
			
		}
		m_pShader->Unbind();
		

		//m_uboParams.Unbind();
	}
	


	void SSDOShader::InitNoiseTexture( int dimx, int dimy )
	{
		Vec3f *random_vecs	= new Vec3f[ dimx * dimy ];
		for( int i=0; i< dimx * dimy; ++i )
		{
			random_vecs[i].x	= ( (rand()%65536) / 65535.0f ) * 2.0f - 1.0f;
			random_vecs[i].y	= ( (rand()%65536) / 65535.0f ) * 2.0f - 1.0f;
			random_vecs[i].z	= ( (rand()%65536) / 65535.0f ) * 2.0f - 1.0f;
		}

		m_TexNoise.Allocate( dimx, dimy, 0, 0, DATA_FORMAT::FORMAT_R32G32B32_FLOAT, random_vecs ); 
		m_TexNoise.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );
		//m_TexNoise.SetAddressMode( TEXTURE_ADDRESS_MODE::TEX_ADDRESS_WRAP, TEXTURE_ADDRESS_MODE::TEX_ADDRESS_WRAP );
		m_TexNoise.GenHardwareTexture();

		SafeDeleteArray( random_vecs );
	}


}// end of namespace