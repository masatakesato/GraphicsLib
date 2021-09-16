#ifndef	SIMPLEX_NOISE_CPU
#define	SIMPLEX_NOISE_CPU

#include	<graphics/gl4x/resource/BufferLayout.h>

#include	"NoiseParams.h"



namespace OreOreLib
{

// Simplex noise in 2D, 3D and 4D
class SimplexNoiseCPU
{
public:

	SimplexNoiseCPU();
	~SimplexNoiseCPU();

	void Release();

	double noise( double xin, double yin );					// 2D simplex noise
	double noise( double xin, double yin, double zin );		// 3D simplex noise
	double noise( double x, double y, double z, double w );	// 4D simplex noise
	float fBm( float x, float y );							// ポストプロセスなしのfBm
	float fBm( float x, float y, float z );					// ポストプロセスなしのfBm
	float fBm( float x, float y, float z, float w );		// ポストプロセスなしのfBm
	float fBmOutput( float x, float y );					// ポストプロセスありのfBm
	float fBmOutput( float x, float y, float z );			// ポストプロセスありのfBm
	float fBmOutput( float x, float y, float z, float w );	// ポストプロセスありのfBm


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


	//static OreOreLib::Vec3i	grad3[];
	//static OreOreLib::Vec4i	grad4[];
	//static int				p[];

	// To remove the need for index wrapping, double the permutation table length
	int perm[/*512*/1024];
	
	// A lookup table to traverse the simplex around a given point in 4D.
	// Details can be found where this table is used, in the 4D noise method.
	//static OreOreLib::Vec4i simplex[];


	// This method is a *lot* faster than using (int)Math.floor(x)
	int fastfloor(double x)
	{
		return x>0 ? (int)x : (int)x-1;
	}
	
	double dot(const int g[], double x, double y)
	{
		return g[0]*x + g[1]*y;
	}
	
	double dot(const int g[], double x, double y, double z)
	{
		return g[0]*x + g[1]*y + g[2]*z;
	}
	
	double dot(const int g[], double x, double y, double z, double w)
	{
		return g[0]*x + g[1]*y + g[2]*z + g[3]*w;
	}

};



}// end of namespace


#endif	// SIMPLEX_NOISE_CPU //