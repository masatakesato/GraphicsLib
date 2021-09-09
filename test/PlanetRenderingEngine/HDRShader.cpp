#include	"HDRShader.h"

#include	<math.h>

#include	<graphics/gl4x/resource/Texture.h>
using namespace OreOreLib;



static void DrawQuad()
{
	float	TexCoordArray[8] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f };
	float	VertexArray[8] = { -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f };
	GLuint	index[6] = { 0,1,2,	3,0,2 };
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glTexCoordPointer(2, GL_FLOAT, 0, TexCoordArray);
	glVertexPointer(2, GL_FLOAT, 0, VertexArray);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, index);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}






HDRShader::HDRShader()
{
	m_Width4x4	= 0;
	m_Height4x4	= 0;

	m_ScaledImage		= NULL;
	for(int i=0; i<4; i++){m_Luminance_curr[i]=NULL;}
	m_Luminance_prev	= NULL;
	m_AdaptedLuminance	= NULL;
	m_BrightPass		= NULL;
	m_BrightPass2x2		= NULL;
	m_BlurredImage		= NULL;
	m_BlurredImage2		= NULL;

	m_fbo				= NULL;
}




HDRShader::HDRShader(int width, int height)
{
tcout << _T("HDRShader::HDRShader()...") << tendl;

	m_Width4x4 = width/4;
	m_Height4x4 = height/4;

	//==================== テクスチャ初期化 =========================//
	//GL_NEAREST, GL_NEAREST,
	m_ScaledImage	= AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_Width4x4, m_Height4x4, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	for(int i=3; i>=0; i--)
		m_Luminance_curr[i] = AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, (int)pow(4.0, i), (int)pow(4.0, i), GL_R16F, GL_RED, GL_FLOAT);
	m_Luminance_prev	= AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, 1, 1, GL_R16F, GL_RED, GL_FLOAT);
	m_AdaptedLuminance	=AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, 1, 1, GL_R16F, GL_RED, GL_FLOAT);
	
	m_BrightPass	= AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_Width4x4, m_Height4x4, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	m_BrightPass2x2	= AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_Width4x4/2, m_Height4x4/2, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	m_BlurredImage	= AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_Width4x4/2, m_Height4x4/2, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	m_BlurredImage2	= AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_Width4x4/2, m_Height4x4/2, GL_RGBA16F, GL_RGBA, GL_FLOAT);

	//===================== シェーダ初期化 ==========================//
	LoadCgEffect();

	glGenFramebuffers(1, &m_fbo);

}


HDRShader::~HDRShader()
{
tcout << _T("HDRShader::~HDRShader~()...") << tendl;

	//====== フレームバッファオブジェクトの削除 ======//
	if(m_fbo)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, NULL, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &m_fbo);
		m_fbo = NULL;
	}

	//================== テクスチャ削除 ==============//
	if(m_ScaledImage){		glDeleteTextures(1, &m_ScaledImage);	m_ScaledImage = NULL; }
	if(m_Luminance_curr)
	{
		glDeleteTextures(4, m_Luminance_curr);
		m_Luminance_curr[0] = NULL; m_Luminance_curr[1] = NULL; m_Luminance_curr[2] = NULL; m_Luminance_curr[3] = NULL;
	}
	if(m_Luminance_prev){	glDeleteTextures(1, &m_Luminance_prev);	m_Luminance_prev= NULL; }
	if(m_AdaptedLuminance){	glDeleteTextures(1, &m_AdaptedLuminance); m_AdaptedLuminance= NULL; }
	if(m_BrightPass){		glDeleteTextures(1, &m_BrightPass);		m_BrightPass= NULL; }
	if(m_BrightPass2x2){	glDeleteTextures(1, &m_BrightPass2x2);	m_BrightPass2x2= NULL; }
	if(m_BlurredImage){		glDeleteTextures(1, &m_BlurredImage);	m_BlurredImage= NULL; }
	if(m_BlurredImage2){	glDeleteTextures(1, &m_BlurredImage2);	m_BlurredImage2= NULL; }

	//================= シェーダの削除 ===============//
	cgDestroyContext(m_CgContext);
	cgDestroyEffect(m_CgEffect);
}




void HDRShader::DoHDRLighting(unsigned int IN, float timestep, float key, float bloomweight, int width, int height)
{
	cgSetParameter1f(m_epKey, key);
	cgSetParameter1f(m_epBloomWeight, bloomweight);
	cgSetParameter1f(m_epTimeStep, timestep);

	//========== 画像からぼかしテクスチャを作る ==========//
	GenScaledImage(IN);
	GenAverageLuminance();
	AdaptLuminance();
	FilterBlightPass();
	BlurImage();

	//===== 画像とぼかしテクスチャとを合成する =====//
	RenderFinalImage(IN, width, height);

	//======== 輝度テクスチャをスワップする ========//
	unsigned int tmp;
	tmp = m_Luminance_prev;
	m_Luminance_prev = m_AdaptedLuminance;
	m_AdaptedLuminance = tmp;

}



// 画像INの縮小イメージを作成する
void HDRShader::GenScaledImage(unsigned int IN)
{
	glViewport(0, 0, m_Width4x4, m_Height4x4);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	{
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ScaledImage, 0);

		m_CgPass = cgGetNamedPass(m_etBasic, "DownSample4x4");
		
		cgGLSetTextureParameter(m_epSourceImage1, IN);
		cgSetParameter2f(m_epTexOffset, 0.5f/(float)m_Width4x4, 0.5f/(float)m_Height4x4);

		cgSetPassState(m_CgPass);
		{
			DrawQuad();
		}
		cgResetPassState(m_CgPass);

	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


// 縮小イメージから平均輝度を計算する
void HDRShader::GenAverageLuminance()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	{
		//==================== 64x64のテクスチャにlog()値を書き込む ====================//
		m_CgPass = cgGetNamedPass(m_etBasic, "ClaculateLogLuminance");

		// ScaledImageを参照先にセットする
		cgGLSetTextureParameter(m_epSourceImage1, m_ScaledImage);
		cgSetParameter2f(m_epTexOffset, 0.5f/64.0f, 0.5f/64.0f);

		// レンダリング
		glViewport(0, 0, 64, 64);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Luminance_curr[3], 0);
		
		cgSetPassState(m_CgPass);
		{
			DrawQuad();
		}
		cgResetPassState(m_CgPass);

	
		//==================== logテクスチャを縮小して平均を求める ====================//
		m_CgPass = cgGetNamedPass(m_etBasic, "AverageLogLuminance");

		cgSetParameter1i(cgGetNamedEffectParameter(m_CgEffect, "g_CalcExponential"), 0);

		for(int i=2; i>=0; i--)
		{
			// 一つ上の解像度のテクスチャを参照先にセットする
			cgGLSetTextureParameter(m_epSourceImage1, m_Luminance_curr[i+1]);
			cgSetParameter2f(m_epTexOffset, float(0.5f/pow(4.0f,i)), float(0.5f/pow(4.0f,i)) );
			if(i==0) cgSetParameter1i(cgGetNamedEffectParameter(m_CgEffect, "g_CalcExponential"), 1);
			
			// レンダリング
			glViewport(0, 0, (int)pow(4.0,i), (int)pow(4.0,i));
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Luminance_curr[i], 0);
			
			cgSetPassState(m_CgPass);
			{
				DrawQuad();
			}
			cgResetPassState(m_CgPass);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


// 明順応のシミュレーション
void HDRShader::AdaptLuminance()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	{
		m_CgPass = cgGetNamedPass(m_etBasic, "ClaculateAdaptedLuminance");

		cgGLSetTextureParameter(m_epSourceImage1, m_Luminance_prev);// 昔の輝度
		cgGLSetTextureParameter(m_epSourceImage2, m_Luminance_curr[0]);// 今の輝度

		// レンダリング
		glViewport(0, 0, 1, 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_AdaptedLuminance, 0);

		cgSetPassState(m_CgPass);
		{
			DrawQuad();
		}
		cgResetPassState(m_CgPass);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



// 縮小イメージの明るい領域だけ取り出す
void HDRShader::FilterBlightPass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	{
		//================ 輝度値の高いピクセルだけ抽出する ================//
		m_CgPass = cgGetNamedPass(m_etBasic, "FilterBrightPass");

		cgGLSetTextureParameter(m_epSourceImage1, m_ScaledImage);
		cgGLSetTextureParameter(m_epLuminance, m_AdaptedLuminance);

		glViewport(0, 0, m_Width4x4, m_Height4x4);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BrightPass, 0);

		cgSetPassState(m_CgPass);
		{
			DrawQuad();
		}
		cgResetPassState(m_CgPass);
		
		//=================== 画像サイズを縮小する(1/2) ====================//
		m_CgPass = cgGetNamedPass(m_etBasic, "DownSample2x2");

		cgGLSetTextureParameter(m_epSourceImage1, m_BrightPass);
		
		glViewport(0, 0, m_Width4x4/2, m_Height4x4/2);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BrightPass2x2, 0);

		cgSetPassState(m_CgPass);
		{
			DrawQuad();
		}
		cgResetPassState(m_CgPass);
		
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


// 画像をぼかす
void HDRShader::BlurImage()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	{
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, m_Width4x4/2, m_Height4x4/2);

		//========================= 縦方向にぼかしをかける ========================//
		m_CgPass = cgGetNamedPass(m_etBasic, "VerticalBlur");

		cgGLSetTextureParameter(m_epSourceImage1, m_BrightPass);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BlurredImage, 0);

		cgSetPassState(m_CgPass);
		{
			DrawQuad();
		}
		cgResetPassState(m_CgPass);
		

		//========================= 横方向にぼかしをかける ========================//
		m_CgPass = cgGetNamedPass(m_etBasic, "HorizontalBlur");
	
		cgGLSetTextureParameter(m_epSourceImage1, m_BlurredImage);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BlurredImage2, 0);

		cgSetPassState(m_CgPass);
		{
			DrawQuad();
		}
		cgResetPassState(m_CgPass);
		
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


// ぼかし画像と合成
void HDRShader::RenderFinalImage(unsigned int IN, int width, int height)
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_CgPass = cgGetNamedPass(m_etBasic, "FinalImage");

	cgGLSetTextureParameter(m_epSourceImage1, IN);
	cgGLSetTextureParameter(m_epSourceImage2, m_BlurredImage2);
	
	glViewport(0, 0, width, height);

	cgSetPassState(m_CgPass);
	{
		DrawQuad();
	}
	cgResetPassState(m_CgPass);
}






void HDRShader::LoadCgEffect()
{
tcout << _T("HDRShader::LoadCgEffect()...") << tendl;

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
	m_CgEffect = cgCreateEffectFromFile(m_CgContext, "Shader/HDRShader.cgfx", NULL);
	cgSetEffectName(m_CgEffect, "HDRShader.cgfx");

	if(IsValidEffect(m_CgEffect)==false) return; //exit(0);

	//=================== テクニック取得 ======================//
	m_etBasic	= cgGetNamedTechnique(m_CgEffect, "Basic");
	
	//==================== シェーダ変数定義 ===================//
	m_epTexOffset		= cgGetNamedEffectParameter(m_CgEffect, "g_offset");
	m_epTexSize			= cgGetNamedEffectParameter(m_CgEffect, "g_TexSize");
	m_epSourceImage1	= cgGetNamedEffectParameter(m_CgEffect, "g_Sampler1");
	m_epSourceImage2	= cgGetNamedEffectParameter(m_CgEffect, "g_Sampler2");
	m_epLuminance		= cgGetNamedEffectParameter(m_CgEffect, "g_Luminance");
	m_epTimeStep		= cgGetNamedEffectParameter(m_CgEffect, "g_TimeStep");
	m_epKey				= cgGetNamedEffectParameter(m_CgEffect, "g_Key");
	m_epBloomWeight		= cgGetNamedEffectParameter(m_CgEffect, "g_BloomWeight");

	//==================== 対応付け ==========================//
	cgSetParameter2f(m_epTexSize, 0.5f*(float)m_Width4x4, 0.5f*(float)m_Height4x4);
	cgSetParameter1f(m_epKey, 0.18f);
}


bool HDRShader::IsValidEffect(CGeffect effect)
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
			tcout << _T("Error: Invalid Technique... ") << cgGetTechniqueName(techniques) << tendl;
		}
		//else
		//{
		//	tcout << _T("OK") << tendl;			
		//}		
		techniques = cgGetNextTechnique(techniques);
	}
	
	return flag;
}


void HDRShader::SetScreenSize(int width, int height)
{
	m_Width4x4 = width/4;
	m_Height4x4 = height/4;

	//==================== テクスチャ再生成 =========================//
	ReallocateTexture2D(m_ScaledImage, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_Width4x4, m_Height4x4, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	ReallocateTexture2D(m_BrightPass, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_Width4x4, m_Height4x4, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	ReallocateTexture2D(m_BrightPass2x2, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_Width4x4/2, m_Height4x4/2, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	ReallocateTexture2D(m_BlurredImage, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_Width4x4/2, m_Height4x4/2, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	ReallocateTexture2D(m_BlurredImage2, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_Width4x4/2, m_Height4x4/2, GL_RGBA16F, GL_RGBA, GL_FLOAT);

	//============== Cgエフェクトファイルの変数対応付け ============//
	cgSetParameter2f(m_epTexSize, 0.5f*(float)m_Width4x4, 0.5f*(float)m_Height4x4);
}