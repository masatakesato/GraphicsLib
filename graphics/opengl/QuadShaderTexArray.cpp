#include	"QuadShaderTexArray.h"


namespace OreOreLib
{
	// Default Constructor
	QuadShaderTexArray::QuadShaderTexArray()
	{
#ifdef	_DEBUG
		tcout << "QuadShaderTexArray()..." << tendl;
#endif // _DEBUG
		//const type_info &id	= typeid(*this);
		//tcout << "class name: " << id.name() << endl;// .name()使うと、コンパイラが間違えてメモリリーク報告してくる. 2013.08.05

		m_pShader	= NULL;
		m_ulTexture	= -1;
		m_Magnitude	= 1.0f;
	}


	// Constructor
	QuadShaderTexArray::QuadShaderTexArray( const TCHAR *filepath, GLSL_VERSION version )
	{
#ifdef	_DEBUG
		tcout << "QuadShaderTexArray( const TCHAR *filepath, GLSL_VERSION version )..." << tendl;
#endif // _DEBUG
		//const type_info &id	= typeid(*this);
		//tcout << "class name: " << id.name() << endl;// .name()使うと、コンパイラが間違えてメモリリーク報告してくる. 2013.08.05

		m_pShader	= NULL;
		m_ulTexture	= -1;
		m_Magnitude	= 1.0f;

		if( filepath )
			InitShader( filepath, version );
	}



	// Destructor
	QuadShaderTexArray::~QuadShaderTexArray()
	{
#ifdef _DEBUG
		tcout << _T("~QuadShaderTexArray()...") << tendl;
#endif // _DEBUG
		SafeDelete( m_pShader );
	}



	void QuadShaderTexArray::InitShader( const TCHAR *filepath, GLSL_VERSION version )
	{
		// create shader
		m_pShader = new GLShader();
		m_pShader->Init( filepath, version );

		// initialize attribute location
		GL_SAFE_CALL( glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );

		// initialize uniform location
		m_ulTexture		= GL_SAFE_CALL( glGetUniformLocation( m_pShader->ID(), "g_TextureArray" ) );
		m_ulLayer		= GL_SAFE_CALL( glGetUniformLocation( m_pShader->ID(), "g_Layer" ) );
		m_ulMagnitude	= GL_SAFE_CALL( glGetUniformLocation( m_pShader->ID(), "g_Magnitude" ) );

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



	void QuadShaderTexArray::Render( Texture2D *tex, int numlayers )
	{
		if( !m_refScreenSpaceQuad )	return;

		m_pShader->Bind();

		glActiveTexture( GL_TEXTURE0 );
		glUniform1i( m_ulTexture, 0 );
		GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D_ARRAY, tex->texID ) );

		const int x_offset	= tex->Width();

		for( int i=0; i<numlayers; ++i )
		{
			glViewport( x_offset/4*i, 0, tex->Width()/4, tex->Height()/4 );
			
			GL_SAFE_CALL( glUniform1i( m_ulLayer, i ) );
			GL_SAFE_CALL( glUniform1f( m_ulMagnitude, m_Magnitude ) );
		
			m_refScreenSpaceQuad->Draw();
		}


		m_pShader->Unbind();

	}



	//void QuadShaderTexArray::Render( uint32 texid, int numlayers )
	//{
	//	if( !m_refScreenSpaceQuad )	return;

	//	m_pShader->Bind();

	//	glActiveTexture( GL_TEXTURE0 );
	//	glUniform1i( m_ulTexture, 0 );
	//	glBindTexture( GL_TEXTURE_2D_ARRAY, texid );

	//	m_refScreenSpaceQuad->Draw();

	//	m_pShader->Unbind();
	//}




}// end of namespace