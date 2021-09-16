#include	"EdgeDetectionShader.h"

#include	<oreore/common/TString.h>

#include	<graphics/gl4x/resource/GLBindPointManager.h>



namespace OreOreLib
{


#define TEX_SIZE 512
#define TEX_SIZE 512


	const Vec2f quad_verts[] =
	{
		{ -1.0, -1.0 },
		{ -1.0, +1.0 },
		{ +1.0, +1.0 },
		{ +1.0, -1.0 },
	};


	const Vec2f quad_texcoord[] =
	{
		{ 0.0, 0.0 },
		{ 0.0, +1.0 },
		{ +1.0, +1.0 },
		{ +1.0, 0.0 },
	};


	const uint32 quad_indices[] =
	{
		0, 1, 2,
		3, 0, 2,
	};





	// テクスチャの合成
	// スケール毎にノイズ関数を用意する
	// 段階的に





	// default constructor
	EdgeDetectionShader::EdgeDetectionShader()
	{
		m_pShader		= NULL;
		m_ulRadius		= -1;
		m_ulTexture		= -1;
		m_refTexture	= NULL;
		m_Radius		= 1;
	}


	// constructor
	EdgeDetectionShader::EdgeDetectionShader( const TCHAR *filepath, GLSL_VERSION version )
	{
		m_pShader		= NULL;
		m_ulRadius		= -1;
		m_ulTexture		= -1;
		m_refTexture	= NULL;
		m_Radius		= 1;

		if( filepath )
			InitShader( filepath, version );
	}


	// destructor
	EdgeDetectionShader::~EdgeDetectionShader()
	{
		Release();
	}


	void EdgeDetectionShader::Release()
	{
		tcout << "EdgeDetectionShader::Release()..." << tendl;

		SafeDelete( m_pShader );
		m_refTexture	= NULL;

		//m_uboAttribute.Release();
	}


	// init shader
	void EdgeDetectionShader::InitShader( const TCHAR *filepath, GLSL_VERSION version )
	{
		const type_info& id = typeid( *this );
		tcout << "AbstractClass-typeid: " << id.name() << tendl;


		//##########################	Init Shader		##########################//
		// create shader
		m_pShader	= new GLShader();
		m_pShader->Init( filepath, version );

		// init attribute location
		glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_POSITION, "POSITION" );
		glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" );

		// uniform location
		m_ulTexture	= glGetUniformLocation( m_pShader->ID(), "g_Texture" );
		m_ulRadius	= glGetUniformLocation( m_pShader->ID(), "g_FilterRadius" );

		// link shader
		m_pShader->Link();


		//######################	Init UniformBuffers		######################//
		//m_uboAttribute.Init( GL_STATIC_DRAW, GLBindPointManager::Reserve( GL_UNIFORM_BUFFER ) );
		//m_uboAttribute.BindToShaderByName( m_pShader->ID(), "ubAttribute" );

	}


	void EdgeDetectionShader::SetFilterRadius( int radius )
	{
		m_Radius = radius;
		//m_uboAttribute.UpdateSubresource( &radius, sizeof(int) );
	}


	void EdgeDetectionShader::Render()
	{
		m_pShader->Bind();


		//============================= AttributerLocationの設定 ==============================//
		// 頂点座標
		glVertexAttribPointer( ATTRIB_LOCATION_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof( Vec2f ), quad_verts );
		glEnableVertexAttribArray( ATTRIB_LOCATION_POSITION );

		// テクスチャ座標
		glVertexAttribPointer( ATTRIB_LOCATION_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof( Vec2f ), quad_texcoord );
		glEnableVertexAttribArray( ATTRIB_LOCATION_TEXCOORD );


		//=============================== UniformLocationの設定 ===============================//
		if( m_refTexture )
		{
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, m_refTexture->texID );
			glUniform1i( m_ulTexture, 0 );
		}

		glUniform1i( m_ulRadius, m_Radius );

		//=============================== オブジェクトの描画 ==================================//
		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, quad_indices );

		//================================ 頂点バッファの解放 =================================//
		glDisableVertexAttribArray( ATTRIB_LOCATION_POSITION );
		glDisableVertexAttribArray( ATTRIB_LOCATION_TEXCOORD );

		m_pShader->Unbind();
	}


}// end of namespace