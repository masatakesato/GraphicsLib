#include	"HDRShader.h"


#include	"GLHelperFunctions.h"



namespace OreOreLib
{

	HDRShader::HDRShader( const TCHAR *filepath, GLSL_VERSION version )
	{
#ifdef	_DEBUG
		tcout << _T( "HDRShader( const TCHAR* filepath )..." ) << tendl;
#endif
		m_Width4x4		= 0;
		m_Height4x4		= 0;
		m_refTexture	= NULL;

		m_PrevAdaptedLuminance	= 0;
		m_CurrAdaptedLuminance	= 0;

		if( filepath )
			InitShader( filepath, version );
	}



	HDRShader::~HDRShader()
	{
#ifdef	_DEBUG
		tcout << _T( "~HDRShader()..." ) << tendl;
#endif
		Release();
	}



	void HDRShader::InitShader( const TCHAR *filepath, GLSL_VERSION version )
	{
		GLuint	program_id;

		//=======================	DOWNSAMPLE_2X2 Pass	========================//
		// Create shader
		m_Pass[DOWNSAMPLE_2X2].Init( _T( "HDRShader/HDRDownSample2x2.glsl" ), version );
		program_id	= m_Pass[DOWNSAMPLE_2X2].ID();

		// Init Attribute Location
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );

		// Init Uniform Location


		// Init Output Fragment Location

		// Link shader
		m_Pass[DOWNSAMPLE_2X2].Link();

		// Setup Texture Unit
		m_Pass[DOWNSAMPLE_2X2].Bind();
		{
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_Sampler1" ), 0 ) );
		}
		m_Pass[DOWNSAMPLE_2X2].Unbind();

		
		//=======================	DOWNSAMPLE_4X4 Pass	========================//
		// Create shader
		m_Pass[DOWNSAMPLE_4X4].Init( _T( "HDRShader/HDRDownSample4x4.glsl" ), version );
		program_id	= m_Pass[DOWNSAMPLE_4X4].ID();

		// Init Attribute Location
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );

		// Init Uniform Location


		// Init Output Fragment Location


		// Link shader
		m_Pass[DOWNSAMPLE_4X4].Link();

		// Setup Texture Unit
		m_Pass[DOWNSAMPLE_4X4].Bind();
		{
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_Sampler1" ), 0 ) );
		}
		m_Pass[DOWNSAMPLE_4X4].Unbind();

		
		//======================	CALC_LOG_LUMINANCE Pass	======================//
		// Create shader
		m_Pass[CALC_LOG_LUMINANCE].Init( _T( "HDRShader/HDRCalcLogLuminance.glsl" ), version );
		program_id	= m_Pass[CALC_LOG_LUMINANCE].ID();

		// Init Attribute Location
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );

		// Init Uniform Location


		// Init Output Fragment Location

		// link shader
		m_Pass[CALC_LOG_LUMINANCE].Link();


		// Setup Texture Unit
		m_Pass[CALC_LOG_LUMINANCE].Bind();
		{
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_Sampler1" ), 0 ) );
		}
		m_Pass[CALC_LOG_LUMINANCE].Unbind();


		
		//=====================	AVERAGE_LOG_LUMINANCE Pass	======================//
		// Create shader
		m_Pass[AVERAGE_LOG_LUMINANCE].Init( _T( "HDRShader/HDRAverageLogLuminance.glsl" ), version );
		program_id	= m_Pass[AVERAGE_LOG_LUMINANCE].ID();

		// Init Attribute Location
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );

		// Init Uniform Location
		m_ulCalcExponential	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_CalcExponential" ) );

		// Init Output Fragment Location

		// Link shader
		m_Pass[AVERAGE_LOG_LUMINANCE].Link();

		// Setup Texture Unit
		m_Pass[AVERAGE_LOG_LUMINANCE].Bind();
		{
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_Sampler1" ), 0 ) );
		}
		m_Pass[AVERAGE_LOG_LUMINANCE].Unbind();


		
		//==================	CALC_ADAPTIVE_LUMINANCE Pass	==================//
		// Create shader
		m_Pass[CALC_ADAPTIVE_LUMINANCE].Init( _T( "HDRShader/HDRCalcAdaptiveLuminance.glsl" ), version );
		program_id	= m_Pass[CALC_ADAPTIVE_LUMINANCE].ID();

		// Init Attribute Location
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );

		// Init Uniform Location
		m_ulTimeStep	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_TimeStep" ) );

		// Init Output Fragment Location

		// Link shader
		m_Pass[CALC_ADAPTIVE_LUMINANCE].Link();

		// Setup Texture Unit
		m_Pass[CALC_ADAPTIVE_LUMINANCE].Bind();
		{
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_Sampler1" ), 0 ) );
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_Sampler2" ), 1 ) );
		}
		m_Pass[CALC_ADAPTIVE_LUMINANCE].Unbind();


		//=======================	BRIGHT_PASS_FILTER Pass	======================//
		// Create shader
		m_Pass[BRIGHT_PASS_FILTER].Init( _T( "HDRShader/HDRBrightPassFilter.glsl" ), version );
		program_id	= m_Pass[BRIGHT_PASS_FILTER].ID();

		// Init Attribute Location
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );

		// Init Uniform Location
		m_ulKey	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Key" ) );

		// Init Output Fragment Location

		// Link shader
		m_Pass[BRIGHT_PASS_FILTER].Link();

		// Setup Texture Unit
		m_Pass[BRIGHT_PASS_FILTER].Bind();
		{
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_Sampler1" ), 0 ) );
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_Luminance" ), 1 ) );
		}
		m_Pass[BRIGHT_PASS_FILTER].Unbind();

		
		//=========================	GAUSSIAN_BLUR Pass	=========================//
		// Create shader
		m_Pass[GAUSSIAN_BLUR].Init( _T( "HDRShader/HDRGaussianBlur.glsl" ), version );
		program_id	= m_Pass[GAUSSIAN_BLUR].ID();

		// Init Attribute Location
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );

		// Init Uniform Location
		m_ulBlurDirection	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_BlurDirection" ) );

		// Init Output Fragment Location

		// Link shader
		m_Pass[GAUSSIAN_BLUR].Link();

		// Setup Texture Unit
		m_Pass[GAUSSIAN_BLUR].Bind();
		{
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_Sampler1" ), 0 ) );
		}
		m_Pass[GAUSSIAN_BLUR].Unbind();


		//==========================	COMPOSITE Pass	=========================//
		// Create shader
		m_Pass[COMPOSITE].Init( _T( "HDRShader/HDRComposite.glsl" ), version );
		program_id	= m_Pass[COMPOSITE].ID();

		// Init Attribute Location
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );

		// Init Uniform Location
		m_ulKey2		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Key" ) );
		m_ulBloomWeight	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_BloomWeight" ) );

		// Init Output Fragment Location

		// Link shader
		m_Pass[COMPOSITE].Link();

		// Setup Texture Unit
		m_Pass[COMPOSITE].Bind();
		{
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_Sampler1" ), 0 ) );
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_Sampler2" ), 1 ) );
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_Luminance" ), 2 ) );
		}
		m_Pass[COMPOSITE].Unbind();



		m_ulViewportResolution.Init( _T( "g_ViewportResolution" ), 3 );


		m_ulViewportResolution.BindToShader( 0, m_Pass[DOWNSAMPLE_4X4].ID() );
		m_ulViewportResolution.BindToShader( 1, m_Pass[CALC_LOG_LUMINANCE].ID() );
		m_ulViewportResolution.BindToShader( 2, m_Pass[AVERAGE_LOG_LUMINANCE].ID() );

	}



	void HDRShader::BindInputTexture( Texture2D *tex )
	{
		if( m_refTexture )
		{
			if( tex->Width()==m_refTexture->Width() && tex->Height()==m_refTexture->Height() )	return;
		}

		m_refTexture	= tex;
		m_Width4x4		= m_refTexture->Width() / 4;
		m_Height4x4		= m_refTexture->Height() / 4;


		//==================== テクスチャ初期化 =========================//
		m_ScaledImage.Allocate( m_Width4x4, m_Height4x4, 0, 0, FORMAT_R16G16B16A16_FLOAT );
		m_ScaledImage.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );
		m_ScaledImage.GenHardwareTexture();

		for( int i=3; i>=0; i-- )
		{
			m_AverageLuminance[i].Allocate( (int)pow( 4.0, i ), (int)pow( 4.0, i ), 0, 0, FORMAT_R16_FLOAT );
			m_AverageLuminance[i].SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );
			m_AverageLuminance[i].GenHardwareTexture();
		}

		const short dummy[1] ={ 0 };
		for( int i=0; i<2; ++i )
		{
			m_AdaptedLuminance[i].Allocate( 1, 1, 0, 0, FORMAT_R16_FLOAT, dummy );//GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, 1, 1, GL_R16F, GL_RED, GL_FLOAT);
			m_AdaptedLuminance[i].SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );
			m_AdaptedLuminance[i].GenHardwareTexture();
		}

		m_PrevAdaptedLuminance	= 0;
		m_CurrAdaptedLuminance	= 1;

		m_BrightPass.Allocate( m_Width4x4, m_Height4x4, 0, 0, FORMAT_R16G16B16A16_FLOAT );//  = AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_Width4x4, m_Height4x4, GL_RGBA16F, GL_RGBA, GL_FLOAT);
		m_BrightPass.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );
		m_BrightPass.GenHardwareTexture();

		m_BrightPass2x2.Allocate( m_Width4x4/2, m_Height4x4/2, 0, 0, FORMAT_R16G16B16A16_FLOAT );//	= AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_Width4x4/2, m_Height4x4/2, GL_RGBA16F, GL_RGBA, GL_FLOAT);
		m_BrightPass2x2.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );
		m_BrightPass2x2.GenHardwareTexture();

		m_BlurredImage.Allocate( m_Width4x4/2, m_Height4x4/2, 0, 0, FORMAT_R16G16B16A16_FLOAT );//	= AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_Width4x4/2, m_Height4x4/2, GL_RGBA16F, GL_RGBA, GL_FLOAT);
		m_BlurredImage.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );
		m_BlurredImage.GenHardwareTexture();

		m_BlurredImage2.Allocate( m_Width4x4/2, m_Height4x4/2, 0, 0, FORMAT_R16G16B16A16_FLOAT );//	= AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_Width4x4/2, m_Height4x4/2, GL_RGBA16F, GL_RGBA, GL_FLOAT);
		m_BlurredImage2.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );
		m_BlurredImage2.GenHardwareTexture();


		//===================== FrameBufferObject初期化 ==================//
		m_Fbo.Init( 0, 0, false );


		//============= シェーダーにテクスチャをバインドする =============//


	}



	void HDRShader::UnbindInputTexture()
	{
		m_refTexture	= NULL;

		m_Width4x4		= 0;
		m_Height4x4		= 0;

		m_PrevAdaptedLuminance	= 0;
		m_CurrAdaptedLuminance	= 0;

		m_ScaledImage.Release();
		for( int i=3; i>=0; i-- )	m_AverageLuminance[i].Release();
		for( int i=0; i<2; ++i )	m_AdaptedLuminance[i].Release();
		m_BrightPass.Release();
		m_BrightPass2x2.Release();
		m_BlurredImage.Release();
		m_BlurredImage2.Release();

	}



	void HDRShader::Release()
	{
		for( int i=0; i<NUM_PASS; ++i )	m_Pass[i].Release();

		m_Fbo.Release();
		UnbindInputTexture();


m_ulViewportResolution.Release();
	}



	void HDRShader::DoHDRLighting( float timestep, float key, float bloomweight )
	{
		if( !m_refScreenSpaceQuad || !m_refTexture )
			return;

		//========== 画像からぼかしテクスチャを作る ==========//
		GenScaledImage();
		GenAverageLuminance();
		AdaptLuminance();
		FilterBlightPass();
		BlurImage();

		//===== 画像とぼかしテクスチャとを合成する =====//
		RenderFinalImage();

		//======== 輝度テクスチャをスワップする ========//
		m_PrevAdaptedLuminance	^= 1;
		m_CurrAdaptedLuminance	^= 1;
	}



	// 画像INの縮小イメージを作成する
	void HDRShader::GenScaledImage()
	{
glViewport( 0, 0, m_Width4x4, m_Height4x4 );

		m_Fbo.Bind();
		{
			GL_SAFE_CALL( glDrawBuffer( GL_COLOR_ATTACHMENT0 ) );
			GL_SAFE_CALL( glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ScaledImage.texID, 0 ) );

			m_ulViewportResolution.SetCurrentShader( 0 );

			m_Pass[DOWNSAMPLE_4X4].Bind();
			{
				// Bind Uniforms
				//	TODO: ビューポート解像度をシェーダーに渡す.Sample4x4を正しく動作させるために必要！!
				m_ulViewportResolution.SetUniform2f( float(m_ScaledImage.Width()), float(m_ScaledImage.Height()) );

				// Bind Textures
				GL_SAFE_CALL( glActiveTexture( g_DefaultTextureUnits[0] ) );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_refTexture->texID ) );
				
				m_refScreenSpaceQuad->Draw();
			}
			m_Pass[DOWNSAMPLE_4X4].Unbind();

		}
		m_Fbo.Unbind();
	}


	// 縮小イメージから平均輝度を計算する
	void HDRShader::GenAverageLuminance()
	{
		m_Fbo.Bind();
		{
			//==================== 64x64のテクスチャにlog()値を書き込む ====================//
			glViewport( 0, 0, m_AverageLuminance[3].Width(), m_AverageLuminance[3].Height() );// 64 x 64 viewport resolution

			GL_SAFE_CALL( glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_AverageLuminance[3].texID, 0 ) );

m_ulViewportResolution.SetCurrentShader( 1 );

			m_Pass[CALC_LOG_LUMINANCE].Bind();
			{
				// Bind Uniforms
//	TODO: ビューポート解像度をシェーダーに渡す.Sample4x4を正しく動作させるために必要！！
m_ulViewportResolution.SetUniform2f( float(m_AverageLuminance[3].Width()), float(m_AverageLuminance[3].Height()) );

				// ScaledImageを入力テクスチャにセットする
				GL_SAFE_CALL( glActiveTexture( g_DefaultTextureUnits[0] ) );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_ScaledImage.texID ) );

				m_refScreenSpaceQuad->Draw();
			}
			m_Pass[CALC_LOG_LUMINANCE].Unbind();

			
			//==================== logテクスチャを縮小して平均を求める ====================//
			m_Pass[AVERAGE_LOG_LUMINANCE].Bind();
			{
m_ulViewportResolution.SetCurrentShader( 2 );

				for( int i=2; i>=0; i-- )
				{
					glViewport( 0, 0, (int)pow( 4.0, i ), (int)pow( 4.0, i ) );

					GL_SAFE_CALL( glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_AverageLuminance[i].texID, 0 ) );

					// Bind Uniforms
//TODO: ビューポート解像度をシェーダーに渡す
m_ulViewportResolution.SetUniform2f( pow( 4.0f, i ), pow( 4.0f, i ) );

					// BInd Texrtures
					GL_SAFE_CALL( glActiveTexture( g_DefaultTextureUnits[0] ) );
					GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_AverageLuminance[i+1].texID ) );

					GL_SAFE_CALL( glUniform1i( m_ulCalcExponential, int( i==0 ) ) );

					m_refScreenSpaceQuad->Draw();
				}
			}
			m_Pass[AVERAGE_LOG_LUMINANCE].Unbind();
			
		}
		m_Fbo.Unbind();

	}


	// 明順応のシミュレーション
	void HDRShader::AdaptLuminance()
	{
		m_Fbo.Bind();
		{
			glViewport( 0, 0, 1, 1 );
			GL_SAFE_CALL( glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_AdaptedLuminance[m_CurrAdaptedLuminance].texID, 0 ) );

			//glClear( GL_COLOR_BUFFER_BIT );

			m_Pass[CALC_ADAPTIVE_LUMINANCE].Bind();
			{
				GL_SAFE_CALL( glActiveTexture( g_DefaultTextureUnits[0] ) );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_AdaptedLuminance[m_PrevAdaptedLuminance].texID ) );

				GL_SAFE_CALL( glActiveTexture( g_DefaultTextureUnits[1] ) );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_AverageLuminance[0].texID ) );

				GL_SAFE_CALL( glUniform1f( m_ulTimeStep, 0.01f ) );

				m_refScreenSpaceQuad->Draw();
			}
			m_Pass[CALC_ADAPTIVE_LUMINANCE].Unbind();
		}
		m_Fbo.Unbind();
	}



	// 縮小イメージの明るい領域だけ取り出す
	void HDRShader::FilterBlightPass()
	{
		m_Fbo.Bind();
		{
			//================ 輝度値の高いピクセルだけ抽出する ================//
			glViewport( 0, 0, m_BrightPass.Width(), m_BrightPass.Height() );
			GL_SAFE_CALL( glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BrightPass.texID, 0 ) );

			//glClear( GL_COLOR_BUFFER_BIT );

			m_Pass[BRIGHT_PASS_FILTER].Bind();
			{
				GL_SAFE_CALL( glActiveTexture( g_DefaultTextureUnits[0] ) );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_ScaledImage.texID ) );

				GL_SAFE_CALL( glActiveTexture( g_DefaultTextureUnits[1] ) );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_AdaptedLuminance[m_CurrAdaptedLuminance].texID ) );

				GL_SAFE_CALL( glUniform1f( m_ulKey, 0.36f ) );

				m_refScreenSpaceQuad->Draw();
			}
			m_Pass[BRIGHT_PASS_FILTER].Unbind();

			
			//=================== 画像サイズを縮小する(1/2) ====================//
			glViewport( 0, 0, m_BrightPass2x2.Width(), m_BrightPass2x2.Height() );
			GL_SAFE_CALL( glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BrightPass2x2.texID, 0 ) );

			//glClear( GL_COLOR_BUFFER_BIT );

			m_Pass[DOWNSAMPLE_2X2].Bind();
			{
				GL_SAFE_CALL( glActiveTexture( g_DefaultTextureUnits[0] ) );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_BrightPass.texID ) );

				m_refScreenSpaceQuad->Draw();
			}
			m_Pass[DOWNSAMPLE_2X2].Unbind();
			
		}
		m_Fbo.Unbind();

	}


	// 画像をぼかす
	void HDRShader::BlurImage()
	{
		m_Fbo.Bind();
		{
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glViewport( 0, 0, m_Width4x4/2, m_Height4x4/2 );

			m_Pass[GAUSSIAN_BLUR].Bind();
			{
				//========================= 縦方向にぼかしをかける ========================//
				GL_SAFE_CALL( glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BlurredImage.texID, 0 ) );

				GL_SAFE_CALL( glActiveTexture( g_DefaultTextureUnits[0] ) );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_BrightPass.texID ) );

				GL_SAFE_CALL( glUniform2f( m_ulBlurDirection, 0.0f, 1.0f ) );

				m_refScreenSpaceQuad->Draw();


				//========================= 横方向にぼかしをかける ========================//
				GL_SAFE_CALL( glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BlurredImage2.texID, 0 ) );

				GL_SAFE_CALL( glActiveTexture( g_DefaultTextureUnits[0] ) );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_BlurredImage.texID ) );

				GL_SAFE_CALL( glUniform2f( m_ulBlurDirection, 1.0f, 0.0f ) );

				m_refScreenSpaceQuad->Draw();

			}
			m_Pass[GAUSSIAN_BLUR].Unbind();

		}
		m_Fbo.Unbind();

	}


	// ぼかし画像と合成
	void HDRShader::RenderFinalImage()
	{
		if( !m_refTexture )
			return;

		glClear( GL_COLOR_BUFFER_BIT );
		glViewport( 0, 0, m_refTexture->Width(), m_refTexture->Height() );

		m_Pass[COMPOSITE].Bind();
		{
			// Bind Uniforms
			GL_SAFE_CALL( glUniform1f( m_ulKey2, 0.36f ) );
			GL_SAFE_CALL( glUniform1f( m_ulBloomWeight, 0.5f ) );

			// Bind Textures
			GL_SAFE_CALL( glActiveTexture( g_DefaultTextureUnits[0] ) );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_refTexture->texID ) );

			GL_SAFE_CALL( glActiveTexture( g_DefaultTextureUnits[1] ) );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_BlurredImage2.texID ) );

			GL_SAFE_CALL( glActiveTexture( g_DefaultTextureUnits[2] ) );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_AdaptedLuminance[m_CurrAdaptedLuminance].texID ) );

			m_refScreenSpaceQuad->Draw();
		}
		m_Pass[COMPOSITE].Unbind();
	}


}// end of namespace