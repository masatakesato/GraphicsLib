#include	"AtmosphericShader.h"


#include	<windows.h>

#include	<oreore/images/Bitmap.h>

#include	<graphics/gl4x/other/MyGlPrimitive.h>
using namespace MyGlPrimitive;

#include	<graphics/gl4x/resource/Texture.h>
using namespace OreOreLib;



static unsigned int bufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };


AtmosphericShader::AtmosphericShader(int width, int height)
{
tcout << _T("AtmosphericShader::AtmosphericShader()...") << tendl;

	//========= 惑星形状パラメータ ============//
	Rg	= 6360.0f;
	Rt	= 6460.0f;

	//========== 大気散乱のパラメータ =========//
	// Rayleigh
	m_HR	= 8.0f;
	InitVec(m_betaR, 5.8e-3f, 1.35e-2f, 3.31e-2f);
	// Mie
	m_HM	= 2.2f;
	InitVec(m_betaMSca, 20.0e-3f, 20.0e-3f, 20.0e-3f);
	m_mieG	= 0.7f;

	
	//=========== ルックアップテーブル解像度 ===========//
	TRANSMITTANCE_W	= 128;//256;
	TRANSMITTANCE_H	= 32;//64;

	SKY_W		= 32;//64;
	SKY_H		= 8;//16;

	RES_R		= 16;//32;
	RES_MU		= 64;//128;
	RES_MU_S	= 16;//32;
	RES_NU		= 4;//8;

	RenderBuffer_SkyColor = AllocateTexture2D(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, width, height, GL_RGB16F, GL_RGB, GL_FLOAT);// 空だけレンダリングするバッファ
	RenderBuffer_Surface = AllocateTexture2D(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT);// 球面形状を保存するバッファ

	unsigned int w, h;
	float *img = Bitmap_load_texture("../../test/PlanetRenderingEngine/Texture/StarField.bmp", w, h);
	StarFieldTexture = AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP, w, h, GL_RGB16F_ARB, GL_RGB, GL_FLOAT, img);


	//================== フレームバッファオブジェクト初期化 =================//
	glGenFramebuffers(1, &m_fbo);// ルックアップテーブル計算用
	
	//========== Cgエフェクトのロード =========//
	m_numEffects = 10;
	CgEffect	= new CGeffect[m_numEffects];
	CgTechnique	= new CGtechnique[m_numEffects];
	LoadCgEffect();

	//======= ルックアップテーブルの計算 ======//
	PrecomputeLookupTable();
}


AtmosphericShader::~AtmosphericShader()
{
tcout << _T("AtmosphericShader::~AtmosphericShader()...") << tendl;

	//===== フレームバッファオブジェクト削除 =========//
	if(m_fbo)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, NULL, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, NULL, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
		glDeleteFramebuffers(1, &m_fbo);
	}

	//================ テクスチャ削除 ================//
	if(transmittanceTexture){ glDeleteTextures(1, &transmittanceTexture); transmittanceTexture = NULL; }
	if(irradianceTexture){ glDeleteTextures(1, &irradianceTexture); irradianceTexture = NULL; }
	if(inscatterTexture){ glDeleteTextures(1, &inscatterTexture); inscatterTexture = NULL; }
	if(deltaETexture){ glDeleteTextures(1, &deltaETexture); deltaETexture = NULL; }
	if(deltaSRTexture){ glDeleteTextures(1, &deltaSRTexture); deltaSRTexture = NULL; }
	if(deltaSMTexture){ glDeleteTextures(1, &deltaSMTexture); deltaSMTexture = NULL; }
	if(deltaJTexture){ glDeleteTextures(1, &deltaJTexture); deltaJTexture = NULL; }
	if(RenderBuffer_SkyColor){ glDeleteTextures(1, &RenderBuffer_SkyColor); RenderBuffer_SkyColor = NULL; }
	if(RenderBuffer_Surface){ glDeleteTextures(1, &RenderBuffer_Surface); RenderBuffer_Surface = NULL; }

	if(StarFieldTexture){ glDeleteTextures(1, &StarFieldTexture); StarFieldTexture = NULL; }


	//================ シェーダ削除 ==================//
	for(int i=0; i<m_numEffects; i++)	cgDestroyEffect(CgEffect[i]);
	cgDestroyContext(CgContext);

	if(CgEffect){	delete [] CgEffect; CgEffect = NULL; }
	if(CgTechnique){	delete [] CgTechnique; CgTechnique = NULL; }
}


void AtmosphericShader::AttachGbuffer(const G_Buffer *gbuffer)
{
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[ATMOSPHERIC_SHADER], "g_PosBuffer"),		gbuffer->Position());		// PositionMap
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[ATMOSPHERIC_SHADER], "g_NormalBuffer"),	gbuffer->Normal());		// NormalMap
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[ATMOSPHERIC_SHADER], "g_ColorBuffer"),	gbuffer->Diffuse());		// DiffuseMap
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[ATMOSPHERIC_SHADER], "g_SpecularBuffer"), gbuffer->Specular());	// SpecularMap
}




void AtmosphericShader::PrecomputeLookupTable()
{
tcout << _T("AtmosphericShader::PrecomputeLookupTable()...") << tendl;

	glEnable(GL_DEPTH_TEST);
	
	//============================= テクスチャ初期化 =========================//
	transmittanceTexture= AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, TRANSMITTANCE_W, TRANSMITTANCE_H, GL_RGB16F, GL_RGB, GL_FLOAT);
	irradianceTexture	= AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, SKY_W, SKY_H, GL_RGB16F, GL_RGB, GL_FLOAT);
	inscatterTexture	= AllocateTexture3D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, RES_MU_S*RES_NU, RES_MU, RES_R, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	
	deltaETexture		= AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, SKY_W, SKY_H, GL_RGB16F, GL_RGB, GL_FLOAT); 
	deltaSRTexture		= AllocateTexture3D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, RES_MU_S*RES_NU, RES_MU, RES_R, GL_RGB16F, GL_RGB, GL_FLOAT);
	deltaSMTexture		= AllocateTexture3D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, RES_MU_S*RES_NU, RES_MU, RES_R, GL_RGB16F, GL_RGB, GL_FLOAT);
	deltaJTexture		= AllocateTexture3D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, RES_MU_S*RES_NU, RES_MU, RES_R, GL_RGB16F, GL_RGB, GL_FLOAT);
	

	//===================== ルックアップテーブル作成開始 =====================//
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);


	// computes transmittance texture T (line 1 in algorithm 4.1)
	PrecomputeTransmittance();
	
	// computes irradiance texture deltaE (line 2 in algorithm 4.1)
	PrecomputeIrradiance1();
	
	// computes single scattering texture deltaS (line 3 in algorithm 4.1)
    // Rayleigh and Mie separated in deltaSR + deltaSM
	PrecomputeInscatter1();

	// copies deltaE into irradiance texture E (line 4 in algorithm 4.1)
	CopyIrradiance(0);
	
	// copies deltaS into inscatter texture S (line 5 in algorithm 4.1)
	CopyInscatter1();
	
	
	// loop for each scattering order (line 6 in algorithm 4.1)
	for (int order = 2; order <= 6; ++order)
	{
		InscatterS(order);	// computes deltaJ (line 7 in algorithm 4.1)
		IrradianceN(order);	// computes deltaE (line 8 in algorithm 4.1)
		InscatterN(order);	// computes deltaS (line 9 in algorithm 4.1)
		
		glEnable(GL_BLEND);
		glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
		glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
		
		CopyIrradiance(1);// adds deltaE into irradiance texture E (line 10 in algorithm 4.1)
		CopyInscatterN();// adds deltaS into inscatter texture S (line 11 in algorithm 4.1)
		
		glDisable(GL_BLEND);
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	
	// Cgパラメータ設定(RenderSky.cgfx)
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[ATMOSPHERIC_SHADER], "inscatterSampler"), inscatterTexture);
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[ATMOSPHERIC_SHADER], "transmittanceSampler"), transmittanceTexture);
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[ATMOSPHERIC_SHADER], "irradianceSampler"), irradianceTexture);
	

	glDisable(GL_DEPTH_TEST);

	glFinish();
}








void AtmosphericShader::PrecomputeTransmittance()
{
tcout << _T("AtmosphericShader::PrecomputeTransmittance()...") << tendl;

	CGpass pass;
	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, transmittanceTexture, 0);
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, TRANSMITTANCE_W, TRANSMITTANCE_H);
	
	// レンダリングパス実行
	pass = cgGetFirstPass(CgTechnique[0]);
	cgSetPassState(pass);
	{	
		DrawQuad();
	}
	cgResetPassState(pass);
}


void AtmosphericShader::PrecomputeIrradiance1()
{
tcout << _T("AtmosphericShader::PrecomputeIrradiance1()...") << tendl;

	CGpass pass;
	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, deltaETexture, 0);
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SKY_W, SKY_H);
	
	// Cgパラメータ設定
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[PRECOMPUTE_IRRADIANCE1], "transmittanceSampler"), transmittanceTexture);
	
	// レンダリングパス実行
	pass = cgGetFirstPass(CgTechnique[1]);
	cgSetPassState(pass);
	{
		DrawQuad();
	}
	cgResetPassState(pass);
}


void AtmosphericShader::setLayer(float &r_float, float *dhdH, const int layer)// 3Dテクスチャのスライス（および付随するパラメータ）を指定する関数
{
	double r = layer / (RES_R - 1.0);// 32階層の高度[0,1]
	r = r * r;
	r = sqrt(Rg * Rg + r * (Rt * Rt - Rg * Rg)) + (layer == 0 ? 0.01 : (layer == RES_R - 1 ? -0.001 : 0.0));
													// ↑この部分は補正係数
	double dmin = Rt - r;// 大気上面 - 高度
	double dmax = sqrt(r * r - Rg * Rg) + sqrt(Rt * Rt - Rg * Rg);// ρ+ H
	double dminp = r - Rg;// 地面からの高さ
	double dmaxp = sqrt(r * r - Rg * Rg);// ρ

	r_float = float(r);
	dhdH[0] = float(dmin);	dhdH[1] = float(dmax);	dhdH[2] = float(dminp);	dhdH[3] = float(dmaxp);
}


void AtmosphericShader::PrecomputeInscatter1()
{
tcout << _T("AtmosphericShader::PrecomputeInscatter1()...") << tendl;

	float	r = 0;// シェーダに渡す値
	float	dhdH[4] = {0,0,0,0};// シェーダに渡す値
	int		layer = 0;// シェーダに渡す値
	CGpass	pass;
	

	pass = cgGetFirstPass(CgTechnique[2]);

	glViewport(0, 0, RES_MU_S * RES_NU, RES_MU);// ビューポートの設定

	// Cgパラメータ設定(固定)
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[PRECOMPUTE_INSCATTER1], "transmittanceSampler"), transmittanceTexture);

	for(layer = 0; layer < RES_R; ++layer)// 3Dテクスチャの各レイヤについて,,,
	{
		// Cgパラメータ設定(動的)
		setLayer(r, dhdH, layer);
		cgSetParameter1f(cgGetNamedEffectParameter(CgEffect[PRECOMPUTE_INSCATTER1], "r"), r);
		cgSetParameter4fv(cgGetNamedEffectParameter(CgEffect[PRECOMPUTE_INSCATTER1], "dhdH"), dhdH);
		cgSetParameter1f(cgGetNamedEffectParameter(CgEffect[PRECOMPUTE_INSCATTER1], "layer"), (float)layer);
		
		// fboに3Dテクスチャのスライスをアタッチ
		glFramebufferTextureLayer(GL_FRAMEBUFFER, bufs[0], deltaSRTexture, 0, layer);
		glFramebufferTextureLayer(GL_FRAMEBUFFER, bufs[1], deltaSMTexture, 0, layer);
		glDrawBuffers(2, bufs);

		// レンダリング
		cgSetPassState(pass);
		{
			DrawQuad();
		}
		cgResetPassState(pass);
    }

	glFramebufferTextureLayer(GL_FRAMEBUFFER, bufs[1], NULL, NULL, NULL);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
}


void AtmosphericShader::CopyIrradiance(int k)
{
tcout << _T("AtmosphericShader::CopyIrradiance()...") << tendl;

	CGpass pass;

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, irradianceTexture, 0);

	glViewport(0, 0, SKY_W, SKY_H);

	// Cgパラメータ設定
	cgSetParameter1f(cgGetNamedEffectParameter(CgEffect[COPY_IRRADIANCE], "k"), (float)k);
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[COPY_IRRADIANCE], "deltaESampler"), deltaETexture);
	
	// レンダリングパス実行
	pass = cgGetFirstPass(CgTechnique[3]);
	cgSetPassState(pass);
	{	
		DrawQuad();
	}
	cgResetPassState(pass);
}


void AtmosphericShader::CopyInscatter1()
{
tcout << _T("AtmosphericShader::CopyInscatter1()...") << tendl;

	float	r = 0;// シェーダに渡す値
	float	dhdH[4] = {0,0,0,0};// シェーダに渡す値
	int		layer = 0;// シェーダに渡す値
	CGpass	pass;

	pass = cgGetFirstPass(CgTechnique[4]);

	glViewport(0, 0, RES_MU_S * RES_NU, RES_MU);// ビューポートの設定
	
	// Cgパラメータ設定(固定)
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[COPY_INSCATTER1], "deltaSRSampler"), deltaSRTexture);
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[COPY_INSCATTER1], "deltaSMSampler"), deltaSMTexture);
	
	for(layer = 0; layer < RES_R; ++layer)// 3Dテクスチャの各レイヤについて,,,
	{
		// Cgパラメータ設定(動的)
		setLayer(r, dhdH, layer);
		cgSetParameter1f(cgGetNamedEffectParameter(CgEffect[COPY_INSCATTER1], "r"), r);
		cgSetParameter4fv(cgGetNamedEffectParameter(CgEffect[COPY_INSCATTER1], "dhdH"), dhdH);
		cgSetParameter1f(cgGetNamedEffectParameter(CgEffect[COPY_INSCATTER1], "layer"), (float)layer);

		// fboに3Dテクスチャのスライスをアタッチ
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, inscatterTexture, 0, layer);

		// レンダリング
		cgSetPassState(pass);
		{
			DrawQuad();
		}
		cgResetPassState(pass);
    }
}


void AtmosphericShader::InscatterS(int order)
{
tcout << _T("AtmosphericShader::InscatterS()...") << tendl;

	float	r = 0;// シェーダに渡す値
	float	dhdH[4] = {0,0,0,0};// シェーダに渡す値
	int		layer = 0;// シェーダに渡す値
	CGpass	pass;

	pass = cgGetFirstPass(CgTechnique[5]);

	glViewport(0, 0, RES_MU_S * RES_NU, RES_MU);// ビューポートの設定

	// Cgパラメータ設定
	cgSetParameter1f(cgGetNamedEffectParameter(CgEffect[INSCATTER_S], "first"),order == 2 ? 1.f : 0.f);
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[INSCATTER_S], "transmittanceSampler"), transmittanceTexture);
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[INSCATTER_S], "deltaESampler"), deltaETexture);
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[INSCATTER_S], "deltaSRSampler"), deltaSRTexture);
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[INSCATTER_S], "deltaSMSampler"), deltaSMTexture);

	for(layer = 0; layer < RES_R; ++layer)// 3Dテクスチャの各レイヤについて,,,
	{
		// Cgパラメータ設定(動的)
		setLayer(r, dhdH, layer);
		cgSetParameter1f(cgGetNamedEffectParameter(CgEffect[INSCATTER_S], "r"), r);
		cgSetParameter4fv(cgGetNamedEffectParameter(CgEffect[INSCATTER_S], "dhdH"), dhdH);
		cgSetParameter1f(cgGetNamedEffectParameter(CgEffect[INSCATTER_S], "layer"), (float)layer);

		// fboに3Dテクスチャのスライスをアタッチ
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, deltaJTexture, 0, layer);
		
		// レンダリング
		cgSetPassState(pass);
		{
			DrawQuad();
		}
		cgResetPassState(pass);
	}
}


void AtmosphericShader::IrradianceN(int order)
{
tcout << _T("AtmosphericShader::IrradianceN()...") << tendl;

	CGpass pass;

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, deltaETexture, 0);
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SKY_W, SKY_H);
	
	// Cgパラメータ設定
	cgSetParameter1f(cgGetNamedEffectParameter(CgEffect[IRRADIANCE_N], "first"),order == 2 ? 1.f : 0.f);
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[IRRADIANCE_N], "transmittanceSampler"), transmittanceTexture);
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[IRRADIANCE_N], "deltaSRSampler"), deltaSRTexture);
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[IRRADIANCE_N], "deltaSMSampler"), deltaSMTexture);
	
	// レンダリングパス実行
	pass = cgGetFirstPass(CgTechnique[6]);
	cgSetPassState(pass);
	{	
		DrawQuad();
	}
	cgResetPassState(pass);
}


void AtmosphericShader::InscatterN(int order)
{
tcout << _T("AtmosphericShader::InscatterN()...") << tendl;

	float	r = 0;// シェーダに渡す値
	float	dhdH[4] = {0,0,0,0};// シェーダに渡す値
	int		layer = 0;// シェーダに渡す値
	CGpass	pass;

	pass = cgGetFirstPass(CgTechnique[7]);

	glViewport(0, 0, RES_MU_S * RES_NU, RES_MU);// ビューポートの設定

	// Cgパラメータ設定
	cgSetParameter1f(cgGetNamedEffectParameter(CgEffect[INSCATTER_N], "first"),order == 2 ? 1.f : 0.f);
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[INSCATTER_N], "transmittanceSampler"), transmittanceTexture);
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[INSCATTER_N], "deltaJSampler"), deltaJTexture);

	for(layer = 0; layer < RES_R; ++layer)// 3Dテクスチャの各レイヤについて,,,
	{
		// Cgパラメータ設定(動的)
		setLayer(r, dhdH, layer);
		cgSetParameter1f(cgGetNamedEffectParameter(CgEffect[INSCATTER_N], "r"), r);
		cgSetParameter4fv(cgGetNamedEffectParameter(CgEffect[INSCATTER_N], "dhdH"), dhdH);
		cgSetParameter1f(cgGetNamedEffectParameter(CgEffect[INSCATTER_N], "layer"), (float)layer);
		
		// fboに3Dテクスチャのスライスをアタッチ
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, deltaSRTexture, 0, layer);
		
		// レンダリング
		cgSetPassState(pass);
		{
			DrawQuad();
		}
		cgResetPassState(pass);
	}
}



void AtmosphericShader::CopyInscatterN()
{
tcout << _T("AtmosphericShader::CopyInscatterN()...") << tendl;

	float	r = 0;// シェーダに渡す値
	float	dhdH[4] = {0,0,0,0};// シェーダに渡す値
	int		layer = 0;// シェーダに渡す値
	CGpass	pass;

	pass = cgGetFirstPass(CgTechnique[8]);

	glViewport(0, 0, RES_MU_S * RES_NU, RES_MU);// ビューポートの設定
	glEnable(GL_BLEND);
	
	// Cgパラメータ設定(固定)
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[COPY_INSCATTER_N], "deltaSSampler"), deltaSRTexture);
	
	
	for(layer = 0; layer < RES_R; ++layer)// 3Dテクスチャの各レイヤについて,,,
	{
		// Cgパラメータ設定(動的)
		setLayer(r, dhdH, layer);
		cgSetParameter1f(cgGetNamedEffectParameter(CgEffect[COPY_INSCATTER_N], "r"), r);
		cgSetParameter4fv(cgGetNamedEffectParameter(CgEffect[COPY_INSCATTER_N], "dhdH"), dhdH);
		cgSetParameter1f(cgGetNamedEffectParameter(CgEffect[COPY_INSCATTER_N], "layer"), (float)layer);

		// fboに3Dテクスチャのスライスをアタッチ
		GL_SAFE_CALL( glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, inscatterTexture, 0, layer) );
		
		// レンダリング
		cgSetPassState(pass);
		{
			DrawQuad();
		}
		cgResetPassState(pass);
    }

	glDisable(GL_BLEND);
}


bool AtmosphericShader::IsValidEffect(CGeffect effect)
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
		//	tcout << _T("OK") << tendl;
		//}		
		techniques = cgGetNextTechnique(techniques);
	}
	
	return flag;
}


void AtmosphericShader::LoadCgEffect()
{
tcout << _T("AtmosphericShader::LoadCgEffect()...") << tendl;

	int i;

	//========================= エラーチェック ===========================//
	CGerror Error = cgGetError();
	if(Error)
	{
		tcout << cgGetErrorString(Error) << tendl;
		return; //exit(0);
	}
	
	//======================== コンテキスト作成 ==========================//
	CgContext = cgCreateContext();
	cgGLRegisterStates(CgContext);

	//==================== エフェクトファイルを読み込む ==================//
	CgEffect[PRECOMPUTE_TRANSMITTANCE] = cgCreateEffectFromFile(CgContext, "Shader/Transmittance.cgfx", NULL);
	cgSetEffectName(CgEffect[PRECOMPUTE_TRANSMITTANCE], "transmittance.cgfx");

	CgEffect[PRECOMPUTE_IRRADIANCE1] = cgCreateEffectFromFile(CgContext, "Shader/Irradiance1.cgfx", NULL);
	cgSetEffectName(CgEffect[PRECOMPUTE_IRRADIANCE1], "irradiance1.cgfx");

	CgEffect[PRECOMPUTE_INSCATTER1] = cgCreateEffectFromFile(CgContext, "Shader/Inscatter1.cgfx", NULL);
	cgSetEffectName(CgEffect[PRECOMPUTE_INSCATTER1], "inscatter1.cgfx");

	CgEffect[COPY_IRRADIANCE] = cgCreateEffectFromFile(CgContext, "Shader/CopyIrradiance.cgfx", NULL);
	cgSetEffectName(CgEffect[COPY_IRRADIANCE], "copyIrradiance.cgfx");

	CgEffect[COPY_INSCATTER1] = cgCreateEffectFromFile(CgContext, "Shader/CopyInscatter1.cgfx", NULL);
	cgSetEffectName(CgEffect[COPY_INSCATTER1], "copyInscatter1.cgfx");

	CgEffect[INSCATTER_S] = cgCreateEffectFromFile(CgContext, "Shader/InscatterS.cgfx", NULL);
	cgSetEffectName(CgEffect[INSCATTER_S], "InscatterS.cgfx");

	CgEffect[IRRADIANCE_N] = cgCreateEffectFromFile(CgContext, "Shader/IrradianceN.cgfx", NULL);
	cgSetEffectName(CgEffect[IRRADIANCE_N], "IrradianceN.cgfx");

	CgEffect[INSCATTER_N] = cgCreateEffectFromFile(CgContext, "Shader/InscatterN.cgfx", NULL);
	cgSetEffectName(CgEffect[INSCATTER_N], "InscatterN.cgfx");

	CgEffect[COPY_INSCATTER_N] = cgCreateEffectFromFile(CgContext, "Shader/CopyInscatterN.cgfx", NULL);
	cgSetEffectName(CgEffect[COPY_INSCATTER_N], "CopyInscatterN.cgfx");

	CgEffect[ATMOSPHERIC_SHADER] = cgCreateEffectFromFile(CgContext, "Shader/AtmosphericShader.cgfx", NULL);
	cgSetEffectName(CgEffect[ATMOSPHERIC_SHADER], "RenderSky.cgfx");


	for(i=0; i<m_numEffects; i++)
		if( IsValidEffect(CgEffect[i])==false )	return; //exit(0);
	
	for(i=0; i<m_numEffects; i++)
		CgTechnique[i] = cgGetFirstTechnique(CgEffect[i]);
	

	//=================== シェーダ変数の作成 ==================//
	m_epRg	= cgCreateParameter(CgContext, CG_FLOAT);
	m_epRt	= cgCreateParameter(CgContext, CG_FLOAT);

	m_epHR		= cgCreateParameter(CgContext, CG_FLOAT);
	m_epBetaR	= cgCreateParameter(CgContext, CG_FLOAT3);
	m_epHM		= cgCreateParameter(CgContext, CG_FLOAT);
	m_epBetaMSca = cgCreateParameter(CgContext, CG_FLOAT3);
	m_epMieG	= cgCreateParameter(CgContext, CG_FLOAT);

	m_epTRANSMITTANCE_W = cgCreateParameter(CgContext, CG_INT);
	m_epTRANSMITTANCE_H = cgCreateParameter(CgContext, CG_INT);
	m_epSKY_W	= cgCreateParameter(CgContext, CG_INT);
	m_epSKY_H	= cgCreateParameter(CgContext, CG_INT);
	m_epRES_R	= cgCreateParameter(CgContext, CG_INT);
	m_epRES_MU	= cgCreateParameter(CgContext, CG_INT);
	m_epRES_MU_S = cgCreateParameter(CgContext, CG_INT);
	m_epRES_NU	= cgCreateParameter(CgContext, CG_INT);


	cgSetParameter1f(m_epRt, Rt);
	cgSetParameter1f(m_epRg, Rg);

	cgSetParameter1f(m_epHR, m_HR);
	cgSetParameter3fv(m_epBetaR, m_betaR.xyz);
	cgSetParameter1f(m_epHM, m_HM);
	cgSetParameter3fv(m_epBetaMSca, m_betaMSca.xyz);
	cgSetParameter1f(m_epMieG, m_mieG);

	cgSetParameter1i(m_epTRANSMITTANCE_W, TRANSMITTANCE_W);
	cgSetParameter1i(m_epTRANSMITTANCE_H, TRANSMITTANCE_H);
	cgSetParameter1i(m_epSKY_W, SKY_W);
	cgSetParameter1i(m_epSKY_H, SKY_H);
	cgSetParameter1i(m_epRES_R, RES_R);
	cgSetParameter1i(m_epRES_MU, RES_MU);
	cgSetParameter1i(m_epRES_MU_S, RES_MU_S);
	cgSetParameter1i(m_epRES_NU, RES_NU);


	//======= 変数対応付け（シェーダファイル個々が保持する同一グローバル変数を一括変更できるようにする）==========//
	for(i=0; i<m_numEffects; i++)
	{
		if( cgGetNamedEffectParameter(CgEffect[i], "Rg") )
			cgConnectParameter( m_epRg, cgGetNamedEffectParameter(CgEffect[i], "Rg") );

		if( cgGetNamedEffectParameter(CgEffect[i], "Rt") )
			cgConnectParameter( m_epRt, cgGetNamedEffectParameter(CgEffect[i], "Rt") );

		if( cgGetNamedEffectParameter(CgEffect[i], "HR") )
			cgConnectParameter( m_epHR, cgGetNamedEffectParameter(CgEffect[i], "HR") );

		if( cgGetNamedEffectParameter(CgEffect[i], "betaR") )
			cgConnectParameter( m_epBetaR, cgGetNamedEffectParameter(CgEffect[i], "betaR") );

		if( cgGetNamedEffectParameter(CgEffect[i], "HM") )
			cgConnectParameter( m_epHM, cgGetNamedEffectParameter(CgEffect[i], "HM") );

		if( cgGetNamedEffectParameter(CgEffect[i], "betaMSca") )
			cgConnectParameter( m_epBetaMSca, cgGetNamedEffectParameter(CgEffect[i], "betaMSca") );

		if( cgGetNamedEffectParameter(CgEffect[i], "mieG") )
			cgConnectParameter( m_epMieG, cgGetNamedEffectParameter(CgEffect[i], "mieG") );

		if( cgGetNamedEffectParameter(CgEffect[i], "TRANSMITTANCE_W") )
			cgConnectParameter( m_epTRANSMITTANCE_W, cgGetNamedEffectParameter(CgEffect[i], "TRANSMITTANCE_W") );

		if( cgGetNamedEffectParameter(CgEffect[i], "TRANSMITTANCE_H") )
			cgConnectParameter( m_epTRANSMITTANCE_H, cgGetNamedEffectParameter(CgEffect[i], "TRANSMITTANCE_H") );

		if( cgGetNamedEffectParameter(CgEffect[i], "SKY_W") )
			cgConnectParameter( m_epSKY_W, cgGetNamedEffectParameter(CgEffect[i], "SKY_W") );

		if( cgGetNamedEffectParameter(CgEffect[i], "SKY_H") )
			cgConnectParameter( m_epSKY_H , cgGetNamedEffectParameter(CgEffect[i], "SKY_H") );

		if( cgGetNamedEffectParameter(CgEffect[i], "RES_R") )
			cgConnectParameter( m_epRES_R, cgGetNamedEffectParameter(CgEffect[i], "RES_R") );

		if( cgGetNamedEffectParameter(CgEffect[i], "RES_MU") )
			cgConnectParameter( m_epRES_MU, cgGetNamedEffectParameter(CgEffect[i], "RES_MU") );

		if( cgGetNamedEffectParameter(CgEffect[i], "RES_MU_S") )
			cgConnectParameter( m_epRES_MU_S, cgGetNamedEffectParameter(CgEffect[i], "RES_MU_S") );

		if( cgGetNamedEffectParameter(CgEffect[i], "RES_NU") )
			cgConnectParameter( m_epRES_NU, cgGetNamedEffectParameter(CgEffect[i], "RES_NU") );
	}

	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[ATMOSPHERIC_SHADER], "g_SkyColor"), RenderBuffer_SkyColor);
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[ATMOSPHERIC_SHADER], "g_Surface"), RenderBuffer_Surface);// 地面のテクスチャ情報
	cgGLSetTextureParameter(cgGetNamedEffectParameter(CgEffect[ATMOSPHERIC_SHADER], "g_StarField"), StarFieldTexture);// 
}



void AtmosphericShader::Draw(Camera *cam, Vec3f L, Vec3f linghtIntensity, unsigned int FinalImage, bool scatterflag)
{
	CGpass pass;

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	{
		// Cgパラメータ設定(RenderSky.cgfx)
		cgSetParameter3fv(cgGetNamedEffectParameter(CgEffect[ATMOSPHERIC_SHADER], "g_EyePos"), cam->GetPosition()->xyz);
		cgSetParameter3fv(cgGetNamedEffectParameter(CgEffect[ATMOSPHERIC_SHADER], "g_LightDir"), L.xyz);
		cgSetParameter3fv(cgGetNamedEffectParameter(CgEffect[ATMOSPHERIC_SHADER], "ISun"), linghtIntensity.xyz);
		cgSetMatrixParameterfr(cgGetNamedEffectParameter(CgEffect[ATMOSPHERIC_SHADER], "g_ModelViewMatrix"), &cam->GetViewProjectionMatrix()->m[0] );
		cgSetParameter1i(cgGetNamedEffectParameter(CgEffect[ATMOSPHERIC_SHADER], "g_EnableScattering"), scatterflag);
		

		//================================ 空だけレンダリング ==============================//
		glDrawBuffers(2, bufs);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, RenderBuffer_SkyColor, 0);// 空バッファのアタッチ
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, RenderBuffer_Surface, 0);// 球面形状バッファのアタッチ
		
		
		// レンダリングパス実行
		pass = cgGetNamedPass(CgTechnique[ATMOSPHERIC_SHADER], "RenderSky");
		cgSetPassState(pass);
		{	
			DrawCube(1.0e+3f);
		}
		cgResetPassState(pass);
		
		
		//============================== 地面だけレンダリング =============================//
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, FinalImage, 0);
		
		// レンダリングパス実行
		pass = cgGetNamedPass(CgTechnique[ATMOSPHERIC_SHADER], "RenderSurface");
		cgSetPassState(pass);
		{	
			DrawQuad();
		}
		cgResetPassState(pass);

	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void AtmosphericShader::SetScreenSize(int width, int height)// 描画領域の大きさをセットする
{
	//============= 新しい解像度でテクスチャを作り直す ============//
	ReallocateTexture2D(RenderBuffer_SkyColor, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);// 空だけレンダリングするバッファ	
	ReallocateTexture2D(RenderBuffer_Surface, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);// 球面形状を保存するバッファ

}




//// Mie散乱デフォルトパラメータのサンプル
//
//
//// DEFAULT
//float	HM = 1.2;
//float3	betaMSca = float3(4.0e-3);
//float	mieG = 0.8;
//static float3	betaMEx = betaMSca / 0.9;
//
//// CLEAR SKY
//float HM = 1.2;
//float3 betaMSca = float3(20.0e-3);
//static float3 betaMEx = betaMSca / 0.9;
//float mieG = 0.76;
//
//// PARTLY CLOUDY
//float HM = 3.0;
//float3 betaMSca = float3(3.0e-3);
//static float3 betaMEx = betaMSca / 0.9;
//float mieG = 0.65;
