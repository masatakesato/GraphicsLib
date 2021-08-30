#ifndef HDR_SHADER_H
#define HDR_SHADER_H


#include	<iostream>
using namespace std;


#include	<GL/glew.h>
#pragma comment(lib, "glew32.lib")

#include	<CG/cg.h>
#include	<CG/cgGL.h>
#pragma comment(lib, "cg.lib")
#pragma comment(lib, "cggl.lib")






class HDRShader
{
private:
	
	int	m_Width4x4;
	int	m_Height4x4;

	//===== テクスチャ（作業用バッファ）=====//
	unsigned int	m_ScaledImage;// 1/4に縮小したテクスチャ
	unsigned int	m_Luminance_curr[4];// 現在の画面の輝度テクスチャ(64x64, 16x16, 4x4, 1x1)
	unsigned int	m_Luminance_prev;	// 直前フレームの画面輝度テクスチャ(1x1)
	unsigned int	m_AdaptedLuminance;
	unsigned int	m_BrightPass, m_BrightPass2x2;
	unsigned int	m_BlurredImage;	// ぼかしテクスチャ
	unsigned int	m_BlurredImage2;	// ぼかしテクスチャ

	// フレームバッファオブジェクト
	unsigned int m_fbo;

	//=========== Cgエフェクト ==============//
	CGcontext		m_CgContext;
	CGeffect		m_CgEffect;
	CGpass			m_CgPass;
	
	//============= テクニック ==============//
	CGtechnique		m_etBasic;

	//======= シェーダグローバル変数 ========//
	CGparameter		m_epTexOffset;
	CGparameter		m_epTexSize;
	CGparameter		m_epSourceImage1;
	CGparameter		m_epSourceImage2;
	CGparameter		m_epLuminance;
	CGparameter		m_epTimeStep;
	CGparameter		m_epKey;
	CGparameter		m_epBloomWeight;

	//============= プライベートメンバ関数 ==============//
	void LoadCgEffect();
	bool IsValidEffect(CGeffect effect);

	void GenScaledImage(unsigned int IN);// 画像INの縮小イメージを作成する
	void GenAverageLuminance();			// 縮小イメージから平均輝度を計算する
	void AdaptLuminance();				// 明順応
	void FilterBlightPass();			// 縮小イメージの明るい領域だけ取り出す
	void BlurImage();					// 画像をぼかす
	void RenderFinalImage(unsigned int IN, int width, int height);


public:

	HDRShader();
	HDRShader(int width, int height);
	~HDRShader();

	void DoHDRLighting(unsigned int IN, float timestep, float key, float bloomweight, int width, int height);

	void SetScreenSize(int width, int height);
};



#endif // HDR_SHADER_H //