#include	"RustShader.h"




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
RustShader::RustShader()
{
	m_pShader			= NULL;
	m_ulRustMask		= -1;
	m_ulPermSimplex		= -1;
	m_ulPermWorley		= -1;

	m_Aging					= 1.0f;
	m_Scale					= 10.0f;
	InitVec( m_Offset, 500.0f, 500.0f, 500.0f );
	m_DistribAmount			= 0.57f;
	m_DistribSmoothness		= 1.56f;
	m_SpotAmount			= 1.68f;
	m_SpotSmoothness		= 0.43f;
	m_BleedAmount			= 0.44f;
	m_BleedSmoothness		= 0.45f;
	m_BaseAmount			= 0.71f;
	m_BaseSmoothness		= 0.53f;
	m_OuterBleedAmount		= 0.16f;
	m_OuterBleedSmoothness	= 0.37f;
	m_OuterBaseAmount		= 0.34f;
	m_OuterBaseSmoothness	= 0.31f;
}

// constructor
RustShader::RustShader( const TCHAR *filepath, GLSL_VERSION version )
{
	const type_info& id = typeid(*this);
	tcout << _T("AbstractClass-typeid: ") << id.name() << tendl;

	InitShader( filepath, version );

	m_Aging					= 1.0f;
	m_Scale					= 10.0f;
	InitVec( m_Offset, 500.0f, 500.0f, 500.0f );
	m_DistribAmount			= 0.57f;
	m_DistribSmoothness		= 1.56f;
	m_SpotAmount			= 1.68f;
	m_SpotSmoothness		= 0.43f;
	m_BleedAmount			= 0.44f;
	m_BleedSmoothness		= 0.45f;
	m_BaseAmount			= 0.71f;
	m_BaseSmoothness		= 0.53f;
	m_OuterBleedAmount		= 0.16f;
	m_OuterBleedSmoothness	= 0.37f;
	m_OuterBaseAmount		= 0.34f;
	m_OuterBaseSmoothness	= 0.31f;
}

// destructor
RustShader::~RustShader()
{
	Release();
}


void RustShader::Release()
{
	SafeDelete( m_pShader );
	m_PermSimplex.Release();
	m_PermWorley.Release();

	m_Aging					= 1.0f;
	m_Scale					= 10.0f;
	InitVec( m_Offset, 500.0f, 500.0f, 500.0f );
	m_DistribAmount			= 0.57f;
	m_DistribSmoothness		= 1.56f;
	m_SpotAmount			= 1.68f;
	m_SpotSmoothness		= 0.43f;
	m_BleedAmount			= 0.44f;
	m_BleedSmoothness		= 0.45f;
	m_BaseAmount			= 0.71f;
	m_BaseSmoothness		= 0.53f;
	m_OuterBleedAmount		= 0.16f;
	m_OuterBleedSmoothness	= 0.37f;
	m_OuterBaseAmount		= 0.34f;
	m_OuterBaseSmoothness	= 0.31f;
}





// init shader
void RustShader::InitShader( const TCHAR *filepath, GLSL_VERSION version )
{
	// create shader
	m_pShader	= new GLShader();
	m_pShader->Init( filepath, version );
	
	// init attribute location
	GL_SAFE_CALL( glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_POSITION, "POSITION" ) );
	GL_SAFE_CALL( glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );

	// init uniform location	
	m_ulRustMask		= glGetUniformLocation( m_pShader->ID(), "g_RustMap" );
	m_ulPermSimplex		= glGetUniformLocation( m_pShader->ID(), "g_SimplexPermTexture" );
	m_ulPermWorley		= glGetUniformLocation( m_pShader->ID(), "g_WorleyPermTexture" );
	
	m_ulAging			= glGetUniformLocation( m_pShader->ID(), "g_Aging" );
	m_ulScale			= glGetUniformLocation( m_pShader->ID(), "g_Scale" );
	m_ulOffset			= glGetUniformLocation( m_pShader->ID(), "g_Offset" );

	m_ulDistribAmount	= glGetUniformLocation( m_pShader->ID(), "g_DistributionAmount" );
	m_ulDistribSmoothness	= glGetUniformLocation( m_pShader->ID(), "g_DistributionSmoothness" );
	m_ulSpotAmount		= glGetUniformLocation( m_pShader->ID(), "g_SpotAmount" );
	m_ulSpotSmoothness	= glGetUniformLocation( m_pShader->ID(), "g_SpotSmoothness" );
	m_ulBleedAmount		= glGetUniformLocation( m_pShader->ID(), "g_BleedAmount" );
	m_ulBleedSmoothness		= glGetUniformLocation( m_pShader->ID(), "g_BleedSmoothness" );
	m_ulBaseAmount		= glGetUniformLocation( m_pShader->ID(), "g_BaseAmount" );
	m_ulBaseSmoothness	= glGetUniformLocation( m_pShader->ID(), "g_BaseSmoothness" );

	m_ulOuterBleedAmount	= glGetUniformLocation( m_pShader->ID(), "g_OuterBleedAmount" );
	m_ulOuterBleedSmoothness	= glGetUniformLocation( m_pShader->ID(), "g_OuterBleedSmoothness" );
	m_ulOuterBaseAmount		= glGetUniformLocation( m_pShader->ID(), "g_OuterBaseAmount" );
	m_ulOuterBaseSmoothness	= glGetUniformLocation( m_pShader->ID(), "g_OuterBaseSmoothness" );


	// link shader
	m_pShader->Link();


	InitPermTexture();
	m_PermSimplex.GenHardwareTexture();
	m_PermWorley.GenHardwareTexture();


	m_pShader->Bind();
	{
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_1D, m_PermSimplex.texID ); 
		glUniform1i( m_ulPermSimplex, 0 );

		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_1D, m_PermWorley.texID ); 
		glUniform1i( m_ulPermWorley, 1 );
	}
	m_pShader->Unbind();


	// その他
	// glBindFragDataLocation( program_id, channel, "FragColor0" );
	// サンプル
}



//void RustShader::BindBufferObject( const IBufferObject* pbufferobj )
//{
//	const type_info &buffer_type	= typeid(*pbufferobj);
//	//tcout << "class name: " << id.name() << tendl;// .name()使うと、コンパイラが間違えてメモリリーク報告してくる. 2013.08.05
//
//
//	if( buffer_type == typeid(ViewTransformBuffer) )	// ビュー行列だった場合
//	{
//		((ViewTransformBuffer *)pbufferobj)->BindToShader( m_Shader.ID() );
//	}
//	else
//	{
//	}
//
//}



// メッシュデータをレンダリングする(シェーダーはバインド済みと想定)
void RustShader::Render( /*uint32 distrib, /*uint32 rustshape, uint32 inner,*/ uint32 rust_mask )
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
	if( rust_mask )
	{
		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D, rust_mask ); 
		glUniform1i( m_ulRustMask, 2 );
	}


	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_1D, m_PermSimplex.texID ); 
	glUniform1i( m_ulPermSimplex, 0 );

	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_1D, m_PermWorley.texID ); 
	glUniform1i( m_ulPermWorley, 1 );

	//if( distrib )
	//{
	//	glActiveTexture( GL_TEXTURE1 );
	//	glBindTexture( GL_TEXTURE_2D, distrib ); 
	//	glUniform1i( m_ulTexture, 1 );
	//}
	//
	//if( inner )
	//{
	//	glActiveTexture( GL_TEXTURE2 );
	//	glBindTexture( GL_TEXTURE_2D, inner ); 
	//	glUniform1i( m_ulInnerRust, 2 );
	//}
	//
	//if( spot_distrib )
	//{
	//	glActiveTexture( GL_TEXTURE3 );
	//	glBindTexture( GL_TEXTURE_2D, spot_distrib ); 
	//	glUniform1i( m_ulSpotDistribution, 3 );
	//}


	glUniform1f( m_ulAging, m_Aging );
	glUniform1f( m_ulScale, m_Scale );
	glUniform3fv( m_ulOffset, 1, m_Offset.xyz );
	glUniform1f( m_ulDistribAmount, m_DistribAmount );
	glUniform1f( m_ulDistribSmoothness, m_DistribSmoothness );
	glUniform1f( m_ulSpotAmount, m_SpotAmount );
	glUniform1f( m_ulSpotSmoothness, m_SpotSmoothness );
	glUniform1f( m_ulBleedAmount, m_BleedAmount );
	glUniform1f( m_ulBleedSmoothness, m_BleedSmoothness );
	glUniform1f( m_ulBaseAmount, m_BaseAmount );
	glUniform1f( m_ulBaseSmoothness, m_BaseSmoothness );
	glUniform1f( m_ulOuterBleedAmount, m_OuterBleedAmount );
	glUniform1f( m_ulOuterBleedSmoothness, m_OuterBleedSmoothness );
	glUniform1f( m_ulOuterBaseAmount, m_OuterBaseAmount );
	glUniform1f( m_ulOuterBaseSmoothness, m_OuterBaseSmoothness );
	

	//=============================== オブジェクトの描画 ==================================//
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, quad_indices );
	
	//================================ 頂点バッファの解放 =================================//
	glDisableVertexAttribArray( ATTRIB_LOCATION_POSITION );
	glDisableVertexAttribArray( ATTRIB_LOCATION_TEXCOORD );
	
	m_pShader->Unbind();
}




void RustShader::InitPermTexture()
{
	// initialize perm texture for simplex
	m_PermSimplex.Allocate( 512, 0, 0, FORMAT_R8_UINT );

	unsigned char *perm	= m_PermSimplex.m_pData;
	int i, j, k;

	for( i=0; i<512; ++i )	perm[i]	= i%256;

	for( i=0; i<512; ++i )
	{
		j		= perm[i];
		k		= rand()&255;
		perm[i]	= perm[k];
		perm[k]	= j;
	}// end of i loop


	// initialize perm texture for worley
	m_PermWorley.Allocate( 256, 0, 0, FORMAT_R8_UINT );
	perm	= m_PermWorley.m_pData;

	for( i=0; i<256; ++i )	perm[i]	= i;

	for( i=0; i<256; ++i )
	{
		j		= perm[i];
		k		= rand()&255;
		perm[i]	= perm[k];
		perm[k]	= j;
	}// end of i loop

}