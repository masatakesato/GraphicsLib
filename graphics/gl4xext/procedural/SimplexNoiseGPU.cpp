#include	"SimplexNoiseGPU.h"


#define DEFAULT_OFFSET 5000.0f


const Vec2f quad_verts[] =
{
	{ -1.0f, -1.0f },
	{ -1.0f, +1.0f },
	{ +1.0f, +1.0f },
	{ +1.0f, -1.0f },
};


const Vec2f quad_texcoord[] =
{
	{ -1.0f, -1.0f },
	{ -1.0f, +1.0f },
	{ +1.0f, +1.0f },
	{ +1.0f, -1.0f },
};


const uint32 quad_indices[] =
{
	0, 1, 2, 
	3, 0, 2,
};



// default constructor
SimplexNoiseGPU::SimplexNoiseGPU()
{
	m_pShader			= NULL;

	m_ulPermTexture		= -1;

	//m_ulScale			= -1;
	//m_ulStretch			= -1;
	m_ulOctaves			= -1;
	m_ulLacunarity		= -1;
	m_ulGain			= -1;
	m_ulMatTransform	= -1;

	m_NoiseParam.Scale			= 1.0f;
	m_NoiseParam.Angle			= 0.0f;
	m_NoiseParam.Stretch		= 0.0f;
	m_NoiseParam.Octaves		= 1.0f;
	m_NoiseParam.Lacunarity		= 1.5f;
	m_NoiseParam.Gain			= 0.5f;

	m_OutputParam.Contrast		= 1.0f;
	m_OutputParam.Brightness	= 0.0f;
	m_OutputParam.Threshold		= 0.5f;
	m_OutputParam.Smoothness	= 1.0f;
	m_OutputParam.Invert		= false;
}

// constructor
SimplexNoiseGPU::SimplexNoiseGPU( const char *filepath, GLSL_VERSION version )
{
	const type_info& id = typeid(*this);
	tcout << "AbstractClass-typeid: " << id.name() << tendl;

	InitShader( filepath, version );
}

// destructor
SimplexNoiseGPU::~SimplexNoiseGPU()
{
	Release();
}


void SimplexNoiseGPU::Release()
{
	SafeDelete( m_pShader );
	m_PermTexture.Release();

	m_ulPermTexture		= -1;

	//m_ulScale			= -1;
	//m_ulStretch			= -1;
	m_ulOctaves			= -1;
	m_ulLacunarity		= -1;
	m_ulGain			= -1;
	m_ulMatTransform	= -1;

	m_NoiseParam.Scale			= 1.0f;
	m_NoiseParam.Angle			= 0.0f;
	m_NoiseParam.Stretch		= 0.0f;
	m_NoiseParam.Octaves		= 1.0f;
	m_NoiseParam.Lacunarity		= 1.5f;
	m_NoiseParam.Gain			= 0.5f;

	m_OutputParam.Contrast		= 1.0f;
	m_OutputParam.Brightness	= 0.0f;
	m_OutputParam.Threshold		= 0.5f;
	m_OutputParam.Smoothness	= 1.0f;
	m_OutputParam.Invert		= false;

}



// init shader
void SimplexNoiseGPU::InitShader( const char *filepath, GLSL_VERSION version )
{
	// create shader
	m_pShader	= new GLShader();
	m_pShader->Init( filepath, version );
	
	// init attribute location
	glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_POSITION, "POSITION" );
	glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" );

	// init uniform location	
	m_ulPermTexture		= glGetUniformLocation( m_pShader->ID(), "g_SimplexPermTexture" );
	//m_ulScale			= glGetUniformLocation( m_pShader->ID(), "g_Scale" );
	//m_ulStretch			= glGetUniformLocation( m_pShader->ID(), "g_Stretch" );
	m_ulOctaves			= glGetUniformLocation( m_pShader->ID(), "g_Octaves" );
	m_ulLacunarity		= glGetUniformLocation( m_pShader->ID(), "g_Lacunarity" );
	m_ulGain			= glGetUniformLocation( m_pShader->ID(), "g_Gain" );
	m_ulMatTransform	= glGetUniformLocation( m_pShader->ID(), "g_MatTransform" );
	
	m_ulPostProcessParam.Contrast		= glGetUniformLocation( m_pShader->ID(), "g_Contrast" );
	m_ulPostProcessParam.Brightness	= glGetUniformLocation( m_pShader->ID(), "g_Brightness" );
	m_ulPostProcessParam.Threshold	= glGetUniformLocation( m_pShader->ID(), "g_Threshold" );
	m_ulPostProcessParam.Smoothness	= glGetUniformLocation( m_pShader->ID(), "g_Smoothness" );
	m_ulPostProcessParam.Invert		= glGetUniformLocation( m_pShader->ID(), "g_Invert" );
	
	// link shader
	m_pShader->Link();

	// init fragment output location
	glBindFragDataLocation( m_pShader->ID(), 0, "outColor" );
	

	// perm textureの初期化とシェーダーへの割り当て
	InitPermTexture();
	m_PermTexture.GenHardwareTexture();
	

	m_pShader->Bind();
	{
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_1D, m_PermTexture.texID ); 
		glUniform1i( m_ulPermTexture, 0 );
	}
	m_pShader->Unbind();
}



// メッシュデータをレンダリングする(シェーダーはバインド済みと想定)
void SimplexNoiseGPU::Render()
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
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_1D, m_PermTexture.texID ); 
	glUniform1i( m_ulPermTexture, 0 );

	//glUniform1f( m_ulScale, m_Scale ); 
	//glUniform1f( m_ulStretch, m_Stretch );
	glUniform1f( m_ulOctaves, m_NoiseParam.Octaves );
	glUniform1f( m_ulLacunarity, m_NoiseParam.Lacunarity );
	glUniform1f( m_ulGain, m_NoiseParam.Gain );


	// トランスフォーム行列を計算
	static Quatf	quat;
	static Mat4f	matRotScale, matScale, matRotation, matTranslation;
	
	MatScale( matScale, m_NoiseParam.Scale, m_NoiseParam.Scale/max(m_NoiseParam.Stretch+1.0f, 1.0f), m_NoiseParam.Scale );

	InitQuat( quat, m_NoiseParam.Angle, 0.0f, 0.0f, 1.0f );
	Quat2Mat( matRotation, quat );
	
	Multiply( matRotScale, matScale, matRotation );

	MatTranslation( matTranslation, DEFAULT_OFFSET, DEFAULT_OFFSET, DEFAULT_OFFSET );

	// m_MatTransform = [ matTranslation ] * [ matRotation ] * [ matScale ] * [ 頂点座標 ]. 回転,拡大縮小が終わった後で平行移動する
	Multiply( m_MatTransform, matTranslation, matRotScale );

	glUniformMatrix4fv( m_ulMatTransform, 1, GL_TRUE, m_MatTransform.m );


	glUniform1f( m_ulPostProcessParam.Contrast,		m_OutputParam.Contrast );
	glUniform1f( m_ulPostProcessParam.Brightness,	m_OutputParam.Brightness );
	glUniform1f( m_ulPostProcessParam.Threshold,	m_OutputParam.Threshold );
	glUniform1f( m_ulPostProcessParam.Smoothness,	m_OutputParam.Smoothness );
	glUniform1f( m_ulPostProcessParam.Invert,		m_OutputParam.Invert ? 1.0f : 0.0f );



	//=============================== オブジェクトの描画 ==================================//
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, quad_indices );
	

	//================================ 頂点バッファの解放 =================================//
	glDisableVertexAttribArray( ATTRIB_LOCATION_POSITION );
	glDisableVertexAttribArray( ATTRIB_LOCATION_TEXCOORD );
	
	m_pShader->Unbind();
}



void SimplexNoiseGPU::InitPermTexture()
{
	//const unsigned char p[256] =
	//{
	//	151,160,137,91,90,15,
	//	131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
	//	190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
	//	88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
	//	77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
	//	102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
	//	135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
	//	5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
	//	223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
	//	129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
	//	251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
	//	49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
	//	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	//};


	//m_PermTexture.Allocate( 512, 0, 0, FORMAT_R8_UINT );

	//unsigned char *pp	= m_PermTexture.m_pData;
	//for( int i=0; i<512; ++i)	pp[i] = p[i&255];

	byte *perm	= new byte[1024];//m_PermTexture.m_pData;
	int i, j, k;

	for( i=0; i</*512*/1024; ++i )	perm[i]	= i%512/*256*/;

	for( i=0; i</*512*/1024; ++i )
	{
		j		= perm[i];
		k		= rand()&511/*255*/;
		perm[i]	= perm[k];
		perm[k]	= j;
	}// end of i loop


	// 注意: アーティファクト回避のため、permルックアップテーブル要素数を拡張した(256->512へ)
	m_PermTexture.Allocate( /*512*/1024, 0, 0, FORMAT_R8_UINT, perm );
	//m_PermTexture.AllocateBuffer

	delete[] perm;
}