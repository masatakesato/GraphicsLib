#include	"SuggestiveContourShader.h"



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



// default constructor
SuggestiveContourShader::SuggestiveContourShader()
{
	m_pShader	= NULL;
	m_ulTexture	= -1;
	m_ulScale	= -1;
}

// constructor
SuggestiveContourShader::SuggestiveContourShader( const char *filepath )
{
	const type_info& id = typeid(*this);
	cout << "AbstractClass-typeid: " << id.name() << endl;

	InitShader( filepath );
}

// destructor
SuggestiveContourShader::~SuggestiveContourShader()
{
	Release();
	/*SafeDelete( m_pShader );*/

}


void SuggestiveContourShader::Release()
{
	SafeDelete( m_pShader );
	m_ulTexture	= -1;
	m_ulScale	= -1;
}



// init shader
void SuggestiveContourShader::InitShader( const char *filepath )
{
	// create shader
	m_pShader	= new GLShader();
	m_pShader->Init( filepath );
	
	// init attribute location
	glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_POSITION, "POSITION" );
	glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" );

	// init uniform location	
	m_ulTexture	= glGetUniformLocation( m_pShader->ID(), "g_Texture" );
	m_ulScale	= glGetUniformLocation( m_pShader->ID(), "g_Scale" );		

	// link shader
	m_pShader->Link();

	// その他
	// glBindFragDataLocation( program_id, channel, "FragColor0" );
	// サンプル
}



// メッシュデータをレンダリングする(シェーダーはバインド済みと想定)
void SuggestiveContourShader::Render( uint32 texid, float scale )
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
	// Texture
	if( texid )
	{
		// TODO: ホントは複数チャンネル対応したいけど、、、
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, texid ); 
		glUniform1i( m_ulTexture, 0 );
	}
	
	glUniform1f( m_ulScale, scale );

	
	//=============================== オブジェクトの描画 ==================================//
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, quad_indices );
	
	//================================ 頂点バッファの解放 =================================//
	glDisableVertexAttribArray( ATTRIB_LOCATION_POSITION );
	glDisableVertexAttribArray( ATTRIB_LOCATION_TEXCOORD );
	
	m_pShader->Unbind();
}




}// end of namespace