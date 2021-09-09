#include	"G_Buffer.h"


#include	<graphics/gl4x/resource/Texture.h>
using namespace OreOreLib;
#include	<graphics/gl4x/other/MyGlPrimitive.h>
using namespace MyGlPrimitive;




G_Buffer::G_Buffer()
{
tcout << _T("G_Buffer::G_Buffer()...") << tendl;

	m_PositionMap	= NULL;
	m_NormalMap		= NULL;
	m_DiffuseMap	= NULL;
	m_SpecularMap	= NULL;
}



G_Buffer::G_Buffer(int width, int height)
{
tcout << _T("G_Buffer::G_Buffer(int width, int height)...") << tendl;

	m_PositionMap	= AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
	m_NormalMap		= AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, width, height, GL_RGB16F, GL_RGB, GL_FLOAT);
	m_DiffuseMap	= AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, width, height, GL_RGBA8, GL_RGBA, GL_FLOAT);
	m_SpecularMap	= AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, width, height, GL_RGBA8, GL_RGBA, GL_FLOAT);

	LoadCgEffect();

	cgGLSetTextureParameter(m_epPosBuffer,		m_PositionMap);
	cgGLSetTextureParameter(m_epNormalBuffer,	m_NormalMap);
	cgGLSetTextureParameter(m_epColorBuffer,	m_DiffuseMap);
	cgGLSetTextureParameter(m_epSpecularBuffer,	m_SpecularMap);
}


G_Buffer::~G_Buffer()
{
tcout << _T("G_Buffer::~G_Buffer()...") << tendl;

	if(m_DiffuseMap){	glDeleteTextures(1, &m_DiffuseMap); m_DiffuseMap = NULL; }
	if(m_SpecularMap){	glDeleteTextures(1, &m_SpecularMap); m_SpecularMap = NULL; }
	if(m_NormalMap){	glDeleteTextures(1, &m_NormalMap); m_NormalMap = NULL; }
	if(m_PositionMap){	glDeleteTextures(1, &m_PositionMap); m_PositionMap = NULL; }

	cgDestroyEffect(m_CgEffect);
	cgDestroyContext(m_CgContext);
}


void G_Buffer::SetSize(int width, int height)
{
	ReallocateTexture2D(m_PositionMap, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
	ReallocateTexture2D(m_NormalMap, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, width, height, GL_RGB16F, GL_RGB, GL_FLOAT);
	ReallocateTexture2D(m_DiffuseMap, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, width, height, GL_RGBA8, GL_RGBA, GL_FLOAT);
	ReallocateTexture2D(m_SpecularMap, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, width, height, GL_RGBA8, GL_RGBA, GL_FLOAT);

	cgGLSetTextureParameter(m_epPosBuffer,		m_PositionMap);
	cgGLSetTextureParameter(m_epNormalBuffer,	m_NormalMap);
	cgGLSetTextureParameter(m_epColorBuffer,	m_DiffuseMap);
	cgGLSetTextureParameter(m_epSpecularBuffer,	m_SpecularMap);
}


void G_Buffer::ShowDiffuseMap( int width, int height )
{
	DrawTexture_RGB( m_DiffuseMap, width, height );
}


void G_Buffer::ShowNormalMap( int width, int height )
{
	DrawTexture_RGB( m_NormalMap, width, height );
}


void G_Buffer::ShowSpecularMap( int width, int height )
{
	DrawTexture_RGB( m_SpecularMap, width, height, 50.0 );
}


void G_Buffer::ShowHeightMap( int width, int height, float heightscale )
{
	DrawTexture_A( m_PositionMap, width, height, heightscale );
}


void G_Buffer::DrawTexture_RGB( unsigned int texture, int width, int height, float color_scale )
{
	glViewport(0, 0, width, height);

	m_CgPass = cgGetNamedPass(m_etDeferredRendering, "DrawTexture_RGB");

	cgSetParameter1f(cgGetNamedEffectParameter(m_CgEffect, "g_Scale"), color_scale);
	cgGLSetTextureParameter(cgGetNamedEffectParameter(m_CgEffect, "g_Sampler"),	texture);

	cgSetPassState(m_CgPass);
	{
		DrawQuad();
	}
	cgResetPassState(m_CgPass);
}


void G_Buffer::DrawTexture_A( unsigned int texture, int width, int height, float color_scale )
{
	glViewport(0, 0, width, height);

	m_CgPass = cgGetNamedPass(m_etDeferredRendering, "DrawTexture_A");

	cgSetParameter1f(cgGetNamedEffectParameter(m_CgEffect, "g_Scale"), color_scale);
	cgGLSetTextureParameter(cgGetNamedEffectParameter(m_CgEffect, "g_Sampler"),	texture);

	cgSetPassState(m_CgPass);
	{
		DrawQuad();
	}
	cgResetPassState(m_CgPass);
}



void G_Buffer::DrawShadedImage( const Vec3f &eyePos, const Vec3f &lightDir, const Vec3f &lightIntensity)
{
	m_CgPass = cgGetNamedPass(m_etDeferredRendering, "DrawShadedImage");

	cgSetParameter3fv(m_epLightDir, lightDir.xyz);
	cgSetParameter3fv(m_epLightIntensity, lightIntensity.xyz);
	cgSetParameter3fv(m_epEyePos, eyePos.xyz);

	cgSetPassState(m_CgPass);
	{
		DrawQuad();
	}
	cgResetPassState(m_CgPass);
}


void G_Buffer::Draw( int width, int height )
{
	glViewport(0, 0, width, height);

	m_CgPass = cgGetNamedPass(m_etDeferredRendering, "DrawGbuffer");

	cgSetPassState(m_CgPass);
	{
		DrawQuad();
	}
	cgResetPassState(m_CgPass);
}




void G_Buffer::LoadCgEffect()
{
tcout << _T("G_Buffer::LoadCgEffect()...") << tendl;

	//===================== エラーチェック ====================//
	CGerror Error = cgGetError();
	if(Error)
	{
		tcout << cgGetErrorString(Error) << tendl;
		return;
	}

	//===================== コンテキスト作成 ==================//
	m_CgContext = cgCreateContext();
	cgGLRegisterStates(m_CgContext);

	//============== エフェクトファイル読み込み ===============//
	m_CgEffect = cgCreateEffectFromFile(m_CgContext, "Shader/G_Buffer.cgfx", NULL);
	cgSetEffectName(m_CgEffect, "G_Buffer.cgfx");

	if(IsValidEffect(m_CgEffect)==false) return;

	//=================== テクニック取得 ======================//
	m_etDeferredRendering	= cgGetNamedTechnique(m_CgEffect, "DeferredRendering");

	//==================== シェーダ変数定義 ===================//
	m_epNormalBuffer	= cgGetNamedEffectParameter(m_CgEffect, "g_NormalBuffer");
	m_epPosBuffer		= cgGetNamedEffectParameter(m_CgEffect, "g_PosBuffer");
	m_epColorBuffer		= cgGetNamedEffectParameter(m_CgEffect, "g_ColorBuffer");
	m_epSpecularBuffer	= cgGetNamedEffectParameter(m_CgEffect, "g_SpecularBuffer");

	m_epLightDir		= cgGetNamedEffectParameter(m_CgEffect, "g_LightDir");
	m_epLightIntensity	= cgGetNamedEffectParameter(m_CgEffect, "g_LightIntensity");
	m_epEyePos			= cgGetNamedEffectParameter(m_CgEffect, "g_EyePos");
	
	//==================== 対応付け ==========================//
	
}


bool G_Buffer::IsValidEffect(CGeffect effect)
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
			tcout << _T("Error: Invalid Technique.... cgGetTechniqueName(techniques)") << tendl;
		}
		else
		{
			//tcout << _T("OK") << tendl;			
		}		
		techniques = cgGetNextTechnique(techniques);
	}
	
	return flag;
}