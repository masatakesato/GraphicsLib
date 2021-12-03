#include	"SeparableSSSShader.h"

#include	<graphics/gl4x/common/GLHelperFunctions.h>
#include	<graphics/gl4x/scene/LightAttributeBuffer.h>
#include	<graphics/gl4x/resource/ShadowBuffer.h>



namespace OreOreLib
{

	// default constructor
	SeparableSSSShader::SeparableSSSShader()
	{
#ifdef _DEBUG
		tcout << "SeparableSSSShader()..." << tendl;
#endif // _DEBUG
		m_pShader			= NULL;

		m_ulBlurDir			= -1;
		//m_ulScreenSize		= -1;
		m_ulScale			= -1;
		m_ulTranslucency	= -1;

		m_ulTexDepth		= -1;
		m_ulTexColor		= -1;
		m_ulTexNormal		= -1;
		m_ulTexPosition		= -1;
		m_ulShadowMapArray	= -1;

		m_refTexDepth		= NULL;
		m_refTexColor		= NULL;
		m_refTexNormal		= NULL;
		m_refTexPosition	= NULL;

		m_AspectRatio		= 1.0f;
		InitVec( m_hBlurDir, 1.0f, 0.0f );
		InitVec( m_vBlurDir, 0.0f, 1.0f );
		m_Translucency		= 0.0f;
	}


	// constructor
	SeparableSSSShader::SeparableSSSShader( const TCHAR* filepath, GLSL_VERSION version )
	{
#ifdef _DEBUG
		tcout << "SeparableSSSShader( const TCHAR* filepath, GLSL_VERSION version )..." << tendl;
#endif // _DEBUG
		//	const type_info& id = typeid(*this);
		//	cout << "AbstractClass-typeid: " << id.name() << endl;

		m_pShader			= NULL;

		m_ulBlurDir			= -1;
		//m_ulScreenSize		= -1;
		m_ulScale			= -1;
		m_ulTranslucency	= -1;

		m_ulTexDepth		= -1;
		m_ulTexColor		= -1;
		m_ulTexNormal		= -1;
		m_ulTexPosition		= -1;
		m_ulShadowMapArray	= -1;

		m_refTexDepth		= NULL;
		m_refTexColor		= NULL;
		m_refTexNormal		= NULL;
		m_refTexPosition	= NULL;

		m_AspectRatio		= 1.0f;
		InitVec( m_hBlurDir, 1.0f, 0.0f );
		InitVec( m_vBlurDir, 0.0f, 1.0f );
		m_Translucency		= 0.0f;

		if( filepath )
			InitShader( filepath, version );
	}


	// destructor
	SeparableSSSShader::~SeparableSSSShader()
	{
#ifdef _DEBUG
		tcout << _T( "~SeparableSSSShader()..." ) << tendl;
#endif
		SafeDelete( m_pShader );
		UnbindTexture();
	}


	// init shader
	void SeparableSSSShader::InitShader( const TCHAR* filepath, GLSL_VERSION version )
	{
		GLuint	program_id;

		// create shader
		m_pShader	= new GLShader();
		m_pShader->Init( filepath, version );
		program_id	= m_pShader->ID();


		// init attribute location
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_NORMAL, "NORMAL" ) );

		// init uniform location
		m_ulBlurDir			= GL_SAFE_CALL( glGetUniformLocation( program_id, "blur_dir" ) );
		//m_ulScreenSize		= glGetUniformLocation( program_id, "screen_size" );
		m_ulScale			= GL_SAFE_CALL( glGetUniformLocation( program_id, "scale_separable" ) );
		m_ulTranslucency	= GL_SAFE_CALL( glGetUniformLocation( program_id, "translucency" ) );

		m_ulTexDepth		= GL_SAFE_CALL( glGetUniformLocation( program_id, "camera_depth_texture" ) );
		m_ulTexColor		= GL_SAFE_CALL( glGetUniformLocation( program_id, "color_texture" ) );
		m_ulTexPosition		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_TexPosition" ) );
		m_ulTexNormal		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_TexNormal" ) );
		m_ulShadowMapArray	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_ShadowMapArray" ) );

		// link shader
		m_pShader->Link();

		// init fragment output location
		GL_SAFE_CALL( glBindFragDataLocation( m_pShader->ID(), FRAG_COLOR, "outColor" ) );
	}



	void SeparableSSSShader::BindBufferObject( const IBufferObject* pbufferobj )
	{
		const type_info& id = typeid(*pbufferobj);

		if( id == typeid(LightAttributeBuffer) )
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

			glActiveTexture( g_DefaultTextureUnits[ TEX_SLOT_SHADOWMAP ] );
			glBindTexture( GL_TEXTURE_2D_ARRAY, pshadowbuf->GetTexture()->texID ); 
			glUniform1i( m_ulShadowMapArray, TEX_SLOT_SHADOWMAP );
			
			m_pShader->Unbind();
		}	
	}



	void SeparableSSSShader::BindTexture( const Texture2D& tex_depth, const Texture2D& tex_color, const Texture2D& tex_normal, const Texture2D& tex_pos )
	{
		// 既にテクスチャをバインド済みで、かつ解像度が同じ場合はバインド中止
		if( m_refTexDepth && m_refTexColor && m_refTexNormal )
		{
			if( m_IntermediateBuffer.Width()==tex_color.Width() && m_IntermediateBuffer.Height()==tex_color.Height() )
				return;
		}


		// 入力テクスチャの参照を更新する
		m_refTexDepth		= (Texture2D *)&tex_depth;
		m_refTexColor		= (Texture2D *)&tex_color;
		m_refTexNormal		= (Texture2D *)&tex_normal;
		m_refTexPosition	= (Texture2D *)&tex_pos;
		
		// Init Blur Directions　// 水平鉛直の方が速いが汚い。斜め45度はきれいだが遅い
		m_AspectRatio		= (float)m_refTexDepth->Width() / (float)m_refTexDepth->Height();
		InitVec( m_hBlurDir, 1.0f / m_AspectRatio, 1.0f );// InitVec( m_hBlurDir, 1.0f/m_AspectRatio, 0.0f );//
		InitVec( m_vBlurDir, -1.0f / m_AspectRatio, 1.0f );// InitVec( m_vBlurDir, 0.0f, 1.0f );//
		Normalize( m_hBlurDir );
		Normalize( m_vBlurDir );
		m_Translucency		= 0.0f;

		// Allocate Intermediate Buffer
		m_IntermediateBuffer.Allocate( tex_color.Width(), tex_color.Height(), 0, 0, tex_color.DataFormat() );
		m_IntermediateBuffer.GenHardwareTexture();


		// Init FramebufferObject
		const uint32 texids[]	={ m_IntermediateBuffer.texID };
		m_RenderTarget.Init( m_IntermediateBuffer.Width(), m_IntermediateBuffer.Height(), /*true*/false );
		m_RenderTarget.BindTextures( ArraySize<int>( texids ), g_DefaultColorAttachments, texids );
	}



	void SeparableSSSShader::UnbindTexture()
	{
		m_refTexDepth		= NULL;
		m_refTexColor		= NULL;
		m_refTexNormal		= NULL;
		m_refTexPosition	= NULL;
		
		m_RenderTarget.UnbindTextures();
		m_IntermediateBuffer.Release();

		m_AspectRatio	= 1.0f;
		InitVec( m_hBlurDir, 1.0f, 0.0f );
		InitVec( m_vBlurDir, 0.0f, 1.0f );
		m_Translucency	= 0.0f;
	}


	
	void SeparableSSSShader::Render()
	{
		if( !m_refTexDepth || !m_refTexColor || !m_refTexNormal || !m_refTexPosition || !m_refScreenSpaceQuad )
			return;


		m_pShader->Bind();
		{
			// Bind Common Uniforms
			//glUniform2f( m_ulScreenSize, (float)m_refTexDepth->Width(), (float)m_refTexDepth->Height() );
			glUniform1f( m_ulScale, m_Scale );
			glUniform1f( m_ulTranslucency, m_Translucency );

			// Bind Textures
			glActiveTexture( g_DefaultTextureUnits[ TEX_SLOT_DEPTH ] );
			glBindTexture( GL_TEXTURE_2D, m_refTexDepth->texID );	// set input depth texture to TEX_SLOT_DEPTH slot
			glUniform1i( m_ulTexDepth, TEX_SLOT_DEPTH );

			glActiveTexture( g_DefaultTextureUnits[TEX_SLOT_NORMAL] );
			glBindTexture( GL_TEXTURE_2D, m_refTexNormal->texID );	// set input normal texture to TEX_SLOT_NORMAL slot
			glUniform1i( m_ulTexNormal, TEX_SLOT_NORMAL );

			glActiveTexture( g_DefaultTextureUnits[TEX_SLOT_POSITION] );
			glBindTexture( GL_TEXTURE_2D, m_refTexPosition->texID );	// set input position texture to TEX_SLOT_POSITION slot
			glUniform1i( m_ulTexPosition, TEX_SLOT_POSITION );


			//============================= Pass1: Horizontal Blur ==============================//
			m_RenderTarget.Bind();
			{
				// Clear Buffer
				glClear( GL_COLOR_BUFFER_BIT /*| GL_DEPTH_BUFFER_BIT*/ );

				// Set Uniform Variables
				glUniform2fv( m_ulBlurDir, 1, m_hBlurDir.xy );
								
				// Set Texture
				glActiveTexture( g_DefaultTextureUnits[TEX_SLOT_COLOR] );
				glBindTexture( GL_TEXTURE_2D, m_refTexColor->texID );	// set input color texture to TEX_SLOT_COLOR slot
				glUniform1i( m_ulTexColor, TEX_SLOT_COLOR );

				// Draw Screen Quad
				m_refScreenSpaceQuad->Draw();
			}
			m_RenderTarget.Unbind();


			//============================ Pass2: Vertical Blur ================================//
			//glClear( GL_COLOR_BUFFER_BIT /*| GL_DEPTH_BUFFER_BIT*/ );
			
			// Set Uniform Variables
			glUniform2fv( m_ulBlurDir, 1, m_vBlurDir.xy );

			// Set Texture
			glActiveTexture( g_DefaultTextureUnits[ TEX_SLOT_COLOR ]  );
			glBindTexture( GL_TEXTURE_2D, m_IntermediateBuffer.texID );	// set blurred color texture to TEX_SLOT_COLOR slot
			glUniform1i( m_ulTexColor, TEX_SLOT_COLOR );

			// draw screen quad
			m_refScreenSpaceQuad->Draw();
			
		}
		m_pShader->Unbind();

	}




}// end of namespace