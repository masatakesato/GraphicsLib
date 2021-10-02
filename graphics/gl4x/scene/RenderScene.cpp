#include	"RenderScene.h"

#include	<Windows.h>	// GetCurrentDirectory, SetCurrentDirectoryで必要。TODO:フルパスでファイル名簡単に指定できるようプログラム改良して廃止したい(2014.04.12)
#include	<algorithm>

#include	<oreore/common/TString.h>
#include	"../other/GLPrimitives.h"

#include	<graphics/gl4xext/rendering/ConstantShader.h>
#include	<graphics/gl4xext/rendering/SimpleMeshRenderer.h>
#include	<graphics/gl4xext/rendering/SimpleDeferredShader.h>
#include	<graphics/gl4xext/rendering/TransparentShader.h>
#include	<graphics/gl4xext/rendering/RenderShadow.h>
#include	<graphics/gl4xext/rendering/GbufferShader.h>
#include	<graphics/gl4xext/rendering/ZPrepassShader.h>
#include	<graphics/gl4xext/rendering/QuadShader.h>
#include	<graphics/gl4xext/rendering/QuadShaderTexArray.h>
#include	<graphics/gl4xext/rendering/VoxelShader.h>
#include	<graphics/gl4xext/rendering/FXAAShader.h>
#include	<graphics/gl4xext/rendering/SMAAShader.h>
#include	<graphics/gl4xext/rendering/SSDOShader.h>
#include	<graphics/gl4xext/rendering/SeparableSSSShader.h>
#include	<graphics/gl4xext/rendering/LightShaftShader.h>
#include	<graphics/gl4xext/rendering/LocalReflectionShader.h>
#include	<graphics/gl4xext/rendering/DOFShader.h>
#include	<graphics/gl4xext/rendering/HDRShader.h>
#include	<graphics/gl4xext/rendering/ReflectiveShadowMapsShader.h>
#include	<graphics/gl4xext/rendering/LPVShader.h>
#include	<graphics/gl4xext/rendering/VoxelConeTracing.h>
#include	<graphics/gl4xext/rendering/VolumeRayCastingShader.h>


namespace OreOreLib
{

	//#########################################################################################################//
	//	global variables
	//#########################################################################################################//
	static Frustum	g_TempFrustum;



	//#########################################################################################################//
	//	static methods
	//#########################################################################################################//
	static IShaderFactory* GetRendererFactory( int shdtype )
	{
		switch( shdtype )
		{
		case SHADER_CONSTANT:
			return (IShaderFactory *)(new ConstantShaderFactory());

		case SHADER_SIMPLEMESH:
			return (IShaderFactory *)(new SimpleMeshRendererFactory());

		case SHADER_SIMPLEDEFERRED:
			return (IShaderFactory *)(new SimpleDeferredShaderFactory());

		case SHADER_TRANSPARENT:
			return (IShaderFactory *)(new TransparentShaderFactory());

		case SHADER_SHADOW:
			return (IShaderFactory *)(new RenderShadowFactory());

		case SHADER_GBUFFER:
			return (IShaderFactory *)(new GbufferShaderFactory());

		case SHADER_Z_PREPASS:
			return (IShaderFactory *)(new ZPrepassShaderFactory());

		case SHADER_QUAD:
			return (IShaderFactory *)(new QuadShaderFactory());

		case SHADER_QUAD_TEXARRAY:
			return (IShaderFactory *)(new QuadShaderTexArrayFactory());

		case SHADER_VOXEL:
			return (IShaderFactory *)(new VoxelShaderFactory());

		case SHADER_FXAA:
			return (IShaderFactory *)(new FXAAShaderFactory());

		case SHADER_SMAA:
			return (IShaderFactory *)( new SMAAShaderFactory() );

		case SHADER_SSDO:
			return (IShaderFactory *)(new SSDOShaderFactory());

		case SHADER_SEPARABLE_SSS:
			return (IShaderFactory *)(new SeparableSSSShaderFactory());

		case SHADER_LIGHT_SHAFT:
			return (IShaderFactory *)(new LightShaftShaderFactory());

		case SHADER_LOCAL_REFLECTION:
			return (IShaderFactory *)(new LocalReflectionShaderFactory());

		case SHADER_DOF:
			return (IShaderFactory *)(new DOFShaderFactory());

		case SHADER_HDR:
			return (IShaderFactory *)( new HDRShaderFactory() );

		case SHADER_REFLECTIVE_SHADOWMAPS:
			return (IShaderFactory *)(new ReflectiveShadowMapsShaderFactory());

		case SHADER_LPV:
			return (IShaderFactory *)(new LPVShaderFactory());

		case SHADER_VOXEL_CONE_TRACING:
			return (IShaderFactory *)( new VoxelConeTracingFactory() );

		case SHADER_VOLUME_RAY_CASTING:
			return (IShaderFactory *)( new VolumeRayCastingShaderFactory() );

		case SHADER_UNKNOWN:
			return NULL;

		default:
			return NULL;
		};





		return NULL;
	}


	// compare function for sorting
	//static bool Compare_Tex( const SceneNode* rLeft, const SceneNode* rRight )// TextureBufferのアドレス順に並べ替える
	//{
	//	return ((MeshObject *)rLeft->m_pObject)->m_pTexture < ((MeshObject *)rRight->m_pObject)->m_pTexture;
	//}


	static bool Compare_Tex1st_Vbo2nd( const MovableObject* rLeft, const MovableObject* rRight )
	{
		const MeshObject *plMesh	= (MeshObject *)rLeft;
		const MeshObject *prMesh	= (MeshObject *)rLeft;
		
		bool texdiff = plMesh->m_pTexture < prMesh->m_pTexture;
		return texdiff ? texdiff : bool(plMesh->m_pVertexBuffer < prMesh->m_pVertexBuffer);
	}


	//#########################################################################################################//
	//	private methods
	//#########################################################################################################//









	//#########################################################################################################//
	//	public methods
	//#########################################################################################################//

	// デフォルトコンストラクタ
	RenderScene::RenderScene()
	{
		//============== Set Default ScreenSize ==================//
		InitVec( m_ScreenSize, 0, 0 );

		//=========== Set SceneGraph Reference to NULL ===========//
		m_pSceneGraph	= NULL;

		//====== Initialize ScreenSpace Quad and Unit Cube ========//
		CreateScreenSpaceQuad( m_ScreenSpaceQuad );
		IShader::BindScreenSpaceQuad( &m_ScreenSpaceQuad );

		CreateUnitCube( m_UnitCube );
		IShader::BindUnitCube( &m_UnitCube );

		//============= Initialize UniformBlock Binding Pionts ===========//
GLBindPointManager::InitBindPoints();
//GLUniformBufferObject::InitBindPoints();

		//===================== Initialize Shader ========================//
		TCHAR currdir[ MAX_PATH ];
		GetCurrentDirectory( MAX_PATH, currdir );

		SetCurrentDirectory( _T( "../shader/glsl" ) );

		m_Shaders.resize( NUM_SHADER_TYPE );
		for( int i=0; i<NUM_SHADER_TYPE; ++i )
		{
			IShaderFactory *irf = GetRendererFactory(i);
			m_Shaders[i]	= irf ? irf->CreateInstance( GLSL_VERSION::GLSL_430 ) : NULL;
			SafeDelete( irf );
		}// end of i loop

		SetCurrentDirectory( currdir );


		//=================== Initialize LightAttributeBuffer ===============//
		InitLightAttributeBuffer( MAX_LIGHT_BUFFER );
		
		//=================== Initialize ViewTranformBuffer ==============//
		InitViewTransformBuffer();
		
		//===================== Initialize Render Queue ====================//
		m_numShadowObjects	= 0;
		for( int i=0; i<MAX_RENDER_QUEUE; ++i )	m_RenderShadowQueue[i] = NULL;

		for( int i=0; i<NUM_SHADER_TYPE; ++i )
		{
			m_numRenderObjects[i] = 0;
			memset( m_RenderObjects[i], 0, sizeof(m_RenderObjects[i]) );
		}


		//===================== Initialize LightList =======================//
		//m_numLights		= 0;
		//memset( m_LightQueue, 0, sizeof(SceneNode *) * MAX_LIGHTS );

		for( int i=0; i<NUM_LIGHT_TYPE; ++i )
		{
			m_numLights[i] = 0;
			for( int j=0; j<MAX_LIGHTS; ++j )
				m_LightSources[i][j] = NULL;
		}


m_bUpdateIrradianceVolume	= false;

	}



	// デストラクタ
	RenderScene::~RenderScene()
	{
		Release();
	}



	// Initialize ViewTransformBuffer
	void RenderScene::InitViewTransformBuffer()
	{
		//=================== Initialize ViewTranformBuffer ==============//
		m_ViewTransformBuffer.Init();

		for( int i=0; i<NUM_SHADER_TYPE; ++i )
		{
			if( !m_Shaders[i] )	continue;
			m_Shaders[i]->BindBufferObject( &m_ViewTransformBuffer );
		}// end of i loop
	}



	// Initialize Offscreen Buffers
	void RenderScene::InitOffScreenBuffers( int screen_width, int screen_height )
	{
		if( screen_width == 0 || screen_height == 0 )
			return;
		if( m_ScreenSize.x == screen_width && m_ScreenSize.y == screen_width )
			return;
		
		// Set new screensize
		InitVec( m_ScreenSize, screen_width, screen_height );

		//=========================	Initialize Gbuffer	===========================//
		// clean up GLFrameBufferObject
//		m_FboGbuffer.Release();

		// allocate texture
		m_texPosition.Allocate( m_ScreenSize.x, m_ScreenSize.y, 0, 0, DATA_FORMAT::FORMAT_R16G16B16A16_FLOAT );
		m_texPosition.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );
		m_texPosition.GenHardwareTexture();

		m_texNormal.Allocate( m_ScreenSize.x, m_ScreenSize.y, 0, 0, DATA_FORMAT::FORMAT_R16G16B16_FLOAT );
		m_texNormal.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );
		//m_texNormal.SetAddressMode( TEXTURE_ADDRESS_MODE::TEX_ADDRESS_MIRROR, TEXTURE_ADDRESS_MODE::TEX_ADDRESS_MIRROR );
		m_texNormal.GenHardwareTexture();

		m_texDepth.Allocate( m_ScreenSize.x, m_ScreenSize.y, 0, 0, DATA_FORMAT::FORMAT_R32_FLOAT );
		m_texDepth.GenHardwareTexture();

		m_texDiffuse.Allocate( m_ScreenSize.x, m_ScreenSize.y, 0, 0, DATA_FORMAT::FORMAT_R16G16B16A16_FLOAT );
		m_texDiffuse.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );
		m_texDiffuse.SetAddressMode( TEXTURE_ADDRESS_MODE::TEX_ADDRESS_MIRROR, TEXTURE_ADDRESS_MODE::TEX_ADDRESS_MIRROR );
		m_texDiffuse.GenHardwareTexture();


		m_RawShadedResult.Allocate( m_ScreenSize.x, m_ScreenSize.y, 0, 0, DATA_FORMAT::FORMAT_R32G32B32A32_FLOAT );
		m_RawShadedResult.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );
		m_RawShadedResult.SetAddressMode( TEXTURE_ADDRESS_MODE::TEX_ADDRESS_MIRROR, TEXTURE_ADDRESS_MODE::TEX_ADDRESS_MIRROR );
		m_RawShadedResult.GenHardwareTexture();

		m_IrradianceBuffer.Allocate( m_ScreenSize.x, m_ScreenSize.y, 0, 0, FORMAT_R16G16B16A16_FLOAT );//FORMAT_R32G32B32_FLOAT );//
		m_IrradianceBuffer.GenHardwareTexture();

		// Init RenferTarget
		const uint32 texids[] ={ m_texPosition.texID, m_texNormal.texID, m_texDepth.texID, m_texDiffuse.texID, m_RawShadedResult.texID };
		m_RTGbuffer.Init( m_ScreenSize.x, m_ScreenSize.y, true );
		m_RTGbuffer.BindTextures( ArraySize( texids ), g_DefaultColorAttachments, texids );


		//================== Initialize SSDO Buffer and Bind to SSDOShader ================//
		m_SSDOBuffer.Init( m_ScreenSize.x/2, m_ScreenSize.y/2 );

		SSDOShader *pshader	= (SSDOShader *)m_Shaders[ SHADER_SSDO ];
		pshader->BindFramebuffer( m_texPosition, m_texNormal, m_texDiffuse ); 
		pshader->BindSSDOBuffer( &m_SSDOBuffer );//, &m_TexOcclusionSH, &m_TexBlurBuffer );

		//=========================	Bind Buffer SMAAShader	=============================//
		( (SMAAShader *)m_Shaders[SHADER_SMAA] )->BindInputTexture( &m_texDiffuse );


		//========================	Bind Buffer to DOFShader	=========================//
		DOFShader *pDofShader	= (DOFShader*)m_Shaders[ SHADER_DOF ];
		pDofShader->BindInputTexture( &m_RawShadedResult, &m_texDepth );

		//========================	Bind Buffer to HDRShader	=========================//
		HDRShader *pHdrShader	= (HDRShader*)m_Shaders[SHADER_HDR];
		pHdrShader->BindInputTexture( &m_RawShadedResult );


		//=====================	Bind Buffer to LightShaftShader	========================//
		LightShaftShader *plightshaftshader	= (LightShaftShader *)m_Shaders[ SHADER_LIGHT_SHAFT ];
		plightshaftshader->BindFramebuffer( &m_texDepth, &m_texPosition );


		//==================	Initialize OITBuffer	======================//
		// Init Texture
		m_OITAccumBuffer.Allocate( m_ScreenSize.x, m_ScreenSize.y, 0, 0, FORMAT_R16G16B16A16_FLOAT );
		m_OITAccumBuffer.GenHardwareTexture();

		m_OITRevealageBuffer.Allocate( m_ScreenSize.x, m_ScreenSize.y, 0, 0, FORMAT_R8_UNORM );
		m_OITRevealageBuffer.GenHardwareTexture();

		// Init RenderTarget
		const uint32 oitTexIds[] ={ m_OITAccumBuffer.texID, m_OITRevealageBuffer.texID };
		m_RTOIT.Init( m_ScreenSize.x, m_ScreenSize.y, true );
		m_RTOIT.BindTextures( ArraySize( oitTexIds ), g_DefaultColorAttachments, oitTexIds );

	}
	


	void RenderScene::InitShadowBuffer( int w, int h, int numshadowmaps )
	{
		if( m_ShadowBuffer.Width()==w && m_ShadowBuffer.Height()==h )	return;

		tcout << _T("RenderScene::InitShadowBuffer()...\n");

		// Allocate ShadowBuffer
		m_ShadowBuffer.Init( w, h, numshadowmaps );
		
		// Tell Shadowcaster to texturesize
		m_ShadowCaster.SetShadowMapSize( w, h );

		// Tell ShadowAttachments to texid
		m_ShadowAttachments.BindShadowMap( m_ShadowBuffer.GetTexture()->texID );

		// Bind ShadowBuffer to Shader
		for( int i=0; i<NUM_SHADER_TYPE; ++i )
		{
			m_Shaders[i]->BindBufferObject( (IBufferObject *)&m_ShadowBuffer );
		}

		//m_Shaders[ SHADER_SIMPLEMESH ]->BindBufferObject( &m_ShadowBuffer );
		//m_Shaders[ SHADER_SHADOW ]->BindBufferObject( &m_ShadowBuffer );
	}



	void RenderScene::InitLightAttributeBuffer( int num_max_lights )
	{
		if( num_max_lights > MAX_LIGHT_BUFFER )
			return;
	
		// LightAttributeBufferを初期化する
		m_LightAttributeBuffer.Init( num_max_lights );

		// ShadowAttachmentArrayを初期化する
		m_ShadowAttachments.Init( num_max_lights );

		// 各シェーダーにUniformBufferObjectをバインドする(?)
		for( int i=0; i<NUM_SHADER_TYPE; ++i )
		{
			m_Shaders[i]->BindBufferObject( (IBufferObject *)&m_LightAttributeBuffer );
		}
	}



	void RenderScene::InitIrradianceVolume( int width, int height, int depth, int num_cascades, float dim )
	{
		// IrradianceVolumeを初期化する
		m_LPVIrradianceVolume.Init( num_cascades, width, height, depth );
		m_LPVIrradianceVolume.SetCellDim( dim );
		

		// LPVシェーダーにアタッチする
		LPVShader* plpvshader	= (LPVShader *)m_Shaders[ SHADER_LPV ];
		plpvshader->BindLPV( &m_LPVIrradianceVolume );

		
		plpvshader->InitRSMTexture( 512, 512 );
		plpvshader->InitVPLSprites();
		plpvshader->InitLPVSlices();
		plpvshader->SetNumIteration( 16 );
	}
	


	void RenderScene::InitVCTVoxelData( int width, int height, int depth, int numcascades, float dim )
	{
		m_VCTVoxelData.Init( width, height, depth, numcascades, dim );
		m_VCTVoxelData.SetOrigin( 0.0f, 4.0f, 0.0f );
		m_VCTVoxelData.SetBasis( 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f );//ワールドxz平面に、3Dテクスチャuvスライス面を対応付ける. ワールドy軸はn座標

		VoxelConeTracing* pVCTShader	= (VoxelConeTracing *)m_Shaders[SHADER_VOXEL_CONE_TRACING];

		pVCTShader->BindVCTVoxelData( &m_VCTVoxelData );
		pVCTShader->InitVPLSprites( 512, 512 );
	}



	void RenderScene::InitRSMBuffer( int width, int height )
	{
		//==================== Init RSM Buffers ====================//
		m_RsmPositionBuffer.Allocate( width, height, 0, 0, FORMAT_R16G16B16A16_FLOAT );// Position(RGB:xyz座標, A:モデルビュー変換後のw)
		m_RsmPositionBuffer.GenHardwareTexture();

		m_RsmNormalBuffer.Allocate( width, height, 0, 0, FORMAT_R16G16B16A16_FLOAT );// Normal
		m_RsmNormalBuffer.GenHardwareTexture();

		m_RsmFluxBuffer.Allocate( width, height, 0, 0, FORMAT_R16G16B16A16_FLOAT );	// Flux
		m_RsmFluxBuffer.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );
		m_RsmFluxBuffer.GenHardwareTexture();
		
		m_RsmDepthBuffer.Allocate( width, height, 0, 0, FORMAT_R16_FLOAT );	// Depth
		m_RsmDepthBuffer.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );
		m_RsmDepthBuffer.GenHardwareTexture();


		//================ Init RenderTarget =================//
		const uint32 texids[]	= { m_RsmPositionBuffer.texID, m_RsmNormalBuffer.texID, m_RsmFluxBuffer.texID, m_RsmDepthBuffer.texID };
		m_RTReflectiveShadowMaps.Init( width, height, true );
		m_RTReflectiveShadowMaps.BindTextures( ARRAYSIZE( texids ), g_DefaultColorAttachments, texids );

		//============= Set RSM Resolution to RSMShader ============//
		ReflectiveShadowMapsShader *pRsmShader	= (ReflectiveShadowMapsShader *)m_Shaders[ SHADER_REFLECTIVE_SHADOWMAPS ];
		pRsmShader->SetRsmSize( m_RTReflectiveShadowMaps.Width(), m_RTReflectiveShadowMaps.Height() );

		//================= Bind RSM to LPVShader ==================//
		LPVShader* plpvshader	= (LPVShader *)m_Shaders[ SHADER_LPV ];
		plpvshader->BindRSMBuffer( &m_RsmPositionBuffer, &m_RsmNormalBuffer, &m_RsmFluxBuffer, &m_RsmDepthBuffer );

		//========= Bind RSM to Voxel Cone Tracing Shader =========//
		VoxelConeTracing *pVctShader	= (VoxelConeTracing *)m_Shaders[SHADER_VOXEL_CONE_TRACING];
		pVctShader->BindRSMBuffer( &m_RsmPositionBuffer, &m_RsmNormalBuffer, &m_RsmFluxBuffer, &m_RsmDepthBuffer );
	}







	// bind scenegraph
	void RenderScene::BindSceneGraph(SceneGraph *sg)
	{
		m_pSceneGraph = sg;

		m_numVAOs = 0;
		AssignVertexArrayObject( sg->m_pMeshGroupTree );


		m_ViewTransformBuffer.BindFrustum( m_pSceneGraph->GetCurrentCameraObject()->GetFrustum() );

	}



	// unbind scenegraph
	void RenderScene::UnbindSceneGraph()
	{
		m_pSceneGraph = NULL;

		for( int i=0; i<m_numVAOs; ++i )	m_GeometryObjects[i].Release();
		m_numVAOs = 0;

		m_ViewTransformBuffer.UnbindFrustum();
	}



//	TODO: 影の座標変換マトリクスを計算して、ShadowBufferの該当スロットに代入する
	// RenderSceneのアップデート. TODO: シーングラフをスキャンして描画オブジェクトリストを作る. + ソートする
	void RenderScene::Update()
	{
		if( !m_pSceneGraph )	return;

		//=============== シーングラフをトラバースして描画オブジェクトを集める ===================//
		// 各レンダーキューの登録オブジェクト数をゼロでリセットする
		m_numShadowObjects	= 0;// 影
		memset( m_numRenderObjects, 0, sizeof( m_numRenderObjects ) );

		// 
		TraverseMeshTree( m_pSceneGraph->m_pMeshGroupTree );
		// TODO: フラスタムカリングで見えないオブジェクトを排除 ?
		// TODO: 不可視属性のオブジェクトも排除 ?

		// シーングラフをトラバースして光源オブジェクトを集める
		for( int i=0; i < NUM_LIGHT_TYPE; ++i )	m_numLights[i] = 0;
		m_ShadowBuffer.Clear();
		m_LightAttributeBuffer.Clear();
		TraverseLightTree( m_pSceneGraph->m_pLightTree );
		
		// LightAttributeBufferのデータをGPUに転送する
		m_LightAttributeBuffer.UpdateSubresource();

		// ViewTransformBufferを更新する
		m_ViewTransformBuffer.UpdateSubresource();

		// シャドウマップを更新する 
		UpdateShadowMatrices( *m_pSceneGraph->GetCurrentCameraObject()->GetFrustum() );
		UpdateShadowBuffer();

		// ステートソーティングする
		for( int i=0; i < NUM_SHADER_TYPE; ++i )
			std::sort( m_RenderObjects[i], m_RenderObjects[i] + m_numRenderObjects[i], Compare_Tex1st_Vbo2nd/*Compare_Tex*/ );	// テクスチャのID順/VBOのID順に並べ替える

		//====================== TODO: Update()関数内で、ClusteredShadingを実行する =========================//

//########################### TODO: 試験実装. VoxelConeTracing用のIrradianceVolumeをアップデートする ######################//
		if( m_bUpdateIrradianceVolume )
		{
			UpdateVCTVoxel();
			m_bUpdateIrradianceVolume	= false;
		}
//GL_CHECK_ERROR();
	}



	void RenderScene::UpdateShadowMatrices( Frustum& cameraFrustum )
	{
		int						numCascades;
		Mat4f					matShadows[ MAX_SHADOW_CASCADES ];
		float					splitDepths[ MAX_SHADOW_CASCADES ];
		FrameBufferAttachment*	pAttachment;
		LightObject*			pLightObj;


		for( int light_id = 0; light_id < m_ShadowAttachments.Size(); ++light_id )
		{
			if( m_LightAttributeBuffer.HasActiveShadowMap( light_id ) == false )	continue;

			pAttachment				= m_ShadowAttachments.GetAttachment( light_id );
			numCascades				= pAttachment->NumAttachPoints();
			pLightObj				= (LightObject *)m_LightAttributeBuffer.GetSceneObject( light_id );
			LIGHT_TYPE lightType	= m_LightAttributeBuffer.GetLightType( light_id );
			const int *targetIds	= pAttachment->GetLayers();

			// シャドウマップの座標変換行列を計算する
			switch( lightType )
			{
			case LIGHT_TYPE::LIGHT_TYPE_DIRECTIONAL:
				m_ShadowCaster.CalcMatrix_Directional( numCascades, matShadows, splitDepths, cameraFrustum, *pLightObj->GetFrustum() );
				break;

			case LIGHT_TYPE::LIGHT_TYPE_SPOT:
				m_ShadowCaster.CalcMatrix_Omni( numCascades, matShadows, splitDepths, cameraFrustum, *pLightObj->GetFrustum() );
				break;

			case LIGHT_TYPE::LIGHT_TYPE_POINT:
				
				break;			
			
			case LIGHT_TYPE::LIGHT_TYPE_AREA:

				break;

			default:
				break;

			};

			// 計算結果をm_ShadowBufferの該当するm_MatShadowsに書き込む
			for( int i=0; i<numCascades; ++i )
				m_ShadowBuffer.SetMatrixAndSplitDepth( targetIds[i], matShadows[i], splitDepths[i] );



		}// end of light_id loop
		
		m_ShadowBuffer.UpdateSubresource();

	}




	void RenderScene::UpdateIrradianceVolume( bool updatepos )
	{
		CameraObject *pcamObj	= m_pSceneGraph->GetCurrentCameraObject();
		LightObject *pLightObj	= m_pSceneGraph->GetCurrentLightObject();

		if( !pLightObj )
			return;

		//==================== ReflectiveShadowMaps生成 =======================//
		UpdateReflectiveShadowMaps( pLightObj );//&g_TempFrustum );//

		//=========== Update TransformMatrices of LPV IrradianceVolume ========//
		static float offset = 0.0f;

		m_LPVIrradianceVolume.SetOrigin( 0, 0.0f, 0.0f, floor( offset / m_LPVIrradianceVolume.GetCellDim( 0 ) ) );//pcamObj->GetPosition()->x, pcamObj->GetPosition()->y, pcamObj->GetPosition()->z );
		m_LPVIrradianceVolume.UpdateMatrix();

		//==================== Update LPV Irradiance Volume ===================//
		LPVShader *pLPV		= ((LPVShader *)m_Shaders[SHADER_LPV]);
		pLPV->Update( *pLightObj->GetFrustum() );

		// ビューポートサイズを元に戻す
		glViewport( 0, 0, m_ScreenSize.x, m_ScreenSize.y );

	}



	void RenderScene::UpdateVCTVoxel()
	{
		CameraObject *pcamObj	= m_pSceneGraph->GetCurrentCameraObject();
		LightObject *pLightObj	= m_pSceneGraph->GetCurrentLightObject();

		if( !pLightObj )
			return;

		//==================== ReflectiveShadowMaps生成 =======================//
		const Frustum *pLightFrustum	= pLightObj->GetFrustum();
		//tcout << pLightFrustum->GetDirection()->x << ", " << pLightFrustum->GetDirection()->y << ", " << pLightFrustum->GetDirection()->z << tendl;
		UpdateReflectiveShadowMaps( pLightObj );

		//============= Update TransformMatrices of VCT VoxelData =============//
		const Frustum *pCamFrustum	= pcamObj->GetFrustum();// カメラ移動に追従してボクセル空間をスナップ移動するテスト.MipMapのレベルによってスナップ幅が変わってくる
		const float snapLength	= 8.0f;
		m_VCTVoxelData.SetOrigin(
		//0.0f, 0.0f, 0.0f );//
		floor( pLightFrustum->GetPosition()->x / snapLength ) * snapLength,
		floor( pLightFrustum->GetPosition()->y / snapLength ) * snapLength,
		floor( pLightFrustum->GetPosition()->z / snapLength ) * snapLength );


		m_VCTVoxelData.UpdateMatrix();
		

		//================= Update VCT Voxel Data ========================//
		VoxelConeTracing *pVCTShader	= (VoxelConeTracing *)m_Shaders[ SHADER_VOXEL_CONE_TRACING ];
		pVCTShader->Update( m_numRenderObjects[ SHADER_VOXEL_CONE_TRACING ], (const MovableObject**)&m_RenderObjects[ SHADER_VOXEL_CONE_TRACING ] );// シーンデータのボクセル化で、レンダーオブジェクトのリストが必要
		

#ifdef _DEBUG
tcout << _T( "VCTVoxels updated..." ) << tendl;
#endif // _DEBUG

		// ビューポートサイズを元に戻す
		glViewport( 0, 0, m_ScreenSize.x, m_ScreenSize.y );
	}



	void RenderScene::UpdateShadowBuffer()
	{
		//######################################### Render ShadowMap ##########################################//	

		RenderShadow *pshadowShader = (RenderShadow *)m_Shaders[SHADER_SHADOW];

		glViewport( 0, 0, m_ShadowBuffer.Width( ), m_ShadowBuffer.Height( ) );

	
		//#ifdef	_DEBUG
		//GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		//#endif

		pshadowShader->Bind( );
		m_ShadowBuffer.Bind( );

		// 光源毎にシャドウマップを計算する
		for ( int light_id = 0; light_id < m_LightAttributeBuffer.NumActiveSlots( ); ++light_id )
		{
			if ( !m_LightAttributeBuffer.HasActiveShadowMap( light_id ) )	continue;			

			FrameBufferAttachment *pAttachment = m_ShadowAttachments.GetAttachment( light_id );
			pAttachment->BindAttachmentLayer( );
			LIGHT_TYPE	ltype = m_LightAttributeBuffer.GetLightType( light_id );

			pshadowShader->SetProjectionType( ltype );

			for( int attch_id = 0; attch_id < pAttachment->NumAttachPoints(); ++attch_id )
			{
				pshadowShader->SetMatShadowIndex( *pAttachment->GetLayer( attch_id ) );

				pAttachment->DrawBuffers( 1, attch_id );// glClearの前に実行する

				glClearColor( 0.0, 0.0, 0.0, 0.0 );// glClearの前に実行する
				glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


				// 影を写像するオブジェクト群を描画する
				for( int shadowobj_id = 0; shadowobj_id < m_numShadowObjects; ++shadowobj_id )
				{
					GeometryBuffer *pGeom	= m_RenderShadowQueue[shadowobj_id]->GetBuffer<GeometryBuffer>();
					pshadowShader->Render( pGeom, NULL );
				}
			}

			pAttachment->UnbindAttachmentLayer();
			

		}// end of light_id loop

		m_ShadowBuffer.Unbind( );
		pshadowShader->Unbind( );


		// ビューポートサイズを元に戻す
		glViewport( 0, 0, m_ScreenSize.x, m_ScreenSize.y );
	
	}



	void RenderScene::UpdateReflectiveShadowMaps( LightObject* pLightObj )
	{
		int lbuffer_id	= pLightObj->GetBufferID();
		if( lbuffer_id == -1 )	return;

		glViewport( 0, 0, m_RTReflectiveShadowMaps.Width(), m_RTReflectiveShadowMaps.Height() );

		m_RTReflectiveShadowMaps.Bind();
		{
			glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

			ReflectiveShadowMapsShader *pRsmShader	= (ReflectiveShadowMapsShader *)m_Shaders[SHADER_REFLECTIVE_SHADOWMAPS];

			pRsmShader->BindShader();
			{
				pRsmShader->SetLightID( lbuffer_id );

				for( int obj_idx=0; obj_idx < m_numShadowObjects; ++obj_idx )
				{
					GeometryBuffer *pGeom	= m_RenderShadowQueue[obj_idx]->GetBuffer<GeometryBuffer>();
					pRsmShader->Render( pGeom, pLightObj->GetFrustum() );
				}
			}
			pRsmShader->UnbindShader();
		}
		m_RTReflectiveShadowMaps.Unbind();

		// デバッグ用テクスチャ可視化
		//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		//glViewport( 0, 0, m_ScreenSize.x, m_ScreenSize.y );
		//DrawQuad( m_RsmFluxBuffer.texID );

	}



	// 描画メソッド. TODO: とりあえずOpaqueメッシュだけ描画する
	void RenderScene::RenderForward()
	{
		glClearColor( 0.0, 0.0, 0.0, 0.0 );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		//================== Render Scene ===================//		
		SimpleMeshRenderer *pshader	=  (SimpleMeshRenderer *)m_Shaders[ SHADER_SIMPLEMESH ];

		pshader->Bind();

		for( int obj_idx=0; obj_idx<m_numRenderObjects[SHADER_SIMPLEMESH]; ++obj_idx )
		{
			const GeometryBuffer *pGeom	= m_RenderObjects[SHADER_SIMPLEMESH][obj_idx]->GetBuffer<GeometryBuffer>();
			pshader->Render( pGeom, NULL );

		}// end of mesh_idx loop

		pshader->Unbind();

		
		//########################################### View ShadowMap ############################################//		
		//ViewShadowMaps();
	}



	void RenderScene::RenderDeferred( bool enableIV )
	{
		m_RTGbuffer.Bind( 1, 4 );

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		SimpleDeferredShader *pDefferedShader	= (SimpleDeferredShader *)m_Shaders[SHADER_SIMPLEDEFERRED];
		pDefferedShader->SetIrrasianceMapMode( enableIV );
		pDefferedShader->Render( &m_texDiffuse, &m_texPosition, &m_texDepth, &m_texNormal, &m_IrradianceBuffer );

		m_RTGbuffer.Unbind();
	}



	void RenderScene::RenderConstant( const Vec4f* color )
	{
		glClearColor( 0.0, 0.0, 0.0, 0.0 );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		//########################################### Render Scene ############################################//		
		ConstantShader *pshader	=  (ConstantShader *)m_Shaders[SHADER_CONSTANT];

		pshader->Bind();

		for( int obj_idx=0; obj_idx<m_numRenderObjects[SHADER_SIMPLEMESH]; ++obj_idx )
		{
			const GeometryBuffer *pGeom	= m_RenderObjects[SHADER_SIMPLEMESH][obj_idx]->GetBuffer<GeometryBuffer>();
			if( color )
				pshader->Render( pGeom, NULL, color );
			else
				pshader->Render( pGeom, NULL );

		}// end of mesh_idx loop

		pshader->Unbind();

	}


//TODO: 動作確認中. 半透明オブジェクトのレンダーキュー実装が必要 2015.04.29
	void RenderScene::RenderTransparent()
	{

		TransparentShader *pTransShader	= (TransparentShader *)m_Shaders[ SHADER_TRANSPARENT ];

		const Vec3f bgColor	={ 0.5f, 0.5f, 0.5f };
		

		//============== Pass1: Accumulate Pixel ===============//		
		glDisable( GL_DEPTH_TEST );

		m_RTOIT.Bind();
		{
			glClearColor( 0.0f, 0.0f, 1.0f, 0.0f );//glClearColor( 0.5f, 0.5f, 0.75f, 1.0f );
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			
			const float clearZero[]	={ 0.0f, 0.0f, 0.0f, 0.0f };
			const float clearOne[]	={ 1.0f, 1.0f, 1.0f, 1.0f };
			glClearBufferfv( GL_COLOR, 0, clearZero );
			glClearBufferfv( GL_COLOR, 1, clearOne );

			glEnable( GL_BLEND );
			glBlendEquation( GL_FUNC_ADD );
			glBlendFunci( 0, GL_ONE, GL_ONE );
			glBlendFunci( 1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR );

			pTransShader->BindShader( 0 );
			{
				// 注意! マテリアルにトランスペアレンシー設定したオブジェクトしかレンダリングしない
				for( int shd_type=0; shd_type < NUM_SHADER_TYPE; ++shd_type )
				{
					for( int obj_idx=0; obj_idx < m_numTransparentObjects[shd_type]; ++obj_idx )
					{
						const GeometryBuffer *pGeom	= m_TransparentObjects[shd_type][obj_idx]->GetBuffer<GeometryBuffer>();
						pTransShader->Render( pGeom, NULL );
					}// end of obj_id loop
				}// end of shd_type loop

				//for( int obj_idx=0; obj_idx<m_numRenderObjects[SHADER_SIMPLEMESH]; ++obj_idx )
				//{
				//	const GeometryBuffer *pGeom	= m_RenderObjects[SHADER_SIMPLEMESH][obj_idx]->GetBuffer<GeometryBuffer>();
				//	pTransShader->Render( pGeom, NULL );

				//}// end of mesh_idx loop

			}
			pTransShader->UnbindShader();

			glDisable( GL_BLEND );
			
		}
		m_RTOIT.Unbind();


		//=================== Pass2: Blend =====================//
		pTransShader->Render( &m_OITAccumBuffer, &m_OITRevealageBuffer, &bgColor );

		glEnable( GL_DEPTH_TEST );

		//=================== Debug Output =====================//
		//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		//QuadShader *pQuadShader	= (QuadShader *)m_Shaders[ SHADER_QUAD ];
		//pQuadShader->Render( &m_OITAccumBuffer );

	}



	void RenderScene::RenderGbuffer()
	{
		GbufferShader *pGbufferShader	= (GbufferShader *)m_Shaders[SHADER_GBUFFER];

		m_RTGbuffer.Bind( 4, 0 );
		{
			glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );// FBOをアタッチした後で実行する

			pGbufferShader->BindShader();
			{
				for( int shd_type=0; shd_type < NUM_SHADER_TYPE; ++shd_type )
				{
					for( int obj_idx=0; obj_idx < m_numRenderObjects[shd_type]; ++obj_idx )
					{
						const GeometryBuffer *pGeom	= m_RenderObjects[shd_type][obj_idx]->GetBuffer<GeometryBuffer>();
						pGbufferShader->Render( pGeom, NULL );
					}// end of obj_id loop
				}// end of shd_type loop
			}
			pGbufferShader->UnbindShader();
		}
		m_RTGbuffer.Unbind();

	}
		
	
	
	void RenderScene::RenderZPrepass()
	{
		

		//################################# Pass1: Z Prepass Rendering  ###################################//
		// RenderState Setup for Z-Prepass FIRST!!!
		glEnable( GL_DEPTH_TEST );								// We want depth test
		glDepthFunc( GL_LESS );									// We want to get the nearest pixels
		glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );	// Disable color, it's useless, we only want depth.
		glDepthMask( GL_TRUE );									// Enable Z writing

		// Then Clear Framebuffer
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		
		
		m_Shaders[ SHADER_Z_PREPASS ]->BindShader();

		for ( int obj_idx = 0; obj_idx<m_numRenderObjects[SHADER_SIMPLEMESH]; ++obj_idx )
		{
			GeometryBuffer *pGeom	= m_RenderObjects[SHADER_SIMPLEMESH][obj_idx]->GetBuffer<GeometryBuffer>();
			m_Shaders[SHADER_Z_PREPASS]->Render( pGeom, NULL );
		}

		m_Shaders[ SHADER_Z_PREPASS ]->UnbindShader();
		

		
		//############################## Pass2: Forward Rendering  ##################################//
		// RenderState Setup FIRST!!!
		glEnable( GL_DEPTH_TEST );							// We still want depth test
		glDepthFunc( GL_EQUAL );							// only draw if the depth of the incoming fragment matches the depth already in the depth buffer
		glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );	// now set the color component
		glDepthMask( GL_FALSE );							// don't write to the depth buffer

		// Then Clear Framebuffer
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		
		SimpleMeshRenderer *pshader = (SimpleMeshRenderer *)m_Shaders[SHADER_SIMPLEMESH];

		pshader->Bind( );

		for ( int obj_idx = 0; obj_idx<m_numRenderObjects[SHADER_SIMPLEMESH]; ++obj_idx )
		{
			GeometryBuffer *pGeom	= m_RenderObjects[SHADER_SIMPLEMESH][obj_idx]->GetBuffer<GeometryBuffer>();
			
			pshader->Render( pGeom, NULL );//m_refMatCamera//

		}// end of mesh_idx loop

		pshader->Unbind( );
		

		//################################ 後片付け.レンダーステートを元に戻す.忘れずにやる!後続パスに影響出る ################################//
		glDepthMask( GL_TRUE );
		glDepthFunc( GL_LESS );
		
	}



	void RenderScene::RenderAntialias( bool enable )
	{
		//======================= Render G-buffer ========================//
		RenderGbuffer();
		
		//======================= No Antialias	==========================//
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		if( enable==false )
		{
			QuadShader	*pquadShader	= (QuadShader *)m_Shaders[ SHADER_QUAD ];
			pquadShader->Render( m_texDiffuse.texID );

			return;
		}



		////======================= Apply FXAA	========================//
		//FXAAShader *pfxaashader = (FXAAShader *)m_Shaders[ SHADER_FXAA ];
		////glClearColor( 0.5f, 0.0f, 0.3f, 0.0f );
		//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		//pfxaashader->Render( &m_texDiffuse );
		

		//=======================	Apply SMAA	========================//
		SMAAShader *psmaashader = (SMAAShader *)m_Shaders[SHADER_SMAA];
		//glClearColor( 0.5f, 0.0f, 0.3f, 0.0f );
		//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		psmaashader->Render( &m_texDiffuse );

		//// SMAA Debug
		//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		//QuadShader	*pquadShader	= (QuadShader *)m_Shaders[SHADER_QUAD];
		//pquadShader->Render( psmaashader->GetTexSmaaWk()->texID );

	}



	void RenderScene::RenderSSS()
	{

		//======================= Render G-buffer ========================//
		RenderGbuffer();

		//===================== Do Deferred Shading ======================//
		RenderDeferred( false );

		//======================= Render SSS ============================//

		glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		SeparableSSSShader *psssShader = (SeparableSSSShader *)m_Shaders[ SHADER_SEPARABLE_SSS ];

		psssShader->BindTexture( m_texDepth, /*m_texDiffuse*/m_RawShadedResult, m_texNormal, m_texPosition );
		psssShader->SetTranslucency( 0.5f );
		psssShader->SetScale( 0.75f );

		psssShader->Render();

	}



	void RenderScene::RenderSSDO()
	{
		//====================== Render G-buffer ======================//

		RenderGbuffer();

		//m_Shaders[ SHADER_GBUFFER ]->BindShader();

		////#pragma omp parallel
		//for( int i=0; i<m_numShadowObjects; ++i )
		//{
		//	GeometryBuffer *pGeom	= m_RenderShadowQueue[i]->GetBuffer<GeometryBuffer>();
		//	m_Shaders[ SHADER_GBUFFER ]->Render( pGeom, /*m_refMatCamera*/NULL );
		//}

		//m_Shaders[ SHADER_GBUFFER ]->UnbindShader();

		//======================= Render SSDO ========================//

		glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		SSDOShader *pssdo	= (SSDOShader *)m_Shaders[ SHADER_SSDO ];

		pssdo->Render(0);


		//// Debug
		//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		//QuadShader	*pquadShader	= (QuadShader *)m_Shaders[SHADER_QUAD];
		//pquadShader->Render( this->m_texPosition.texID );


	}



	void RenderScene::RenderLocalReflection()
	{
		Frustum *pcamera	= m_pSceneGraph->GetCurrentCameraObject()->GetFrustum();

		//==================== Pass1: Create G-Buffer =====================//
		RenderGbuffer();

		//===================== Pass2: Do Lighting ========================//
		RenderDeferred( false );

		//====================== Pass2: Local Reflection ==================//
		LocalReflectionShader	*pLRShader	= (LocalReflectionShader *)m_Shaders[ SHADER_LOCAL_REFLECTION ];

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		pLRShader->Render( m_texPosition.texID, m_texDepth.texID, m_texNormal.texID, m_RawShadedResult.texID );
	}



	void RenderScene::RenderLightShaft()
	{
		
		//==================== Pass1: Render G-Buffer =====================//
		RenderGbuffer();
		
		//====================== Pass2: LightShaft Shader ==================//
		LightShaftShader *pLSShader = (LightShaftShader *)m_Shaders[ SHADER_LIGHT_SHAFT ];
		
		//TODO: 実装. ComputeShaderの動作チェック
		//m_LightAttributeBuffer.
			
		const Vec3f wsLightPos = { 15.0f, 20.0f, 20.0f };
		//pLSShader->RenderSliceEndPoints( wsLightPos, *m_pSceneGraph->GetCurrentCameraObject()->GetMatrix() );

		pLSShader->Render( wsLightPos, 0, *m_pSceneGraph->GetCurrentCameraObject()->GetMatrix() );
		
		
		//====================== Debug: Visualize Texture ==================//
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		QuadShader *pquadshader = (QuadShader *)m_Shaders[ SHADER_QUAD ];

		pquadshader->Render( pLSShader->Get1DMinMaxShadowMap(), 0.1f );// );//pLSShader->GetSliceUvOriginDirection() );//pLSShader->GetEpipolarCoordinate() );//pLSShader->GetEpipolarDepthStencil() );//
		
	}



	void RenderScene::RenderDOF()
	{
		Frustum *pcamera	= m_pSceneGraph->GetCurrentCameraObject()->GetFrustum();

		//==================== Pass1: Create G-Buffer =====================//
		RenderGbuffer();

		//===================== Pass2: Do Shading =================//
		RenderDeferred(false);

		//====================== Pass3: DOF Shader ==================//
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		DOFShader	*pDofShader	= (DOFShader *)m_Shaders[SHADER_DOF];

		pDofShader->Render();

	}



	void RenderScene::RenderHDR()
	{
		//================== Pass1: Create G-Buffer ===============//
		RenderGbuffer();

		//===================== Pass2: Do Shading =================//
		RenderDeferred( false );

		//================ Pass3: HDR PostProcess =================//
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		HDRShader *pHdrShader	= (HDRShader *)m_Shaders[SHADER_HDR];
		pHdrShader->DoHDRLighting( 1.0f, 0.18f, 0.5f );


		// Debug
		//glViewport( 0, 0, m_ScreenSize.x, m_ScreenSize.y );
		//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		//QuadShader *pquadshader = (QuadShader *)m_Shaders[SHADER_QUAD];
		//pquadshader->Render( pHdrShader->GetBlurredImage2()->texID );
	}



	void RenderScene::RenderLightPropagationVolumes()
	{
		CameraObject *pcamObj	= m_pSceneGraph->GetCurrentCameraObject();
		LPVShader *pLPV			= ((LPVShader *)m_Shaders[ SHADER_LPV ]);
		

		pLPV->BindDrawSurface();
		
		// とりあえず光源1個だけ、
		for( int i=0; i<m_numLights[LIGHT_TYPE_SPOT]; ++i )
		{
			LightObject *pLight	= (LightObject *)m_LightSources[ LIGHT_TYPE_SPOT ][i]->GetAttachedObject();

			Vec3f lightColorIntensity;
			Scale( lightColorIntensity, pLight->GetLightAttrib()->Color, pLight->GetLightAttrib()->Intensity );
			
			for( int obj_id=0; obj_id < m_numShadowObjects; ++obj_id )
			{
				GeometryBuffer *pGeom	= m_RenderShadowQueue[obj_id]->GetBuffer<GeometryBuffer>();
				pLPV->Render( pGeom, pcamObj->GetFrustum(), g_TempFrustum, lightColorIntensity );
			}
			
			break;
		}// end of i loop
		
		pLPV->UnbindDrawSurface();

	}


	
	void RenderScene::RenderVoxelConeTracing( bool useIrrad )
	{
		glViewport( 0, 0, m_ScreenSize.x, m_ScreenSize.y );
		
		//================ Draw Gbuffer ===================//
		RenderGbuffer();

		//================= Execute Voxel Cone Tracing ========================//
		VoxelConeTracing *pVCTShader	= (VoxelConeTracing *)m_Shaders[SHADER_VOXEL_CONE_TRACING];
		pVCTShader->TraceVoxel( &m_texPosition, &m_texNormal, &m_IrradianceBuffer );
		
		//============================= Composite ==================================//
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		SimpleDeferredShader *pDeferredShader	= (SimpleDeferredShader *)m_Shaders[SHADER_SIMPLEDEFERRED];
		pDeferredShader->SetIrrasianceMapMode( useIrrad );
		pDeferredShader->Render( &m_texDiffuse, &m_texPosition, &m_texDepth, &m_texNormal, &m_IrradianceBuffer );

		//=================== DEBUG: Preview Irradiance Buffer ======================//
		//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		//((QuadShader *)m_Shaders[SHADER_QUAD])->Render( m_IrradianceBuffer.texID );//m_texDiffuse.texID );//
		

		//================ DEBUG: View Internal 3D Texture Buffers ==================//
		//glClearColor( 0.2f, 0.2f, 0.2f, 0.0f );
		//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		//VoxelShader *pVoxelShader	= (VoxelShader *)m_Shaders[ SHADER_VOXEL ];
		//pVoxelShader->Render( m_VCTVoxelData.GetTexNormal()/*m_VCTVoxelData.GetTexPosition()*//*m_VCTVoxelData.GetTexIrradiance()/*m_VCTVoxelData.GetTexIrradianceAniso(0)*/, 0, m_VCTVoxelData.GetMatCellIdToWorldSpace() );//m_VCTVoxelData.GetTexIrradianceHalfSize() );//

	}



	void RenderScene::RenderVolumeRayCasting()
	{
		VolumeRayCastingShader *pRayCastingShader	= (VolumeRayCastingShader*)m_Shaders[SHADER_VOLUME_RAY_CASTING];

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


		pRayCastingShader->Render( m_VCTVoxelData.GetTexIrradianceAniso( 0 ), m_VCTVoxelData.GetMatVoxelSpace(), m_VCTVoxelData.GetMatInvVoxelSpace() );

	}



	void RenderScene::ViewShadowMaps( int width, int height )
	{
		QuadShaderTexArray	*pQuadShaderTexArray	= (QuadShaderTexArray *)m_Shaders[ SHADER_QUAD_TEXARRAY ];

		glViewport( 0, 0, width, height );
		//glViewport( 0, 0, m_ShadowBuffer.Width(), m_ShadowBuffer.Height() );

		pQuadShaderTexArray->SetMagnitude( 0.1f );
		pQuadShaderTexArray->Render( m_ShadowBuffer.GetTexture(), 2 );
	}



	void RenderScene::ViewReflectiveShadowMaps( int width, int height )
	{
		glViewport( 0, 0, width, height );
		glClear( /*GL_COLOR_BUFFER_BIT |*/ GL_DEPTH_BUFFER_BIT );
		((QuadShader *)m_Shaders[SHADER_QUAD])->Render( m_RsmFluxBuffer.texID );

		glViewport( 0, 0, m_ScreenSize.x, m_ScreenSize.y );
	}



	void RenderScene::Release()
	{
		// Unbind SceneGraph
		UnbindSceneGraph();
		
		// Delete Shader
		IShader::Release();
		for( int i=0; i<m_Shaders.size(); ++i )
		{
			SafeDelete( m_Shaders[i] );
		}
		m_Shaders.clear();
		
		// Delete Render Queue
		for( int i=0; i<MAX_RENDER_QUEUE; ++i )	m_RenderShadowQueue[i] = NULL;

		for( int i=0; i<NUM_SHADER_TYPE; ++i )
		{
			for( int j=0; j<MAX_RENDER_QUEUE; ++j )	m_RenderObjects[i][j]	= NULL;
			m_numRenderObjects[i]	= 0;
		}

		// Delete Lightsource Lists
		for( int i=0; i<NUM_LIGHT_TYPE; ++i )
		{
			for( int j=0; j<MAX_LIGHTS; ++j )	m_LightSources[i][j]	= NULL;
			m_numLights[i]	= 0;
		}
		//for( int i=0; i<MAX_LIGHTS; ++i )
		//	m_LightQueue[i]	= NULL;


		// Delete Gbuffer
m_RTGbuffer.Release();
//		m_AttachmentGbuffer.Clear();
//		m_FboGbuffer.Release();

		m_texPosition.Release();
		m_texNormal.Release();
		m_texDepth.Release();
		m_texDiffuse.Release();
m_RawShadedResult.Release();

		// Delete Irradiance Buffer
		m_IrradianceBuffer.Release();
		m_VCTVoxelData.Release();


		// Delete ReflectiveShadowMaps
		m_RTReflectiveShadowMaps.Release();
		//m_AttachmentRSM.Clear();
		//m_FboRSM.Release();

		// Delete Gbuffer
		m_RsmPositionBuffer.Release();
		m_RsmNormalBuffer.Release();
		m_RsmFluxBuffer.Release();
		m_RsmDepthBuffer.Release();

		// Release OIT Buffers and FBO
		m_RTOIT.Release();
		//m_AttachmentOIT.Clear();
		//m_FboOIT.Release();
		m_OITAccumBuffer.Release();
		m_OITRevealageBuffer.Release();
		
		// Delete Irradiance Volume
		m_LPVIrradianceVolume.Release();

		// Delete SSDO Buffer
		m_SSDOBuffer.Release();

		// Delete ShadowBuffer
		m_ShadowBuffer.Release();

		// Delete GeometryObjects
		for( int i=0; i<MAX_VAOS; ++i )	m_GeometryObjects[i].Release();

		// Delete ScreenSpace Quad Geometry
		m_ScreenSpaceQuad.Release();

		// Delete Unit Cube Geometry
		m_UnitCube.Release();

		// Delete LightAttributeBuffer
		m_LightAttributeBuffer.Release();
		m_ShadowAttachments.Release();

		// DeleteShadowCaster
		m_ShadowCaster.Release();
		m_ShadowAttachments.Release();

		// Delete ViewTransformBuffer
		m_ViewTransformBuffer.Release();

		// Release UBO Bind Points
//GLUniformBufferObject::ReleaseBindPoints();
GLBindPointManager::ReleaseBindPoints();

	}




}// end of namespace
