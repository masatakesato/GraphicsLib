#include	"QuadShader.h"


namespace OreOreLib
{
	// Default Constructor
	QuadShader::QuadShader()
	{
#ifdef _DEBUG
		tcout << "QuadShader()..."<< tendl;
#endif // _DEBUG
		//const type_info &id	= typeid(*this);
		//tcout << "class name: " << id.name() << endl;// .name()使うと、コンパイラが間違えてメモリリーク報告してくる. 2013.08.05

		m_pShader		= NULL;
		m_ulTexture1D	= -1;
		m_ulTexture2D	= -1;
		m_ulScale		= -1;
		m_ulMode		= -1;
	}



	// Constructor
	QuadShader::QuadShader( const TCHAR *filepath, GLSL_VERSION version )
	{
#ifdef _DEBUG
		tcout << "QuadShader( const TCHAR *filepath, GLSL_VERSION version )..." << tendl;
#endif // _DEBUG
		//const type_info &id	= typeid(*this);
		//tcout << "class name: " << id.name() << endl;// .name()使うと、コンパイラが間違えてメモリリーク報告してくる. 2013.08.05

		m_pShader		= NULL;
		m_ulTexture1D	= -1;
		m_ulTexture2D	= -1;
		m_ulScale		= -1;
		m_ulMode		= -1;

		if( filepath )
			InitShader( filepath, version );
	}



	// Destructor
	QuadShader::~QuadShader()
	{
#ifdef _DEBUG
		tcout << _T("~QuadShader()...") << tendl;
#endif
		Release();
	}



	void QuadShader::Release()
	{
		SafeDelete( m_pShader );
		m_ulTexture1D	= -1;
		m_ulTexture2D	= -1;
		m_ulScale		= -1;
		m_ulMode		= -1;
	}



	void QuadShader::InitShader( const TCHAR *filepath, GLSL_VERSION version )
	{
		// create shader
		m_pShader	= new GLShader();
		m_pShader->Init( filepath, version );

		// initialize attribute location
		GL_SAFE_CALL( glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );

		// initialize uniform location
		m_ulTexture1D	= GL_SAFE_CALL( glGetUniformLocation( m_pShader->ID(), "g_Texture1D" ) );
		m_ulTexture2D	= GL_SAFE_CALL( glGetUniformLocation( m_pShader->ID(), "g_Texture2D" ) );
		m_ulScale		= GL_SAFE_CALL( glGetUniformLocation( m_pShader->ID(), "g_Scale" ) );
		m_ulMode		= GL_SAFE_CALL( glGetUniformLocation( m_pShader->ID(), "g_Mode" ) );

		// link shader
		m_pShader->Link();
	}



	void QuadShader::Render( uint32 texid, float scale )
	{
		if( !m_refScreenSpaceQuad )	return;

		m_pShader->Bind();

		// UniformLocationの設定
		if( texid )
		{
			glActiveTexture( GL_TEXTURE0 );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, texid ) );
			GL_SAFE_CALL( glUniform1i( m_ulTexture2D, 0 ) );
		}

		GL_SAFE_CALL( glUniform1f( m_ulScale, scale ) );
		GL_SAFE_CALL( glUniform1i( m_ulMode, 1 ) );

		// 描画
		m_refScreenSpaceQuad->Draw();

		m_pShader->Unbind();
	}



	void QuadShader::Render( Texture1D *tex1d, float scale )
	{
		if( !m_refScreenSpaceQuad )	return;

		m_pShader->Bind();
		{
			// UniformLocationの設定
			glActiveTexture( GL_TEXTURE1 );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_1D, tex1d->texID ) );
			GL_SAFE_CALL( glUniform1i( m_ulTexture1D, 1 ) );

			GL_SAFE_CALL( glUniform1f( m_ulScale, scale ) );
			GL_SAFE_CALL( glUniform1i( m_ulMode, 0 ) );

			// 描画
			m_refScreenSpaceQuad->Draw();
		}
		m_pShader->Unbind();
	}



	void QuadShader::Render( Texture2D *tex2d, float scale )
	{
		if( !m_refScreenSpaceQuad )
			return;

		m_pShader->Bind();
		{
			// UniformLocationの設定
			glActiveTexture( GL_TEXTURE0 );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, tex2d->texID ) );
			GL_SAFE_CALL( glUniform1i( m_ulTexture2D, 0 ) );

			GL_SAFE_CALL( glUniform1f( m_ulScale, scale ) );
			GL_SAFE_CALL( glUniform1i( m_ulMode, 1 ) );

			// 描画
			m_refScreenSpaceQuad->Draw();
		}
		m_pShader->Unbind();	
	
	
	}






}// end of namespace