#include	"VoxelConeTracing.h"

#include	<graphics/gl4x/resource/GLBindPointManager.h>
#include	<graphics/gl4x/scene/ViewTransformBuffer.h>
#include	<graphics/gl4x/scene/LightAttributeBuffer.h>
#include	<graphics/gl4x/resource/ShadowBuffer.h>
#include	<graphics/gl4x/scene/MeshObject.h>



namespace OreOreLib
{

	GLuint	VoxelConeTracing::m_BindPoints[2]	={ 0, 0, };


	// Default Constructor
	VoxelConeTracing::VoxelConeTracing()
	{
#ifdef _DEBUG
		tcout << _T( "VoxelConeTracing()..." ) << tendl;
#endif // _DEBUG

		m_refVCTVoxel			= NULL;
		m_refRsmPositionBuffer	= NULL;
		m_refRsmNormalBuffer	= NULL;
		m_refRsmFluxBuffer		= NULL;
		m_refRsmDepthBuffer		= NULL;

		InitVec( m_VplSize, 0, 0 );
	}


	// constructor
	VoxelConeTracing::VoxelConeTracing( const TCHAR *filepath, GLSL_VERSION version )
	{
#ifdef _DEBUG
		tcout << _T( "VoxelConeTracing( const TCHAR *filepath, GLSL_VERSION version )..." ) << tendl;
#endif // _DEBUG

		m_refVCTVoxel			= NULL;
		m_refRsmPositionBuffer	= NULL;
		m_refRsmNormalBuffer	= NULL;
		m_refRsmFluxBuffer		= NULL;
		m_refRsmDepthBuffer		= NULL;

		InitVec( m_VplSize, 0, 0 );

		if( filepath )
			InitShader( filepath, version );
	}



	// destructor
	VoxelConeTracing::~VoxelConeTracing()
	{
#ifdef	_DEBUG
		tcout << _T("~VoxelConeTracing()...") << tendl;
#endif
		Release();
	}



	// init shader
	void VoxelConeTracing::InitShader( const TCHAR *filepath, GLSL_VERSION version )
	{
		GLuint	program_id;


		//########################	Init Voxelization Pass 	########################//
		// Create Shader
		m_Pass[PASS::VOXELIZATION].Init( _T( "vct/VCTVoxelization.glsl" ), version );
		program_id			= m_Pass[ PASS::VOXELIZATION ].ID();

		// Init Attribute location
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_NORMAL, "NORMAL" ) );

		// Init Uniform Location
		m_ulMatWorld			= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_MatWorld" ) );
		m_ulViewportResolution	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_ViewportResolution" ) );
		m_ulDiffuse				= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Diffuse" ) );
		m_ulSpecular			= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Specular" ) );
		m_ulShininess			= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Shininess" ) );
		m_ulOpacity				= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Opacity" ) );

		// Bind FragData Location


		// Link Shader
		m_Pass[ PASS::VOXELIZATION ].Link();
		

		//########################	Init Injection Pass 	########################//
		// Create Shader
		m_Pass[PASS::INJECTION].Init( _T( "vct/VCTInjection.glsl" ), version );
		program_id			= m_Pass[ PASS::INJECTION ].ID();

		// Init Attribute Locations
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );

		// Init Uniform Locations
		m_ulVPLAreaScale		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_VPLAreaScale" ) );

		// Bind FragData Locations
		//glBindFragDataLocation( program_id, 0, "outColor" );
		GL_SAFE_CALL( glBindFragDataLocation( program_id, 0, "outColor0" ) );
		GL_SAFE_CALL( glBindFragDataLocation( program_id, 1, "outColor1" ) );
		GL_SAFE_CALL( glBindFragDataLocation( program_id, 2, "outColor2" ) );
		GL_SAFE_CALL( glBindFragDataLocation( program_id, 3, "outColor3" ) );
		GL_SAFE_CALL( glBindFragDataLocation( program_id, 4, "outColor4" ) );
		GL_SAFE_CALL( glBindFragDataLocation( program_id, 5, "outColor5" ) );
		
		// Link Shader
		m_Pass[ PASS::INJECTION ].Link();
		

		//#####################	Init CreateSurfaceVoxeList Pass	####################//
		// Create Shader
		m_CreateSurfaceVoxelList.Init( _T( "vct/VCTCreateSurfaceVoxelList.glsl" ), version );
		program_id			= m_CreateSurfaceVoxelList.ID();

		// Init Block Indices for SSBO
		m_biSurfaceVoxelsOutput	= GL_SAFE_CALL( glGetProgramResourceIndex( program_id, GL_SHADER_STORAGE_BLOCK, "surfaceVoxelList" ) );
		
		// Link Shader
		m_CreateSurfaceVoxelList.Link();


		//####################	Init PerVoxelLightInjection Pass	################//
		// Create Shader
		m_PerVoxelLightInjection.Init( _T( "vct/VCTPerVoxelLightInjection.glsl" ), version );
		program_id			= m_PerVoxelLightInjection.ID();

		// Init Block Indices for SSBO
		m_biSurfaceVoxelsInput	= GL_SAFE_CALL( glGetProgramResourceIndex( program_id, GL_SHADER_STORAGE_BLOCK, "surfaceVoxelList" ) );
		
		// Link Shader
		m_PerVoxelLightInjection.Link();


		//####################	Init Anisotropic Voxel Setup Pass	################//
		// Create Shader
		m_SetupAnisotropicVoxels.Init( _T( "vct/VCTSetupAnisotropicVoxels.glsl" ), version );

		// Link Shader
		m_SetupAnisotropicVoxels.Link();


		//########################	Init DownSample Pass	########################//
		// Create Shader
		m_ComputeDownSample.Init( _T( "vct/VCTDownSample.glsl" ), version );

		// Link Shader
		m_ComputeDownSample.Link();

		// Bind Texture Unit. glProgram bust be LINKED before
		m_ComputeDownSample.Bind();
		{
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_TexSource3D" ), 0 ) );
		}
		m_ComputeDownSample.Unbind();

		
		//########################	Init DownSampleAniso Pass	########################//
		//for( int i=0; i<6; ++i )
		//{
		//	//m_ComputeDownSampleAniso[i].Init( _T("vct/VCTDownSampleAniso.glsl"), TODO: プリプロセッサ定義をシェーダーコードに直接追加して、処理フロー切り替えたい );

		//}// end of i loop

		const TCHAR *downSampleAnisoShaderFileName[] =
		{
			_T("vct/VCTDownSampleAnisoXPlus.glsl"),
			_T("vct/VCTDownSampleAnisoXMinus.glsl"),
			_T("vct/VCTDownSampleAnisoYPlus.glsl"),
			_T("vct/VCTDownSampleAnisoYMinus.glsl"),
			_T("vct/VCTDownSampleAnisoZPlus.glsl"),
			_T("vct/VCTDownSampleAnisoZMinus.glsl"),
		};
		
		for( int i=0; i<6; ++i )
		{
			m_ComputeDownSampleAniso[i].Init( downSampleAnisoShaderFileName[i], version );

			// Link Shader
			m_ComputeDownSampleAniso[i].Link();
		}


		//########################	Init TraceVoxel Pass	######################//
		// Create Shader
		m_Pass[PASS::TRACE_VOXEL].Init( _T( "vct/VCTTraceVoxel.glsl" ), version );
		program_id		= m_Pass[ PASS::TRACE_VOXEL ].ID();

		// Init Attribute Locations
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_NORMAL, "NORMAL" ) );

		// Init Uniform Locations
		
		// Init FragData Locations
		GL_SAFE_CALL( glBindFragDataLocation( program_id, 0, "outColor" ) );

		// Link Shader
		m_Pass[ PASS::TRACE_VOXEL ].Link();


		m_Pass[ PASS::TRACE_VOXEL ].Bind();
		{
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_TexPosition" ), TEX_TRACE_POSITION ) );
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_TexNormal" ), TEX_TRACE_NORMAL ) );

			//GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_TexIrradianceVolume" ), TEX_TRACE_VOXEL ) );
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_TexIrradianceVolumeAniso[0]" ), 3 ) );
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_TexIrradianceVolumeAniso[1]" ), 4 ) );
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_TexIrradianceVolumeAniso[2]" ), 5 ) );
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_TexIrradianceVolumeAniso[3]" ), 6 ) );
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_TexIrradianceVolumeAniso[4]" ), 7 ) );
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_TexIrradianceVolumeAniso[5]" ), 8 ) );
		}
		m_Pass[ PASS::TRACE_VOXEL ].Unbind();
		

		//######################	Init UniformBuffers		######################//
		for( int i=0; i<2; ++i )	if( m_BindPoints[i]==0 )	m_BindPoints[i]	= GLBindPointManager::Reserve( GL_UNIFORM_BUFFER );		

		m_uboVoxelAttribute.Init( GL_STATIC_DRAW, m_BindPoints[0] );
		m_uboVoxelTransform.Init( GL_DYNAMIC_DRAW, m_BindPoints[1] );

		for( int i=0; i<NUM_PASS; ++i )
		{
			m_uboVoxelAttribute.BindToShaderByName( m_Pass[i].ID(), "ubVCTVoxelAttribute" );
			m_uboVoxelTransform.BindToShaderByName( m_Pass[i].ID(), "ubVCTVoxelTransform" );
		}

		m_uboVoxelAttribute.BindToShaderByName( m_PerVoxelLightInjection.ID(), "ubVCTVoxelAttribute" );
		m_uboVoxelTransform.BindToShaderByName( m_PerVoxelLightInjection.ID(), "ubVCTVoxelTransform" );


		m_uboVoxelTransform.BindToShaderByName( m_SetupAnisotropicVoxels.ID(), "ubVCTVoxelTransform" );


		//#####################	Init FramebufferObjects		######################//
		m_FboAttachment.Init( 7, g_DefaultColorAttachments );// for anisoIrradiance x6 + isotropic ittaadiance x1
		m_FboAttachment2.Init( 3, g_DefaultColorAttachments );// for compressed texpos, normal, and color voxel data 
		
		m_Fbo.Init( 0, 0, false );
	}



	bool VoxelConeTracing::InitVPLSprites( int vpl_width, int vpl_height )
	{
		int x, y, i;
		float dx, dy, fx, fy;
		float scale_x, scale_y;
		float offset_x, offset_y;


		//================ メモリを確保する =================//
		InitVec( m_VplSize, vpl_width, vpl_height );
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
		const InputElementDesc layout[] =
		{
			{ ATTRIB_LOCATION::ATTRIB_LOCATION_POSITION, DATA_FORMAT::FORMAT_R32G32_FLOAT, false, sizeof(Vec2f), 0 },
		};

		m_vaoVPLSprites.Init();
		m_vaoVPLSprites.SetVertexAttributes( layout, ARRAYSIZE(layout), PRIMITIVE_TYPE::PRIM_POINT );
		m_vaoVPLSprites.SetVertexBuffers( &m_VPLSprites[0], sizeof(Vec2f)*m_VPLSprites.size(), GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		m_vaoVPLSprites.Update();


		return true;
	}



	void VoxelConeTracing::Release()
	{

		UnbindVCTVoxelData();
		UnbindRSMBuffer();

		for( int i=0; i<NUM_PASS; ++i )
			m_Pass[i].Release();

		m_ComputeDownSample.Release();

		for( int i=0; i<6; ++i )
			m_ComputeDownSampleAniso[i].Release();

		m_VPLSprites.clear();
		m_vaoVPLSprites.Release();

		m_uboVoxelAttribute.Release();
		m_uboVoxelTransform.Release();

		m_Fbo.Release();

		m_CreateSurfaceVoxelList.Release();
		m_PerVoxelLightInjection.Release();
		m_SetupAnisotropicVoxels.Release();

	}



	// TODO: テクスチャフォーマットが違う->同じFBOに同時バインドできない->フォーマット毎に分割してバインド
	// TODO: まだおかしい -> GL_MAX_COLOR_ATTACHMENTSは8まで。FBOに9チャンネル以上バインドするとおかしくなる
	void VoxelConeTracing::ClearVoxels()
	{
//#ifdef _DEBUG
		//m_Fbo.CheckStatus();
//#endif
		m_Fbo.Bind();

		//============ IrradianctTexture6枚だけ最初に初期化 ================//
		m_FboAttachment.DrawBuffers();
		m_FboAttachment.BindAttachment();

		glClearColor( 0.0, 0.0, 0.0, 0.0 );
		glClear( GL_COLOR_BUFFER_BIT );

		m_FboAttachment.UnbindAttachment();
		
		//============ Position, Normal Colorバッファを初期化 ==============//
		m_FboAttachment2.DrawBuffers();
		m_FboAttachment2.BindAttachment();

		glClearColor( 0.0, 0.0, 0.0, 0.0 );
		glClear( GL_COLOR_BUFFER_BIT );

		m_FboAttachment2.UnbindAttachment();
		
		m_Fbo.Unbind();
	}



	void VoxelConeTracing::BindVCTVoxelData( VCTVoxelData *vct_voxel )
	{
		m_refVCTVoxel = vct_voxel;

		//int numMaxAttachments	 = 0;
		//glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS, &numMaxAttachments );
		
		// FboAttachmentを初期化する
		for( int i=0; i<m_FboAttachment.NumAttachPoints()-1; ++i )
			m_FboAttachment.SetTexID( i, m_refVCTVoxel->m_TexIrradianceAniso[i].texID );
		m_FboAttachment.SetTexID( m_FboAttachment.NumAttachPoints() - 1, m_refVCTVoxel->m_TexIrradiance.texID );

		m_FboAttachment2.SetTexID( 0, m_refVCTVoxel->m_TexPosition.texID );
		m_FboAttachment2.SetTexID( 1, m_refVCTVoxel->m_TexNormal.texID );
		m_FboAttachment2.SetTexID( 2, m_refVCTVoxel->m_TexColor.texID );
		
	}
	

	
	void VoxelConeTracing::UnbindVCTVoxelData()
	{
		m_refVCTVoxel	= NULL;

		//m_Fbo.UnbindAttachment();

		m_FboAttachment.Clear();
		m_FboAttachment2.Clear();
	}



	void VoxelConeTracing::BindRSMBuffer( Texture2D* rsm_pos, Texture2D* rsm_normal, Texture2D* rsm_flux, Texture2D* rsm_depth )
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

		
		//==============	シェーダ変数、テクスチャユニットの設定	==============//
		GLuint	program_id;

		// Injection Shader
		program_id	= m_Pass[ INJECTION ].ID();
		m_Pass[ INJECTION ].Bind();
		{
			// RSM Position
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_RSMPosition" ), RSM_POSITION ) );

			// RSM Noraml
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_RSMNormal" ), RSM_NORMAL ) );

			// RSM Flux
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_RSMFlux" ), RSM_FLUX ) );
		}
		m_Pass[ INJECTION ].Unbind();

	}



	void VoxelConeTracing::UnbindRSMBuffer()
	{
		m_refRsmPositionBuffer	= NULL;
		m_refRsmNormalBuffer	= NULL;
		m_refRsmFluxBuffer		= NULL;
		m_refRsmDepthBuffer		= NULL;
	}



	void VoxelConeTracing::BindBufferObject( const IBufferObject* pbufferobj )
	{
		const type_info& id = typeid(*pbufferobj);

		if( id == typeid(ViewTransformBuffer) )
		{
			((ViewTransformBuffer *)pbufferobj)->BindToShader( m_Pass[VOXELIZATION].ID() );
			((ViewTransformBuffer *)pbufferobj)->BindToShader( m_Pass[TRACE_VOXEL].ID() );
			((ViewTransformBuffer *)pbufferobj)->BindToShader( m_PerVoxelLightInjection.ID() );
		}
		else if( id == typeid(LightAttributeBuffer) )
		{
			((LightAttributeBuffer *)pbufferobj)->BindToShader( m_PerVoxelLightInjection.ID() );
			
		}
		else if( id == typeid(ShadowBuffer) )
		{
			ShadowBuffer *pshadowbuf	= (ShadowBuffer *)pbufferobj;

			// Bind Uniform Buffer Objects
			pshadowbuf->BindToShader( m_PerVoxelLightInjection.ID() );

			// Bind ShadowmapArray Texture(シェーダーバインドするたびにテクスチャバインドしなおし?)
			m_PerVoxelLightInjection.Bind();
			{
				glActiveTexture( g_DefaultTextureUnits[0] );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D_ARRAY, pshadowbuf->GetTexture()->texID ) );
			}
			m_PerVoxelLightInjection.Unbind();
		}


	}
	


	void VoxelConeTracing::Update( int numObjects, const MovableObject *renderQueue[] )
	{
		if( !m_refVCTVoxel || !m_refRsmPositionBuffer || !m_refRsmNormalBuffer || !m_refRsmFluxBuffer || !m_refRsmDepthBuffer )
			return;

		// Update UniformBuffers
		m_uboVoxelAttribute.UpdateSubresource( &m_refVCTVoxel->m_Attrib, sizeof(VCTVoxelData::VCTVoxelAttribute) );
		m_uboVoxelTransform.UpdateSubresource( &m_refVCTVoxel->m_Transform, sizeof(VCTVoxelData::VCTVoxelTransform) );

		// Clear Voxel Data
		ClearVoxels();

		// Execute VoxelConeTracing Pass
		VoxelizeScene( numObjects, renderQueue );

		// Voxel Lighting - based Injection
		CreateSurfaceVoxelList();
		PerVoxelLightInjection();
		SetupAnisotropicVoxels();

		// Reflective Shadow Map Injection
		InjectRSM();

		DownSampleAniso();//DownSample();
		
	}



	// シーンデータをボクセル化する
	void VoxelConeTracing::VoxelizeScene( int numObjects, const MovableObject *renderQueue[] )
	{
		
		//====================== レンダーステートの設定 =======================//
		//glDisable( GL_BLEND );
		//glDisable( GL_CULL_FACE );	// 面のカリング無効化.
		glDisable( GL_DEPTH_TEST );	// デプステスト無効化
		glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );

		glViewport( 0, 0, m_refVCTVoxel->GetResolution()->x, m_refVCTVoxel->GetResolution()->y );

		glClearColor( 0.0, 0.0, 0.0, 0.0 );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		
		//=========================== バッファの設定 ==========================//
		GL_SAFE_CALL( glBindImageTexture( TEX_POSITION, m_refVCTVoxel->m_TexPosition.texID, 0, GL_TRUE, 0, GL_WRITE_ONLY, m_refVCTVoxel->m_TexPosition.GLInternalFormat() ) );
		GL_SAFE_CALL( glBindImageTexture( TEX_NORMAL, m_refVCTVoxel->m_TexNormal.texID, 0, GL_TRUE, 0, GL_WRITE_ONLY, m_refVCTVoxel->m_TexNormal.GLInternalFormat() ) );
		GL_SAFE_CALL( glBindImageTexture( TEX_COLOR, m_refVCTVoxel->m_TexColor.texID, 0, GL_TRUE, 0, GL_WRITE_ONLY, m_refVCTVoxel->m_TexColor.GLInternalFormat() ) );

		
		//==================== ポリゴンメッシュをボクセライズ =================//
		m_Pass[ VOXELIZATION ].Bind();
		{
			for( int i=0; i<numObjects; ++i )
			{
				MeshObject *pMesh		= (MeshObject *)renderQueue[i];
				GeometryBuffer *pVao	= pMesh->GetBuffer<GeometryBuffer>();
				
				// Set Uniform Locations
				GL_SAFE_CALL( glUniformMatrix4fv( m_ulMatWorld, 1, GL_FALSE, renderQueue[i]->GetTransformMatrix()->matWorld.m ) );
				GL_SAFE_CALL( glUniform2f( m_ulViewportResolution, float( m_refVCTVoxel->GetResolution()->x ), float( m_refVCTVoxel->GetResolution()->y ) ) );
				
				// Set Material Params
				const MaterialBuffer *pMat	= (MaterialBuffer *)pMesh->m_pMaterial;
				
				if( pMesh->m_pMaterial->pData )
				{
					GL_SAFE_CALL( glUniform3fv( m_ulDiffuse, /*pMat->numElms*/1, (float *)pMat->Diffuse() ) );
					GL_SAFE_CALL( glUniform3fv( m_ulSpecular, /*pMat->numElms*/1, (float *)pMat->Specular() ) );
					GL_SAFE_CALL( glUniform1fv( m_ulShininess, /*pMat->numElms*/1, (float *)pMat->Shininess() ) );
					GL_SAFE_CALL( glUniform1fv( m_ulOpacity, /*pMat->numElms*/1, (float *)pMat->Opacity() ) );
				}
				
				// Draw Mesh
				pVao->Draw();
				
				//glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );
				
			}// end of i loop
		}
		m_Pass[ VOXELIZATION ].Unbind();
		

		//===================== レンダーステート元に戻す =======================//
		//glEnable( GL_BLEND );
		//glEnable( GL_CULL_FACE );	// 面のカリング有効化
		glEnable( GL_DEPTH_TEST );	// デプステスト有効化
		glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

		//glViewport( 0, 0, 1280, 720 );
	}



	void VoxelConeTracing::InjectRSM()
	{

		//============ GL設定初期化 =============//
		// アルファブレンド有効化
		glEnable(GL_BLEND);
		glBlendEquationSeparate( GL_FUNC_ADD, GL_MAX );
		glBlendFuncSeparate( GL_ONE, GL_ONE, GL_ONE, GL_ONE );// RSMテクセルの面積を考慮してボクセルに累積する場合
		//glBlendFunc( GL_ONE, GL_ZERO );// 単一RSMテクセルの明るさをそのまま使う場合


		// ビューポート設定
		glViewport( 0, 0, m_refVCTVoxel->Width(), m_refVCTVoxel->Height() );

		
		//=============== FBOにテクスチャをアタッチ =============//
		m_Fbo.Bind();
		m_FboAttachment.DrawBuffers( 6, 0 );
		m_FboAttachment.BindAttachment( 6, 0 );
		
// TODO: PerVoxelLightInjectionの出力結果にRSMを付け足す場合は、コメントアウトしてバッファクリアしないようにすること!
//glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
//glClear( GL_COLOR_BUFFER_BIT );
		
		
		//============ シェーダーバインド =============//
		m_Pass[ INJECTION ].Bind();
		{
			glActiveTexture( g_DefaultTextureUnits[ RSM_POSITION ] );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_refRsmPositionBuffer->texID ) );

			// RSM Noraml
			glActiveTexture( g_DefaultTextureUnits[RSM_NORMAL] );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_refRsmNormalBuffer->texID ) );

			// RSM Flux
			glActiveTexture( g_DefaultTextureUnits[RSM_FLUX] );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, m_refRsmFluxBuffer->texID ) );


			//================== シェーダ変数の設定 =================//
			GL_SAFE_CALL( glUniform1f( m_ulVPLAreaScale, float( m_refRsmFluxBuffer->Width() * m_refRsmFluxBuffer->Height() ) / float( m_VplSize.x*m_VplSize.y ) ) );
			

			//===================== VPLを描画 ==================//
			m_vaoVPLSprites.DrawArrays( 0,  m_VPLSprites.size(), GL_POINTS );


			glActiveTexture( g_DefaultTextureUnits[RSM_POSITION] );
			glBindTexture( GL_TEXTURE_2D, 0 );

			glActiveTexture( g_DefaultTextureUnits[RSM_NORMAL] );
			glBindTexture( GL_TEXTURE_2D, 0);

			glActiveTexture( g_DefaultTextureUnits[ RSM_FLUX ] );
			glBindTexture( GL_TEXTURE_2D, 0 );
			
		}
		m_Pass[ INJECTION ].Unbind();
		

		//=================== 後片付け =================//
		
		m_FboAttachment.UnbindAttachment( 6, 0 );
		m_Fbo.Unbind();
		
		glDisable(GL_BLEND);// アルファブレンド無効化
		
	}


	
	//void VoxelConeTracing::DownSample()
	//{
 //       glBindTexture( GL_TEXTURE_3D, m_refVCTVoxel->m_TexIrradiance.texID );
 //       glGenerateMipmap( GL_TEXTURE_3D );//ミップマップの生成
 //       glBindTexture( GL_TEXTURE_3D, 0 );


	//	m_ComputeDownSample.Bind();
	//	{
	//		Vec4i destRes	= *m_refVCTVoxel->GetResolution();

	//		for( int i=0; i<5; ++ i)
	//		{
	//			destRes.x /= 2;
	//			destRes.y /= 2;
	//			destRes.z /= 2;

	//			// オリジナル解像度テクスチャバインド. 注意. 3Dテクスチャは4番目の引数(layered)をGL_TRUEにすること
	//			glBindImageTexture( 0, m_refVCTVoxel->m_TexIrradiance.texID, i, GL_TRUE, 0, GL_READ_ONLY, m_refVCTVoxel->m_TexIrradiance.GLInternalFormat() );
	//			// 縮小テクスチャのバインド. 注意. 3Dテクスチャは4番目の引数(layered)をGL_TRUEにすること
	//			glBindImageTexture( 1, m_refVCTVoxel->m_TexIrradiance.texID, i+1, GL_TRUE, 0, GL_WRITE_ONLY, m_refVCTVoxel->m_TexIrradiance.GLInternalFormat() );

	//			//glActiveTexture( g_DefaultTextureUnits[ 0 ] );
	//			//glBindTexture( GL_TEXTURE_3D, m_refVCTVoxel->m_TexIrradiance.texID );

	//			// 縮小後の3Dテクスチャセル毎に1スレッドを実行する
	//			glDispatchCompute( DivUp( destRes.x, 8 ), DivUp( destRes.y, 8 ), DivUp( destRes.z, 8 ) );
	//			//checkErrors("Dispatch compute shader");

	//			glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );
	//		}

	//	}
	//	m_ComputeDownSample.Unbind();

	//}
	


	void VoxelConeTracing::DownSampleAniso()
	{
	
		for( int dirIdx=0; dirIdx<6; ++dirIdx )
		{
			Texture3D *pTexIVAniso	= &m_refVCTVoxel->m_TexIrradianceAniso[ dirIdx ];

			
			m_ComputeDownSampleAniso[ dirIdx ].Bind();
			{
				Vec4i destRes	= *m_refVCTVoxel->GetResolution();
			
				// オリジナル解像度テクスチャバインド. 注意. 3Dテクスチャは4番目の引数(layered)をGL_TRUEにすること				
				glActiveTexture( g_DefaultTextureUnits[ 0 ] );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_3D, pTexIVAniso->texID ) );


				for( int i=0; i<pTexIVAniso->numMipLevels()-1; ++i )
				{
					destRes.x /= 2;
					destRes.y /= 2;
					destRes.z /= 2;

					// 縮小テクスチャのバインド. 注意. 3Dテクスチャは4番目の引数(layered)をGL_TRUEにすること
					GL_SAFE_CALL( glBindImageTexture( 1, pTexIVAniso->texID, i+1, GL_TRUE, 0, GL_WRITE_ONLY, pTexIVAniso->GLInternalFormat() ) );
				
					GL_SAFE_CALL( glUniform1i( glGetUniformLocation( m_ComputeDownSampleAniso[dirIdx].ID(), "g_SrcMipMapLevel" ), i ) );

					// 縮小後の3Dテクスチャセル毎に1スレッドを実行する
					GL_SAFE_CALL( glDispatchCompute( DivUp( destRes.x, 8 ), DivUp( destRes.y, 8 ), DivUp( destRes.z, 8 ) ) );
					//checkErrors("Dispatch compute shader");

					glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );
				
				}// end of i loop

				glActiveTexture( g_DefaultTextureUnits[0] );
				glBindTexture( GL_TEXTURE_3D, 0 );
			}
			m_ComputeDownSampleAniso[ dirIdx ].Unbind();
			
		}// end of dirIdx loop
	
	}



	void VoxelConeTracing::TraceVoxel( const Texture2D* pTexWorldPos, const Texture2D* pTexWorldNormal, Texture2D* pOutIrradiance )
	{
		if( !m_refScreenSpaceQuad || !m_refVCTVoxel )
			return;

		m_Fbo.Bind();
		GL_SAFE_CALL( glDrawBuffers( 1, &g_DefaultColorAttachments[0] ) );
		GL_SAFE_CALL( glFramebufferTexture( GL_FRAMEBUFFER, g_DefaultColorAttachments[0], pOutIrradiance->texID, 0 ) );
		
		glClear( GL_COLOR_BUFFER_BIT );

		m_Pass[ PASS::TRACE_VOXEL ].Bind();
		{
			// Bind Gbuffer Tex Position
			glActiveTexture( g_DefaultTextureUnits[TEX_TRACE_POSITION] );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, pTexWorldPos->texID ) );

			// Bind Gbuffer Tex Normal
			glActiveTexture( g_DefaultTextureUnits[TEX_TRACE_NORMAL] );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, pTexWorldNormal->texID ) );

			// Bind VCT Voxel Data
//			glActiveTexture( g_DefaultTextureUnits[ TEX_TRACE_VOXEL ] );
//			glBindTexture( GL_TEXTURE_3D, m_refVCTVoxel->GetTexIrradiance()->texID );

			glActiveTexture( g_DefaultTextureUnits[3] );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_3D, m_refVCTVoxel->m_TexIrradianceAniso[0].texID ) );

			glActiveTexture( g_DefaultTextureUnits[4] );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_3D, m_refVCTVoxel->m_TexIrradianceAniso[1].texID ) );

			glActiveTexture( g_DefaultTextureUnits[ 5 ] );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_3D, m_refVCTVoxel->m_TexIrradianceAniso[2].texID ) );

			glActiveTexture( g_DefaultTextureUnits[ 6 ] );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_3D, m_refVCTVoxel->m_TexIrradianceAniso[3].texID ) );

			glActiveTexture( g_DefaultTextureUnits[ 7 ] );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_3D, m_refVCTVoxel->m_TexIrradianceAniso[4].texID ) );

			glActiveTexture( g_DefaultTextureUnits[ 8 ] );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_3D, m_refVCTVoxel->m_TexIrradianceAniso[5].texID ) );



			m_refScreenSpaceQuad->Draw();
			

			glActiveTexture( g_DefaultTextureUnits[ TEX_TRACE_POSITION ] );
			glBindTexture( GL_TEXTURE_2D, 0 );
			
			glActiveTexture( g_DefaultTextureUnits[ TEX_TRACE_NORMAL ] );
			glBindTexture( GL_TEXTURE_2D, 0 );

//			glActiveTexture( g_DefaultTextureUnits[ TEX_TRACE_VOXEL ] );
//			glBindTexture( GL_TEXTURE_3D, 0 );

			glActiveTexture( g_DefaultTextureUnits[ 3 ] );
			glBindTexture( GL_TEXTURE_3D, 0 );

			glActiveTexture( g_DefaultTextureUnits[ 4 ] );
			glBindTexture( GL_TEXTURE_3D, 0 );

			glActiveTexture( g_DefaultTextureUnits[ 5 ] );
			glBindTexture( GL_TEXTURE_3D, 0 );

			glActiveTexture( g_DefaultTextureUnits[ 6 ] );
			glBindTexture( GL_TEXTURE_3D, 0 );

			glActiveTexture( g_DefaultTextureUnits[ 7 ] );
			glBindTexture( GL_TEXTURE_3D, 0 );

			glActiveTexture( g_DefaultTextureUnits[ 8 ] );
			glBindTexture( GL_TEXTURE_3D, 0 );
			
		}
		m_Pass[ PASS::TRACE_VOXEL ].Unbind();


		m_Fbo.Unbind();

	}


	// 中身の詰まったセルを識別する
	void VoxelConeTracing::CreateSurfaceVoxelList()
	{
		if( !m_refVCTVoxel )
			return;
	
		GL_SAFE_CALL( glBindImageTexture( 0, m_refVCTVoxel->m_TexColor.texID, 0, GL_TRUE, 0, GL_READ_ONLY, m_refVCTVoxel->m_TexColor.GLInternalFormat() ) );

		m_refVCTVoxel->m_SurfaceVoxelCounter.Clear();
		m_refVCTVoxel->m_SurfaceVoxelCounter.BindBufferBase(0);
//m_refVCTVoxel->m_TboSurfaceVoxelList.BindImageTexture( 1, 0, GL_FALSE, 0, GL_WRITE_ONLY );// TBO version
m_refVCTVoxel->m_ssboSurfaceVoxelList.BindToShaderByIndex( m_CreateSurfaceVoxelList.ID(), m_biSurfaceVoxelsOutput );// SSBO version

		m_CreateSurfaceVoxelList.Bind();
		{
			GL_SAFE_CALL( glDispatchCompute( DivUp( m_refVCTVoxel->Width(), (uint32)8 ), DivUp( m_refVCTVoxel->Height(), (uint32)8 ), DivUp( m_refVCTVoxel->Depth(), (uint32)8 ) ) );

		//	glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );// TBO version
			glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );// SSBO version
		}
		m_CreateSurfaceVoxelList.Unbind();
		
		m_refVCTVoxel->m_SurfaceVoxelCounter.Unbind();
		
		m_refVCTVoxel->m_SurfaceVoxelCounter.Readback2CPU();
	}



	// ボクセル毎にシェーディング計算する
	void VoxelConeTracing::PerVoxelLightInjection()
	{
		// Bind Shader Input/Output
		m_refVCTVoxel->m_SurfaceVoxelCounter.BindBufferBase(0);
//m_refVCTVoxel->m_TboSurfaceVoxelList.BindImageTexture( 1, 0, GL_FALSE, 0, GL_READ_ONLY );// TBO version
m_refVCTVoxel->m_ssboSurfaceVoxelList.BindToShaderByIndex( m_PerVoxelLightInjection.ID(), m_biSurfaceVoxelsInput );// SSBO version
		GL_SAFE_CALL( glBindImageTexture( 2, m_refVCTVoxel->m_TexColor.texID, 0, GL_TRUE, 0, GL_READ_ONLY, m_refVCTVoxel->m_TexColor.GLInternalFormat() ) );
		GL_SAFE_CALL( glBindImageTexture( 3, m_refVCTVoxel->m_TexNormal.texID, 0, GL_TRUE, 0, GL_READ_ONLY, m_refVCTVoxel->m_TexNormal.GLInternalFormat() ) );
		GL_SAFE_CALL( glBindImageTexture( 4, m_refVCTVoxel->m_TexIrradiance.texID, 0, GL_TRUE, 0, GL_WRITE_ONLY, m_refVCTVoxel->m_TexIrradiance.GLInternalFormat() ) );// Output
	

		// Execute ComputeShader
		m_PerVoxelLightInjection.Bind();
		{
			GL_SAFE_CALL( glDispatchCompute( DivUp( m_refVCTVoxel->m_SurfaceVoxelCounter.GetCount(), (uint32)256 ), 1, 1 ) );
		}
		m_PerVoxelLightInjection.Unbind();
	}



	// AnisotropicVoxelを計算する
	void VoxelConeTracing::SetupAnisotropicVoxels()
	{
		// Bind Inputs
		GL_SAFE_CALL( glBindImageTexture( 0, m_refVCTVoxel->m_TexIrradiance.texID, 0, GL_TRUE, 0, GL_READ_ONLY, m_refVCTVoxel->m_TexIrradiance.GLInternalFormat() ) );
		GL_SAFE_CALL( glBindImageTexture( 1, m_refVCTVoxel->m_TexNormal.texID, 0, GL_TRUE, 0, GL_READ_ONLY, m_refVCTVoxel->m_TexNormal.GLInternalFormat() ) );

		// Bind Outputs
		GL_SAFE_CALL( glBindImageTexture( 2, m_refVCTVoxel->m_TexIrradianceAniso[0].texID, 0, GL_TRUE, 0, GL_WRITE_ONLY, m_refVCTVoxel->m_TexIrradianceAniso[0].GLInternalFormat() ) );
		GL_SAFE_CALL( glBindImageTexture( 3, m_refVCTVoxel->m_TexIrradianceAniso[1].texID, 0, GL_TRUE, 0, GL_WRITE_ONLY, m_refVCTVoxel->m_TexIrradianceAniso[1].GLInternalFormat() ) );
		GL_SAFE_CALL( glBindImageTexture( 4, m_refVCTVoxel->m_TexIrradianceAniso[2].texID, 0, GL_TRUE, 0, GL_WRITE_ONLY, m_refVCTVoxel->m_TexIrradianceAniso[2].GLInternalFormat() ) );
		GL_SAFE_CALL( glBindImageTexture( 5, m_refVCTVoxel->m_TexIrradianceAniso[3].texID, 0, GL_TRUE, 0, GL_WRITE_ONLY, m_refVCTVoxel->m_TexIrradianceAniso[3].GLInternalFormat() ) );
		GL_SAFE_CALL( glBindImageTexture( 6, m_refVCTVoxel->m_TexIrradianceAniso[4].texID, 0, GL_TRUE, 0, GL_WRITE_ONLY, m_refVCTVoxel->m_TexIrradianceAniso[4].GLInternalFormat() ) );
		GL_SAFE_CALL( glBindImageTexture( 7, m_refVCTVoxel->m_TexIrradianceAniso[5].texID, 0, GL_TRUE, 0, GL_WRITE_ONLY, m_refVCTVoxel->m_TexIrradianceAniso[5].GLInternalFormat() ) );
		

		// Execute ComputeShader
		m_SetupAnisotropicVoxels.Bind();
		{
			GL_SAFE_CALL( glDispatchCompute( DivUp( m_refVCTVoxel->Width(), (uint32)8 ), DivUp( m_refVCTVoxel->Height(), (uint32)8 ), DivUp( m_refVCTVoxel->Depth(), (uint32)8 ) ) );
		}
		m_SetupAnisotropicVoxels.Unbind();
		
	}



}// end of namespace