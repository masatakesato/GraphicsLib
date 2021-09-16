#ifndef	SILHOUETTE_SHADER_H
#define	SILHOUETTE_SHADER_H

#include	<graphics/gl4x/shader/IShader.h">
#include	<graphics/gl4x/shader/GLShader.h>



namespace OreOreLib
{

// 輪郭線描画シェーダー
class SilhouetteShader : public IShader
{
private:

	// shader
	GLShader	*m_pShader;
	
	// uniform location
	GLint		m_ulMatWorld;			// world matrix
	GLint		m_ulMatViewProj;		// view projection matrix
	
	GLint		m_ulTexture;			// texture
	GLint		m_ulDepthMap;
	GLint		m_ulNormalMap;
	GLint		m_ulDiffuse;			// diffuse material TODO: Add entire material





	GLint		m_ulLineWidth;
    GLint		m_ulOverhangLength;

    GLint		m_ulNormalMatrix;
    GLint		m_ulProjection;
    GLint		m_ulSize;
    GLint		m_ulDiffuseMaterial;
    GLint		m_ulSpecularMaterial;
    GLint		m_ulShininess;
    GLint		m_ulAmbientMaterial;
    GLint		m_ulLightPosition;
   
	
	
public:

	SilhouetteShader();
	SilhouetteShader( const char* filepath );
	~SilhouetteShader();

	void InitShader( const char* filepath );
	
	inline virtual void BindShader(int passid=0)
	{
		if( CurrentShader == m_pShader )	return;
	
		CurrentShader = m_pShader;
		CurrentShader->Bind();
	}

	virtual void Render( MovableObject *pObj, Frustum *pfrustum );
	void Render( MovableObject *pObj, MatViewTransform *cp, const Texture2D& texDepth );

};




// SilhouetteShaderオブジェクトを生成するファクトリクラス
class SilhouetteShaderFactory : IRendererFactory
{
private:

	IRenderer* CreateInstance()
	{
		return new SilhouetteShader( "labworks/SilhouetteShader.glsl" );
	}

public:

	SilhouetteShaderFactory(){};
	//~SilhouetteShaderFactory(){};
};




}// end of namespace


#endif	// SILHOUETTE_SHADER_H //