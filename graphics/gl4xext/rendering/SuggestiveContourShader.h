#ifndef	SUGGESTIVE_CONTOUR_SHADER_H
#define	SUGGESTIVE_CONTOUR_SHADER_H



#include	"../oreore/IRenderer.h"
#include	"../oreore/GLShader.h"



namespace OreOreLib
{


class SuggestiveContourShader : public IRenderer
{
private:

	// shader
	GLShader	*m_pShader;	// デフォルトのシェーダー

	// fbo
	FramebufferObject	*m_Fbo;


	// uniform location	
	GLint		m_ulTexture;	// texture
	GLint		m_ulScale;		// color scale;



	
public:

	SuggestiveContourShader();
	SuggestiveContourShader( const char* filepath );
	~SuggestiveContourShader();

	void Release();

	void InitShader( const char* filepath );
	
	inline virtual void BindShader( int passid=0 )
	{
		if( CurrentShader == m_pShader )	return;
	
		CurrentShader = m_pShader;
		CurrentShader->Bind();
	}


	void Render( uint32 texid, float scale=1.0f );
	virtual void Render( MovableObject *pObj, Frustum *pfrustum ){};
	
};



// SuggestiveContourShaderオブジェクトを生成するファクトリクラス
class SuggestiveContourShaderFactory : IRendererFactory
{
private:

	IRenderer* CreateInstance()
	{
		return new SuggestiveContourShader( "labworks/SuggestiveContourShader.glsl" );
	}

public:

	SuggestiveContourShaderFactory(){};
	//~SuggestiveContourShaderFactory(){};
};




}// end of namespace


#endif	// SUGGESTIVE_CONTOUR_SHADER_H //