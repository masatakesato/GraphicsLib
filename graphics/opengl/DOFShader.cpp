#include	"DOFShader.h"



namespace OreOreLib
{

	// default constructor
	DOFShader::DOFShader()
	{
#ifdef _DEBUG
		tcout << _T( "DOFShader()..." ) << tendl;
#endif // _DEBUG

		m_pShader			= NULL;
		m_ulTexRendered		= -1;
		m_ulTexDepth		= -1;

		m_refTexRendered	= NULL;
		m_refTexDepth		= NULL;
	}



	// constructor
	DOFShader::DOFShader( const TCHAR *filepath, GLSL_VERSION version )
	{
#ifdef _DEBUG
		tcout << _T("DOFShader( const TCHAR *filepath, GLSL_VERSION version )...") << tendl;
#endif // _DEBUG

		m_pShader			= NULL;
		m_ulTexRendered		= -1;
		m_ulTexDepth		= -1;

		m_refTexRendered	= NULL;
		m_refTexDepth		= NULL;
		
		if( filepath )
			InitShader( filepath, version );
	}



	// destructor
	DOFShader::~DOFShader()
	{
#ifdef _DEBUG
		tcout << _T( "~DOFShader()..." ) << tendl;
#endif // _DEBUG
		Release();
	}
	
	
	
	// init shader
	void DOFShader::InitShader( const TCHAR *filepath, GLSL_VERSION version )
	{
		// Create Shader
		m_pShader	= new GLShader();
		m_pShader->Init( filepath, version );
		GLuint program_id	= m_pShader->ID();

		// Init Attribute Locations
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );

		// Init Uniform Locations
		m_ulTexRendered	= GL_SAFE_CALL( glGetUniformLocation( program_id, "bgl_RenderedTexture" ) );
		m_ulTexDepth	= GL_SAFE_CALL( glGetUniformLocation( program_id, "bgl_DepthTexture" ) );

		// Bind Frag Data Locations
		// glBindFragDataLocation( program_id, channel, "outColor" );

		// Link Shader
		m_pShader->Link();
		
	}



	void DOFShader::BindInputTexture( Texture2D* texrendered, Texture2D* texdepth )
	{
		m_refTexRendered	= texrendered;
		m_refTexDepth		= texdepth;
	}
	
	
	
	void DOFShader::UnbindInputTexture()
	{
		m_refTexRendered	= NULL;
		m_refTexDepth		= NULL;
	}
	
	
	
	void DOFShader::Release()
	{
		SafeDelete( m_pShader );
		m_ulTexRendered		= -1;
		m_ulTexDepth		= -1;

		m_refTexRendered	= NULL;
		m_refTexDepth		= NULL;
	}





	// メッシュデータをレンダリングする(シェーダーはバインド済みと想定)
	void DOFShader::Render()
	{
		if( !m_refScreenSpaceQuad || !m_refTexRendered || !m_refTexDepth )
			return;

		m_pShader->Bind();

		// UniformLocationの設定
		GL_SAFE_CALL( glActiveTexture( GL_TEXTURE0 ) );
		GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_refTexRendered->texID ) );
		GL_SAFE_CALL( glUniform1i( m_ulTexRendered, TEX_SLOT_RENDERED ) );

		GL_SAFE_CALL( glActiveTexture( GL_TEXTURE1 ) );
		GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_refTexDepth->texID ) );
		GL_SAFE_CALL( glUniform1i( m_ulTexDepth, TEX_SLOT_DEPTH ) );

		// 描画
		m_refScreenSpaceQuad->Draw();


		m_pShader->Unbind();
	}


}// end of namespace