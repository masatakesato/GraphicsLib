#ifndef	RUST_SHADER_H
#define	RUST_SHADER_H

#include	<graphics/gl4x/shader/IShader.h>
#include	<graphics/gl4x/shader/GLShader.h>
using namespace OreOreLib;




// Rust Shader
class RustShader : public IShader
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
	RustShader( const TCHAR *filepath, GLSL_VERSION version );
	~RustShader();

	void Release();

	void InitShader( const TCHAR *filepath, GLSL_VERSION version );
	
	inline virtual void Bind( )
	{
		m_pShader->Bind();
	}


	inline void Unbind()
	{
		m_pShader->Unbind();
	}

	
	inline virtual int NumPasses() const						{ return 1; }
	inline virtual GLuint GetPassID( int shader_idx=0 ) const	{ return m_pShader->ID(); }

	inline virtual void BindShader( int shader_idx=0 )	// シェーダーをバインドする
	{
		if( m_pCurrShader == m_pShader )	return;
		m_pCurrShader = m_pShader;
		m_pCurrShader->Bind();
	}


	virtual void BindBufferObject( const IBufferObject* pbufferobj ){}
	virtual void UnbindBufferObject(){}

	//virtual void Render(){}
	virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat ){}
	void Render( /*uint32 distrib, uint32 rustshape, uint32 inner,*/ uint32 rust_mask );


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



	const float& Aging()					{ return m_Aging; }
	const float& Scale()					{ return m_Scale; }
	const float& OffsetX()				{ return m_Offset.x; }
	const float& OffsetY()				{ return m_Offset.y; }
	const float& OffsetZ()				{ return m_Offset.z; }
	const float& DistribAmount()			{ return m_DistribAmount; }
	const float& DistribSmoothness()		{ return m_DistribSmoothness; }
	const float& SpotAmount()			{ return m_SpotAmount; }
	const float& SpotSmoothness()		{ return m_SpotSmoothness; }
	const float& BleedAmount()			{ return m_BleedAmount; }
	const float& BleedSmoothness()		{ return m_BleedSmoothness; }
	const float& BaseAmount()			{ return m_BaseAmount; }
	const float& BaseSmoothness()		{ return m_BaseSmoothness; }
	const float& OuterBleedAmount()		{ return m_OuterBleedAmount; }
	const float& OuterBleedSmoothness()	{ return m_OuterBleedSmoothness; }
	const float& OuterBaseAmount()		{ return m_OuterBaseAmount; }
	const float& OuterBaseSmoothness()	{ return m_OuterBaseSmoothness; }




};





// RustShaderオブジェクトを生成するファクトリクラス
class RustShaderFactory : IShaderFactory
{
private:

	IShader* Create( GLSL_VERSION version ){ return new RustShader( _T("RustShader.glsl"), version ); }

public:

	virtual ~RustShaderFactory(){}
	IShader* CreateInstance( GLSL_VERSION version );

};





#endif	// RUST_SHADER_H //