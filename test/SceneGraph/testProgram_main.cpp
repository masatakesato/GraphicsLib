#include	<graphics/gl4x/scene/RenderScene.h>
using namespace OreOreLib;



SceneGraph	*m_pSceneGraph = NULL;



int main()
{
	glewInit();

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);


	m_pSceneGraph = new SceneGraph();


	for(int i=0; i<1; ++i)
	{
	for(int j=0; j<1; ++j)
	{
	for(int k=0; k<1; ++k)
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
	MeshAttribute attrib = { SHD_SIMPLEMESH, VISIBLE, CAST_SHADOW_ON, RECEIVE_SHADOW_ON };

	// 頂点バッファの初期化
	AllocateVertexBuffer( &vb, 24, sizeof(VertexLayout), vertices );

	// インデックスバッファの初期化
	AllocateIndexBuffer( &ib, 36, 3, indices );
	
	// マテリアルバッファの初期化
	AllocateMaterialBuffer( &mb, 2, MTL_COMMON );
	
	InitVec( *mb.Ambient(1), 0.15f, 0.25f, 0.15f );
	InitVec( *mb.Diffuse(1), float(i)*0.1f, float(j)*0.1f, float(k)*0.1f );
	InitVec( *mb.Specular(1), 0.5f, 0.5f, 0.2f );
	*mb.Transparency(1)	= 0.0f;
	*mb.Shininess(1)		= 50.0f;

	
	// テクスチャバッファの初期化
	AllocateTextureBuffer( &tb, 1 );

	if(i==0 && j==0 && k==0)
		m_pSceneGraph->AddMesh( &attrib, &vb, &ib, &mb, &tb, NULL );	// メッシュオブジェクト追加
	else
		m_pSceneGraph->AddMeshInstance( &attrib, &vb, &ib, &mb, &tb, NULL );	// メッシュインスタンス追加

	Vec3f scale = { 0.4f, 0.4f, 0.4f };
	Vec3f trans	= { float(i), float(j), float(k) };
	m_pSceneGraph->GetCurrentMesh()->SetScale( scale );
	m_pSceneGraph->GetCurrentMesh()->SetTranslation( trans );

	
	ReleaseVertexBuffer( &vb );
	ReleaseIndexBuffer( &ib );
	ReleaseMaterialBuffer( &mb );
	ReleaseTextureBuffer( &tb );

	}
	}
	}



	//##########################################################################################//
	//									 Create Lightsource										//
	//##########################################################################################//
	LightAttribute	lgt_attr = {0};
	lgt_attr.LightType	= LGT_DIRECTIONAL;
	lgt_attr.Enable		= true;
	lgt_attr.CastShadow	= true;

	Vec3f dir			= { 1.0, 2.0, 3.0 };
	Vec3f indensity		= {1.0f, 1.0f, 1.0f };

	m_pSceneGraph->AddLight( &lgt_attr, NULL );
	m_pSceneGraph->GetCurrentLight()->SetIntensity( indensity );
	m_pSceneGraph->GetCurrentLight()->SetDirection( dir );
	m_pSceneGraph->GetCurrentLight()->Update();

	//##########################################################################################//
	//									 Create Camera											//
	//##########################################################################################//
	m_pSceneGraph->AddCamera( NULL );




	SafeDelete( m_pSceneGraph );



	return 0;
}