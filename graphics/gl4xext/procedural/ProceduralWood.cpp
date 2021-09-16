#include	"ProceduralWood.h"


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
ProceduralWood::ProceduralWood()
{
	m_pShader				= NULL;

	m_ulMatWorld			= -1;
	m_ulMatViewProj			= -1;
	m_ulMatWorldViewProj	= -1;
	
	m_ulTexture				= -1;
	m_ulDiffuse				= -1;

//	m_ulPermTexture			= -1;
//	m_ulGradTexture			= -1;
	m_ulPermSimplex			= -1;
	m_ulPermWorley			= -1;

	m_ulRingInterval		= -1;
	m_ulEdgeSmoothness		= -1;

	m_ulAmplitude1			= -1;
	m_ulStretch1			= -1;
	m_ulRoughness1			= -1;

	m_ulAmplitude2			= -1;
	m_ulStretch2			= -1;
	m_ulRoughness2			= -1;


	//m_RingInterval			= 1.0f;
	//m_EdgeSmoothness		= 1.0f;

	//m_Amplitude1			= 1.0f;
	//m_Stretch1				= 1.0f;
	//m_Roughness1			= 1.0f;

	//m_Amplitude2			= 1.0f;
	//m_Stretch2				= 1.0f;
	//m_Roughness2			= 1.0f;

}

// constructor
ProceduralWood::ProceduralWood( const TCHAR *filepath, GLSL_VERSION version )
{
	InitShader( filepath, version );
}

// destructor
ProceduralWood::~ProceduralWood()
{
	// delete textures
	//m_PermTexture.Release();
	//m_PermTexture2D.Release();
	//m_GradTexture.Release();
	m_PermSimplex.Release();
	m_PermWorley.Release();

	// delete shaders
	SafeDelete( m_pShader );
}

// init shader
void ProceduralWood::InitShader( const TCHAR *filepath, GLSL_VERSION version )
{
	const type_info& id = typeid(*this);
	tcout << _T("AbstractClass-typeid: ") << id.name() << tendl;

	// init lookup tables
	//InitLookupTables();
	InitPermTexture();
	m_PermSimplex.GenHardwareTexture();
	m_PermWorley.GenHardwareTexture();

	
	// create shader
	m_pShader	= new GLShader();
	m_pShader->Init( filepath, version );
	
	// init attribute location
	glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_POSITION, "POSITION" );
	glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" );
	glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_NORMAL, "NORMAL" );

	// init uniform location
	m_ulMatWorld			= glGetUniformLocation( m_pShader->ID(), "g_MatWorld" );
	m_ulMatViewProj			= glGetUniformLocation( m_pShader->ID(), "g_MatViewProj" );
	m_ulMatWorldViewProj	= glGetUniformLocation( m_pShader->ID(), "g_MatWorldViewProj" );
	
	m_ulTexture				= glGetUniformLocation( m_pShader->ID(), "g_Texture" );
	m_ulDiffuse				= glGetUniformLocation( m_pShader->ID(), "g_Diffuse" );
	
	//m_ulPermTexture			= glGetUniformLocation( m_pShader->ID(), "permTexture" );
	//m_ulPermTexture2D		= glGetUniformLocation( m_pShader->ID(), "permTexture2d" );
	//m_ulGradTexture			= glGetUniformLocation( m_pShader->ID(), "permGradTexture" );
	m_ulPermSimplex			= glGetUniformLocation( m_pShader->ID(), "g_SimplexPermTexture" );
	m_ulPermWorley			= glGetUniformLocation( m_pShader->ID(), "g_WorleyPermTexture" );

	m_ulRingInterval		= glGetUniformLocation( m_pShader->ID(), "g_RingInterval" );
	m_ulEdgeSmoothness		= glGetUniformLocation( m_pShader->ID(), "g_Smoothness" );

	m_ulAmplitude1			= glGetUniformLocation( m_pShader->ID(), "g_Amplitude1" );
	m_ulStretch1			= glGetUniformLocation( m_pShader->ID(), "g_Stretch1" );
	m_ulRoughness1			= glGetUniformLocation( m_pShader->ID(), "g_Roughness1" );

	m_ulAmplitude2			= glGetUniformLocation( m_pShader->ID(), "g_Amplitude2" );
	m_ulStretch2			= glGetUniformLocation( m_pShader->ID(), "g_Stretch2" );
	m_ulRoughness2			= glGetUniformLocation( m_pShader->ID(), "g_Roughness2" );

	
	// link shader
	m_pShader->Link();

	
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




	// init fragment output location
	glBindFragDataLocation( m_pShader->ID(), 0, "outColor" );

}



void ProceduralWood::SetParams( const WoodMaterialParams& params )
{
	m_Params = params;
	//m_RingInterval			= *param->RingInterval();
	//m_EdgeSmoothness		= *param->EdgeSmoothness();

	//m_Amplitude1			= *param->Amplitude(0);
	//m_Stretch1				= *param->Stretch(0);
	//m_Roughness1			= *param->Roughness(0);

	//m_Amplitude2			= *param->Amplitude(1);
	//m_Stretch2				= *param->Stretch(1);
	//m_Roughness2			= *param->Roughness(1);
}



void ProceduralWood::Render()
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
	// PermTexture
	//glActiveTexture( GL_TEXTURE1 );
	//glBindTexture( GL_TEXTURE_1D, m_PermTexture.texID ); 
	//glUniform1i( m_ulPermTexture, 1 );
	//
	//// PermTexture2D
	//glActiveTexture( GL_TEXTURE3 );
	//glBindTexture( GL_TEXTURE_2D, m_PermTexture2D.texID );
	//glUniform1i( m_ulPermTexture2D, 3 );

	//// GradTexture
	//glActiveTexture( GL_TEXTURE2 );
	//glBindTexture( GL_TEXTURE_1D, m_GradTexture.texID ); 
	//glUniform1i( m_ulGradTexture, 2 );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_1D, m_PermSimplex.texID ); 
	glUniform1i( m_ulPermSimplex, 0 );

	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_1D, m_PermWorley.texID ); 
	glUniform1i( m_ulPermWorley, 1 );



	// Material Parameter
	glUniform1f( m_ulRingInterval,		m_Params.RingInterval );
	glUniform1f( m_ulEdgeSmoothness,	m_Params.EdgeSmoothness );

	glUniform1f( m_ulAmplitude1,	m_Params.Amplitude[0] );
	glUniform1f( m_ulStretch1,		m_Params.Stretch[0] );
	glUniform1f( m_ulRoughness1,	m_Params.Roughness[0] );

	glUniform1f( m_ulAmplitude2,	m_Params.Amplitude[1] );
	glUniform1f( m_ulStretch2,		m_Params.Stretch[1] );
	glUniform1f( m_ulRoughness2,	m_Params.Roughness[1] );


	//=============================== オブジェクトの描画 ==================================//
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, quad_indices );
	

	//================================ 頂点バッファの解放 =================================//
	glDisableVertexAttribArray( ATTRIB_LOCATION_POSITION );
	glDisableVertexAttribArray( ATTRIB_LOCATION_TEXCOORD );
	
	m_pShader->Unbind();
}





/*
// メッシュデータをレンダリングする(シェーダーはバインド済みと想定)
void ProceduralWood::Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat )
{

	MeshObject *pMesh = (MeshObject *)node->m_refObject;
	pCurrVB = pMesh->m_pVertexBuffer;

	m_pShader->Bind();
	

	//============================= AttributerLocationの設定 ==============================//
	
	// 頂点座標
	glVertexAttribPointer( POSITION, 3, GL_FLOAT, GL_FALSE, pCurrVB->size, pCurrVB->Position() );
	glEnableVertexAttribArray( POSITION );
	
	// テクスチャ座標
	glVertexAttribPointer( TEXCOORD, 2, GL_FLOAT, GL_FALSE, pCurrVB->size, pCurrVB->TexCoord() );
	glEnableVertexAttribArray( TEXCOORD );
	
	// 法線ベクトル
	glVertexAttribPointer( NORMAL, 3, GL_FLOAT, GL_FALSE, pCurrVB->size, pCurrVB->Normal() );
	glEnableVertexAttribArray( NORMAL );
	
	
	//=============================== UniformLocationの設定 ===============================//
	// World Matrix
	glUniformMatrix4fv( m_ulMatWorld, 1, GL_TRUE, node->refTransformMats.matWorldTransform.m );

	// View Projection Matrix
	glUniformMatrix4fv( m_ulMatViewProj, 1, GL_TRUE, cp->matViewProjection.m );

	//m_ulMatWorld			= glGetUniformLocation( m_pShader->ID(), "g_MatWorld" );
	//m_ulMatViewProj			= glGetUniformLocation( m_pShader->ID(), "g_MatViewProj" );
	//m_ulMatWorldViewProj	= glGetUniformLocation( m_pShader->ID(), "g_MatWorldViewProj" );
	

	// Texture
	glEnable(GL_TEXTURE_2D);
	if( pMesh->m_pTextureBuffer->pData )
	{
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, *pMesh->m_pTextureBuffer->texID() ); 
		glUniform1i( m_ulTexture, 0 );
	}
	
	//// PermTexture
	//glActiveTexture( GL_TEXTURE1 );
	//glBindTexture( GL_TEXTURE_1D, m_PermTexture.texID ); 
	//glUniform1i( m_ulPermTexture, 1 );
	//
	//// PermTexture2D
	//glActiveTexture( GL_TEXTURE3 );
	//glBindTexture( GL_TEXTURE_2D, m_PermTexture2D.texID );
	//glUniform1i( m_ulPermTexture2D, 3 );

	//// GradTexture
	//glActiveTexture( GL_TEXTURE2 );
	//glBindTexture( GL_TEXTURE_1D, m_GradTexture.texID ); 
	//glUniform1i( m_ulGradTexture, 2 );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_1D, m_PermSimplex.texID ); 
	glUniform1i( m_ulPermSimplex, 0 );

	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_1D, m_PermWorley.texID ); 
	glUniform1i( m_ulPermWorley, 1 );



	// Material Parameter
	if( pMesh->m_pMaterialBuffer )
	{
		ProceduralMaterial	*procmat	= (ProceduralMaterial *)pMesh->m_pMaterialBuffer;

		glUniform1f( m_ulRingInterval,	*procmat->RingInterval() );
		glUniform1f( m_ulEdgeSmoothness,	*procmat->EdgeSmoothness() );

		glUniform1f( m_ulAmplitude1,	*procmat->Amplitude(0) );
		glUniform1f( m_ulStretch1,		*procmat->Stretch(0) );
		glUniform1f( m_ulRoughness1,	*procmat->Roughness(0) );

		glUniform1f( m_ulAmplitude2,	*procmat->Amplitude(1) );
		glUniform1f( m_ulStretch2,		*procmat->Stretch(1) );
		glUniform1f( m_ulRoughness2,	*procmat->Roughness(1) );

	}
	
	
	//=============================== オブジェクトの描画 ==================================//
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, pMesh->m_pIndexBuffer->pData );
	
	//glutSolidSphere( 5.0, 64, 64 );


	//================================ 頂点バッファの解放 =================================//
	glDisableVertexAttribArray( POSITION );
	glDisableVertexAttribArray( TEXCOORD );
	glDisableVertexAttribArray( NORMAL );
	
	m_pShader->Unbind();
	


	//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//glEnable(GL_TEXTURE_2D);
	//glBindTexture( GL_TEXTURE_2D, m_PermTexture.texID );

	//glBegin( GL_QUADS );

	////glColor3f(0.1, 1.0, 0.1);

	//glTexCoord2f(0,0);  glVertex3f(-1,-1,0);
	//glTexCoord2f(1,0);  glVertex3f(+1,-1,0);
	//glTexCoord2f(1,1);  glVertex3f(+1,+1,0);
	//glTexCoord2f(0,1);  glVertex3f(-1,+1,0);

	//glEnd();
	//
	//glBindTexture( GL_TEXTURE_2D, 0 );
	


}
*/






/*
void ProceduralWood::InitLookupTables()
{	
	//=============== Init Texture Object ================//
	const int permutation[256] =
	{
		151,160,137,91,90,15,
		131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
		129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
		49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};
	
	// gradients for 3D noise
	static Vec3f g[] =
	{
	  {1,1,0},	{-1,1,0},    {1,-1,0},    {-1,-1,0},
	  {1,0,1},  {-1,0,1},    {1,0,-1},    {-1,0,-1},
	  {0,1,1},  {0,-1,1},    {0,1,-1},    {0,-1,-1},
	  {1,1,0},  {0,-1,1},    {-1,1,0},    {0,-1,-1},
	};
	

	// allocate perm texture
	m_PermTexture.Allocate( 256, 0, 0, FORMAT_R32_FLOAT );

	// allocate pertexture2d(AA,AB,BA,BB)/255.0
	m_PermTexture2D.Allocate( 256, 256, 0, 0, FORMAT_R32G32B32A32_FLOAT );

	// allocate grad texture
	m_GradTexture.Allocate( 16, 0, 0, FORMAT_R32G32B32A32_FLOAT );


	//================ Generate Perm Texture ==============//
//float4 GeneratePermTexture(float p : POSITION) : COLOR
//{
//  return permutation[p * 256] / 255.0;
//}
	float	*pPix	= (float *)( m_PermTexture.m_pData );
	for(int i=0; i<m_PermTexture.Width(); ++i)
	{
		pPix[i]	= float(permutation[i%256]);//float(rand()%256) / 256.0f;//
	}

	//for(int i=0; i<m_PermTexture.Width(); ++i)
	//{
	//	cout << pPix[i] << endl;
	//}


	//============== Generate ParmTexture2d =============//
	Vec4f *pPixel = (Vec4f *)m_PermTexture2D.m_pData;
	int width = m_PermTexture2D.Width();
	for( int y=0; y<m_PermTexture2D.Height(); ++y )
	{
		for( int x=0; x<m_PermTexture2D.Width(); ++x )
		{			
			int A	= pPix[x%width] + y;
			int AA	= pPix[A%width];
			int AB	= pPix[(A + 1)%width];
			int B	= pPix[(x + 1)%width] + y;
			int BA	= pPix[B%width];
			int BB	= pPix[(B + 1)%width];
			
			//cout << AA << ", " << AB << ", " << BA << ", " << BB << endl;

			int index = y * m_PermTexture2D.Width() + x;
			InitVec( pPixel[ index ], (float)AA/256.0f, (float)AB/256.0f, (float)BA/256.0f, (float)BB/256.0f );
		}
	}




	//=============== Generate Grad Texture ==============//
//float3 GenerateGradTexture( float p : POSITION) : COLOR	// まずはOK
//{
// return g[p * 16];
//}
	Vec4f *pPixels = (Vec4f *)( m_GradTexture.m_pData );
	for( int i=0; i<m_GradTexture.Width(); ++i )
	{
		Vec3f g_ = g[ int(fmod( (float)permutation[i%256], (float)m_GradTexture.Width()) ) ];
		InitVec( pPixels[i], g_.y, g_.z, g_.x, 0.0f );// randomize???
	}

	//for(int i=0; i<m_GradTexture.Width(); i++)
	//{
	//	cout << pPixels[i].x << ", " << pPixels[i].y << ", " << pPixels[i].z << ", " << pPixels[i].w << endl;
	//}


	//================ Init GLTexture ================//
	m_PermTexture.GenTexture();
	m_PermTexture2D.GenTexture();
	m_GradTexture.GenTexture();
}
*/



void ProceduralWood::InitPermTexture()
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