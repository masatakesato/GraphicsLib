#include	"FXAAShader.h"


namespace OreOreLib
{
	// Default Constructor
	FXAAShader::FXAAShader()
	{
#ifdef _DEBUG
		tcout << "FXAAShader()..." << tendl;
#endif // _DEBUG
		//const type_info &id	= typeid(*this);
		//tcout << "class name: " << id.name() << endl;// .name()使うと、コンパイラが間違えてメモリリーク報告してくる. 2013.08.05

		m_pShader	= NULL;
		m_ulTexSize	= 0;
		m_ulTexture	= 0;
	}



	// Constructor
	FXAAShader::FXAAShader( const TCHAR *filepath, GLSL_VERSION version )
	{
#ifdef _DEBUG
		tcout << "FXAAShader( const TCHAR *filepath, GLSL_VERSION version )..." << tendl;
#endif // _DEBUG
		//const type_info &id	= typeid(*this);
		//tcout << "class name: " << id.name() << endl;// .name()使うと、コンパイラが間違えてメモリリーク報告してくる. 2013.08.05

		m_pShader	= NULL;
		m_ulTexSize	= 0;
		m_ulTexture	= 0;

		if( filepath )	InitShader( filepath, version );
	}


	// destructor
	FXAAShader::~FXAAShader()
	{
#ifdef _DEBUG
		tcout << _T("~FXAAShader()...") << tendl;
#endif
		SafeDelete( m_pShader );
		m_ulTexSize	= 0;
		m_ulTexture	= 0;
	}


	void FXAAShader::InitShader( const TCHAR *filepath, GLSL_VERSION version )
	{
		// create shader
		m_pShader	= new GLShader();
		m_pShader->Init( filepath, version );

		// initialize attribute location
		GL_SAFE_CALL( glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );

		// initialize uniform location
		m_ulTexSize	= GL_SAFE_CALL( glGetUniformLocation( m_pShader->ID(), "g_TexSize" ) );
		m_ulTexture	= GL_SAFE_CALL( glGetUniformLocation( m_pShader->ID(), "g_Texture" ) );

		// link shader
		m_pShader->Link();

		// assign texture unit


	}


	//static void DrawQuad()
	//{
	//	float	TexCoordArray[8] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f };
	//	float	VertexArray[8] = { -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f,  -1.0f, 1.0f };
	//	GLuint	index[6] = { 0,1,2,	3,0,2 };
	//
	//	glEnableClientState(GL_VERTEX_ARRAY);
	//	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//
	//	glTexCoordPointer(2, GL_FLOAT, 0, TexCoordArray);
	//	glVertexPointer(2, GL_FLOAT, 0, VertexArray);
	//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, index);
	//
	//	glDisableClientState(GL_VERTEX_ARRAY);
	//	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	//}



	void FXAAShader::Render( const Texture2D *tex )
	{
		if( !m_refScreenSpaceQuad )
			return;

		m_pShader->Bind();

		const float w	= float(tex->Width()), h = float(tex->Height());
		GL_SAFE_CALL( glUniform4f( m_ulTexSize, w, h, 1.0f/w, 1.0f/h ) );

		GL_SAFE_CALL( glActiveTexture( GL_TEXTURE0 ) );
		GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, tex->texID ) );
		GL_SAFE_CALL( glUniform1i( m_ulTexture, 0 ) );

		m_refScreenSpaceQuad->Draw();

		m_pShader->Unbind();
	}




}// end of namespace