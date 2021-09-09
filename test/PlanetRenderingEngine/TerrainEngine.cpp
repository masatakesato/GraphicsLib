 #include	"TerrainEngine.h"

#include	<math.h>

#include	<graphics/gl4x/resource/Texture.h>
using namespace OreOreLib;



// 中点を計算する
#define MID_POINT(dest, v1, v2)				\
	      dest.x = 0.5f * (v1.x + v2.x);	\
	      dest.y = 0.5f * (v1.y + v2.y);	\
	      dest.z = 0.5f * (v1.z + v2.z);

// 点cから線分abまでの最短距離を計算する
inline float DIST_POINT_TO_SEGMENT(Vec3f &a, Vec3f &b, Vec3f &c)
{
	Vec3f ab, ac, bc;
	Subtract(ab, b, a);
	Subtract(ac, c, a);
	Subtract(bc, c, b);
	
	float	e = DotProduct(ac, ab);
	if(e <= 0.0f) return Length(ac);

	float	f = LengthSqrd(ab);
	if(e >= f) return Length(bc);
	
	return sqrt(LengthSqrd(ac) - e * e / f);
}



// 半径1の球に内接する立方体
static const Vec3f CubeVertices[] = 
{
	{-1.0f, -1.0f, -1.0f},	//0: -x-y-z
	{-1.0f, -1.0f, +1.0f},	//1: -x-y+z
	{-1.0f, +1.0f, -1.0f},	//2: -x+y-z
	{-1.0f, +1.0f, +1.0f},	//3: -x+y+z
	{+1.0f, -1.0f, -1.0f},	//4: +x-y-z
	{+1.0f, -1.0f, +1.0f},	//5: +x-y+z
	{+1.0f, +1.0f, -1.0f},	//6: +x+y-z
	{+1.0f, +1.0f, +1.0f},	//7: +x+y+z
};

static const int CubeFaces[] =
{
	2,3,1,0,// 側面時計回り
	6,7,3,2,// 側面時計回り
	4,5,7,6,// 側面時計回り
	0,1,5,4,// 側面時計回り
	1,3,7,5,// 上面時計回り
	0,4,6,2,// 底面時計回り
};


// デフォルトコンストラクタ
TerrainEngine::TerrainEngine()
{
	InitVec(m_Position, 0.0f, 0.0f, 0.0f);
	InitVec(m_Orientation[0], 1.0f, 0.0f, 0.0f);
	InitVec(m_Orientation[1], 0.0f, 1.0f, 0.0f);
	InitVec(m_Orientation[2], 0.0f, 0.0f, 1.0f);
	m_Radius				= 0.0f;
	m_HeightRange			= 0.0f;
	m_MinimumViewDistance	= 0.0f;
	m_SandScaleTexture		= NULL;
	m_TerrainMaterial		= NULL;
	m_pGraphicsDevice		= NULL;
	m_HeightmapGenerator	= NULL;
	m_Rho					= 0.0f;
	m_MaxLevel				= 0;
	m_Dimension				= 0.0f;
	m_numMeshRes			= 0;
	m_MeshObj				= NULL;
	m_Frustum				= NULL;
	m_FixFrustum			= false;
	m_pVirtualTexture		= NULL;
}


// デストラクタ
TerrainEngine::~TerrainEngine()
{
tcout << _T("TerrainEngine::~TerrainEngine()...") << tendl;

	m_pVirtualTexture	= NULL;

	//================ メッシュデータ削除 =============//
	if(m_MeshObj){ delete m_MeshObj; m_MeshObj = NULL; }
	
	//================ 地表マテリアル削除 =============//
	if(m_TerrainMaterial){ delete m_TerrainMaterial; m_TerrainMaterial = NULL; }

	//======== ハイトマップ生成オブジェクト削除 =======//
	if(m_HeightmapGenerator){ delete m_HeightmapGenerator; m_HeightmapGenerator = NULL; }


	//============= ビューフラスタム削除 =============//
	if(m_Frustum){ delete m_Frustum; m_Frustum = NULL; }

	//================ テクスチャ削除 ================//
	if(m_SandScaleTexture && m_pGraphicsDevice)
	{
		m_pGraphicsDevice->Begin();
		glDeleteTextures(1, &m_SandScaleTexture);
		m_pGraphicsDevice->End();

		m_SandScaleTexture = NULL;
		m_pGraphicsDevice = NULL;
	}
	
	//================= シェーダの削除 ===============//
	cgDestroyContext(m_CgContext);
	cgDestroyEffect(m_CgEffect);
}



void TerrainEngine::Init(Vec3f pos, float r, float heightrange, int numLv, float rho)
{
tcout << _T("TerrainEngine::Init()...") << tendl;

	m_Position	= pos;
	m_Radius	= r;
	m_HeightRange= heightrange;
	m_MinimumViewDistance = sqrt( pow(m_Radius+m_HeightRange, 2) - m_Radius*m_Radius );// 水平線までの距離（カメラが地表にある時）

	m_Rho		= rho;
	m_MaxLevel	= numLv-1;

	m_Frustum = new ViewFrustum();

	//============= ６面体ノードを作る =============//
	m_Dimension = 2.0f * r;
	GeneratePatches();

	for(int i=0; i<6; i++)	m_TableID[i] = i;

	m_pGraphicsDevice->Begin();

	//========== 地層テクスチャをロードする ========//
	float *img;
	unsigned int w,h;
	
	img = Bitmap_load_texture("../../test/PlanetRenderingEngine/Texture/sandcolorscale.bmp", w,h);
	m_SandScaleTexture	= AllocateTexture1D(GL_LINEAR, GL_LINEAR, (int)w, GL_RGB8, GL_RGB, GL_FLOAT, img);

	//============= Cgエフェクトをロードする ==========//
	LoadCgEffect();

	m_pGraphicsDevice->End();
}


void TerrainEngine::InitHeightmapGenerator(int mapsize, float border, float heightrange, float scale_horizontal, float scale_height)
{
tcout << _T("TerrainEngine::InitHeightmapGenerator()...") << tendl;

	m_pGraphicsDevice->Begin();

	m_HeightmapGenerator = new HeightmapGenerator(mapsize, border, heightrange);
	m_HeightmapGenerator->SetScale(scale_horizontal, scale_height);

	m_pGraphicsDevice->End();
}


void TerrainEngine::InitMeshObject(int numdiv, int numlv)
{
tcout << _T("TerrainEngine::InitMeshObject()...") << tendl;

	m_pGraphicsDevice->Begin();

	m_numMeshRes = numlv;
	m_MeshObj = new MeshGenerator(numdiv, m_numMeshRes);

	m_pGraphicsDevice->End();
}


void TerrainEngine::InitSurfaceProperties(	int nummats,
										const char **name, const Vec3f ambient[], const Vec3f diffuse[],
										const Vec3f specular[], const float intensity[], const float power[], 
										const Vec3f distribution[], const float slope[])
{
tcout << _T("TerrainEngine::InitSurfaceProperties()...") << tendl;
	int i;
	CGparameter mat_element;

	m_TerrainMaterial = new TerrainMaterial(nummats);
	
	cgSetParameter1i(cgGetNamedEffectParameter(m_CgEffect, "g_numMats"), m_TerrainMaterial->numMaterials());

	for(i=0; i<nummats; i++)
	{
		m_TerrainMaterial->SetSurfaceProperties(i, ambient[i], diffuse[i], specular[i], intensity[i], power[i], distribution[i], slope[i]);

		SurfaceProperty &sp = m_TerrainMaterial->GetSurfaceProperty(i);
		
		mat_element = cgGetArrayParameter(m_epSurfaceProperty, i);
		cgSetParameter4f( cgGetNamedStructParameter(mat_element, "Diffuse"), sp.GetDiffuse()->x, sp.GetDiffuse()->y, sp.GetDiffuse()->z, sp.GetSpecularIntensity() );
		cgSetParameter4f( cgGetNamedStructParameter(mat_element, "Specular"), sp.GetSpecular()->x, sp.GetSpecular()->y, sp.GetSpecular()->z, sp.GetSpecularPower() );
		cgSetParameter3fv( cgGetNamedStructParameter(mat_element, "Distribution"), sp.GetDistribution().xyz );
		cgSetParameter1f( cgGetNamedStructParameter(mat_element, "Slope"), sp.GetSlopeParameter());
	}

}




void TerrainEngine::AttachGraphicsDevice(GraphicsDevice *pgd)
{
tcout << _T("TerrainEngine::AttachGraphicsDevice()...") << tendl;
	m_pGraphicsDevice = pgd;
}


void TerrainEngine::SetHeightRange(float heightrange)
{
	m_HeightRange = heightrange;
}

void TerrainEngine::SetHeightmapScale(float scale_horizontal, float scale_vertical)
{
	m_HeightmapGenerator->SetScale(scale_horizontal, scale_vertical);
}


void TerrainEngine::SetRadius(float radius)
{
	m_Radius = radius;
}

void TerrainEngine::SetMaximumLevel(int maxLv)
{
	m_MaxLevel = maxLv;
}

void TerrainEngine::SetRho(float rho)
{
	m_Rho = rho;
}


void TerrainEngine::SetDistribution(int i, const Vec3f &distribution)
{
	CGparameter	mat_element;

	m_TerrainMaterial->SetDistribution(i, distribution);
	mat_element = cgGetArrayParameter(m_epSurfaceProperty, i);
	cgSetParameter3fv(cgGetNamedStructParameter(mat_element, "Distribution"), distribution.xyz);

}


void TerrainEngine::SetSlope(int i, const float slope)
{
	CGparameter	mat_element;

	m_TerrainMaterial->SetSlopeParam(i, slope);
	mat_element = cgGetArrayParameter(m_epSurfaceProperty, i);
	cgSetParameter1f(cgGetNamedStructParameter(mat_element, "Slope"), slope);
}


void TerrainEngine::AttachVirtualTexture(VirtualTexture *vt)
{
tcout << _T("TerrainEngine::AttachVirtualTexture()...") << tendl;
	int	i;
	
	//==== VirtualTextureにハイトマップ生成関数を登録する ====//
	vt->AttachNoiseFunc(m_HeightmapGenerator);
	
	//===== VirtualTextureにページテーブルを登録する=======//
	for(i=0; i<6; i++)
		m_TableID[i] = vt->RegisterPageTables(m_MaxLevel+1);// 登録時に発行されたIDをTerrainEngineクラスが保持する

	m_pVirtualTexture = vt;// VirtualTextureオブジェクトを参照する
	UpdateTextureParameter();
}


void TerrainEngine::UpdateTextureParameter()
{
tcout << _T("TerrainEngine::UpdateTextureParameter()...") << tendl;

	int		tile_size;
	float	cache_size[4];

	if(!m_pVirtualTexture || !m_HeightmapGenerator)	return;

	m_pGraphicsDevice->Begin();

	//============= シェーダ変数を設定する ==============//
	tile_size = m_pVirtualTexture->GetTileSize();
	m_HeightmapGenerator->SetMapSize(tile_size);
	m_pVirtualTexture->GetCacheSize(cache_size);

	cgSetParameter1i(m_epTileSize, tile_size);// タイル解像度
	cgSetParameter4fv(cgGetNamedEffectParameter(m_CgEffect, "g_CacheSize"), cache_size);// TileCacheの解像度
	cgGLSetTextureParameter(m_epHeightMap, m_pVirtualTexture->GetTileCache() );			// タイルキャッシュID（高さマップ）とシェーダ変数を対応付け
	cgGLSetTextureParameter(m_epNormalMap, m_pVirtualTexture->GetTileCache_normal() );	// タイルキャッシュID（法線マップ）とシェーダ変数を対応付け

	m_pGraphicsDevice->End();
}


void TerrainEngine::FixFrustum(bool flag)
{
	m_FixFrustum = flag;
}




void TerrainEngine::DrawGbuffer(const Camera &camera, unsigned int polygonmode)
{
	Mat4f matView, matProj, matViewProj;
	static Vec3f	posFrustum;
	static Mat4f matViewFrustum;
	
	Vec3f	eyepos = *camera.GetPosition();
	


	//===================== ビューフラスタム設定 =======================//
	if(m_FixFrustum==false)
	{
		posFrustum		= eyepos;
		matViewFrustum	= *camera.GetViewProjectionMatrix();	
	}
	m_Frustum->CalculateFrustum( matViewFrustum );

	//===================== カメラ設定 =================================//
	matProj = *camera.GetProjectionMatrix();
	matView = *camera.GetViewTransformationMatrix();
//	matView.m03 = 0.0f;	matView.m13 = 0.0f;	matView.m23 = 0.0f;// カメラ原点に行列を変更する
	Multiply(matViewProj, matProj, matView);

	//===================== Cgパラメータ設定設定 =======================//
	cgSetParameter3fv(m_epEyePos, eyepos.xyz);		// カメラの位置
	cgSetParameter1f(m_epFarClip, camera.GetFarClip());
	cgSetParameter1f(m_epHeightRange, m_HeightRange);// ハイトマップの幅
	cgSetParameter1f(m_epRho, m_Rho);	// テッセレーションの感度
	cgSetMatrixParameterfr(m_epModelViewMatrix, &matViewProj.m[0] );// モデルビュー変換行列

	//========================== レンダリング ==========================//
	glPolygonMode(GL_FRONT, polygonmode);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cgSetPassState(m_passRenderTerrain);

	m_MeshObj->Bind();// メッシュインスタンスをバインドする

	for(int i=0; i<6; i++)
	{	
		cgSetParameter3fv(m_epUvec, m_Maps[i].m_UVN[0].xyz);//タイルU軸ベクトル
		cgSetParameter3fv(m_epVvec, m_Maps[i].m_UVN[1].xyz);//タイルV軸ベクトル
		m_currTable = m_TableID[i];
		SubDivide_Spherical(/*eyepos*/posFrustum, 0, 0, 0);
	}

	m_MeshObj->Unbind();// メッシュインスタンスをアンバインドする

	cgResetPassState(m_passRenderTerrain);

	glPolygonMode(GL_FRONT, GL_FILL);
}


// 立方体を作成する（半径rの球に外接する立方体）
void TerrainEngine::GeneratePatches()
{
tcout << _T("TerrainEngine::GeneratePatches()...") << tendl;
	int i;
	Vec3f		v0, v1, v2, v3;// パッチ頂点左下，パッチ頂点左上，パッチ頂点右上，パッチ頂点右下
	Vec3f	du, dv, dn;
	
	// 6面体の各面のパッチを作成する
	for(i=0; i<6; i++)
	{
		// パッチの頂点を取得する
		AddScaled(v0, m_Position, m_Radius, CubeVertices[ CubeFaces[i*4]   ] );
		AddScaled(v1, m_Position, m_Radius, CubeVertices[ CubeFaces[i*4+1] ] );
		AddScaled(v2, m_Position, m_Radius, CubeVertices[ CubeFaces[i*4+2] ] );
		AddScaled(v3, m_Position, m_Radius, CubeVertices[ CubeFaces[i*4+3] ] );

		Subtract(du, v3, v0);
		Subtract(dv, v1, v0);
		CrossProduct(dn, du, dv);

		// ローカル座標軸を求める
		Normalize(du);
		Normalize(dv);
		Normalize(dn);

		m_Maps[i].m_Vertices[0] = v0;
		m_Maps[i].m_Vertices[1] = v1;
		m_Maps[i].m_Vertices[2] = v2;
		m_Maps[i].m_Vertices[3] = v3;
		m_Maps[i].m_UVN[0] = du;
		m_Maps[i].m_UVN[1] = dv;
		m_Maps[i].m_UVN[2] = dn;

	}// end of i loop

}



Vec3f TerrainEngine::MapToSphere(Vec3f In)
{
	Vec3f Out;
	
	Scale( In, 1.0f/m_Radius );
	Out.x = m_Radius * In.x * sqrt(1.0f - 0.5f*In.y*In.y - 0.5f*In.z*In.z + In.y*In.y*In.z*In.z / 3.f);
	Out.y = m_Radius * In.y * sqrt(1.0f - 0.5f*In.z*In.z - 0.5f*In.x*In.x + In.x*In.x*In.z*In.z / 3.f);
	Out.z = m_Radius * In.z * sqrt(1.0f - 0.5f*In.x*In.x - 0.5f*In.y*In.y + In.x*In.x*In.y*In.y / 3.f);

	return Out;
}



void TerrainEngine::SubDivide_Spherical(Vec3f eyepos, int tile_x, int tile_y, int miplv )
{
	//int i;
	int		jump = (int)pow(2.0f, m_MaxLevel - (miplv+1));// タイルインデックスのジャンプ幅
	float	l = m_Dimension * pow(2.0f, -miplv);	// パッチのスケール
	float	eps[4];
	float	eps_dash;
	
	Vec2f	vtex_tc;
	Vec3f du, dv, dn;
	Vec3f	center,	v0, v1, v2, v3,	mid_01, mid_12, mid_23, mid_30;// タイル頂点
	Vec3f	sph_center,	sph_v0,	sph_v1,	sph_v2,	sph_v3, sph_m01, sph_m12, sph_m23, sph_m30;// 球面上にマッピングしたタイル頂点

	//======================= 頂点座標のを計算 ==============================//
	InitVec(vtex_tc, float(tile_x), float(tile_y));
	Scale(vtex_tc, 1.0f/pow(2.0f, m_MaxLevel) );
	Scale(du, m_Maps[m_currTable].m_UVN[0], l);
	Scale(dv, m_Maps[m_currTable].m_UVN[1], l);
	
	AddScaled(v0, vtex_tc.x*m_Dimension, m_Maps[m_currTable].m_UVN[0], vtex_tc.y*m_Dimension, m_Maps[m_currTable].m_UVN[1]);
	Add(v0, v0, m_Maps[m_currTable].m_Vertices[0]);
	Add(v1, v0, dv);
	Add(v2, v1, du);
	Add(v3, v0, du);
	
	//============================ 中点座標の計算 ============================//
	AddScaled(center, 0.5f, du, 0.5f, dv);
	Add(center, center, v0);
	MID_POINT(mid_01, v0, v1);
	MID_POINT(mid_12, v1, v2);
	MID_POINT(mid_23, v2, v3);
	MID_POINT(mid_30, v3, v0);

	//=========== 球面上にマッピングした各頂点（中点含む）座標を計算 =========//
	sph_v0 = MapToSphere(v0);
	sph_v1 = MapToSphere(v1);
	sph_v2 = MapToSphere(v2);
	sph_v3 = MapToSphere(v3);
	sph_center = MapToSphere(center);

	sph_m01 = MapToSphere(mid_01);
	sph_m12 = MapToSphere(mid_12);
	sph_m23 = MapToSphere(mid_23);
	sph_m30 = MapToSphere(mid_30);

	//=========================== パッチの可視判定 ===============================//
	float PatchRange, HorizontalDistance;
	float checkdist = sqrt(l*l*0.707106781 + m_HeightRange*m_HeightRange);

	// ViewFrustum-Culling
	PatchRange = max(max(max(max(checkdist,
					Distance(sph_center, sph_v0)), Distance(sph_center, sph_v1)), 
					Distance(sph_center, sph_v2)), Distance(sph_center, sph_v3));
	
	if(m_Frustum->SphereInFrustum(sph_center.x, sph_center.y, sph_center.z, PatchRange) == false)
		return;
	
	// Horizon-Culling
	HorizontalDistance = max(m_MinimumViewDistance, sqrt(DistanceSqrd(eyepos, m_Position) - m_Radius*m_Radius ) );
	if(Distance(sph_center, eyepos) > PatchRange + HorizontalDistance)
		return;

	//========== 視点位置とパッチ各辺との最短距離から，辺の重みを計算する ========//
	// 何れかの辺の解像度レベルが0.5より大きくパッチ再分割の可能性がある場合，中点を球面上にマッピングしてepsを再計算する
	
	// LEFT
	eps[0] = m_Rho * l / DIST_POINT_TO_SEGMENT(sph_v0, sph_v1, eyepos);
	
	if(eps[0] > 0.5f){
		eps_dash = m_Rho * l / min( DIST_POINT_TO_SEGMENT(sph_v0, sph_m01, eyepos), DIST_POINT_TO_SEGMENT(sph_m01, sph_v1, eyepos) );
		eps[0] = max(eps_dash, eps[0]);
	}

	// TOP
	eps[1] = m_Rho * l / DIST_POINT_TO_SEGMENT(sph_v1, sph_v2, eyepos);
	
	if(eps[1] > 0.5f){
		eps_dash = m_Rho * l / min(	DIST_POINT_TO_SEGMENT(sph_v1, sph_m12, eyepos), DIST_POINT_TO_SEGMENT(sph_m12, sph_v2, eyepos) );
		eps[1] = max(eps_dash, eps[1]);
	}
	
	// RIGHT
	eps[2] = m_Rho * l / DIST_POINT_TO_SEGMENT(sph_v2, sph_v3, eyepos);
	
	if(eps[2] > 0.5f){
		eps_dash = m_Rho * l / min( DIST_POINT_TO_SEGMENT(sph_v2, sph_m23, eyepos), DIST_POINT_TO_SEGMENT(sph_m23, sph_v3, eyepos) );
		eps[2] = max(eps_dash, eps[2]);
	}
	
	// BOTTOM
	eps[3] = m_Rho * l / DIST_POINT_TO_SEGMENT(sph_v3, sph_v0, eyepos);

	if(eps[3] > 0.5f){
		eps_dash = m_Rho * l / min( DIST_POINT_TO_SEGMENT(sph_v3, sph_m30, eyepos), DIST_POINT_TO_SEGMENT(sph_m30, sph_v0, eyepos) );
		eps[3] = max(eps_dash, eps[3]);
	}

	//==================== 重みに基づいてパッチを再分割する ======================//
	
	// 全ての辺の重みが閾値未満 or 分割最大レベルまで達した場合はパッチ情報を出力して処理終了
	if(miplv >= m_MaxLevel || (eps[0] < 1.0f &&  eps[1] < 1.0f && eps[2] < 1.0f && eps[3] < 1.0f) )
	{
		Vec2f page_phys_tc_high={0.0f, 0.0f}, page_phys_tc_low={0.0f, 0.0f};
		Vec3f	pos;
		int	 texlv_high = 0, texlv_low = 0;
		float scale_high = 0, scale_low = 0;

		Scale(du, m_Maps[m_currTable].m_UVN[0], m_Dimension);
		Scale(dv, m_Maps[m_currTable].m_UVN[1], m_Dimension);
		dn = m_Maps[m_currTable].m_UVN[2];
		pos = m_Maps[m_currTable].m_Vertices[0];
		
		if(m_pVirtualTexture) 
		m_pVirtualTexture->GetTileTexture(	page_phys_tc_high, page_phys_tc_low,
											texlv_high, texlv_low, scale_high, scale_low,
											du, dv, dn, vtex_tc, miplv, m_currTable);
		RenderTile( center, l, eps, page_phys_tc_high, page_phys_tc_low, vtex_tc, texlv_high, texlv_low, scale_high, scale_low);// レンダリング

		return;
	}
	else// 何れかの辺の重みが閾値を超えていたらパッチを4つに再分割
	{
		// ノードを分割する
		SubDivide_Spherical( eyepos, tile_x,		tile_y,			miplv+1 );// 左下
		SubDivide_Spherical( eyepos, tile_x,		tile_y+jump,	miplv+1 );// 左上
		SubDivide_Spherical( eyepos, tile_x+jump,	tile_y+jump,	miplv+1 );// 右上
		SubDivide_Spherical( eyepos, tile_x+jump,	tile_y,			miplv+1 );// 右下
	}// end of else

}





void TerrainEngine::RenderTile(Vec3f tilepos, float dimension, float eps[],
							  Vec2f page_phys_tc, Vec2f page_phys_tc_low, Vec2f vtex_tc, int texlv_high, int texlv_low, float scale_high, float scale_low)
{
	//====================== タイル各辺の解像度を計算する ========================//
	for(int i=0; i<4; i++)
	{
		tileres[i] = min(m_numMeshRes-1, (int)ceil(eps[i]*(m_numMeshRes-1)) );
		//tileres[i] = min(m_numMeshRes-1, (int)eps[i]*(m_numMeshRes-1) );
	}

	//====================== タイルの中心と寸法をシェーダに渡す ===================//
	cgSetParameter3fv(m_epTilePos, tilepos.xyz);	// タイル中心座標
	cgSetParameter1f(m_epDimension, dimension);		// タイルの辺の長さ
	
	// バーチャルテクスチャのパラメータをシェーダに渡す
	cgSetParameter2f(cgGetNamedEffectParameter(m_CgEffect, "rescale_page_to_physical"), scale_high, scale_low );//rescale_page_to_physical
	cgSetParameter2i(cgGetNamedEffectParameter(m_CgEffect, "g_mip_level"), texlv_high, texlv_low);// テクスチャのMipMapレベルを指定する
	cgSetParameter2fv(cgGetNamedEffectParameter(m_CgEffect, "page_phys_tc_high"), page_phys_tc.xy);
	cgSetParameter2fv(cgGetNamedEffectParameter(m_CgEffect, "page_phys_tc_low"), page_phys_tc_low.xy);
	cgSetParameter2fv(cgGetNamedEffectParameter(m_CgEffect, "g_vtex_tc"), vtex_tc.xy);
	
	//=================== パッチインスタンスをレンダリングする =====================//
	m_MeshObj->Draw(tileres);
}




bool TerrainEngine::IsValidEffect(CGeffect effect)
{	
	//tcout << _T("Checking effect techniques (") << cgGetEffectName(effect) << _T(" )...") << tendl;
	bool	flag = true;
	CGtechnique techniques = cgGetFirstTechnique(effect);
	
	if(!techniques)
	{
		//tcout << _T("	Error : No Valid Technique") << tendl;
		flag = false;
		return flag;
	}

	while(techniques)
	{
		//tcout << _T("  ") << cgGetTechniqueName(techniques) << _T("... ");

		if(cgValidateTechnique(techniques)==CG_FALSE)
		{
			flag = false;
			//tcout << _T("Invalid") << tendl;
		}
		//else
		//{
			//tcout << _T("OK") << tendl;			
		//}		
		techniques = cgGetNextTechnique(techniques);
	}
	
	return flag;
}



void TerrainEngine::LoadCgEffect()
{
tcout << _T("TerrainEngine::LoadCgEffect()...") << tendl;

	//===================== エラーチェック ====================//
	CGerror Error = cgGetError();
	if(Error)
	{
		tcout << cgGetErrorString(Error) << tendl;
		return; //exit(0);
	}

	//===================== コンテキスト作成 ==================//
	m_CgContext = cgCreateContext();
	cgGLRegisterStates(m_CgContext);

	//============== エフェクトファイル読み込み ===============//
	m_CgEffect = cgCreateEffectFromFile(m_CgContext, "Shader/TerrainEngine.cgfx", NULL);
	cgSetEffectName(m_CgEffect, "TerrainEngine.cgfx");
	
	if(IsValidEffect(m_CgEffect)==false) return; //exit(0);


	//================ テクニック/パスの取得 ===================//
	m_etRenderTerrain	= cgGetNamedTechnique(m_CgEffect, "RenderTerrain");	
	m_passRenderTerrain = cgGetFirstPass(m_etRenderTerrain);

	//==================== シェーダ変数定義 ===================//
	m_epEyePos			= cgGetNamedEffectParameter(m_CgEffect, "g_EyePos");
	m_epFarClip			= cgGetNamedEffectParameter(m_CgEffect, "g_FarClip");
	m_epModelViewMatrix	= cgGetNamedEffectParameter(m_CgEffect, "g_ModelViewMatrix");

	m_epHeightRange		= cgGetNamedEffectParameter(m_CgEffect, "g_HeightRange");
	m_epRadius			= cgGetNamedEffectParameter(m_CgEffect, "g_Radius");
	m_epDimension		= cgGetNamedEffectParameter(m_CgEffect, "g_Dimension");
	m_epTilePos			= cgGetNamedEffectParameter(m_CgEffect, "g_TilePos");
	m_epUvec			= cgGetNamedEffectParameter(m_CgEffect, "g_Uvec");
	m_epVvec			= cgGetNamedEffectParameter(m_CgEffect, "g_Vvec");
	m_epRho				= cgGetNamedEffectParameter(m_CgEffect, "g_Rho");

	m_epTileSize		= cgGetNamedEffectParameter(m_CgEffect, "g_TileSize");
	m_epSandScaleSampler= cgGetNamedEffectParameter(m_CgEffect, "g_StratumSampler");
	m_epHeightMap		= cgGetNamedEffectParameter(m_CgEffect, "g_HeightSampler");	
	m_epNormalMap		= cgGetNamedEffectParameter(m_CgEffect, "g_NormalSampler");

	m_epSurfaceProperty	= cgGetNamedEffectParameter(m_CgEffect, "g_SurfaceParam");

	//======== 全エフェクト共通のシェーダ変数を対応付け =======//
	cgSetParameter1f(m_epRadius, m_Radius);// 惑星の半径
	cgGLSetTextureParameter(m_epSandScaleSampler, m_SandScaleTexture);// 地層テクスチャ

}
