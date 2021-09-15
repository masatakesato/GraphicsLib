#include	"ErosionShader.h"




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
ErosionShader::ErosionShader()
{
	m_pShader	= NULL;
	
	refTex		= NULL;	
}

// constructor
ErosionShader::ErosionShader( const TCHAR *filepath, GLSL_VERSION version )
{
	const type_info& id = typeid(*this);
#ifdef _DEBUG
	tcout << _T( "AbstractClass-typeid: " ) << id.name() << _T( "...\n" );
#endif // _DEBUG

	InitShader( filepath, version );
}

// destructor
ErosionShader::~ErosionShader()
{
	SafeDelete( m_pShader );


}

// init shader
void ErosionShader::InitShader( const TCHAR *filepath, GLSL_VERSION version )
{
	// create shader
	m_pShader	= new GLShader();
	m_pShader->Init( filepath, version );
	
	// initialize attribute location
	GL_SAFE_CALL( glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_POSITION, "POSITION" ) );
	GL_SAFE_CALL( glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );

	// init uniform location		
	m_ulTerrainHeight				= glGetUniformLocation( m_pShader->ID(), "g_TerrainHeight" );
	m_ulTerrainNormal				= glGetUniformLocation( m_pShader->ID(), "g_TerrainNormal" );
	m_ulWaterLevel					= glGetUniformLocation( m_pShader->ID(), "g_WaterLevel" );
	m_ulWaterNormal					= glGetUniformLocation( m_pShader->ID(), "g_WaterNormal" );
	m_ulFlow						= glGetUniformLocation( m_pShader->ID(), "g_Flow" );
	m_ulDP							= glGetUniformLocation( m_pShader->ID(), "g_DP" );
	m_ulAccelation					= glGetUniformLocation( m_pShader->ID(), "g_Accelation" );
	
	m_ulVelocity					= glGetUniformLocation( m_pShader->ID(), "g_Velocity" );
	m_ulSedimentTransportCapacity	= glGetUniformLocation( m_pShader->ID(), "g_SedimentTransportCapacity" );
	m_ulActualSediment				= glGetUniformLocation( m_pShader->ID(), "g_ActualSediment" );
	m_ulRegolithThickness			= glGetUniformLocation( m_pShader->ID(), "g_RegolithThickness" );

	// link shader
	m_pShader->Link();

	// bind texture unit
	m_pShader->Bind();
	{
		glUniform1i( m_ulTerrainHeight, 0 );
		glUniform1i( m_ulTerrainNormal, 1 );
		glUniform1i( m_ulWaterLevel, 2 );
		glUniform1i( m_ulWaterNormal, 3 );
		glUniform1i( m_ulFlow, 4 );
		glUniform1i( m_ulDP, 5 );
		glUniform1i( m_ulAccelation, 6 );
		glUniform1i( m_ulVelocity, 7 );
		glUniform1i( m_ulSedimentTransportCapacity, 8 );
		glUniform1i( m_ulActualSediment, 9 );
		glUniform1i( m_ulRegolithThickness, 10 );


	}
	m_pShader->Unbind();



	// その他
	// glBindFragDataLocation( program_id, channel, "FragColor0" );
	// サンプル
}



// メッシュデータをレンダリングする(シェーダーはバインド済みと想定)
void ErosionShader::Render()
{
	if( !m_refScreenSpaceQuad || !refTex )	return;


	m_pShader->Bind();
	{
		//  UniformLocationの設定
		// Texture

		// TODO: ホントは複数チャンネル対応したいけど、、、
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, refTex->refHeight->texID );

		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, refTex->refTerrainNormal->texID );

		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D, refTex->refWaterLevel->texID );

		glActiveTexture( GL_TEXTURE3 );
		glBindTexture( GL_TEXTURE_2D, refTex->refWaterNormal->texID );

#ifdef _DEBUG
		glActiveTexture( GL_TEXTURE4 );
		glBindTexture( GL_TEXTURE_2D, refTex->refPressure->texID );

		glActiveTexture( GL_TEXTURE5 );
		glBindTexture( GL_TEXTURE_2D, refTex->refAccel->texID );
#endif

		glActiveTexture( GL_TEXTURE6 );
		glBindTexture( GL_TEXTURE_2D, refTex->refFlow->texID );

		glActiveTexture( GL_TEXTURE7 );
		glBindTexture( GL_TEXTURE_2D, refTex->refVelocity->texID );

		glActiveTexture( GL_TEXTURE8 );
		glBindTexture( GL_TEXTURE_2D, refTex->refCapacity->texID );

		glActiveTexture( GL_TEXTURE9 );
		glBindTexture( GL_TEXTURE_2D, refTex->refSediment->texID );

		glActiveTexture( GL_TEXTURE10 );
		glBindTexture( GL_TEXTURE_2D, refTex->refRegolith->texID );


		// 描画
		m_refScreenSpaceQuad->Draw();
	}
	m_pShader->Unbind();
}



//// メッシュデータをレンダリングする(シェーダーはバインド済みと想定)
//void ErosionShader::Render()
//{
//	m_pShader->Bind();
//
//
//	//============================= AttributerLocationの設定 ==============================//
//	// 頂点座標
//	glVertexAttribPointer( POSITION, 2, GL_FLOAT, GL_FALSE, sizeof( Vec2f ), quad_verts );
//	glEnableVertexAttribArray( POSITION );
//
//	// テクスチャ座標
//	glVertexAttribPointer( TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof( Vec2f ), quad_texcoord );
//	glEnableVertexAttribArray( TEXCOORD );
//
//
//	//=============================== UniformLocationの設定 ===============================//
//	// Texture
//	if( refTex )
//	{
//		// TODO: ホントは複数チャンネル対応したいけど、、、
//		glActiveTexture( GL_TEXTURE0 );
//		glBindTexture( GL_TEXTURE_2D, refTex->refHeight->texID );
//
//		glActiveTexture( GL_TEXTURE1 );
//		glBindTexture( GL_TEXTURE_2D, refTex->refTerrainNormal->texID );
//
//		glActiveTexture( GL_TEXTURE2 );
//		glBindTexture( GL_TEXTURE_2D, refTex->refWaterLevel->texID );
//
//		glActiveTexture( GL_TEXTURE3 );
//		glBindTexture( GL_TEXTURE_2D, refTex->refWaterNormal->texID );
//
//#ifdef _DEBUG
//		glActiveTexture( GL_TEXTURE4 );
//		glBindTexture( GL_TEXTURE_2D, refTex->refPressure->texID );
//
//		glActiveTexture( GL_TEXTURE5 );
//		glBindTexture( GL_TEXTURE_2D, refTex->refAccel->texID );
//#endif
//
//		glActiveTexture( GL_TEXTURE6 );
//		glBindTexture( GL_TEXTURE_2D, refTex->refFlow->texID );
//
//		glActiveTexture( GL_TEXTURE7 );
//		glBindTexture( GL_TEXTURE_2D, refTex->refVelocity->texID );
//
//		glActiveTexture( GL_TEXTURE8 );
//		glBindTexture( GL_TEXTURE_2D, refTex->refCapacity->texID );
//
//		glActiveTexture( GL_TEXTURE9 );
//		glBindTexture( GL_TEXTURE_2D, refTex->refSediment->texID );
//
//		glActiveTexture( GL_TEXTURE10 );
//		glBindTexture( GL_TEXTURE_2D, refTex->refRegolith->texID );
//
//	}
//
//	//=============================== オブジェクトの描画 ==================================//
//	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, quad_indices );
//
//	//================================ 頂点バッファの解放 =================================//
//	glDisableVertexAttribArray( POSITION );
//	glDisableVertexAttribArray( TEXCOORD );
//
//	m_pShader->Unbind();
//}