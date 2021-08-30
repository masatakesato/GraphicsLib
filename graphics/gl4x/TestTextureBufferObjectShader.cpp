#include	"TestTextureBufferObjectShader.h"

#include	"GLPrimitives.h"


namespace OreOreLib
{
	// constructor
	TestTextureBufferObjectShader::TestTextureBufferObjectShader( const char *filepath )
	{
#ifdef	_DEBUG
		const type_info& id = typeid(*this);
		tcout << "AbstractClass-typeid: " << id.name() << tendl;
#endif

		
		m_TBO.Release();

		if( filepath )
		{
			m_AtomicCounter.Init();

			CreateOverlappedScreenSpaceQuad( m_OverlappedQuad );

			Vec4f color[512];

			for( int i=0; i<512; ++i )
				InitVec( color[i], 0.0f, 0.0f, 0.0f, 0.0f );

			m_TBO.Init( DATA_FORMAT::FORMAT_R32G32B32A32_FLOAT, 512, GL_STATIC_DRAW, color );
			InitShader( filepath );
		}
	}



	// destructor
	TestTextureBufferObjectShader::~TestTextureBufferObjectShader()
	{
		Release();
	}



	void TestTextureBufferObjectShader::Release()
	{
		for( int i=0; i<NUM_PASS; ++i )	m_Pass[i].Release();
		m_TBO.Release();
		m_AtomicCounter.Release();


m_OverlappedQuad.Release();
	}



	// init shader
	void TestTextureBufferObjectShader::InitShader( const char *filepath )
	{
		GLuint	program_id;

		//======================= Init 1st Pass =========================//
		// Create Shader
		m_Pass[ PASS_COUNTUP_PIXELS ].Init( "TestTBOCountupPixels.glsl" );
		program_id	= m_Pass[ PASS_COUNTUP_PIXELS ].ID();

		// Init Attribute Locations
		glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" );
		glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" );

		// Init Uniform Locations
//		m_ulTBOResolution	= glGetUniformLocation( program_id, "g_TBOResolution" );

		// Bind Fragment Data Locations
		// glBindFragDataLocation( program_id, 0, "outColor" );
		
		// Link shader
		m_Pass[ PASS_COUNTUP_PIXELS ].Link();


		//====================== Init 2nd Pass =========================//
		// Create Shader
		m_Pass[ PASS_PREVIEW_TBO ].Init( "TestTBOPreview.glsl" );
		program_id	= m_Pass[ PASS_PREVIEW_TBO ].ID();

		// Init Attribute Locations
		glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" );
		glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" );

		// Init Uniform Locations
//		m_ulTBOResolution	= glGetUniformLocation( program_id, "g_TBOResolution" );

		// Bind Fragment Data Locations
		// glBindFragDataLocation( program_id, 0, "outColor" );
		
		// Link shader
		m_Pass[ PASS_PREVIEW_TBO ].Link();
	}



	// メッシュデータをレンダリングする(シェーダーはバインド済みと想定)
	void TestTextureBufferObjectShader::Render()
	{
		if( !m_refScreenSpaceQuad )	return;
		
		//================ コンピュートシェーダーを使ってTBOに値を書き込む ==============//
		const Vec4f clearVal	= {0.0f, 0.0f, 0.0f, 0.0f};
		m_TBO.Clear( &clearVal );
		m_TBO.BindImageTexture( 0, 0, GL_FALSE, 0, GL_READ_ONLY );

		// 16x16のピクセル描画 -> もし重複がなければアトミックカウンタは256になるはず,,,
		glViewport( 0,0,16,16 );

		m_AtomicCounter.Clear();
		m_AtomicCounter.BindBufferBase(0);

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		m_Pass[ PASS_COUNTUP_PIXELS ].Bind();
		
	//	for( int i=0; i<2; ++i )
		m_OverlappedQuad.Draw();

		m_Pass[ PASS_COUNTUP_PIXELS ].Unbind();


		//==================== プレビュー用シェーダーでTBOを可視化する ==================//
		glViewport( 0,0,1280,720 );

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		

		m_Pass[ PASS_PREVIEW_TBO ].Bind();
		
//		glUniform2f( m_ulTBOResolution, float(width), float(height) );
		m_refScreenSpaceQuad->Draw();

		m_Pass[ PASS_PREVIEW_TBO ].Unbind();
	}


}// end of namespace