#include	"LPVShader.h"

#include	<graphics/gl4x/resource/Texture.h>



namespace OreOreLib
{

	// デフォルトコンストラクタ
	LPVShader::LPVShader()
	{
#ifdef _DEBUG
		tcout << _T( "LPVShader()..." ) << tendl;
#endif // _DEBUG

		m_numIter			= 8;
		//	NumCascades()		= 0;
		//	InitVec(m_LpvSize, 0, 0, 0);
		//	InitVec(m_RsmSize, 0, 0);
		InitVec( m_VplSize, 0, 0 );

		m_Mode				= NORMAL;

		m_ulMatMVP			= -1;
		m_ulLightIntensity	= -1;
		m_ulLightPos		= -1;
		m_ulLightDir		= -1;
		m_ulRsmSize			= -1;

		m_ulRsmSizeInj		= -1;
		//m_ulPositionSampler	= -1;
		//m_ulNormalSampler	= -1;
		//m_ulFluxSampler		= -1;
		m_ulLpvCellDim		= -1;
		//	m_ulMatLPVScreen	= -1;
		m_ulVPLScale		= -1;

		//m_ulIter			= -1;
		//	m_ulSH_R			= -1;
		//	m_ulSH_G			= -1;
		//	m_ulSH_B			= -1;
		//	m_ulSH_GV			= -1;
		//	m_ulSHaccum_R		= -1;
		//	m_ulSHaccum_G		= -1;
		//	m_ulSHaccum_B		= -1;
		m_ulLpvCellSize		= -1;

		m_Fbo_lpv			= 0;

		m_VAO[0]			= 0;
		m_VAO[1]			= 0;
		m_VBO[0]			= 0;
		m_VBO[1]			= 0;
		eao					= 0;

		m_refLPVTex				= NULL;
		m_refRsmPositionBuffer	= NULL;
		m_refRsmNormalBuffer	= NULL;
		m_refRsmFluxBuffer		= NULL;
		m_refRsmDepthBuffer		= NULL;
	}



	LPVShader::LPVShader( const TCHAR *filepath, GLSL_VERSION version )
	{
#ifdef _DEBUG
		tcout << _T( "LPVShader( const TCHAR *filepath, GLSL_VERSION version )..." ) << tendl;
#endif // _DEBUG


		m_numIter			= 8;
		InitVec( m_VplSize, 0, 0 );

		m_Mode				= NORMAL;

		m_ulMatMVP			= -1;
		m_ulLightIntensity	= -1;
		m_ulLightPos		= -1;
		m_ulLightDir		= -1;
		m_ulRsmSize			= -1;

		m_ulRsmSizeInj		= -1;
		//m_ulPositionSampler	= -1;
		//m_ulNormalSampler	= -1;
		//m_ulFluxSampler		= -1;
		m_ulLpvCellDim		= -1;
		//	m_ulMatLPVScreen	= -1;
		m_ulVPLScale		= -1;

		//m_ulIter			= -1;
		//	m_ulSH_R			= -1;
		//	m_ulSH_G			= -1;
		//	m_ulSH_B			= -1;
		//	m_ulSH_GV			= -1;
		//	m_ulSHaccum_R		= -1;
		//	m_ulSHaccum_G		= -1;
		//	m_ulSHaccum_B		= -1;
		m_ulLpvCellSize		= -1;

		m_Fbo_lpv			= 0;

		m_VAO[0]			= 0;
		m_VAO[1]			= 0;
		m_VBO[0]			= 0;
		m_VBO[1]			= 0;
		eao					= 0;

		m_refLPVTex				= NULL;
		m_refRsmPositionBuffer	= NULL;
		m_refRsmNormalBuffer	= NULL;
		m_refRsmFluxBuffer		= NULL;
		m_refRsmDepthBuffer		= NULL;


		InitShader( /*filepath, */version );
	}

	

	// デストラクタ
	LPVShader::~LPVShader()
	{
#ifdef _DEBUG
		tcout << _T("~LPVShader()...") << tendl;
#endif // _DEBUG

		if(m_Fbo_lpv)
		{
			GL_SAFE_CALL( glDeleteFramebuffers( 1, &m_Fbo_lpv ) );
			m_Fbo_lpv = NULL;
		}


		//=============== 仮想点光源の削除 =================//
		m_VPLSprites.clear();
		if( m_VBO[0] ) { GL_SAFE_CALL( glDeleteBuffers( 1, &m_VBO[0] ) ); m_VBO[0] = 0; }//VBOを削除

		m_LPVSlices.clear();
		if( m_VBO[1] ) { GL_SAFE_CALL( glDeleteBuffers( 1, &m_VBO[1] ) ); m_VBO[1] = 0; }//VBOを削除

		m_LPVIndices.clear();
		if( eao ) { GL_SAFE_CALL( glDeleteBuffers( 1, &eao ) ); eao = 0; }	//VBOを削除


		if( matUboID[0] ) { GL_SAFE_CALL( glDeleteBuffers( 1, matUboID ) ); matUboID[0] = 0; }

	}





	bool LPVShader::InitShader( GLSL_VERSION version )
	{
		GLuint program_id;


		//=================== LPVInjectionパスの初期化 ===================//
		// シェーダオブジェクト生成
		m_Pass[LPV_INJECTION].Init( _T( "lpv/LPVInjection.glsl" ), version );
		program_id		= m_Pass[LPV_INJECTION].ID();

		// 入力データの設定
		GL_SAFE_CALL( glBindAttribLocation( program_id, m_alPosition, "POSITION" ) );

		// シェーダ変数の設定
		//m_ulPositionSampler	= glGetUniformLocation( program_id, "g_PositionSampler" );
		//m_ulNormalSampler	= glGetUniformLocation( program_id, "g_NormalSampler" );
		//m_ulFluxSampler		= glGetUniformLocation( program_id, "g_FluxSampler" );
		m_ulLpvCellDim		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_LpvCellDim" ) );
		m_ulLpvCellSize		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_LpvCellSize" ) );
		//m_ulMatLPVScreen	= glGetUniformLocation( program_id, "g_matLPVScreen" );
		m_ulVPLScale		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_VPLScale" ) );
		m_ulLightPos_		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_LightPos" ) );

		// 出力データの設定
		GL_SAFE_CALL( glBindFragDataLocation( program_id, SH_R, "fSH_R" ) );
		GL_SAFE_CALL( glBindFragDataLocation( program_id, SH_G, "fSH_G" ) );
		GL_SAFE_CALL( glBindFragDataLocation( program_id, SH_B, "fSH_B" ) );
		GL_SAFE_CALL( glBindFragDataLocation( program_id, SHaccum_R, "fSHaccum_R" ) );
		GL_SAFE_CALL( glBindFragDataLocation( program_id, SHaccum_G, "fSHaccum_G" ) );
		GL_SAFE_CALL( glBindFragDataLocation( program_id, SHaccum_B, "fSHaccum_B" ) );

		// プログラムのリンク
		m_Pass[LPV_INJECTION].Link();


		//########################### UBOテスト #############################//

		// シェーダーからuniform blockを取得して、バインディングポイントUBP_MATRIXに対応付ける
		matBlockIndex = GL_SAFE_CALL( glGetUniformBlockIndex( m_Pass[LPV_INJECTION].ID(), "matParam" ) );
		GL_SAFE_CALL( glUniformBlockBinding( m_Pass[LPV_INJECTION].ID(), matBlockIndex, UBP_MATRIX ) );

		// UniformBufferObjectを作成する
		GL_SAFE_CALL( glGetActiveUniformBlockiv( m_Pass[LPV_INJECTION].ID(), matBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &matBlockSize ) );

		GL_SAFE_CALL( glGenBuffers( 1, matUboID ) );
		GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, matUboID[0] ) );
		{
			// GL_SAFE_CALL( glBufferData(GL_UNIFORM_BUFFER, matBlockSize, NULL, GL_DYNAMIC_DRAW) );// 必要???
			GL_SAFE_CALL( glBindBufferBase( GL_UNIFORM_BUFFER, UBP_MATRIX, matUboID[0] ) );
		}
		GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, 0 ) );

		//###################################################################//


		//// シェーダ変数およびテクスチャユニットの設定
		//m_Pass[LPV_INJECTION].Bind();
		//{
		//	// RSM Position
		//	glUniform1i( glGetUniformLocation( program_id, "g_PositionSampler" ), RsmPosition );
		//	glActiveTexture( g_DefaultTextureUnits[RsmPosition] );
		//	glBindTexture( GL_TEXTURE_2D, m_RSM[RsmPosition].texID );

		//	// RSM Noraml
		//	glUniform1i( glGetUniformLocation( program_id, "g_NormalSampler" ), RsmNormal );
		//	glActiveTexture( g_DefaultTextureUnits[RsmNormal] );
		//	glBindTexture( GL_TEXTURE_2D, m_RSM[RsmNormal].texID );

		//	// RSM Flux
		//	glUniform1i( glGetUniformLocation( program_id, "g_FluxSampler" ), RsmFlux );
		//	glActiveTexture(g_DefaultTextureUnits[RsmFlux]);
		//	glBindTexture( GL_TEXTURE_2D, m_RSM[RsmFlux].texID );

		//	glUniform1f(m_ulVPLScale, float(m_RsmSize.x*m_RsmSize.y) / float(m_VplSize.x*m_VplSize.y));
		//}
		//m_Pass[LPV_INJECTION].Unbind();


		//=================== GVInjectionパスの初期化 ===================//
		// シェーダオブジェクト生成
		m_Pass[GV_INJECTION].Init( _T( "lpv/LPVGVInjection.glsl" ), version );
		program_id		= m_Pass[GV_INJECTION].ID();

		// 入力データの設定
		GL_SAFE_CALL( glBindAttribLocation( program_id, m_alPosition, "POSITION" ) );

		// シェーダ変数の設定
		m_ulRsmSizeInj		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_RsmSize" ) );
		//m_ulPositionSampler2	= glGetUniformLocation( program_id, "g_PositionSampler" );
		//m_ulNormalSampler2	= glGetUniformLocation( program_id, "g_NormalSampler" );
		m_ulLpvCellDim2			= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_LpvCellDim" ) );
		m_ulLpvCellSize2		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_LpvCellSize" ) );
		//	m_ulMatLPVScreen2		= glGetUniformLocation( program_id, "g_matGVScreen" );

		// 出力データの設定
		GL_SAFE_CALL( glBindFragDataLocation( program_id, SH_GV, "fSH_GV" ) );

		// プログラムのリンク
		m_Pass[GV_INJECTION].Link();


		//########################### UBOテスト #############################//

		// シェーダーからuniform blockを取得して、バインディングポイントUBP_MATRIXに対応付ける
		matBlockIndex = GL_SAFE_CALL( glGetUniformBlockIndex( m_Pass[GV_INJECTION].ID(), "matParam" ) );
		GL_SAFE_CALL( glUniformBlockBinding( m_Pass[GV_INJECTION].ID(), matBlockIndex, UBP_MATRIX ) );
		/*
		// UniformBufferObjectを作成する
		GL_SAFE_CALL( glGetActiveUniformBlockiv( m_Pass[GV_INJECTION].ID(), matBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &matBlockSize ));

		GL_SAFE_CALL( glGenBuffers(1, matUboID) );
		GL_SAFE_CALL( glBindBuffer(GL_UNIFORM_BUFFER, matUboID[0]) );
		{
		GL_SAFE_CALL( glBufferData(GL_UNIFORM_BUFFER, matBlockSize, NULL, GL_DYNAMIC_DRAW)); 
		GL_SAFE_CALL( glBindBufferBase( GL_UNIFORM_BUFFER, UBP_MATRIX, matUboID[0] ));
		}
		GL_SAFE_CALL( glBindBuffer(GL_UNIFORM_BUFFER, 0));
		*/
		//###################################################################//


		//// シェーダ変数およびテクスチャユニットの設定
		//m_Pass[GV_INJECTION].Bind();
		//{
		//	glUniform2iv(m_ulRsmSizeInj, 1, m_RsmSize.xy);

		//	glUniform1i( glGetUniformLocation( program_id, "g_PositionSampler" ), RsmPosition );
		//	glActiveTexture(g_DefaultTextureUnits[RsmPosition]);
		//	glBindTexture( GL_TEXTURE_2D, m_RSM[RsmPosition].texID );

		//	glUniform1i( glGetUniformLocation( program_id, "g_NormalSampler" ), RsmNormal );
		//	glActiveTexture(g_DefaultTextureUnits[RsmNormal]);
		//	glBindTexture( GL_TEXTURE_2D, m_RSM[RsmNormal].texID );
		//}
		//m_Pass[GV_INJECTION].Unbind();


		//================== Propagationパスの初期化 ==================//
		// シェーダオブジェクト生成
		m_Pass[PROPAGATION].Init( _T( "lpv/LPVPropagation.glsl" ), version );
		program_id		= m_Pass[PROPAGATION].ID();

		// 入力データの設定
		GL_SAFE_CALL( glBindAttribLocation( program_id, m_alPosition, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, m_alTexCoord, "TEXCOORD" ) );

		// シェーダ変数の設定
		//m_ulIter		= glGetUniformLocation( program_id, "g_Iter" );
		//m_ulSH_R		= glGetUniformLocation( program_id, "g_LPV_SH_R" );
		//m_ulSH_G		= glGetUniformLocation( program_id, "g_LPV_SH_G" );
		//m_ulSH_B		= glGetUniformLocation( program_id, "g_LPV_SH_B" );
		//m_ulSHaccum_R	= glGetUniformLocation( program_id, "g_SHaccum_R" );
		//m_ulSHaccum_G	= glGetUniformLocation( program_id, "g_SHaccum_G" );
		//m_ulSHaccum_B	= glGetUniformLocation( program_id, "g_SHaccum_B" );
		//m_ulSH_GV		= glGetUniformLocation( program_id, "g_GV_SH" );
		m_ulLpvCellSize_prop	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_LpvSize" ) );

		// 出力データの設定
		GL_SAFE_CALL( glBindFragDataLocation( program_id, SH_R, "fSH_R" ) );
		GL_SAFE_CALL( glBindFragDataLocation( program_id, SH_G, "fSH_G" ) );
		GL_SAFE_CALL( glBindFragDataLocation( program_id, SH_B, "fSH_B" ) );
		GL_SAFE_CALL( glBindFragDataLocation( program_id, SHaccum_R, "fSHaccum_R" ) );
		GL_SAFE_CALL( glBindFragDataLocation( program_id, SHaccum_G, "fSHaccum_G" ) );
		GL_SAFE_CALL( glBindFragDataLocation( program_id, SHaccum_B, "fSHaccum_B" ) );


		// プログラムのリンク
		m_Pass[PROPAGATION].Link();

		//// シェーダ変数およびテクスチャユニットの設定
		//m_Pass[PROPAGATION].Bind();
		//{
		//	glUniform1i( glGetUniformLocation( program_id, "g_LPV_SH_R" ), SH_R );
		//	glUniform1i( glGetUniformLocation( program_id, "g_LPV_SH_G" ), SH_G );
		//	glUniform1i( glGetUniformLocation( program_id, "g_LPV_SH_B" ), SH_B );
		//	glUniform1i( glGetUniformLocation( program_id, "g_SHaccum_R" ), SHaccum_R );
		//	glUniform1i( glGetUniformLocation( program_id, "g_SHaccum_G" ), SHaccum_G );
		//	glUniform1i( glGetUniformLocation( program_id, "g_SHaccum_B" ), SHaccum_B );
		//	glUniform1i( glGetUniformLocation( program_id, "g_GV_SH" ), SH_GV);

		//	glUniform3f(m_ulLpvCellSize, float(m_refLPVTex->Width()), float(m_refLPVTex->Height()), float(m_refLPVTex->Depth()));
		//}
		//m_Pass[PROPAGATION].Unbind();


		//================ SurfaceShaderパスの初期化 ================//
		// シェーダオブジェクト生成
		m_Pass[SURFACE_SHADER].Init( _T( "lpv/LPVSurfaceShader.glsl" ), version );
		program_id = m_Pass[SURFACE_SHADER].ID();

		// 入力データの設定
		GL_SAFE_CALL( glBindAttribLocation( program_id, m_alPosition, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, m_alNormal, "NORMAL" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, m_alColor, "COLOR" ) );

		// シェーダ変数の設定
		m_ulMatWorld		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_MatWorld" ) );
		m_ulMatViewProj		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_matMVP" ) );
		m_ulViewPos			= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_EyePos" ) );

		m_ulLightPos2		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_LightPos" ) );

		m_SHR				= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_LPV_SH_R" ) );
		m_SHG				= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_LPV_SH_G" ) );
		m_SHB				= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_LPV_SH_B" ) );
		m_ShadowMap			= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_ShadowMap" ) );

		m_ulLpvCellSize_	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_LpvCellSize" ) );
		m_ulLpvCellDim_		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_LpvCellDim" ) );


		m_ulTexture			= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Texture" ) );
		m_ulDiffuse			= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Diffuse" ) );
		m_ulSpecular		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Specular" ) );
		m_ulShininess		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Shininess" ) );
		m_ulMatTexCoord		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_MatTexCoord" ) );




		// 出力データの設定


		// プログラムのリンク
		m_Pass[SURFACE_SHADER].Link();

		// シェーダ変数およびテクスチャユニットの設定
		m_Pass[SURFACE_SHADER].Bind();
		{
			glUniform1i(m_SHR, SH_R);
			glUniform1i(m_SHG, SH_G);
			glUniform1i(m_SHB, SH_B);
			glUniform1i(m_ShadowMap, 3);
		}	
		m_Pass[SURFACE_SHADER].Unbind();


		return true;

	}



	// テクスチャオブジェクトを初期化する
	bool LPVShader::InitRSMTexture( int vpl_width, int vpl_height )
	{
		InitVec( m_RsmSize, 2048, 2048 );//	m_RsmSize		= rsm_size;	// TODO: RSM生成処理はクラス外部へ移す
		InitVec( m_VplSize, vpl_width, vpl_height );

		//================== LPV用フレームバッファオブジェクトの初期化 ===================//
		GL_SAFE_CALL( glGenFramebuffers( 1, &m_Fbo_lpv ) );

		return true;
	}


	// RSM仮想点光源スプライトの初期化
	bool LPVShader::InitVPLSprites()
	{
		int x, y, i;
		float dx, dy, fx, fy;
		float scale_x, scale_y;
		float offset_x, offset_y;


		//================ メモリを確保する =================//
		m_VPLSprites.clear();
		m_VPLSprites.resize(m_VplSize.x*m_VplSize.y);


		//========== 座標計算用パラメータを準備する =========//
		scale_x		= float(m_VplSize.x-1)/float(m_VplSize.x);
		scale_y		= float(m_VplSize.x-1)/float(m_VplSize.x);

		offset_x	= 0.5f / float(m_VplSize.x);
		offset_y	= 0.5f / float(m_VplSize.y);

		dx = 1.0f / float(m_VplSize.x-1);
		dy = 1.0f / float(m_VplSize.y-1);


		//========= 頂点(テクスチャ座標)を設定する =========//
		i=0;
		for(y=0; y<m_VplSize.y; y++)
		{
			fy = dy * float(y) * scale_y + offset_y; // y方向内側に0.5テクセルだけ縮める

			for(x=0; x<m_VplSize.x; x++)
			{
				fx = dx * float(x) * scale_x + offset_x;

				InitVec( m_VPLSprites[i], fx, fy );
				i++;
			}// end of x loop
		}// end of y loop


		//=============== VAOを初期化する ================//
		// VBOの作成
		GL_SAFE_CALL( glGenBuffers( 1, &m_VBO[0] ) );				// VBOを1つ作成
		GL_SAFE_CALL( glBindBuffer( GL_ARRAY_BUFFER, m_VBO[0] ) );	// VBOをバインド
		GL_SAFE_CALL( glBufferData( GL_ARRAY_BUFFER, sizeof( Vec2f )*m_VPLSprites.size(), &m_VPLSprites[0], GL_STATIC_DRAW ) );// 頂点データを登録
		GL_SAFE_CALL( glBindBuffer( GL_ARRAY_BUFFER, 0 ) ); // VBOをアンバインド

		// VAOの作成
		GL_SAFE_CALL( glGenVertexArrays( 1, &m_VAO[0] ) );// VAOを1つ作成
		GL_SAFE_CALL( glBindVertexArray( m_VAO[0] ) );	// VAOをバインド
		{
			GL_SAFE_CALL( glBindBuffer( GL_ARRAY_BUFFER, m_VBO[0] ) ); // VBOをバインド
			{
				GL_SAFE_CALL( glVertexAttribPointer( m_alPosition, 2, GL_FLOAT, GL_FALSE, sizeof( Vec2f ), 0 ) );
				GL_SAFE_CALL( glEnableVertexAttribArray( m_alPosition ) );
			}
			GL_SAFE_CALL( glBindBuffer( GL_ARRAY_BUFFER, 0 ) ); // VBOをアンバインド
		}
		GL_SAFE_CALL( glBindVertexArray( 0 ) );// VAOをアンバインド

		return true;
	}



	// LPVセル中心点スプライトの初期化
	bool LPVShader::InitLPVSlices()
	{
		int z;
		int i, j; // i:頂点配列の要素カウント, j:インデックス配列の要素カウント
		float dx,dy,dz, posz, texz;
		float tex_scale_z;
		//Vec3f tex_offset;
		float z_offset;

		//================ メモリを確保する =================//
		m_LPVSlices.clear();
		m_LPVSlices.resize( m_refLPVTex->Depth() * 4 );// zスライスあたり4頂点を確保する
		m_LPVIndices.clear();
		m_LPVIndices.resize( m_refLPVTex->Depth() * 6 );//zスライスあたり6つ必要


		//========== 座標計算用パラメータを準備する =========//
		dx = 1.0f / float( m_refLPVTex->Width() - 1 );
		dy = 1.0f / float( m_refLPVTex->Height() - 1 );
		dz = 1.0f / float( m_refLPVTex->Depth() - 1 );

		tex_scale_z = float( m_refLPVTex->Depth() - 1 ) / float( m_refLPVTex->Depth() );

		//InitVec( tex_offset, 0.5f/float(m_refLPVTex->Width()), 0.5f/float(m_refLPVTex->Height()), 0.5f/float(m_refLPVTex->Depth()) );
		z_offset	= 0.5f / float( m_refLPVTex->Depth() );

		//========= 頂点(スクリーン空間座標[-1,1])を設定する =========//
		i=0; j=0;
		for( z=0; z<m_refLPVTex->Depth(); ++z )// z平面のスライス毎に，頂点座標と頂点インデックスを生成する
		{
			posz = 2.0f*dz*float(z) - 1.0f;					// 頂点座標z値[-1,1]
			texz = dz*float(z) * tex_scale_z + z_offset;//tex_offset.z;// テクスチャ座標z値

			//========= 頂点インデックスを格納 =========//
			m_LPVIndices[j++] = i;
			m_LPVIndices[j++] = i+1;
			m_LPVIndices[j++] = i+3;
			m_LPVIndices[j++] = i+1;
			m_LPVIndices[j++] = i+2;
			m_LPVIndices[j++] = i+3;

			//============= 頂点座標を格納 =============//
			// v0(-1,-1)
			InitVec( m_LPVSlices[i].Position, -1.0f, -1.0f, posz );
			InitVec( m_LPVSlices[i].TexCoord, /*tex_offset.x, tex_offset.y*/0.0f, 0.0f, texz );
			i++;
			// v1(+1,-1)
			InitVec(m_LPVSlices[i].Position, +1.0f, -1.0f, posz );
			InitVec(m_LPVSlices[i].TexCoord, /*1.0f-tex_offset.x, tex_offset.y*/1.0f, 0.0f, texz );
			i++;
			// v2(+1,+1)
			InitVec(m_LPVSlices[i].Position, +1.0f, +1.0f, posz);
			InitVec(m_LPVSlices[i].TexCoord, /*1.0f-tex_offset.x, 1.0f-tex_offset.y*/1.0f, 1.0f, texz);
			i++;
			// v3(-1,+1)
			InitVec(m_LPVSlices[i].Position, -1.0f, +1.0f, posz);
			InitVec(m_LPVSlices[i].TexCoord, /*tex_offset.x, 1.0f-tex_offset.y*/0.0f, 1.0f, texz);
			i++;

		}// end of z loop


		//=============== VAOを初期化する ================//
		// VBOの作成
		GL_SAFE_CALL( glGenBuffers( 1, &m_VBO[1] ) );					//VBOを1つ作成
		GL_SAFE_CALL( glBindBuffer( GL_ARRAY_BUFFER, m_VBO[1] ) );	// VBOをバインド
		GL_SAFE_CALL( glBufferData( GL_ARRAY_BUFFER, sizeof( stLPVSlice )*m_LPVSlices.size(), &m_LPVSlices[0], GL_STATIC_DRAW ) );// 頂点データを登録
		GL_SAFE_CALL( glBindBuffer( GL_ARRAY_BUFFER, 0 ) ); // VBOをアンバインド

		// インデックスバッファの作成
		GL_SAFE_CALL( glGenBuffers( 1, &eao ) );
		GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, eao ) );
		GL_SAFE_CALL( glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_LPVIndices.size()*sizeof( GLuint ), &m_LPVIndices[0], GL_STATIC_DRAW ) );// インデックスを登録
		GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) );

		// VAOの作成
		GL_SAFE_CALL( glGenVertexArrays( 1, &m_VAO[1] ) );// VAOを1つ作成
		GL_SAFE_CALL( glBindVertexArray( m_VAO[1] ) );	//VAOをバインド
		{
			GL_SAFE_CALL( glBindBuffer( GL_ARRAY_BUFFER, m_VBO[1] ) ); // VBOをバインド
			{
				GL_SAFE_CALL( glVertexAttribPointer( m_alPosition, 3, GL_FLOAT, GL_FALSE, sizeof( stLPVSlice ), 0 ) );
				GL_SAFE_CALL( glVertexAttribPointer( m_alTexCoord, 3, GL_FLOAT, GL_FALSE, sizeof( stLPVSlice ), (GLfloat*)0 + 3 ) );
				GL_SAFE_CALL( glEnableVertexAttribArray( m_alPosition ) );
				GL_SAFE_CALL( glEnableVertexAttribArray( m_alTexCoord ) );
			}
			GL_SAFE_CALL( glBindBuffer( GL_ARRAY_BUFFER, 0 ) ); // VBOをアンバインド
		}
		GL_SAFE_CALL( glBindVertexArray( 0 ) );// VAOをアンバインド


		return true;
	}



	void LPVShader::BindLPV( LPVIrradianceVolume *lpv_tex )
	{
		m_refLPVTex	= lpv_tex;

		GLuint program_id		= m_Pass[PROPAGATION].ID();

		// シェーダ変数およびテクスチャユニットの設定
		m_Pass[PROPAGATION].Bind();
		{
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_LPV_SH_R" ), SH_R ) );
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_LPV_SH_G" ), SH_G ) );
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_LPV_SH_B" ), SH_B ) );
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_SHaccum_R" ), SHaccum_R ) );
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_SHaccum_G" ), SHaccum_G ) );
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_SHaccum_B" ), SHaccum_B ) );
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_GV_SH" ), SH_GV ) );

			GL_SAFE_CALL( glUniform3f( m_ulLpvCellSize_prop, float( m_refLPVTex->Width() ), float( m_refLPVTex->Height() ), float( m_refLPVTex->Depth() ) ) );
		}
		m_Pass[PROPAGATION].Unbind();

	}



	void LPVShader::BindRSMBuffer( Texture2D* rsm_pos, Texture2D* rsm_normal, Texture2D* rsm_flux, Texture2D* rsm_depth )
	{
		if( m_refRsmPositionBuffer == rsm_pos && m_refRsmNormalBuffer == rsm_normal && m_refRsmFluxBuffer == rsm_flux && m_refRsmDepthBuffer == rsm_depth )
		{
			if( m_refRsmPositionBuffer->Width()==rsm_pos->Width() && m_refRsmPositionBuffer->Height()==rsm_pos->Height() )
				return;
		}

		m_refRsmPositionBuffer	= rsm_pos;
		m_refRsmNormalBuffer	= rsm_normal;
		m_refRsmFluxBuffer		= rsm_flux;
		m_refRsmDepthBuffer		= rsm_depth;

		
		// シェーダ変数およびテクスチャユニットの設定
		GLuint	program_id	= m_Pass[LPV_INJECTION].ID();
		m_Pass[LPV_INJECTION].Bind();
		{
			// RSM Position
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_PositionSampler" ), RsmPosition ) );
			glActiveTexture( g_DefaultTextureUnits[RsmPosition] );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, rsm_pos->texID ) );

			// RSM Noraml
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_NormalSampler" ), RsmNormal ) );
			glActiveTexture( g_DefaultTextureUnits[RsmNormal] );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, rsm_normal->texID ) );

			// RSM Flux
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_FluxSampler" ), RsmFlux ) );
			glActiveTexture(g_DefaultTextureUnits[RsmFlux]);
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, rsm_flux->texID ) );

			GL_SAFE_CALL( glUniform1f( m_ulVPLScale, float( m_RsmSize.x*m_RsmSize.y ) / float( m_VplSize.x*m_VplSize.y ) ) );
		}
		m_Pass[LPV_INJECTION].Unbind();


		// シェーダ変数およびテクスチャユニットの設定
		program_id	= m_Pass[GV_INJECTION].ID();
		m_Pass[GV_INJECTION].Bind();
		{
			GL_SAFE_CALL( glUniform2iv( m_ulRsmSizeInj, 1, m_RsmSize.xy ) );

			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_PositionSampler" ), RsmPosition ) );
			glActiveTexture(g_DefaultTextureUnits[RsmPosition]);
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, rsm_pos->texID ) );

			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_NormalSampler" ), RsmNormal ) );
			glActiveTexture(g_DefaultTextureUnits[RsmNormal]);
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, rsm_normal->texID ) );
		}
		m_Pass[GV_INJECTION].Unbind();
	
	}



	// LPVInjectionパス
	// LPVのセルに仮想点光源の放射束を書き込む．
	// 同一セルに複数の点光源を書き込む場合は加算合成する
	void LPVShader::LPVInjection( const Vec3f &lightpos )
	{
		Mat4f	m_matLvpScr;// LPVスクリーン座標空間(x,y,zが[-1,1],z軸上方向から見下ろす)への座標変換行列
		MatIdentity(m_matLvpScr);

		//============ GL設定初期化 =============//
		// アルファブレンド有効化
		glEnable(GL_BLEND);
		glBlendEquationSeparate( GL_FUNC_ADD, GL_MAX );
		glBlendFuncSeparate( GL_ONE, GL_ONE, GL_ONE, GL_ONE );

		// ビューポート設定
		glViewport( 0, 0, m_refLPVTex->Width(), m_refLPVTex->Height() );

		GL_SAFE_CALL( glBindFramebuffer( GL_FRAMEBUFFER, m_Fbo_lpv ) );
		GL_SAFE_CALL( glDrawBuffers( 6, g_DefaultColorAttachments ) );


		GL_SAFE_CALL( glBindVertexArray( m_VAO[0] ) );
		GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, matUboID[0] ) );
		

		//============ シェーダ起動 =============//
		m_Pass[LPV_INJECTION].Bind();
		{
			for(int i=0; i<m_refLPVTex->NumCascades(); i++)
			{
				//============ LPVスクリーン座標変換行列の計算 ==========//
				//m_refLPVTex->GetTransformMatrix( &m_matLvpScr, i );
				
				//=============== FBOにテクスチャをアタッチ =============//
				GL_SAFE_CALL( glFramebufferTexture( GL_FRAMEBUFFER, g_DefaultColorAttachments[SH_R], m_refLPVTex->GetDestLPV( i, COLOR_R )->texID, 0 ) );	// GL_COLOR_ATTACHMENT0: RのSH係数
				GL_SAFE_CALL( glFramebufferTexture( GL_FRAMEBUFFER, g_DefaultColorAttachments[SH_G], m_refLPVTex->GetDestLPV( i, COLOR_G )->texID, 0 ) );	// GL_COLOR_ATTACHMENT1: GのSH係数
				GL_SAFE_CALL( glFramebufferTexture( GL_FRAMEBUFFER, g_DefaultColorAttachments[SH_B], m_refLPVTex->GetDestLPV( i, COLOR_B )->texID, 0 ) );	// GL_COLOR_ATTACHMENT2: BのSH係数
				GL_SAFE_CALL( glFramebufferTexture( GL_FRAMEBUFFER, g_DefaultColorAttachments[SHaccum_R], m_refLPVTex->GetDestAccumSH( COLOR_R )->texID, 0 ) );	// GL_COLOR_ATTACHMENT0: RのSH係数
				GL_SAFE_CALL( glFramebufferTexture( GL_FRAMEBUFFER, g_DefaultColorAttachments[SHaccum_G], m_refLPVTex->GetDestAccumSH( COLOR_G )->texID, 0 ) );	// GL_COLOR_ATTACHMENT1: GのSH係数
				GL_SAFE_CALL( glFramebufferTexture( GL_FRAMEBUFFER, g_DefaultColorAttachments[SHaccum_B], m_refLPVTex->GetDestAccumSH( COLOR_B )->texID, 0 ) );	// GL_COLOR_ATTACHMENT2: BのSH係数
				
				glClear( GL_COLOR_BUFFER_BIT );


				//================== シェーダ変数の設定 =================//
				GL_SAFE_CALL( glBufferData( GL_UNIFORM_BUFFER, matBlockSize, m_refLPVTex->GetTransformMatrix( i )->m, GL_DYNAMIC_DRAW ) );
				//glUniformMatrix4fv(m_ulMatLPVScreen, 1, GL_FALSE, *(m_matLvpScr.m) );// TODO: UniformBlockを用いた処理へ変更。削除予定
				GL_SAFE_CALL( glUniform1f( m_ulLpvCellDim, m_refLPVTex->GetCellDim( i ) ) );
				GL_SAFE_CALL( glUniform3f( m_ulLpvCellSize, m_refLPVTex->Width(), m_refLPVTex->Height(), m_refLPVTex->Depth() ) );
				GL_SAFE_CALL( glUniform3fv( m_ulLightPos_, 1, lightpos.xyz ) );
				
				//===================== VPLを描画 ==================//
				GL_SAFE_CALL( glDrawArrays( GL_POINTS, 0, m_VPLSprites.size() ) );
				
			}// end of i loop
			
		}
		m_Pass[LPV_INJECTION].Unbind();
		

		GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, 0 ) );


		//=================== 後片付け =================//
		glBindVertexArray(0);// 仮想点光源VAOの無効化
		//glBindFramebuffer( GL_FRAMEBUFFER, 0 );// フレームバッファオブジェクトの解除.後続のPropagation処理でも同じFBOを利用するので，ここで明示的に解除する必要はない
		glDisable(GL_BLEND);// アルファブレンド無効化

		m_refLPVTex->SwapBuffers();

	}



	// フレームバッファ（頂点座標と法線）からGeometryVolumeを計算する
	void LPVShader::GVInjection()
	{
		//Mat4f	m_matLvpScr;// LPVスクリーン座標空間(x,y,zが[-1,1],z軸上方向から見下ろす)への座標変換行列
		//MatIdentity(m_matLvpScr);

		//============ GL設定初期化 =============//
		// アルファブレンド有効化
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		// ビューポート設定
		glViewport(0, 0, m_refLPVTex->Width(), m_refLPVTex->Height());

		//	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo_lpv);// FBOのバインド．先行のLPVInjectionで使ったFBOをそのまま使うので，ここで明示的にバインドする必要はない
		GL_SAFE_CALL( glDrawBuffers( 1, &g_DefaultColorAttachments[SH_GV] ) );

		GL_SAFE_CALL( glBindVertexArray( m_VAO[0] ) );
		GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, matUboID[0] ) );

		//============ シェーダ起動 =============//
		m_Pass[GV_INJECTION].Bind();
		{
			for(int i=0; i<m_refLPVTex->NumCascades(); i++)
			{
				//============ LPVスクリーン座標変換行列の計算 ==========//
				//m_refLPVTex->GetTransformMatrix( &m_matLvpScr, i );

				//=============== FBOにテクスチャをアタッチ =============//
				GL_SAFE_CALL( glFramebufferTexture( GL_FRAMEBUFFER, g_DefaultColorAttachments[SH_GV], m_refLPVTex->GetGeometryVolume( i )->texID, 0 ) );//m_SH_GV[i].texID, 0);	// GL_COLOR_ATTACHMENT6: GVのSH係数

				glClear( GL_COLOR_BUFFER_BIT );


				//================== シェーダ変数の設定 =================//
				//glUniformMatrix4fv(m_ulMatLPVScreen2, 1, GL_FALSE, *(m_matLvpScr.m) );// TODO: UniformBlockを用いた処理へ変更。削除予定
				GL_SAFE_CALL( glUniform1f( m_ulLpvCellDim2, m_refLPVTex->GetCellDim( i ) ) );
				GL_SAFE_CALL( glUniform3f( m_ulLpvCellSize2, m_refLPVTex->Width(), m_refLPVTex->Height(), m_refLPVTex->Depth() ) );

				GL_SAFE_CALL( glBufferData( GL_UNIFORM_BUFFER, matBlockSize, m_refLPVTex->GetTransformMatrix( i )->m, GL_DYNAMIC_DRAW ) );

				//===================== VPLを描画 ==================//
				GL_SAFE_CALL( glDrawArrays( GL_POINTS, 0, m_VPLSprites.size() ) );

			}// end of i loop
		}
		m_Pass[GV_INJECTION].Unbind();


		//=================== 後片付け =================//
		glBindVertexArray(0);// 仮想点光源VAOの無効化
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);// フレームバッファオブジェクトの解除.後続のPropagation処理でも同じFBOを利用するので，ここで明示的に解除する必要はない
		glDisable(GL_BLEND);// アルファブレンド無効化

	}




	// Propagationパス: 

	// Src,Dstで切り替えるシェーダー

	void LPVShader::Propagation()
	{

		int i, k;

		//============ GL設定初期化 =============//
		// ビューポート設定
		glViewport(0, 0, m_refLPVTex->Width(), m_refLPVTex->Height());

		// 描画する形状データのバインド(頂点とインデックス)
		GL_SAFE_CALL( glBindVertexArray( m_VAO[1] ) );
		GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, eao ) );

		// レンダーターゲットの設定
		//glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo_lpv);// FBOのバインド．先行のInjectionで使ったFBOをそのまま使うので，ここで明示的にバインドする必要はない
		GL_SAFE_CALL( glDrawBuffers( 6, g_DefaultColorAttachments ) );

		//============ シェーダ起動 =============//
		m_Pass[PROPAGATION].Bind();
		{			
			//============= シェーダ変数のアタッチ ==============//
			// セルあたりのテクスチャ座標移動量

			for(i=0; i<m_refLPVTex->NumCascades(); i++)
			{
				glActiveTexture( g_DefaultTextureUnits[SH_GV] );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_3D, m_refLPVTex->GetGeometryVolume( i )->texID ) );

				for(k=0; k<m_numIter; k++)
				{
					//glUniform1i(m_ulIter, /*m_numIter*/k);

					//====== 参照するLPVテクスチャのシェーダ変数へのアタッチ ======//
					glActiveTexture( g_DefaultTextureUnits[SH_R] );		GL_SAFE_CALL( glBindTexture( GL_TEXTURE_3D, m_refLPVTex->GetSourceLPV( i, COLOR_R )->texID ) );
					glActiveTexture( g_DefaultTextureUnits[SH_G] );		GL_SAFE_CALL( glBindTexture( GL_TEXTURE_3D, m_refLPVTex->GetSourceLPV( i, COLOR_G )->texID ) );
					glActiveTexture( g_DefaultTextureUnits[SH_B] );		GL_SAFE_CALL( glBindTexture( GL_TEXTURE_3D, m_refLPVTex->GetSourceLPV( i, COLOR_B )->texID ) );
					glActiveTexture( g_DefaultTextureUnits[SHaccum_R] );	GL_SAFE_CALL( glBindTexture( GL_TEXTURE_3D, m_refLPVTex->GetSourceAccumSH( COLOR_R )->texID ) );
					glActiveTexture( g_DefaultTextureUnits[SHaccum_G] );	GL_SAFE_CALL( glBindTexture( GL_TEXTURE_3D, m_refLPVTex->GetSourceAccumSH( COLOR_G )->texID ) );
					glActiveTexture( g_DefaultTextureUnits[SHaccum_B] );	GL_SAFE_CALL( glBindTexture( GL_TEXTURE_3D, m_refLPVTex->GetSourceAccumSH( COLOR_B )->texID ) );
					

					//====== 伝播結果を書き込むLPVテクスチャのFBOへのアタッチ =====//
					GL_SAFE_CALL( glFramebufferTexture( GL_FRAMEBUFFER, g_DefaultColorAttachments[SH_R], m_refLPVTex->GetDestLPV( i, COLOR_R )->texID, 0 ) );	// GL_COLOR_ATTACHMENT0: RのSH係数
					GL_SAFE_CALL( glFramebufferTexture( GL_FRAMEBUFFER, g_DefaultColorAttachments[SH_G], m_refLPVTex->GetDestLPV( i, COLOR_G )->texID, 0 ) );	// GL_COLOR_ATTACHMENT1: GのSH係数
					GL_SAFE_CALL( glFramebufferTexture( GL_FRAMEBUFFER, g_DefaultColorAttachments[SH_B], m_refLPVTex->GetDestLPV( i, COLOR_B )->texID, 0 ) );	// GL_COLOR_ATTACHMENT2: BのSH係数
					GL_SAFE_CALL( glFramebufferTexture( GL_FRAMEBUFFER, g_DefaultColorAttachments[SHaccum_R], m_refLPVTex->GetDestAccumSH( COLOR_R )->texID, 0 ) );	// GL_COLOR_ATTACHMENT0: RのSH係数
					GL_SAFE_CALL( glFramebufferTexture( GL_FRAMEBUFFER, g_DefaultColorAttachments[SHaccum_G], m_refLPVTex->GetDestAccumSH( COLOR_G )->texID, 0 ) );	// GL_COLOR_ATTACHMENT1: GのSH係数
					GL_SAFE_CALL( glFramebufferTexture( GL_FRAMEBUFFER, g_DefaultColorAttachments[SHaccum_B], m_refLPVTex->GetDestAccumSH( COLOR_B )->texID, 0 ) );	// GL_COLOR_ATTACHMENT2: BのSH係数

					//glDrawBuffers(6, g_DefaultColorAttachments);


					//======================== 描画処理を実行 =====================//
					GL_SAFE_CALL( glDrawElements( GL_TRIANGLES, m_LPVIndices.size(), GL_UNSIGNED_INT, 0 ) );
					//glDrawElementsInstanced(GL_TRIANGLES, m_LPVIndices.size(), GL_UNSIGNED_INT, 0, Depth());

					// LPVテクスチャの参照/書き込みバッファの入れ替え
//					Src ^= 1;//Src = Dst;
//					Dst ^= 1;//Dst = (Src+1)%2;
m_refLPVTex->SwapBuffers();

				}// end of k loop

			}// end of i loop

		}
		m_Pass[PROPAGATION].Unbind();

		//=================== 後片付け =================//
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		glBindVertexArray(0);

	}



	void LPVShader::Update( const Frustum& lightFrustum )
	{
		if( !m_refLPVTex )	return;

		//=============== LPVにRSMを投影する ==============//
		LPVInjection( *lightFrustum.GetPosition() );

		//================ GVにRSMを投影する ==============//
		GVInjection();

		//====== 反復計算でLPV上で放射束を伝播させる ======//
		Propagation();
		
		m_refLPVTex->SwapBuffers();

	}



	void LPVShader::BindDrawSurface()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		m_Pass[SURFACE_SHADER].Bind();

		glActiveTexture( g_DefaultTextureUnits[SH_R] );	GL_SAFE_CALL( glBindTexture( GL_TEXTURE_3D, m_refLPVTex->GetSourceLPV( 0, COLOR_R )->texID ) );
		glActiveTexture( g_DefaultTextureUnits[SH_G] );	GL_SAFE_CALL( glBindTexture( GL_TEXTURE_3D, m_refLPVTex->GetSourceLPV( 0, COLOR_G )->texID ) );
		glActiveTexture( g_DefaultTextureUnits[SH_B] );	GL_SAFE_CALL( glBindTexture( GL_TEXTURE_3D, m_refLPVTex->GetSourceLPV( 0, COLOR_B )->texID ) );
		glActiveTexture( g_DefaultTextureUnits[3] );	GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_refRsmDepthBuffer->texID/*m_RSM[RsmDepth].texID*/ ) );

		GL_SAFE_CALL( glUniform1f( m_ulLpvCellDim_, m_refLPVTex->GetCellDim( 0 ) ) );
		GL_SAFE_CALL( glUniform3f( m_ulLpvCellSize_, float( m_refLPVTex->Width() ), float( m_refLPVTex->Height() ), float( m_refLPVTex->Depth() ) ) );
	}



	void LPVShader::UnbindDrawSurface()
	{
		m_Pass[SURFACE_SHADER].Unbind();
	}



	void LPVShader::Render( const GeometryBuffer *pGeom, Frustum *pfrustum, const Frustum &lightFrustum, const Vec3f &lightIntensity )
	{
		const MaterialBuffer *pMat	= (MaterialBuffer *)pGeom->GetMaterial();


		//=============================== UniformLocationの設定 ===============================//
		// World Matrix
		GL_SAFE_CALL( glUniformMatrix4fv( m_ulMatWorld, 1, GL_FALSE, pGeom->GetTransform()->matWorld.m ) );

		// View Projection Matrix
		GL_SAFE_CALL( glUniformMatrix4fv( m_ulMatViewProj, 1, GL_FALSE, pfrustum->GetViewProjectionMatrix()->m ) );


		GL_SAFE_CALL( glUniformMatrix4fv( glGetUniformLocation( m_Pass[SURFACE_SHADER].ID(), "g_matLight" ), 1, GL_FALSE, lightFrustum.GetViewProjectionMatrix()->m ) );
		GL_SAFE_CALL( glUniform3fv( glGetUniformLocation( m_Pass[SURFACE_SHADER].ID(), "g_LightIntensity" ), 1, lightIntensity.xyz ) );

		GL_SAFE_CALL( glUniform1i( glGetUniformLocation( m_Pass[SURFACE_SHADER].ID(), "g_Mode" ), m_Mode ) );

		// View Position
		GL_SAFE_CALL( glUniform3fv( m_ulLightPos2, 1, lightFrustum.GetPosition()->xyz ) );
		GL_SAFE_CALL( glUniform3fv( m_ulViewPos, 1, pfrustum->GetPosition()->xyz ) );



		// Texture
		//if( pMesh->m_pTextureBuffer->pData )
		//{
		//	glActiveTexture( GL_TEXTURE0 );
		//	glBindTexture( GL_TEXTURE_2D, *pMesh->m_pTextureBuffer->texID() ); 
		//	glUniform1i( m_ulTexture, 0 );
		//}
		
		// Material Parameter
		if( pMat->pData )
		{
			const MaterialAttribute *mtlparam = (MaterialAttribute *)pMat->pData;
			//const MaterialParams *mtlparam = ((MaterialBuffer *)pMesh->m_pMaterialBuffer)->Layer();
			GL_SAFE_CALL( glUniform3fv( m_ulDiffuse, 1, mtlparam->Diffuse.xyz ) );
			GL_SAFE_CALL( glUniform3fv( m_ulSpecular, 1, mtlparam->Specular.xyz ) );
			GL_SAFE_CALL( glUniform1f( m_ulShininess, mtlparam->Shininess ) );
		}
		
		// MatTexcoord
		GL_SAFE_CALL( glUniformMatrix4fv( m_ulMatTexCoord, 1, GL_FALSE, m_refLPVTex->GetTexCoordMatrix( 0 )->m ) );
		

		//=============================== オブジェクトの描画 ==================================//
		pGeom->Draw();

	}









	/*
	// SHテクスチャの中身を表示する(OpenGL2.0ベース)
	void LPVShader::ViewSH(const Camera &camera)
	{
	int x, y, z;

	glViewport(0,0,512,512);
	//glClearColor(0.2, 0.2, 0.2, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);// テクスチャユニット0を指定
	glEnable(GL_TEXTURE_3D);

	glBindTexture(GL_TEXTURE_3D, m_SH_LPVs[Dst][0].G);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, camera.GetNearClip(), camera.GetFarClip());


	glMatrixMode(GL_MODELVIEW);


	const Vec3f	*pos = camera.GetPosition(),
	*dir = camera.GetForward(),
	*up =camera.GetVertical();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(	pos->x, pos->y, pos->z,
	pos->x + dir->x, pos->y + dir->y, pos->z + dir->z,
	up->x, up->y, up->z);



	glPointSize(1.5);
	glColor3f(1,1,1);
	glBegin(GL_POINTS);

	for(z=0; z<Depth(); z++)
	{
	for(y=0; y<Height(); y++)
	{
	for(x=0; x<Width(); x++)
	{
	glTexCoord3f( float(x)/float(Width()), float(y)/float(Height()), float(z)/float(Depth()) );
	glVertex3f( float(x)/float(Width()), float(y)/float(Height()), float(z)/float(Depth()) );
	}// end of x loop
	}// end of y loop
	}// end of z loop

	glEnd();

	glDisable(GL_TEXTURE_3D);

	}
	*/


}