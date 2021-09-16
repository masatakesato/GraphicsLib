#include	"BlendShader.h"



namespace OreOreLib
{




const Vec2f quad_verts[] =
{
	{ -1.0, -1.0 },
	{ -1.0, +1.0 },
	{ +1.0, +1.0 },
	{ +1.0, -1.0 },
};


const Vec2f quad_texcoord[] =
{
	{ 0.0f, 0.0f },
	{ 0.0f, +1.0f },
	{ +1.0f, +1.0f },
	{ +1.0f, 0.0f },
};


const uint32 quad_indices[] =
{
	0, 1, 2, 
	3, 0, 2,
};






// default constructor
BlendShader::BlendShader()
{
	m_pShader				= NULL;
	
	for( int i=0; i<16; ++i )	m_ulInputTextures[i]	= -1;
	m_ulNumTextures			= -1;

	m_numTextures			= 0;
	m_InputTextureArray		= NULL;

	
	m_Opacity[0] = 0.3f;
	m_Opacity[1] = 0.5f;
	m_Opacity[2] = 0.5f;


	for( int i=0; i<8; ++i)
	{
		InitVec( m_Diffuse0[i], 0.0f, 0.0f, 0.0f );
		InitVec( m_Diffuse1[i], 1.0f, 1.0f, 1.0f );
	}



}


// constructor
BlendShader::BlendShader( const char *filepath, GLSL_VERSION version )
{
	const type_info& id = typeid(*this);
	tcout << "AbstractClass-typeid: " << id.name() << tendl;

	m_Opacity[0] = 0.3f;
	m_Opacity[1] = 0.5f;
	m_Opacity[2] = 0.5f;


	for( int i=0; i<8; ++i)
	{
		InitVec( m_Diffuse0[i], 0.0f, 0.0f, 0.0f );
		InitVec( m_Diffuse1[i], 1.0f, 1.0f, 1.0f );
	}


	InitShader( filepath, version );
}


// destructor
BlendShader::~BlendShader()
{
	Release();
}


void BlendShader::Release()
{
	SafeDelete( m_pShader );
	
	m_numTextures		= 0;
	m_InputTextureArray	= NULL;

	m_Opacity[0] = 0.3f;
	m_Opacity[1] = 0.5f;
	m_Opacity[2] = 0.5f;
	m_Opacity[3] = 0.5f;
	m_Opacity[4] = 0.5f;
	m_Opacity[5] = 0.5f;
	m_Opacity[6] = 0.5f;
	m_Opacity[7] = 0.5f;

	for( int i=0; i<8; ++i)
	{
		InitVec( m_Diffuse0[i], 0.0f, 0.0f, 0.0f );
		InitVec( m_Diffuse1[i], 1.0f, 1.0f, 1.0f );
	}

}


// init shader
void BlendShader::InitShader( const char *filepath, GLSL_VERSION version )
{
	// create shader
	m_pShader	= new GLShader();
	m_pShader->Init( filepath, version );
	
	// init attribute location
	glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_POSITION, "POSITION" );
	glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" );


	// init uniform location
	m_ulInputTextures[0]	= glGetUniformLocation( m_pShader->ID(), "g_Texture0" );
	m_ulInputTextures[1]	= glGetUniformLocation( m_pShader->ID(), "g_Texture1" );
	m_ulInputTextures[2]	= glGetUniformLocation( m_pShader->ID(), "g_Texture2" );
	m_ulInputTextures[3]	= glGetUniformLocation( m_pShader->ID(), "g_Texture3" );
	m_ulInputTextures[4]	= glGetUniformLocation( m_pShader->ID(), "g_Texture4" );
	m_ulInputTextures[5]	= glGetUniformLocation( m_pShader->ID(), "g_Texture5" );
	m_ulInputTextures[6]	= glGetUniformLocation( m_pShader->ID(), "g_Texture6" );
	m_ulInputTextures[7]	= glGetUniformLocation( m_pShader->ID(), "g_Texture7" );
	m_ulInputTextures[8]	= glGetUniformLocation( m_pShader->ID(), "g_Texture8" );
	m_ulInputTextures[9]	= glGetUniformLocation( m_pShader->ID(), "g_Texture9" );
	m_ulInputTextures[10]	= glGetUniformLocation( m_pShader->ID(), "g_Texture10" );
	m_ulInputTextures[11]	= glGetUniformLocation( m_pShader->ID(), "g_Texture11" );
	m_ulInputTextures[12]	= glGetUniformLocation( m_pShader->ID(), "g_Texture12" );
	m_ulInputTextures[13]	= glGetUniformLocation( m_pShader->ID(), "g_Texture13" );
	m_ulInputTextures[14]	= glGetUniformLocation( m_pShader->ID(), "g_Texture14" );
	m_ulInputTextures[15]	= glGetUniformLocation( m_pShader->ID(), "g_Texture15" );

	m_ulNumTextures			= glGetUniformLocation( m_pShader->ID(), "g_NumTextures" );
	m_ulOpacity				= glGetUniformLocation( m_pShader->ID(), "g_Opacity" );

	m_ulDiffuse0			= glGetUniformLocation( m_pShader->ID(), "g_Diffuse0" );
	m_ulDiffuse1			= glGetUniformLocation( m_pShader->ID(), "g_Diffuse1" );
	

	// link shader
	m_pShader->Link();

	// init fragment output location
	glBindFragDataLocation( m_pShader->ID(), 0, "outColor" );
	
}



void BlendShader::BindInputTextures( int numTex, Texture2D texArray[] )
{
	// テクスチャユニット
	const GLenum TextureUnit[] =
	{
		GL_TEXTURE0,	GL_TEXTURE1,	GL_TEXTURE2,	GL_TEXTURE3, 
		GL_TEXTURE4,	GL_TEXTURE5,	GL_TEXTURE6,	GL_TEXTURE7,
		GL_TEXTURE8,	GL_TEXTURE9,	GL_TEXTURE10,	GL_TEXTURE11,
		GL_TEXTURE12,	GL_TEXTURE13,	GL_TEXTURE14,	GL_TEXTURE15,
	};



	m_pShader->Bind();
	{
		glUniform1i( m_ulNumTextures, numTex );


		for( int i=0; i<numTex; ++i )
		{
			glActiveTexture( TextureUnit[i] );
			glBindTexture( GL_TEXTURE_2D, texArray[i].texID ); 
			glUniform1i( m_ulInputTextures[i], i );
		}
		
	}
	m_pShader->Unbind();
}



// メッシュデータをレンダリングする(シェーダーはバインド済みと想定)
void BlendShader::Render()
{
	m_pShader->Bind();
	

	//============================= AttributerLocationの設定 ==============================//
	// 頂点座標
	glVertexAttribPointer( ATTRIB_LOCATION_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f), quad_verts );
	glEnableVertexAttribArray( ATTRIB_LOCATION_POSITION );
	
	// テクスチャ座標
	glVertexAttribPointer( ATTRIB_LOCATION_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f), quad_texcoord );
	glEnableVertexAttribArray( ATTRIB_LOCATION_TEXCOORD );
	

	//=============================== UniformLocationの設定 ===============================//
	glUniform1fv( m_ulOpacity, 8, m_Opacity );
	glUniform3fv( m_ulDiffuse0, 8, &m_Diffuse0[0].xyz[0] );
	glUniform3fv( m_ulDiffuse1, 8, &m_Diffuse1[0].xyz[0] );

	//=============================== オブジェクトの描画 ==================================//
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, quad_indices );
	
	//================================ 頂点バッファの解放 =================================//
	glDisableVertexAttribArray( ATTRIB_LOCATION_POSITION );
	glDisableVertexAttribArray( ATTRIB_LOCATION_TEXCOORD );
	
	m_pShader->Unbind();
}


}// end of namespace