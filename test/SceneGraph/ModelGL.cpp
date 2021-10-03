#include	"ModelGL.h"

#include	<graphics/gl4x/io/SceneLoader.h>


FbxLoader	g_SceneLoader;


static bool	g_Enable	= false;


ModelGL::ModelGL()
{
#ifdef _DEBUG
	tcout << "ModelGL()..." << endl;
#endif // _DEBUG

	m_pSceneGraph	= NULL;
	m_pRenderScene	= NULL;


}


ModelGL::~ModelGL()
{
#ifdef _DEBUG
tcout << "~ModelGL()..." << endl;
#endif // _DEBUG

	Release();

#if defined( FREEIMAGE_SUPPORT )

	DeinitFreeImage();

#endif

}


void ModelGL::Init()
{

#if defined( FREEIMAGE_SUPPORT )
	InitFreeImage();

#elif defined( RESIL_SUPPORT )
	InitIL();

#endif


	//============ Init SceneGraph ============//
	m_pSceneGraph	= new SceneGraph();

	//######## 親子関係にあるオブジェクトのトランスフォーム伝播実験用コード 2013.07.06 #########//
	MeshObject	*pMesh = NULL;
	MeshObject	*pMesh2 = NULL;
	//##########################################################################################//

	g_SceneLoader.Init();
	g_SceneLoader.Load( "V:/\sample3dmodels/fbx/testscene.fbx" );
	//g_SceneLoader.GetGeometry(m_pSceneGraph);
	g_SceneLoader.ScanGeometry(m_pSceneGraph);

//	MeshObject *pMesh3 = m_pSceneGraph->GetCurrentMeshObject();
//
//for( int i=0; i<pMesh3->m_pIndexBuffer->numElms; ++i )
//	tcout << *pMesh3->m_pIndexBuffer->Index(i) << endl;


/*
	for(int i=0; i<1; ++i)
	{
	for(int j=0; j<1; ++j)
	{
	for(int k=0; k<5; ++k)
	{
	
	VertexBuffer	vb;
	IndexBuffer		ib;
	MaterialBuffer	mb;
	TextureBuffer	tb;

	VertexLayout	vertices[] =
	{
		{ {-1.0f, -1.0f, -1.0f}, {0,0,0},	{0,0,-1}, {0.5, 1.0, 1.0} },
		{ {1.0f, -1.0f, -1.0f}, {1,0,0},	{0,0,-1}, {0.5, 1.0, 1.0} },
		{ {1.0f, 1.0f, -1.0f},	{1,1,0},	{0,0,-1}, {0.5, 1.0, 1.0} },
		{ {-1.0f, 1.0f, -1.0f},	{0,1,0},	{0,0,-1}, {0.5, 1.0, 1.0} },
		
		{ {1.0f, -1.0f, -1.0f}, {1,0,0},	{1,0,0}, {0.5, 1.0, 1.0} },
		{ {1.0f, -1.0f, 1.0f},	{1,0,1},	{1,0,0}, {0.5, 1.0, 1.0} },
		{ {1.0f, 1.0f, 1.0f},	{1,1,1},	{1,0,0}, {0.5, 1.0, 1.0} },
		{ {1.0f, 1.0f, -1.0f},	{1,1,0},	{1,0,0}, {0.5, 1.0, 1.0} },
		
		{ {1.0f, -1.0f, 1.0f},	{1,0,1},	{0,0,1}, {0.5, 1.0, 1.0} },
		{ {-1.0f, -1.0f, 1.0f},	{0,0,1},	{0,0,1}, {0.5, 1.0, 1.0} },
		{ {-1.0f, 1.0f, 1.0f},	{0,1,1},	{0,0,1}, {0.5, 1.0, 1.0} },
		{ {1.0f, 1.0f, 1.0f},	{1,1,1},	{0,0,1}, {0.5, 1.0, 1.0} },
	
		{ {-1.0f, -1.0f, 1.0f},	{0,0,1},	{-1,0,0}, {0.5, 1.0, 1.0} },
		{ {-1.0f, -1.0f, -1.0f},{0,0,0},	{-1,0,0}, {0.5, 1.0, 1.0} },
		{ {-1.0f, 1.0f, -1.0f},	{0,1,0},	{-1,0,0}, {0.5, 1.0, 1.0} },
		{ {-1.0f, 1.0f, 1.0f},	{0,1,1},	{-1,0,0}, {0.5, 1.0, 1.0} },

		{ {-1.0f, 1.0f, -1.0f},	{0,1,0},	{0,1,0}, {0.5, 1.0, 1.0} },
		{ {1.0f, 1.0f, -1.0f},	{1,1,0},	{0,1,0}, {0.5, 1.0, 1.0} },
		{ {1.0f, 1.0f, 1.0f},	{1,1,1},	{0,1,0}, {0.5, 1.0, 1.0} },
		{ {-1.0f, 1.0f, 1.0f},	{0,1,1},	{0,1,0}, {0.5, 1.0, 1.0} },

		{ {-1.0f, -1.0f, -1.0f},{0,0,0},	{0,-1,0}, {0.5, 1.0, 1.0} },
		{ {-1.0f, -1.0f, 1.0f},	{0,0,1},	{0,-1,0}, {0.5, 1.0, 1.0} },
		{ {1.0f, -1.0f, 1.0f},	{1,0,1},	{0,-1,0}, {0.5, 1.0, 1.0} },
		{ {1.0f, -1.0f, -1.0f},	{1,0,0},	{0,-1,0}, {0.5, 1.0, 1.0} },
	};

	int	indices[] =
	{
		0,2,1, 0,3,2,
		5,4,6, 4,7,6,
		9,8,10, 8,11,10,
		13,12,14, 12,15,14,
		17,16,18, 16,19,18,
		21,20,22, 20,23,22,
	};

	// 属性情報の初期化
	MeshAttribute attrib = { SHADER_SIMPLEMESH, VISIBLE, CAST_SHADOW_ON, RECIEVE_SHADOW_ON };

	// 頂点バッファの初期化
	AllocateVertexBuffer( &vb, 24, sizeof(VertexLayout), vertices );

	// インデックスバッファの初期化
	AllocateIndexBuffer( &ib, 36, 3, indices );
	
	// マテリアルバッファの初期化
	AllocateMaterialBuffer( &mb, 2, MTL_COMMON );
	
	InitVec( *mb.Ambient(0), 0.15f, 0.25f, 0.15f );
	InitVec( *mb.Diffuse(0), float(i)*0.1f, float(j)*0.1f, float(k)*0.1f );
	InitVec( *mb.Specular(0), 0.5f, 0.5f, 0.2f );
	InitVec( *mb.Transparency(0), 0.0f, 0.0f, 0.0f );
	*mb.Shininess(0)		= 50.0f;

	// テクスチャバッファの初期化
	AllocateTextureBuffer( &tb, 1 );

	TransformAttribute	trans_attrib = {0};
	InitVec( trans_attrib.Translation, 1.0f*float(i), 1.0f*float(j), 10.0f*float(k) );
	InitVec( trans_attrib.Scale, 0.85f, 0.85f, 0.85f);//0.25f, 0.25f, 0.25f );
	//######## 親子関係にあるオブジェクトのトランスフォーム伝播実験用コード 2013.07.06 #########//
	//InitVec( trans_attrib.Translation, float(i), float(j), 4.0f*float(k) );
	//if(k==0)
	//	InitVec( trans_attrib.Scale, 1.0f, 1.0f, 1.0f );
	//else
	//	InitVec( trans_attrib.Scale, 0.5f, 1.0f, 1.0f );// 親に対する相対スケール
	//##########################################################################################//
	QuatIdentity( trans_attrib.Quaternion );
	MatIdentity( trans_attrib.Rotation );


	if(i==0 && j==0 && k==0)
		m_pSceneGraph->AddMesh( &attrib, &vb, &ib, &mb, &tb, &trans_attrib );	// メッシュオブジェクト追加
//	else
		//m_pSceneGraph->AddMeshInstance( &attrib, &vb, &ib, &mb, &tb, &trans_attrib );//m_pSceneGraph->AddChildMesh( &attrib, &vb, &ib, &mb, &tb, &trans_attrib );//	// メッシュインスタンス追加
//		m_pSceneGraph->AddChildMesh( &attrib, &vb, &ib, &mb, &tb, &trans_attrib );
	//Vec3f scale = { 2.0f, 1.0f, 1.0f };//{ 6.0f - (float)k, 6.0f - (float)k, 0.4f };//Vec3f scale = { 0.4f, 0.4f, 0.4f };//
	//Vec3f trans	= { i==0 ? 0.0f : float(i), float(j), 4.0f*float(k) };
	//m_pSceneGraph->GetCurrentMesh()->SetScale( scale );
	//m_pSceneGraph->GetCurrentMesh()->SetTranslation( trans );

	
	ReleaseVertexBuffer( &vb );
	ReleaseIndexBuffer( &ib );
	ReleaseMaterialBuffer( &mb );
	ReleaseTextureBuffer( &tb );

	if(k==1)	pMesh	= m_pSceneGraph->GetCurrentMeshObject();

	if(k==3)	pMesh2	= m_pSceneGraph->GetCurrentMeshObject();

	}
	}
	}
	*/


	//######## 親子関係にあるオブジェクトのトランスフォーム伝播実験用コード 2013.07.06 #########//
	pMesh	= m_pSceneGraph->GetCurrentMeshObject();
	Vec3f trans	= { 5.0f, 5.0f, 5.0f };
	Vec3f scale = { 1.0f, 1.0f, 1.0f };
	//pMesh->SetScale( scale );	// 注意: 親に対する相対スケール入力
	//pMesh->SetTranslation( trans );

	//InitVec(scale, 2.0f, 1.0f, 0.5f);
	//pMesh2->SetScale( scale );
	//##########################################################################################//



	//##########################################################################################//
	//									 Create Lightsource										//
	//##########################################################################################//
	LightAttribute	lgt_attr;
	ZeroMemory( &lgt_attr, sizeof(LightAttribute) );
	lgt_attr.LightType		= LIGHT_TYPE::LIGHT_TYPE_SPOT;//LIGHT_TYPE::LIGHT_TYPE_DIRECTIONAL;//
	lgt_attr.Enable			= true;
	lgt_attr.EmitDiffuse	= true;
	lgt_attr.EmitSpecular	= true;
	lgt_attr.CastShadow		= true;
	InitVec( lgt_attr.Color, 1.0f, 1.0f, 1.0f );
	lgt_attr.Intensity		= 1.0f;
	//Vec3f dir			= { -1.0, -1.0, -1.0 };
	//Vec3f up			= { 0.0, 1.0, 0.0 };
	Vec3f color		= { 1.0f, 1.0f, 1.0f };

	

	m_pSceneGraph->AddLight( &lgt_attr );

	LightObject *pLightObj	= m_pSceneGraph->GetCurrentLightObject();

	pLightObj->SetColor( color );
	pLightObj->SetIntensity( 1.5f );// 1.5f
	//pLightObj->SetDirection( dir, up );
	pLightObj->SetViewParams( -15.0f, 15.0f, -15.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f );
	pLightObj->SetConeAngle( float(M_PI_2) );
	pLightObj->SetRadius( 1000.0f );
	pLightObj->SetNear( 0.01f );
	//pLightObj->Update();


	//##########################################################################################//
	//									 Create Camera											//
	//##########################################################################################//
	m_pSceneGraph->AddCamera( NULL );

	CameraObject *pCameraObj	= m_pSceneGraph->GetCurrentCameraObject();
	pCameraObj->SetZNear( 0.001f );
	pCameraObj->SetZFar( 1000.0f );
	m_pSceneGraph->GetCurrentCameraObject()->Update();

	
	//============ Init RenderScene ===========//
	m_pRenderScene	= new RenderScene();
	m_pRenderScene->BindSceneGraph( m_pSceneGraph );
	m_pRenderScene->InitShadowBuffer( SHADOWMAP_SIZE, SHADOWMAP_SIZE, MAX_SHADOW_MAPS );
	//m_pRenderScene->InitIrradianceVolume( 48, 48, 48, 1, 1.0f );
	m_pRenderScene->InitVCTVoxelData( 96, 96, 96, 8, 1.0f );
	m_pRenderScene->InitRSMBuffer( /*2048, 2048*/SHADOWMAP_SIZE, SHADOWMAP_SIZE );


	//m_pRenderScene->UpdateIrradianceVolume( true );

}



void ModelGL::Release()
{
	SafeDelete( m_pSceneGraph );
	SafeDelete( m_pRenderScene );

	g_SceneLoader.Release();
}



void ModelGL::ExecuteKeyDownEvent( int key )
{
	switch(key)
	{
		case 'P':
		{
			//=================== カメラ位置に光源を配置する ======================//
			LightObject *pLightObj	= m_pSceneGraph->GetCurrentLightObject();

			if( pLightObj )
			{
				const Frustum *pcamFrustum	= m_pSceneGraph->GetCurrentCameraObject()->GetFrustum();
				pLightObj->SetViewParams( *pcamFrustum->GetPosition(), *pcamFrustum->GetDirection(), *pcamFrustum->GetVertical(), *pcamFrustum->GetHorizontal() );
				
				//const CameraObject *pCameraObj	= m_pSceneGraph->GetCurrentCameraObject();
				//pCameraObj->
				//pLightObj->SetViewParams( *pcamFrustum->GetPosition(), *pcamFrustum->GetDirection(), *pcamFrustum->GetVertical(), *pcamFrustum->GetHorizontal() );
			}
			break;
		}
		default:
		{
			break;
		}
	}// end of switch
}



void ModelGL::ExecuteKeyUpEvent( int key )
{
	switch(key)
	{
		case 'P':
		{
			//========== IrradianceVolumeアップデートフラグをセットする ===========//
			m_pRenderScene->SetIrradianceVolumeUpdateFrag();
			break;
		}
		case 'L':
		{
			g_Enable	= !g_Enable;
			break;
		}
		default:
		{
			break;
		}
	}// end of switch
}



void ModelGL::ExecuteKeyPressEvent( int numkeys, const bool keys[] )
{
	const float deltaTranslation	= 0.1f;//0.01f;
	const float deltaRotation		= 0.01f;//0.001f;

	for(int	i=0; i<numkeys; i++)
	{
		if( keys[i]==false ) continue;

		switch(i)
		{
		case 'W':{	TranslateCamera( +deltaTranslation, 0, 0 );	break; }
		case 'S':{	TranslateCamera( -deltaTranslation, 0, 0 );	break; }
		case 'A':{	TranslateCamera( 0, +deltaTranslation, 0 );	break; }
		case 'D':{ TranslateCamera( 0, -deltaTranslation, 0 );	break; }
		case 'R':{	TranslateCamera( 0, 0, +deltaTranslation );	break; }
		case 'F':{	TranslateCamera( 0, 0, -deltaTranslation );	break; }
		case 'Q':{	RollCamera( -deltaRotation );			break; }
		case 'E':{	RollCamera( +deltaRotation );			break; }
		default :{	break; }
		}// end of switch
		
	}// end of i loop

}



void ModelGL::InitCamera(const Vec3f &pos, const Vec3f &dir, const Vec3f &up, float aspect_ratio, float fovy, float znear, float zfar)
{
	m_pSceneGraph->GetCurrentCameraObject()->SetViewParams( pos, dir, up );
	m_pSceneGraph->GetCurrentCameraObject()->SetProjectionParams( fovy, aspect_ratio, znear, zfar );
}


void ModelGL::SetCameraView(const Vec3f &pos, const Vec3f &dir, const Vec3f &up)
{
	m_pSceneGraph->GetCurrentCameraObject()->SetViewParams( pos, dir, up );
}


void ModelGL::SetCameraProjection( float aspect_ratio, float fovy, float znear, float zfar )
{
	m_pSceneGraph->GetCurrentCameraObject()->SetProjectionParams( fovy, aspect_ratio, znear, zfar );
}


void ModelGL::Draw()
{

	//=============== Update  ================//
	m_pSceneGraph->Update();
	m_pRenderScene->Update();


	const Vec4f color ={ 0.5f, 0.8f, 0.6f, 0.0f };
	//=============== Render =================//	
	//m_pRenderScene->RenderForward();	// シーングラフを描画する
	//m_pRenderScene->RenderConstant( &color );
	//m_pRenderScene->RenderTransparent();
	//m_pRenderScene->RenderZPrepass();// Zプリパスレンダリング. オブジェクトが沢山重なっている && シェーダーが重い場合は効果あり。ポリゴン数多い場合は重くなる
	//m_pRenderScene->RenderAntialias( g_Enable );
	//m_pRenderScene->RenderSSS();
	//m_pRenderScene->RenderLightShaft();
	//m_pRenderScene->RenderDOF();
	//m_pRenderScene->RenderHDR();
	//m_pRenderScene->RenderSSDO();
	//m_pRenderScene->RenderLocalReflection();
	//m_pRenderScene->RenderQuad();
	//m_pRenderScene->RenderLightPropagationVolumes();
	m_pRenderScene->RenderVoxelConeTracing( g_Enable );
	//m_pRenderScene->RenderVolumeRayCasting();


//m_pRenderScene->ViewShadowMaps( 512, 512 );
//m_pRenderScene->ViewReflectiveShadowMaps( 512, 512 );


}