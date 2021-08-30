#ifndef	G_BUFFER_H
#define G_BUFFER_H


#include	<iostream>
using namespace std;

#include	<GL/glew.h>
#include	<GL/glut.h>
#pragma comment(lib, "glew32.lib")

#include	<CG/cg.h>
#include	<CG/cgGL.h>
#pragma comment(lib, "cg.lib")
#pragma comment(lib, "cggl.lib")

#include	<oreore/mathlib/GraphicsMath.h>



// Gバッファ
class G_Buffer
{
private:

	unsigned int	m_PositionMap;	// 座標RGB32F(rgb:座標xyz）
	unsigned int	m_NormalMap;	// 法線RGB16F(rgb:法線ベクトルxyz）
	unsigned int	m_DiffuseMap;	// 拡散反射率RGB8F(rgb:色, a:鏡面反射の強さ)
	unsigned int	m_SpecularMap;	// 鏡面反射率RGB8F(rgb:色，a:鏡面反射の鋭さ）


	//=========== Cgエフェクト ==============//
	CGcontext	m_CgContext;
	CGeffect	m_CgEffect;
	CGpass		m_CgPass;

	//============= テクニック ==============//
	CGtechnique	m_etDeferredRendering;

	//======= シェーダグローバル変数 ========//
	CGparameter	m_epNormalBuffer;	// 法線マップ
	CGparameter	m_epPosBuffer;		// 座標マップ
	CGparameter	m_epColorBuffer;	// 色マップ
	CGparameter	m_epSpecularBuffer;	// 色マップ
	
	CGparameter	m_epLightDir;		// 光源ベクトル
	CGparameter	m_epLightIntensity;	// 光源の強さ
	CGparameter	m_epEyePos;			// カメラ位置
	
	void	LoadCgEffect();
	bool	IsValidEffect(CGeffect effect);



public:

	G_Buffer();
	G_Buffer(int width, int height);
	~G_Buffer();

	void SetSize(int width, int height);

	void ShowDiffuseMap( int width, int height );
	void ShowNormalMap( int width, int height );
	void ShowHeightMap( int width, int height, float heightscale);
	void ShowSpecularMap( int width, int height );
	void DrawShadedImage( const Vec3f &eyePos, const Vec3f &lightDir, const Vec3f &lightIntensity);// シェーディングしたイメージをレンダリングする

	void Draw( int width, int height );// Gバッファを表示する
	void DrawTexture_RGB( unsigned int texture, int width, int height, float color_scale=1.0f );
	void DrawTexture_A( unsigned int texture, int width, int height, float color_scale=1.0f );


	inline unsigned int Position()	const{ return m_PositionMap; };
	inline unsigned int Normal()	const{ return m_NormalMap; };
	inline unsigned int Diffuse()	const{ return m_DiffuseMap; };
	inline unsigned int Specular()	const{ return m_SpecularMap; };

};



#endif // G_BUFFER_H //