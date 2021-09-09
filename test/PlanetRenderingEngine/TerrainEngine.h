#ifndef TERRAIN_ENGINE_H
#define TERRAIN_ENGINE_H


#include <iostream>
using namespace std;

#include	<GL/glew.h>
#include	<GL/glut.h>
#pragma comment(lib, "glew32.lib")

#include	<CG/cg.h>
#include	<CG/cgGL.h>
#pragma comment(lib, "cg.lib")
#pragma comment(lib, "cggl.lib")


#include	<oreore/mathlib/GraphicsMath.h>
#include	<oreore/images/Bitmap.h>

#include	<graphics/gl4x/scene/ViewFrustum.h>
#include	<graphics/gl4x/scene/Camera.h>

#include	"SeamlessPatches.h"
#include	"TerrainMaterial.h"
#include	"HeightmapGenerator.h"
#include	"VirtualTexture.h"
#include	"GraphicsDevice.h"



// 惑星の骨組み（正6面体，各面の座標系）
typedef struct PlanetFrame
{
	Vec3f			m_Vertices[4];	// マップ４頂点座標
	Vec3f		m_UVN[3];		// 3次元空間上でのパッチの向き(必要)
}Terrainmap;



class TerrainEngine// 惑星の形状を描画するクラス
{

private:
	
	//============= 惑星パラメータ =============//
	Vec3f		m_Position;				// 中心位置
	Vec3f	m_Orientation[3];		// 回転
	float		m_Radius;				// 惑星半径
	float		m_HeightRange;			// 標高の幅
	float		m_MinimumViewDistance;	// 水平線までの距離（カメラが地表にある時）

	//================= 地層テクスチャ ===============//
	unsigned int	m_SandScaleTexture;

	//================= マテリアル ===================//
	TerrainMaterial	*m_TerrainMaterial;

	//============= グラフィックスデバイス ===========//
	GraphicsDevice	*m_pGraphicsDevice;

	//========= ハイトマップ生成オブジェクト ===========//
	HeightmapGenerator	*m_HeightmapGenerator;

	//========== テッセレーションのパラメータ ========//
	float	m_Rho;			// メッシュ詳細化の感度パラメータ(惑星固有)
	int		m_MaxLevel;		// パッチ最大分割深度（惑星固有）
	float	m_Dimension;	// パッチの辺長さ（惑星固有）
	int		tileres[4];

	//=============== 正６面体の幾何情報 ============//
	Terrainmap	m_Maps[6];

	//==== メッシュ（タイルに敷き詰めるポリゴン）====//
	int				m_numMeshRes;	// メッシュの多重解像度レベル数
	MeshGenerator	*m_MeshObj;	// 多重解像度メッシュ
	
	//================= ビューフラスタム ============//
	ViewFrustum		*m_Frustum;	//
	bool			m_FixFrustum;//ビューフラスタム固定

	//========= VirtualTextureへのポインタ ==========//
	VirtualTexture	*m_pVirtualTexture;
	int				m_TableID[6];	// ページテーブルのインデックス
	int				m_currTable;	// カレントページテーブル
		
	//================ Cgエフェクト =================//
	CGcontext	m_CgContext;
	CGeffect	m_CgEffect;
	
	//================= テクニック ==================//
	CGpass		m_passRenderTerrain;
	CGtechnique	m_etRenderTerrain;

	//=========== シェーダグローバル変数 ============//
	CGparameter	m_epEyePos;			// 視点座標
	CGparameter	m_epFarClip;		// カメラ奥行き方向クリッピング距離
	CGparameter	m_epModelViewMatrix;// モデルビュー変換行列
	CGparameter	m_epHeightRange;	// 標高の幅
	CGparameter	m_epRadius;			// 惑星の半径
	CGparameter	m_epDimension;		// パッチ一辺の長さ
	CGparameter	m_epTilePos;		// タイルの中心座標
	CGparameter	m_epUvec;			// タイル横方向ベクトル
	CGparameter	m_epVvec;			// タイル上方向ベクトル
	CGparameter	m_epRho;			// タイル細分化の感度パラメータ

	CGparameter	m_epTileSize;		// タイルに貼り付けるテクスチャの解像度
	CGparameter	m_epSandScaleSampler;// 地面テクスチャ
	CGparameter	m_epHeightMap;		// タイルのハイトマップテクスチャ
	CGparameter	m_epNormalMap;		// タイルの法線マップテクスチャ
	
	CGparameter	m_epSurfaceProperty;



	//================ メンバ関数 ================//
	Vec3f	MapToSphere(Vec3f In);
	void	GeneratePatches();			// 初期分割数を指定して6面体メッシュを生成する
	void	SubDivide_Spherical(Vec3f eyepos, int tile_x, int tile_y, int miplv);
	void	RenderTile(
						Vec3f tilepos, float dimension, float eps[], // ジオメトリ
						Vec2f page_phys_tc, Vec2f page_phys_tc_low, Vec2f vtex_tc, int texlv_high, int texlv_low,
						float scale_high, float scale_low // テクスチャ座標, mipレベル
						);
	void	LoadCgEffect();
	bool	IsValidEffect(CGeffect effect);
	

public:
	
	TerrainEngine();	// デフォルトコンストラクタ
	~TerrainEngine();	// デストラクタ

	void Init(Vec3f pos, float r, float heightrange, int numLv, float rho);
	void InitHeightmapGenerator(int mapsize, float border, float heightrange, float scale_horizontal, float scale_height);// ハイトマップ生成関数の初期化
	void InitMeshObject(int numdiv, int numlv);// メッシュ数と再分割レベルを指定してメッシュオブジェクトを初期化する

	void SetHeightRange(float heightrange);// 標高の幅を設定する
	void SetHeightmapScale(float scale_horizontal, float scale_vertical);// ハイトマップのスケールを調整する
	void SetRadius(float radius);		// 惑星半径を設定する
	void SetMaximumLevel(int maxLv);	// 再分割レベルを設定する
	void SetRho(float rho);				// テッセレーション感度を設定する
	void UpdateTextureParameter();		// VirtualTextureからテクスチャ情報を設定する
	void InitSurfaceProperties(	int nummats,
								const char **name, const Vec3f ambient[], const Vec3f diffuse[],
								const Vec3f specular[], const float specpow[], const float roughness[],
								const Vec3f distribution[], const float slope[]);
	
	void SetDistribution(int i, const Vec3f &distribution);
	void SetSlope(int i, const float slope);
	


	void FixFrustum(bool flag);		// ビューフラスタムの固定/移動切り替え
	
	void AttachGraphicsDevice(GraphicsDevice *pgd);
	void AttachVirtualTexture(VirtualTexture *vt);

	void DrawGbuffer(const Camera &camera, unsigned int polygonmode = 0x1B02);
//	void DrawOcean(Camera camera, unsigned int polygonmode = 0x1B02);

};


#endif // TERRAIN_ENGINE_H //