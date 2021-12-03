#include	"SMAAShader.h"


//#include	"SMAAAreaTex.h"
//#include	"SMAASearchTex.h"


#ifndef AREATEX_WIDTH
#define AREATEX_WIDTH 160
#endif // !AREATEX_WIDTH

#ifndef AREATEX_HEIGHT
#define AREATEX_HEIGHT 560
#endif // !AREATEX_HEIGHT

#ifndef SEARCHTEX_WIDTH
#define SEARCHTEX_WIDTH 66
#endif // !SEARCHTEX_WIDTH


#ifndef SEARCHTEX_HEIGHT
#define SEARCHTEX_HEIGHT 33
#endif // !SEARCHTEX_HEIGHT





namespace OreOreLib
{

	// Default Constructor
	SMAAShader::SMAAShader()
	{
#ifdef _DEBUG
		tcout << _T("SMAAShader()...") << tendl;
#endif // _DEBUG
		m_pShader		= NULL;
		m_ulPass		= -1;
		m_ulInpTex		= -1;
		m_ulInpTex2		= -1;
		m_ulInpTex3		= -1;
		m_refTexInput	= NULL;
	}



	// Constructor
	SMAAShader::SMAAShader( const TCHAR* filepath, GLSL_VERSION version )
	{
#ifdef _DEBUG
tcout << _T("SMAAShader( const TCHAR* filepath, GLSL_VERSION version )...") << tendl;
#endif // _DEBUG

	m_pShader		= NULL;
	m_ulPass		= -1;
	m_ulInpTex		= -1;
	m_ulInpTex2		= -1;
	m_ulInpTex3		= -1;
	m_refTexInput	= NULL;

	if( filepath )
		InitShader( filepath, version );
	}



	// Destructor
	SMAAShader::~SMAAShader()
	{
		Release();
	}



	void SMAAShader::Release()
	{
		SafeDelete( m_pShader );

		UnbindInputTexture();

		m_ulPass	= -1;
		m_ulInpTex	= -1;
		m_ulInpTex2	= -1;
		m_ulInpTex3	= -1;
	}



	// init shader
	void SMAAShader::InitShader( const TCHAR* filepath, GLSL_VERSION version )
	{
		Release();

		// Create shader
		m_pShader	= new GLShader();
		m_pShader->Init( filepath, version );
		GLuint program_id	= m_pShader->ID();

		// Init Attribute Locations
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );

		// Init Uniform Locations
		m_ulPass	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Pass" ) );
		m_ulInpTex	= GL_SAFE_CALL( glGetUniformLocation( program_id, "uInpTex" ) );
		m_ulInpTex2	= GL_SAFE_CALL( glGetUniformLocation( program_id, "uInpTex2" ) );
		m_ulInpTex3	= GL_SAFE_CALL( glGetUniformLocation( program_id, "uInpTex3" ) );

		// Init Frag Data Locations
		GL_SAFE_CALL( glBindFragDataLocation( program_id, 0, "outColor" ) );

		// Link shader
		m_pShader->Link();




unsigned char* buffer = 0;
FILE* f = 0;

buffer = new unsigned char[1024 * 1024];

fopen_s( &f, "smaa_area.raw", "rb" ); //rb stands for "read binary file"


if( !f )
{
	std::cerr << "Couldn't open smaa_area.raw.\n";
}

fread( buffer, AREATEX_WIDTH * AREATEX_HEIGHT * 2, 1, f );
fclose( f );

f = 0;

		//// Init SMAA Texture
		//Vec2uc	*pdata_area	= new Vec2uc[AREATEX_WIDTH * AREATEX_HEIGHT];

		//for( int y=0; y<AREATEX_HEIGHT; ++y )
		//	for( int x=0; x<AREATEX_WIDTH; ++x )
		//	{
		//		int y_inv	= AREATEX_HEIGHT-1-y;
		//		pdata_area[y*AREATEX_WIDTH + x]	= ( (Vec2uc *)areaTexBytes )[y_inv*AREATEX_WIDTH + x];

		//	}


		m_TexSmaaArea.Allocate( AREATEX_WIDTH, AREATEX_HEIGHT, 0, 0, DATA_FORMAT::FORMAT_R8G8_UNORM, buffer/*areaTexBytes*//*pdata_area*/ );
		m_TexSmaaArea.SetAddressMode( TEXTURE_ADDRESS_MODE::TEX_ADDRESS_CLAMP, TEXTURE_ADDRESS_MODE::TEX_ADDRESS_CLAMP );
		m_TexSmaaArea.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );
		m_TexSmaaArea.GenHardwareTexture();


fopen_s( &f, "smaa_search.raw", "rb" );

if( !f )
{
	std::cerr << "Couldn't open smaa_search.raw.\n";
}

fread( buffer, SEARCHTEX_WIDTH * SEARCHTEX_HEIGHT, 1, f );
fclose( f );

f = 0;


		//uint8 *pdata_search	= new uint8[ SEARCHTEX_WIDTH * SEARCHTEX_HEIGHT ];
		//for( int y=0; y<SEARCHTEX_HEIGHT; ++y )
		//	for( int x=0; x<SEARCHTEX_WIDTH; ++x )
		//	{
		//		int y_inv	= SEARCHTEX_HEIGHT-1-y;
		//		float f = 0.0f;
		//		pdata_search[ y*SEARCHTEX_WIDTH + x ]	= searchTexBytes[ y_inv*SEARCHTEX_WIDTH + x ];

		//	}


		m_TexSmaaSearch.Allocate( SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT, 0, 0, DATA_FORMAT::FORMAT_R8_UNORM, buffer/*searchTexBytes*//*pdata_search*/ );
		m_TexSmaaSearch.SetAddressMode( TEXTURE_ADDRESS_MODE::TEX_ADDRESS_CLAMP, TEXTURE_ADDRESS_MODE::TEX_ADDRESS_CLAMP );
		m_TexSmaaSearch.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_NEAREST );//m_TexSmaaSearch.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );//
		m_TexSmaaSearch.GenHardwareTexture();

		SafeDeleteArray( buffer );
		//SafeDeleteArray( pdata_area );
		//SafeDeleteArray( pdata_search );
		
	}



	void SMAAShader::BindInputTexture( Texture2D* reftex )
	{
		
		if( m_RenderTarget.Width() == reftex->Width() && m_RenderTarget.Height() == reftex->Height() )
			return;

		m_refTexInput	= reftex;
		//m_Fbo.Init( m_refTexInput->Width(), m_refTexInput->Height(), false );
		m_RenderTarget.Init( m_refTexInput->Width(), m_refTexInput->Height(), false );


		// Init Internal Buffer
		m_TexAA.Allocate( m_RenderTarget.Width(), m_RenderTarget.Height(), 0, 0, DATA_FORMAT::FORMAT_R8G8B8A8_UNORM );//FORMAT_R16G16B16A16_FLOAT );//
		m_TexAA.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );
		m_TexAA.GenHardwareTexture();

		m_TexSmaaWk.Allocate( m_RenderTarget.Width(), m_RenderTarget.Height(), 0, 0, DATA_FORMAT::FORMAT_R8G8B8A8_UNORM );//FORMAT_R16G16B16A16_FLOAT );//
		m_TexSmaaWk.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );
		m_TexSmaaWk.GenHardwareTexture();


		// Init FramebufferObject
		const uint32 texids[]			= { m_TexAA.texID, m_TexSmaaWk.texID };
		//m_Attachment.Init( ARRAYSIZE(texids), g_DefaultColorAttachments, texids );
		//m_Fbo.BindAttachment( &m_Attachment );

		m_RenderTarget.BindTextures( ArraySize<int>( texids ), g_DefaultColorAttachments, texids );

	}



	void SMAAShader::UnbindInputTexture()
	{
		m_RenderTarget.Release();
		m_refTexInput	= NULL;
		
		//m_Fbo.Release();
		//m_Attachment.Clear();
	}



	// メッシュデータをレンダリングする(シェーダーはバインド済みと想定)
	void SMAAShader::Render( const Texture2D *tex, float scale )
	{
		if( !m_refTexInput || !m_refScreenSpaceQuad )
			return;


		m_pShader->Bind();
		{

			//==================== エッジ検出パス ==========================//
			m_RenderTarget.Bind( 0 );
			//m_Fbo.Bind( 0 );// 1, 0 );
			{
				glColorMask(GL_TRUE,GL_TRUE,GL_FALSE,GL_FALSE);
				glClear( GL_COLOR_BUFFER_BIT );

				// Set Pass Index
				glUniform1i( m_ulPass, 0 );

				// Set Texture Inputs
				glActiveTexture( GL_TEXTURE0 );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_refTexInput->texID ) );
				glUniform1i( m_ulInpTex, 0 );

				// Draw ScreenSpace Quad
				m_refScreenSpaceQuad->Draw();

				glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
			}
			//m_Fbo.Unbind();
			m_RenderTarget.Unbind();

			//=================== ブレンド重み計算パス =====================//
			m_RenderTarget.Bind( 1 );
			//m_Fbo.Bind( 1 );// 1, 1 );
			{
				glClear( GL_COLOR_BUFFER_BIT );

				// Set Pass Index
				glUniform1i( m_ulPass, 1 );

				// Set Texture Inputs
				glActiveTexture( GL_TEXTURE0 );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_TexAA.texID ) );
				glUniform1i( m_ulInpTex, 0 );

				glActiveTexture( GL_TEXTURE1 );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_TexSmaaArea.texID ) );
				glUniform1i( m_ulInpTex2, 1 );

				glActiveTexture( GL_TEXTURE2 );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_TexSmaaSearch.texID ) );
				glUniform1i( m_ulInpTex3, 2 );

				// Draw ScreenSpace Quad
				m_refScreenSpaceQuad->Draw();
			}
			//m_Fbo.Unbind();
			m_RenderTarget.Unbind();

			//==================== 近傍ピクセル合成パス ====================//
			
//			m_Fbo.Bind( 1, 0 );
			{
				glClear( GL_COLOR_BUFFER_BIT );

				// Set Pass Index
				glUniform1i( m_ulPass, 2 );

				// Set Texture Inputs
				glActiveTexture( GL_TEXTURE0 );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_refTexInput->texID ) );
				glUniform1i( m_ulInpTex, 0 );

				glActiveTexture( GL_TEXTURE1 );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_TexSmaaWk.texID ) );
				glUniform1i( m_ulInpTex2, 1 );

				// Draw ScreenSpace Quad
				m_refScreenSpaceQuad->Draw();

			}
//			m_Fbo.Unbind();
		


			// Cleanup TextureUnits
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, 0 );

			glActiveTexture( GL_TEXTURE1 );
			glBindTexture( GL_TEXTURE_2D, 0 );

			glActiveTexture( GL_TEXTURE2 );
			glBindTexture( GL_TEXTURE_2D, 0 );


		}
		m_pShader->Unbind();
	}



}// end of namespace