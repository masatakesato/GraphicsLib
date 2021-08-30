#ifndef HEIGHTMAP_GENERATOR_H
#define	HEIGHTMAP_GENERATOR_H

#include	<iostream>
using namespace std;

#include	<GL/glew.h>
#pragma comment(lib, "glew32.lib")

#include	<CG/cg.h>
#include	<CG/cgGL.h>
#pragma comment(lib, "cg.lib")
#pragma comment(lib, "cggl.lib")


#include	<oreore/mathlib/GraphicsMath.h>
#include	<graphics/gl4x/scene/Camera.h>



class HeightmapGenerator
{
private:

	float			m_HeightRange;		// 高さの上限値
	float			m_HorizontalScale;	// ハイトマップの水平方向スケール
	float			m_VerticalScale;	// ハイトマップの鉛直方向スケール
	
	int				m_MapSize;		// ハイトマップ一辺のピクセル数
	float			m_BorderSize;	// のりしろのピクセル数
	float			m_FoV;			// カメラの視野角[rad]（のりしろを持たせてテクスチャレンダリングするために使う）
	unsigned int	m_HeightBuffer;
	unsigned int	m_PosBuffer;
	unsigned int	m_NormalBuffer;
	
	Camera			*m_Camera;


	//============ ノイズ生成用ルックアップテクスチャ ===============//
	unsigned int	m_PermTexture;		//1Dテクスチャ pernumation taxture (Luminance8)
	unsigned int	m_PermTexture2d;	//2Dテクスチャ"a8r8g8b8"
	unsigned int	m_PermGradTexture;	//1DテクスチャDq8w8v8u8
	unsigned int	m_GradTexture4d;	//1Dテクスチャq8w8v8u8


	//==== フレームバッファオブジェクト =====//
	unsigned int	m_fbo;

	//=========== Cgエフェクト ==============//
	CGcontext	m_CgContext;
	CGeffect	m_CgEffect;
	CGpass		m_CgPass;

	//============= テクニック ==============//
	CGtechnique	m_etInitLookupTable;
	CGtechnique	m_etGenSphericalHeightMap;// ハイトマップ作成処理

	//======= シェーダグローバル変数 ========//
	CGparameter	m_epPermSampler;
	CGparameter	m_epPermSampler2d;
	CGparameter	m_epPermGradSampler;
	CGparameter	m_epGradSampler4d;

	CGparameter	m_epMapSize;
	CGparameter	m_epHeightRange;
	CGparameter	m_epHorizontalScale;
	CGparameter	m_epVerticalScale;
	
	//============= メンバ関数 ==============//
	void	LoadCgEffect();
	bool	IsValidEffect(CGeffect effect);
	
	// PerlinNoise用のルックアップテーブルを前計算するメンバ関数(シェーダで実行)
	void InitLookupTable();
	void GeneratePermTexture();
	void GeneratePermTexture2d();
	void GeneratePermGradTexture();
	void GenerateGradTexture4d();
	
	HeightmapGenerator(){};// デフォルトコンストラクタは禁止


public:
	
	HeightmapGenerator(int mapsize, float border, float heightrange);
	~HeightmapGenerator();
	
	void SetScale(float horizontal, float vertical);// ハイトマップのスケールを設定する
	void SetMapSize(int mapsize);					// テクスチャサイズを設定する

	// ハイトマップをレンダリングする
	void Draw(	uint32 tex_height, uint32 tex_normal,
				int xoffset, int yoffset, float dimension,
				const Vec3f &pos, const Vec3f &dn, const Vec3f &dv);
};



#endif // HEIGHTMAP_GENERATOR_H //