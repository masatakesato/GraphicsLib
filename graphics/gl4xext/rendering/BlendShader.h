#ifndef	BLEND_SHADER_H
#define	BLEND_SHADER_H


#include	<IShader.h>


namespace OreOreLib
{

struct BlendParams
{
	bool	bReverseBlendOrder[3];
	float	Opacity[3];
	float	BlendWeight[3];
};



class BlendShader : public IShader
{	
public:

	BlendShader();
	BlendShader( const char* filepath, GLSL_VERSION version );
	~BlendShader();

	void Release();

	void InitShader( const char* filepath, GLSL_VERSION version );

	void SetOpacity( const float param[], int numelm ){	int elm = min( numelm, 8 );	for( int i=0; i<elm; ++i )	m_Opacity[i] = param[i]; }
	void SetDiffuse0( const Vec3f diffuse0[], int numelm ){	int elm = min( numelm, 8 );	for( int i=0; i<elm; ++i )	m_Diffuse0[i] = diffuse0[i]; }
	void SetDiffuse1( const Vec3f diffuse1[], int numelm ){	int elm = min( numelm, 8 );	for( int i=0; i<elm; ++i )	m_Diffuse1[i] = diffuse1[i]; }

	float *GetOpacity() const	{ return (float *)&m_Opacity; }
	Vec3f *GetDiffuse0() const	{ return (Vec3f *)&m_Diffuse0; }
	Vec3f *GetDiffuse1() const	{ return (Vec3f *)&m_Diffuse1; }

	void BindInputTextures( int numTex, Texture2D texArray[] );

	// Override Virtual Functions
	inline virtual int NumPasses() const	{ return 1; }
	inline virtual GLuint GetPassID( int shader_idx=0 ) const{ return m_pShader->ID(); }

	inline virtual void BindShader( int shader_idx=0 )
	{
		if( m_pCurrShader == m_pShader )	return;
		m_pCurrShader = m_pShader;
		m_pCurrShader->Bind();
	}

	virtual void BindBufferObject( const IBufferObject* pbufferobj ){}
	virtual void UnbindBufferObject(){}

	virtual void Render();
	virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat ){}


private:

	// shader
	GLShader	*m_pShader;	// デフォルトのシェーダー
	
	// uniform location	
	GLint		m_ulInputTextures[16];	// permutation texture
	GLint		m_ulNumTextures;

	GLint		m_ulOpacity;
	GLint		m_ulDiffuse0;
	GLint		m_ulDiffuse1;

	// input textures
	int			m_numTextures;
	Texture2D	*m_InputTextureArray;
	
	// parameter
	float		m_Opacity[8];
	Vec3f		m_Diffuse0[8];
	Vec3f		m_Diffuse1[8];

};





//// BlendShaderオブジェクトを生成するファクトリクラス
//class BlendShaderFactory : IRendererFactory
//{
//private:
//
//	IRenderer* CreateInstance(){ return new BlendShader( "BlendShader_main.glsl" ); }
//
//public:
//
//	BlendShaderFactory(){};
//	//~BlendShaderFactory(){};
//};




}// end of namespace


#endif	// BLEND_SHADER_H //