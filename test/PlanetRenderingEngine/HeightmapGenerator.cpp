#include "HeightmapGenerator.h"

#include	<graphics/gl4x/resource/Texture.h>
using namespace OreOreLib;
#include	<graphics/gl4x/other/MyGlPrimitive.h>
using namespace MyGlPrimitive;



HeightmapGenerator::HeightmapGenerator(int mapsize, float border, float heightrange)
{
tcout << _T("HeightmapGenerator::HeightmapGenerator()...") << tendl;

	m_HeightRange	= heightrange;
	m_MapSize		= mapsize;
	m_BorderSize	= border;
	m_FoV			= 2.0f * atan(float(m_MapSize/2) / (float(m_MapSize/2) - m_BorderSize));
	
	m_Camera		= new Camera();

	//========================== テクスチャの初期化 ==============================//
	m_PermTexture		= AllocateTexture1D(GL_NEAREST, GL_NEAREST, 256, GL_R32F, GL_RED, GL_FLOAT);
	m_PermTexture2d		= AllocateTexture2D(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT, 256, 256, GL_RGBA32F, GL_RGBA, GL_FLOAT);
	m_PermGradTexture	= AllocateTexture1D(GL_NEAREST, GL_NEAREST, 256, GL_RGBA32F, GL_RGBA, GL_FLOAT);
	m_GradTexture4d		= AllocateTexture1D(GL_NEAREST, GL_NEAREST, 32, GL_RGBA32F, GL_RGBA, GL_FLOAT);
	
	m_HeightBuffer	= AllocateTexture2D(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_MapSize, m_MapSize, GL_R32F, GL_RED, GL_FLOAT);
	m_PosBuffer		= AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_MapSize, m_MapSize, GL_RGB32F, GL_RGB, GL_FLOAT);
	m_NormalBuffer	= AllocateTexture2D(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_MapSize, m_MapSize, GL_RGBA32F, GL_RGBA, GL_FLOAT);

	//====================== フレームバッファオブジェクト初期化 ======================//
	m_fbo = NULL;
	glGenFramebuffers(1, &m_fbo);

	//============================= エフェクトのロード ===============================//
	LoadCgEffect();
	
	InitLookupTable();

	// ルックアップテクスチャの参照
	cgGLSetTextureParameter(m_epPermSampler, m_PermTexture);
	cgGLSetTextureParameter(m_epPermSampler2d, m_PermTexture2d);
	cgGLSetTextureParameter(m_epPermGradSampler, m_PermGradTexture);
	cgGLSetTextureParameter(m_epGradSampler4d, m_GradTexture4d);
	
}


HeightmapGenerator::~HeightmapGenerator()
{
tcout << _T("HeightmapGenerator::~HeightmapGenerator()...") << tendl;

	//====== フレームバッファオブジェクトの削除 ======//
	if(m_fbo)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, NULL, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, NULL, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &m_fbo);
	}

	//=============== テクスチャの削除 ================//
	if(m_PermTexture){		glDeleteTextures(1, &m_PermTexture);	m_PermTexture = NULL; }
	if(m_PermTexture2d){	glDeleteTextures(1, &m_PermTexture2d);	m_PermTexture2d = NULL; }
	if(m_PermGradTexture){	glDeleteTextures(1, &m_PermGradTexture);m_PermGradTexture = NULL; }
	if(m_GradTexture4d){	glDeleteTextures(1, &m_GradTexture4d);	m_GradTexture4d = NULL; }

	if(m_HeightBuffer){		glDeleteTextures(1, &m_HeightBuffer);	m_HeightBuffer = NULL; }
	if(m_PosBuffer){		glDeleteTextures(1, &m_PosBuffer);		m_PosBuffer = NULL; }
	if(m_NormalBuffer){		glDeleteTextures(1, &m_NormalBuffer);	m_NormalBuffer = NULL; }

	//================= シェーダの削除 ===============//
	cgDestroyContext(m_CgContext);
	cgDestroyEffect(m_CgEffect);

	if(m_Camera)	delete m_Camera;
}


void HeightmapGenerator::SetScale(float horizontal, float vertical)
{
	m_HorizontalScale	= horizontal;
	m_VerticalScale		= vertical;

	//==================== 対応付け ==========================//
	cgSetParameter1f(m_epHorizontalScale, m_HorizontalScale );
	cgSetParameter1f(m_epVerticalScale, m_VerticalScale );
}


void HeightmapGenerator::SetMapSize(int mapsize)
{
tcout << _T("HeightmapGenerator::SetMapSize()") << tendl;

	//============== 透視パラメータの更新 ==================//
	m_MapSize	= mapsize;
	m_FoV		= 2.0f * atan(float(m_MapSize/2) / (float(m_MapSize/2) - m_BorderSize));

	//==== フレームバッファオブジェクトにアタッチしたテクスチャを外す ====//
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, NULL, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, NULL, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//================= テクスチャの再度確保 ===============//
	ReallocateTexture2D(m_HeightBuffer, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_MapSize, m_MapSize, GL_R32F, GL_RED, GL_FLOAT);
	ReallocateTexture2D(m_PosBuffer,  GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_MapSize, m_MapSize, GL_RGB32F, GL_RGB, GL_FLOAT);
	ReallocateTexture2D(m_NormalBuffer, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_MapSize, m_MapSize, GL_RGBA32F, GL_RGBA, GL_FLOAT);
}





// ハイトマップおよび法線マップを生成する
void HeightmapGenerator::Draw(	uint32 tex_height, uint32 tex_normal,
								int xoffset, int yoffset, float dimension,
								const Vec3f &pos, const Vec3f &dn, const Vec3f &dv)
{
	unsigned int bufs[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	
	glViewport(0,0,m_MapSize,m_MapSize);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	{	
		glClearColor(0,0,0,0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, bufs[0], GL_TEXTURE_2D, m_HeightBuffer, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, bufs[1], GL_TEXTURE_2D, m_PosBuffer, 0);
		glDrawBuffers(2, bufs);

		//========================= ハイトマップの計算 ==============================//
		m_Camera->SetViewParameter(pos, dn, dv);
		m_Camera->SetProjectionParameter( 1.0f, -m_FoV, 0.01f, dimension*0.5f );// fovyマイナスで画面が上下反転，aspectマイナスで画面が左右反転.CullFaceも反転！！
		m_Camera->Update();

		m_CgPass = cgGetFirstPass( m_etGenSphericalHeightMap );
		
		cgSetParameter1f( cgGetNamedEffectParameter(m_CgEffect, "g_Radius"), dimension*0.5f );
		cgSetParameter1f( cgGetNamedEffectParameter(m_CgEffect, "g_HeightRange"), m_HeightRange);
		cgSetMatrixParameterfr( cgGetNamedEffectParameter(m_CgEffect, "g_matViewProj"), m_Camera->GetViewProjectionMatrix()->m );


		cgSetPassState(m_CgPass);
		{
			DrawCube(dimension*0.5f);
		}
		cgResetPassState(m_CgPass);


		glReadBuffer(bufs[0]);
		glEnable(GL_TEXTURE_2D);
		glBindTexture( GL_TEXTURE_2D, tex_height );
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, xoffset,yoffset, 0,0, m_MapSize, m_MapSize);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);

		
		//============================ 法線マップの計算 =============================//
		glClearColor(0,0,0,0);
		//チャンネル1を無効化する
		glFramebufferTexture2D(GL_FRAMEBUFFER, bufs[1], GL_TEXTURE_2D, 0, 0);
		// チャンネル1を有効化し，法線テクスチャをフレームバッファに割り当てる．
		glFramebufferTexture2D(GL_FRAMEBUFFER, bufs[0], GL_TEXTURE_2D, m_NormalBuffer, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		m_CgPass = cgGetNextPass(m_CgPass);

		cgGLSetTextureParameter( cgGetNamedEffectParameter(m_CgEffect, "g_PosSampler"), m_PosBuffer);
		
		cgSetPassState(m_CgPass);
		{
			DrawQuad();
		}
		cgResetPassState(m_CgPass);
		

		glReadBuffer(bufs[0]);
		glEnable(GL_TEXTURE_2D);
		glBindTexture( GL_TEXTURE_2D, tex_normal );
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, xoffset,yoffset, 0,0, m_MapSize, m_MapSize);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
		
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glFinish();
}


// LookupTableを生成する
void HeightmapGenerator::InitLookupTable()
{
tcout << _T("HeightmapGenerator::InitLookupTable()...") << tendl;
	
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	{
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		GeneratePermTexture();
		GeneratePermTexture2d();
		GeneratePermGradTexture();
		GenerateGradTexture4d();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glFinish();
}



void HeightmapGenerator::GeneratePermTexture()// 1channel
{
tcout << _T("HeightmapGenerator::GeneratePermTexture()...") << tendl;
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_PermTexture, 0);
	
	glClearColor(0,0,0,0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, 256, 1);


	m_CgPass = cgGetNamedPass(m_etInitLookupTable, "GenPermTexture");

	cgSetPassState(m_CgPass);
	{	
		DrawLine();
	}
	cgResetPassState(m_CgPass);
}


void HeightmapGenerator::GeneratePermTexture2d()//4channel
{
tcout << _T("HeightmapGenerator::GeneratePermTexture2d()...") << tendl;
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_PermTexture2d, 0);

	glClearColor(0,0,0,0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, 256, 256);

	m_CgPass = cgGetNamedPass(m_etInitLookupTable, "GenPermTexture2d");

	cgSetPassState(m_CgPass);
	{	
		DrawQuad();
	}
	cgResetPassState(m_CgPass);
}


void HeightmapGenerator::GeneratePermGradTexture()//4channel
{
tcout << _T("HeightmapGenerator::GeneratePermGradTexture()...") << tendl;
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_PermGradTexture, 0);

	glClearColor(1,1,1,0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, 256, 1);

	m_CgPass = cgGetNamedPass(m_etInitLookupTable, "GenPermGradTexture");

	cgSetPassState(m_CgPass);
	{		
		DrawLine();
	}
	cgResetPassState(m_CgPass);
}


void HeightmapGenerator::GenerateGradTexture4d()//4channel
{	
tcout << _T("HeightmapGenerator::GenerateGradTexture4d()...") << tendl;
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_GradTexture4d, 0);

	glClearColor(0,0,0,0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, 32, 1);

	m_CgPass = cgGetNamedPass(m_etInitLookupTable, "GenGradTexture4d");

	cgSetPassState(m_CgPass);
	{
		DrawLine();
	}
	cgResetPassState(m_CgPass);
}


void HeightmapGenerator::LoadCgEffect()
{
tcout << _T("HeightmapGenerator::LoadCgEffect()...") << tendl;

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
	m_CgEffect = cgCreateEffectFromFile(m_CgContext, "Shader/HeightmapGenerator.cgfx", NULL);
	cgSetEffectName(m_CgEffect, "HeightmapGenerator.cgfx");

	if(IsValidEffect(m_CgEffect)==false) return; //exit(0);


	//=================== テクニック取得 ======================//
	m_etInitLookupTable	= cgGetNamedTechnique(m_CgEffect, "InitLookupTable");
	m_etGenSphericalHeightMap	= cgGetNamedTechnique(m_CgEffect, "GenSphericalHeightMap");
	
	//==================== シェーダ変数定義 ===================//
	m_epPermSampler		= cgGetNamedEffectParameter(m_CgEffect, "permSampler");
	m_epPermSampler2d	= cgGetNamedEffectParameter(m_CgEffect, "permSampler2d");
	m_epPermGradSampler	= cgGetNamedEffectParameter(m_CgEffect, "permGradSampler");
	m_epGradSampler4d	= cgGetNamedEffectParameter(m_CgEffect, "gradSampler4d");

	m_epMapSize			= cgGetNamedEffectParameter(m_CgEffect, "g_MapSize");
	m_epHeightRange		= cgGetNamedEffectParameter(m_CgEffect, "g_HeightRange");
	m_epHorizontalScale	= cgGetNamedEffectParameter(m_CgEffect, "g_HorizontalScale");
	m_epVerticalScale	= cgGetNamedEffectParameter(m_CgEffect, "g_VerticalScale");
	
	//==================== 対応付け ==========================//
	cgSetParameter2f(m_epMapSize, float(m_MapSize), 1.0f/float(m_MapSize) );

}


bool HeightmapGenerator::IsValidEffect(CGeffect effect)
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
			//tcout << _T("Error : Invalid Technique") << tendl;
		}
		//else
		//{
		//	tcout << _T("OK") << tendl;
		//}		
		techniques = cgGetNextTechnique(techniques);
	}
	
	return flag;
}