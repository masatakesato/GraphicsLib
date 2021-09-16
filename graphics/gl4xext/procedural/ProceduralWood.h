#ifndef	PROCEDURAL_SHADER_H
#define	PROCEDURAL_SHADER_H

#include	<graphics/gl4x/resource/Texture.h>
#include	<graphics/gl4x/shader/IShader.h>
#include	<graphics/gl4x/shader/GLShader.h>
using namespace OreOreLib;






struct WoodMaterialParams
{
	float RingInterval	 = 1.0f;
	float EdgeSmoothness	= 0.0f;

	float Amplitude[2]		= { 1.0f, 1.0f };
	float Stretch[2]		= { 1.0f, 1.0f };
	float Roughness[2]		= { 1.0f, 1.0f };
};




class ProceduralWood : public IShader
{
private:

	// LookupTables for Procedural Noise Generation
//	Texture1D	m_PermTexture;
//	Texture2D	m_PermTexture2D;
//	Texture1D	m_GradTexture;

	Texture1D	m_PermSimplex;
	Texture1D	m_PermWorley;
	

	// shader
	GLShader	*m_pShader;	// デフォルトのシェーダー
	
	// attribute location

	// uniform location
	GLint		m_ulMatWorld;			// world matrix
	GLint		m_ulMatViewProj;		// view projection matrix
	GLint		m_ulMatWorldViewProj;	// world view projection matrix
	
	GLint		m_ulTexture;			// texture
	GLint		m_ulDiffuse;			// diffuse material TODO: Add entire material
	
	//GLint		m_ulPermTexture;		// permutation lookup table(1D, 256x1, red channel only)
	//GLint		m_ulPermTexture2D;		// permutation lookup table(AA, AB, BA, BB)
	//GLint		m_ulGradTexture;		// gradient lookup table(1D, 16x1, rgb )	//rgba8. 16x1[pixels]
	GLint		m_ulPermSimplex;		// SimplexNoiseのPermTexture
	GLint		m_ulPermWorley;			// WotrleyNoiseのPermTexture


	GLint		m_ulRingInterval;
	GLint		m_ulEdgeSmoothness;

	GLint		m_ulAmplitude1;
	GLint		m_ulStretch1;
	GLint		m_ulRoughness1;

	GLint		m_ulAmplitude2;
	GLint		m_ulStretch2;
	GLint		m_ulRoughness2;
	

	WoodMaterialParams	m_Params;
	//float		m_RingInterval;
	//float		m_EdgeSmoothness;

	//float		m_Amplitude1;
	//float		m_Stretch1;
	//float		m_Roughness1;

	//float		m_Amplitude2;
	//float		m_Stretch2;
	//float		m_Roughness2;


	//void InitLookupTables();	//
	void InitPermTexture();


public:

	ProceduralWood();
	ProceduralWood( const TCHAR* filepath, GLSL_VERSION version );
	~ProceduralWood();

	void InitShader( const TCHAR* filepath, GLSL_VERSION version );
	void SetParams( const WoodMaterialParams& params );


	virtual int NumPasses() const	{ return 1; }
	virtual GLuint GetPassID( int shader_idx=0 ) const { return m_pShader->ID(); }

	virtual void BindShader( int shader_idx=0 ){ m_pCurrShader=m_pShader; }

	virtual void BindBufferObject( const IBufferObject* pbufferobj ){}
	virtual void UnbindBufferObject(){}

	virtual void Render();
	virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat ){}


};



// ProceduralShaderオブジェクトを生成するファクトリクラス
class ProceduralShaderFactory : IShaderFactory
{
private:

	IShader* Create( GLSL_VERSION version ){ return new ProceduralWood( _T("ProceduralTexture.glsl"), version ); }

public:

	virtual ~ProceduralShaderFactory(){}
	IShader* CreateInstance( GLSL_VERSION version );

};





#endif	// PROCEDURAL_SHADER_H //