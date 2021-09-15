#ifndef	SIMPLEX_NOISE_GPU_H
#define	SIMPLEX_NOISE_GPU_H


#include	<IShader.h>
using namespace OreOreLib;

#include	"NoiseParams.h"



class SimplexNoiseGPU : public IShader
{
public:

	SimplexNoiseGPU();
	SimplexNoiseGPU( const char* filepath, GLSL_VERSION version );
	~SimplexNoiseGPU();

	void Release();

	void InitShader( const char* filepath, GLSL_VERSION version );

	void SetNoiseParam( const fBmNoiseParam& param )	{ m_NoiseParam = param; }
	void SetScale( float scale )		{ m_NoiseParam.Scale = scale; }
	void SetRotation( float degree )	{ m_NoiseParam.Angle = TO_RADIAN( degree ); }
	void SetStretch( float stretch )	{ m_NoiseParam.Stretch = stretch; }
	void SetOctaves( float val )		{ m_NoiseParam.Octaves	= val; }
	void SetLacunarity( float val )		{ m_NoiseParam.Lacunarity = val; }
	void SetGain( float val )			{ m_NoiseParam.Gain = val; }

	// output value control (test implementation 2013.12.03)
	void SetOutputParam( const PostProcessParam& param )	{ m_OutputParam	= param; }
	//void SetBrightness( float val )	{ m_OutputParam.Brightness = val; }
	void SetContrast( float val )	{ m_OutputParam.Contrast = val; }
	void SetThreshold( float val )	{ m_OutputParam.Threshold = val; }
	void SetSmoothness( float val )	{ m_OutputParam.Smoothness = val;}
	void SetInvert( bool val )		{ m_OutputParam.Invert = val; }



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

	struct ulPostProcessParam
	{
		GLint	Contrast;
		GLint	Brightness;
		GLint	Threshold;
		GLint	Smoothness;
		GLint	Invert;
	};

	// shader
	GLShader	*m_pShader;		// デフォルトのシェーダー
	
	// uniform location	
	GLint		m_ulPermTexture;	// permutation texture

	//GLint		m_ulScale;
	//GLint		m_ulStretch;
	GLint		m_ulOctaves;
	GLint		m_ulLacunarity;
	GLint		m_ulGain;
	GLint		m_ulMatTransform;

	// output value control parameters (test implementation 2017.02.27)
	ulPostProcessParam	m_ulPostProcessParam;


	// perm texture
	Texture1D		m_PermTexture;

	// noise parameters
	fBmNoiseParam		m_NoiseParam;
	Mat4f				m_MatTransform;
	PostProcessParam	m_OutputParam;


	// private functions
	void InitPermTexture();
	
};





//// SimplexNoiseGPUオブジェクトを生成するファクトリクラス
//class SimplexNoiseGPUFactory : IRendererFactory
//{
//private:
//
//	IRenderer* CreateInstance(){ return new SimplexNoiseGPU( "SimplexNoise_main.glsl" ); }
//
//public:
//
//	SimplexNoiseGPUFactory(){};
//	//~SimplexNoiseGPUFactory(){};
//};





#endif	// SIMPLEX_NOISE_GPU_H //