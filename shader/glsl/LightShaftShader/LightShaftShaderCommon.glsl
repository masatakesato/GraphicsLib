#ifndef	LIGHT_SHAFT_SHADER_COMMON_GLSL
#define	LIGHT_SHAFT_SHADER_COMMON_GLSL


#ifndef	FLT_MAX
#define	FLT_MAX	3.402823466e+38F
#endif


// TEX_UNIT Macro
#define TEX_UNIT_SLICE_END_POINTS		0
//#define  TEX_UNIT_POSITION,
#define TEX_UNIT_DEPTH					1
#define TEX_UNIT_EPIPOLAR_COORDINATE	2
#define TEX_UNIT_EPIPOLAR_CAM_SPACE_Z	3
#define TEX_UNIT_EPIPOLAR_DEPTH			4
//#define	TEX_UNIT_SHADOWMAP			5





struct EpipolarParam
{
	int		NumSamplesInSlice;
	int		NumEpipolarSlices;
	vec2	ScreenResolution;

	float	RefinementThreshold;
	float	EpipoleSamplingDensityFactor;
};



struct EpipolarLightParam
{
	vec2	ScreenLightPos;
	//int		NumCascades;
	//vec2	ShadowMapTexelSize;
	//mat4	MatLight;
	int		LightID;
};



uniform ubEpipolarParam
{
	EpipolarParam g_EpipolarParam;
};


uniform ubEpipolarLightParam
{
	EpipolarLightParam	g_EpipolarLightParam;
};



// Parameter Access Macros

#define	g_numSamplesInSlice		g_EpipolarParam.NumSamplesInSlice
#define	g_numEpipolarSlices		g_EpipolarParam.NumEpipolarSlices
#define	g_ScreenResolution		g_EpipolarParam.ScreenResolution

#define g_ScreenLightPos		g_EpipolarLightParam.ScreenLightPos
//#define g_numCascades			g_EpipolarLightParam.NumCascades
//#define	g_ShadowMapTexelSize	g_EpipolarLightParam.ShadowMapTexelSize
//#define	g_MatWorldToLight		g_EpipolarLightParam.MatLight;




#endif	// LIGHT_SHAFT_SHADER_COMMON_GLSL //
