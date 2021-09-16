#ifndef	WORLEY_NOISE_CPU
#define	WORLEY_NOISE_CPU

#include	<graphics/gl4x/resource/BufferLayout.h>

#include	"NoiseParams.h"



namespace OreOreLib
{

class WorleyNoiseCPU
{
public:

	WorleyNoiseCPU();
	~WorleyNoiseCPU();

	void Release();
	
	float noise( float x, float y );
	float noise( float x, float y, float z );
	float fBm( float x, float y );					// ポストプロセスなしのfBm
	float fBm( float x, float y, float z );			// ポストプロセスなしのfBm
	float fBmOutput( float x, float y );			// ポストプロセスありのfBm
	float fBmOutput( float x, float y, float z );	// ポストプロセスありのfBm


	void SetNoiseParam( const fBmNoiseParam& param )		{ m_NoiseParam = param; }
	void SetScale( float scale )							{ m_NoiseParam.Scale = scale; }
	void SetRotation( float degree )						{ m_NoiseParam.Angle = ToRadian<float>( degree ); }
	void SetStretch( float stretch )						{ m_NoiseParam.Stretch = stretch; }
	void SetOctaves( float val )							{ m_NoiseParam.Octaves	= val; }
	void SetLacunarity( float val )							{ m_NoiseParam.Lacunarity = val; }
	void SetGain( float val )								{ m_NoiseParam.Gain = val; }

	// output value control (test implementation 2013.12.03)
	void SetOutputParam( const PostProcessParam& param )	{ m_OutputParam	= param; }
	void SetBrightness( float val )							{ m_OutputParam.Brightness = val; }
	void SetContrast( float val )							{ m_OutputParam.Contrast = val; }
	void SetThreshold( float val )							{ m_OutputParam.Threshold = val; }
	void SetSmoothness( float val )							{ m_OutputParam.Smoothness = val;}
	void SetInvert( bool val )								{ m_OutputParam.Invert = val; }

	void UpdateTransformMatrix();


private:

	// noise parameters
	fBmNoiseParam	m_NoiseParam;
	Mat4f			m_MatTransform;

	// output value control parameters
	PostProcessParam	m_OutputParam;



	int permutations[256];

	int ranInt();
	float random( int x, int y, int z );
	float random( int x, int y, int z, int w );

	// ユークリッド距離
	float euclidean( float x1, float y1, float x2, float y2 );
	float euclidean( float x1, float y1, float z1, float x2, float y2, float z2 );

	// マンハッタン距離
	float manhattan( float x1, float y1, float x2, float y2 );

	// チェビシェフ距離
	float chebychev( float x1, float y1, float x2, float y2 );

	// ２乗距離
	float quadratic( float x1, float y1, float x2, float y2 );

};


}// end of namespace


#endif	// WORLEY_NOISE_CPU //