#include	"LocalReflectionShader.h"


#include	"./common/TString.h"
#include	"ViewTransformBuffer.h"
#include	"LightAttributeBuffer.h"
#include	"ShadowBuffer.h"
#include	"MeshObject.h"


namespace OreOreLib
{

	// default constructor
	LocalReflectionShader::LocalReflectionShader()
	{
#ifdef _DEBUG		
		tcout << _T( "LocalReflectionShader()..." ) << tendl;
#endif
		m_pShader		= NULL;
		m_ulTexPosition	= -1;
		m_ulTexDepth	= -1;
		m_ulTexNormal	= -1;
		m_ulTexColor	= -1;
	}



	// constructor
	LocalReflectionShader::LocalReflectionShader( const TCHAR *filepath, GLSL_VERSION version )
	{
#ifdef _DEBUG		
		tcout << _T( "LocalReflectionShader( const TCHAR *filepath, GLSL_VERSION version )..." ) << tendl;
#endif
		m_pShader		= NULL;
		m_ulTexPosition	= -1;
		m_ulTexDepth	= -1;
		m_ulTexNormal	= -1;
		m_ulTexColor	= -1;

		if( filepath )
			InitShader( filepath, version );
	}



	// destructor
	LocalReflectionShader::~LocalReflectionShader()
	{
#ifdef _DEBUG		
		tcout << _T( "~LocalReflectionShader()..." ) << tendl;
#endif
		Release();
	}

	

	void LocalReflectionShader::InitShader( const TCHAR *filepath, GLSL_VERSION version )
	{
		// create shader
		m_pShader	= new GLShader();
		m_pShader->Init( filepath, version );

		// init attribute location
		GL_SAFE_CALL( glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );

		// init uniform location
		m_ulTexPosition	= GL_SAFE_CALL( glGetUniformLocation( m_pShader->ID(), "g_TexPosition" ) );
		m_ulTexDepth	= GL_SAFE_CALL( glGetUniformLocation( m_pShader->ID(), "g_TexDepth" ) );
		m_ulTexNormal	= GL_SAFE_CALL( glGetUniformLocation( m_pShader->ID(), "g_TexNormal" ) );
		m_ulTexColor	= GL_SAFE_CALL( glGetUniformLocation( m_pShader->ID(), "g_TexColor" ) );


		// link shader
		m_pShader->Link();

		// その他
		// glBindFragDataLocation( program_id, channel, "outColor" );
		// サンプル
	}



	void LocalReflectionShader::Release()
	{
		SafeDelete( m_pShader );
		m_ulTexPosition	= -1;
		m_ulTexDepth	= -1;
		m_ulTexNormal	= -1;
		m_ulTexColor	= -1;
	}



	void LocalReflectionShader::BindBufferObject( const IBufferObject* pbufferobj )
	{
		const type_info& id = typeid(*pbufferobj);

		if( id == typeid(ViewTransformBuffer) )
		{
			((ViewTransformBuffer *)pbufferobj)->BindToShader( m_pShader->ID() );
		}
		//else if( id == typeid(LightAttributeBuffer) )
		//{
		//	((LightAttributeBuffer *)pbufferobj)->BindToShader( m_pShader->ID() );
		//}
		//else if( id == typeid(ShadowBuffer) )
		//{
		//	ShadowBuffer *pshadowbuf	= (ShadowBuffer *)buffer;

		//	// Bind Uniform Buffer Objects
		//	pshadowbuf->BindToShader( m_pShader->ID() );

		//	// Bind ShadowmapArray Texture(シェーダーバインドするたびにテクスチャバインドしなおし?)
		//	m_pShader->Bind();

		//	glActiveTexture( GL_TEXTURE6 );
		//	glBindTexture( GL_TEXTURE_2D_ARRAY, pshadowbuf->GetShadowMap()->texID ); 
		//	glUniform1i( m_ulShadowMapArray, 6 );

		//	m_pShader->Unbind();
		//}
		else
		{

		}

	}



	// メッシュデータをレンダリングする
	void LocalReflectionShader::Render( uint32 tex_pos, uint32 tex_depth, uint32 tex_normal, uint32 tex_color/*, Frustum *pcamera*/ )
	{
		if( tex_pos<0 || tex_depth<0 || tex_normal<0 || tex_color<0 || !m_refScreenSpaceQuad )
			return;

		m_pShader->Bind();
		{
			// Bind Textures
			glActiveTexture( TEX_SLOT_POSITION );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, tex_pos ) );
			GL_SAFE_CALL( glUniform1i( m_ulTexPosition, TEX_POSITION ) );

			glActiveTexture( TEX_SLOT_DEPTH );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, tex_depth ) );
			GL_SAFE_CALL( glUniform1i( m_ulTexDepth, TEX_DEPTH ) );

			glActiveTexture( TEX_SLOT_NORMAL );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, tex_normal ) );
			GL_SAFE_CALL( glUniform1i( m_ulTexNormal, TEX_NORMAL ) );

			glActiveTexture( TEX_SLOT_COLOR );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, tex_color ) );
			GL_SAFE_CALL( glUniform1i( m_ulTexColor, TEX_COLOR ) );

			// Bind Uniform Locations
			//GL_SAFE_CALL( glUniform3fv( m_ulViewPos, 1, pcamera->GetPosition()->xyz ) );

			// Draw ScreenSpace Quad
			m_refScreenSpaceQuad->Draw();
		}
		m_pShader->Unbind();
	}



}// end of namespace