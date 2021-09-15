#ifndef	RUST_SHADER_H
#define	RUST_SHADER_H


#include	"../oreore/IRenderer.h"
#include	"../oreore/GLShader.h"
using namespace OreOreLib;




// Rust Shader
class RustShader : public IRenderer
{
private:

	// shader
	GLShader	*m_pShader;	// デフォルトのシェーダー
	
	// uniform location	
	GLint		m_ulRustMask;			// 錆領域のマスクテクスチャ.
	GLint		m_ulPermSimplex;		// SimplexNoiseのPermTexture
	GLint		m_ulPermWorley;			// WotrleyNoiseのPermTexture

	//GLint		m_ulTexture;			// 形状全体の錆分布
	//GLint		m_ulSpotDistribution;	// 錆発生点の分布マップ. GPU化に伴い破棄
	//GLint		m_ulInnerRust;			// 錆領域内のマテリアル分布. GPU

	GLint		m_ulAging;				// 分布量
	GLint		m_ulScale;				// スケール
	GLint		m_ulOffset;				// ノイズ生成に使う座標値のオフセット
	GLint		m_ulDistribAmount;		// 
	GLint		m_ulDistribSmoothness;	// 
	GLint		m_ulSpotAmount;			// 錆の濃さ
	GLint		m_ulSpotSmoothness;		// 錆境界の連続変化度合い
	GLint		m_ulBleedAmount;		// 流れ錆の濃さ
	GLint		m_ulBleedSmoothness;	// 流れ錆の連続変化度合い
	GLint		m_ulBaseAmount;
	GLint		m_ulBaseSmoothness;		// 流れ錆の連続変化度合い
	GLint		m_ulOuterBleedAmount;
	GLint		m_ulOuterBleedSmoothness;
	GLint		m_ulOuterBaseAmount;
	GLint		m_ulOuterBaseSmoothness;
	

	Texture1D	m_PermSimplex;
	Texture1D	m_PermWorley;

	float		m_Aging;
	float		m_Scale;
	Vec3f		m_Offset;
	float		m_DistribAmount;
	float		m_DistribSmoothness;
	float		m_SpotAmount;
	float		m_SpotSmoothness;
	float		m_BleedAmount;
	float		m_BleedSmoothness;
	float		m_BaseAmount;
	float		m_BaseSmoothness;
	float		m_OuterBleedAmount;
	float		m_OuterBleedSmoothness;
	float		m_OuterBaseAmount;
	float		m_OuterBaseSmoothness;


	void InitPermTexture();

	
public:

	RustShader();
	RustShader( const char* filepath );
	~RustShader();

	void Release();

	void InitShader( const char* filepath );
	
	inline virtual void BindShader( )
	{
		if( CurrentShader == m_pShader )	return;
	
		CurrentShader = m_pShader;
		CurrentShader->Bind();
	}


	void Render( /*uint32 distrib, uint32 rustshape, uint32 inner,*/ uint32 rust_mask );
	virtual void Render( SceneNode *node, CameraMatrix *cp ){};
	

	void SetAging( float val )					{ m_Aging = val; }
	void SetScale( float val )					{ m_Scale = val; }
	void SetOffset( float x, float y, float z )	{ m_Offset.x=x; m_Offset.y=y; m_Offset.z=z; }
	void SetDistribAmount( float val )			{ m_DistribAmount	= val; }
	void SetDistribSmoothness( float val )		{ m_DistribSmoothness	= val; }
	void SetSpotAmount( float val )				{ m_SpotAmount	= val; }
	void SetSpotSmoothness( float val )			{ m_SpotSmoothness	= val; }
	void SetBleedAmount( float val )			{ m_BleedAmount	= val; }
	void SetBleedSmoothness( float val )		{ m_BleedSmoothness	= val; }
	void SetBaseAmount( float val )				{ m_BaseAmount	= val; }
	void SetBaseSmoothness( float val )			{ m_BaseSmoothness	= val; }
	void SetOuterBleedAmount( float val )		{ m_OuterBleedAmount	= val; }
	void SetOuterBleedSmoothness( float val )	{ m_OuterBleedSmoothness	= val; }
	void SetOuterBaseAmount( float val )		{ m_OuterBaseAmount	= val; }
	void SetOuterBaseSmoothness( float val )	{ m_OuterBaseSmoothness	= val; }

};





// RustShaderオブジェクトを生成するファクトリクラス
class RustShaderFactory : IRendererFactory
{
private:

	IRenderer* CreateInstance(){ return new RustShader( "RustShader.glsl" ); }

public:

	RustShaderFactory(){};
	//~RustShaderFactory(){};
};





#endif	// RUST_SHADER_H //