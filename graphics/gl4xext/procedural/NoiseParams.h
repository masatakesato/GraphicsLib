#ifndef	NOISE_PARAMS_H
#define	NOISE_PARAMS_H


#include	<MathLib.h>


namespace OreOreLib
{


	// procedural noise type
	enum NOISE_TYPE
	{
		NOISE_UNKNOWN		= -1,
		NOISE_SIMPLEX		= 0,
		NOISE_WORLEY		= 1,
		NOISE_WORLEY_STONE	= 2,
		//NOISE_WORLEY_CELL	= 3,
		//NOISE_PATTERN_BRICK	= 4,
		NUM_NOISE,
	};



	// ノイズ座標系の変形パラメータ. 下に内包されるので現状省略
	//struct NoiseTransformParam
	//{
	//	Vec3f	Scale;		// scale
	//	Vec3f	Offset;		// offset
	//	Mat4f	Rotation;	// rotation
	//};


	// fractional Brownian Motionノイズのパラメータ
	struct fBmNoiseParam
	{
		float Scale;//			= 1.0f;
		float Angle;//			= 0.0f;
		float Stretch;//		= 0.0f;
		float Octaves;//		= 1.0f;
		float Lacunarity;//		= 1.5f;
		float Gain;//			= 0.5f;
	};


	// ノイズ関数の値補正パラメータ
	struct PostProcessParam
	{
		float	Contrast;	//	= 1.0f;
		float	Brightness;//	= 0.0f;
		float	Threshold;//	= 0.5f;
		float	Smoothness;//	= 1.0f;
		bool	Invert;//		= false;
	};



}// end of namespace




#endif	// NOISE_PARAMS_H //