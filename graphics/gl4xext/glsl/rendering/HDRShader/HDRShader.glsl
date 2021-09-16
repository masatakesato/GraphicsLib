#pragma include	"TextureSampler.glsl"



#ifdef _VERTEX_SHADER_

// Shader Input
in vec3			POSITION;
in vec2			TEXCOORD;

// Uniform location


// Vertex Shader Output
out vec2		vTexCoord;


// Vertex Shader
void main(void)
{
	gl_Position = vec4( POSITION, 1.0 );//
	vTexCoord	= TEXCOORD;
}


#endif





#ifdef _FRAGMENT_SHADER_


// Unfiorm Locations
uniform sampler2D	g_Sampler1;		// 参照テクスチャ1
uniform sampler2D	g_Sampler2;		// 参照テクスチャ2
uniform sampler2D	g_Luminance;	// 画像の平均輝度

uniform float		g_TimeStep;		// 明順応のタイムステップ
uniform float		g_Key;			// 明るさパラメータ
uniform float		g_BloomWeight;	// ブルーム効果の重み係数
uniform int			g_CalcExponential;// expを計算するか否かのフラグ(平均輝度テクスチャ作成時に使用)
uniform vec2		g_BlurDirection;	// ブラーをかける方向
uniform vec2		g_ViewportResolution;


// Constant Variables
const vec2	g_TexSize	= vec2( textureSize( g_Sampler1, 0 ) );
const vec2	g_HalfTexelSize			= 0.5 / g_ViewportResolution;
const float	SENSITIVITY				= 15.0;// 明順応の速さ
const float	BRIGHT_PASS_THRESHOLD	= 0.75;//5.0;
const float BRIGHT_PASS_OFFSET		= 1.0;//10.0;
const float	LUM_AVG_MIN				= 0.3;// 平均輝度の下限値
const float	LUM_AVG_MAX				= 2.0;// 平均輝度の上限値


const float	g_AverageLuminance		= texture( g_Luminance, vec2(0.5) ).x;// 画面の平均輝度
const float g_LuminanceScale		= g_Key / ( g_AverageLuminance + 1.0e-6 );


// Fragment Shader Input
in vec2		vTexCoord;


// Fragment Shader Outout
out vec4	outColor;


// Fragment Shader

#if defined( SAMPLE_2X2 )

// Sample2x2
void main()
{	
	outColor = texture( g_Sampler1, vTexCoord );
}



#elif defined( SAMPLE_4X4 )

// Sample4x4
void main()
{
	outColor = Sample4x4( g_Sampler1, vTexCoord, g_HalfTexelSize );
}



#elif defined( CALC_LOG_LUMINANCE )

// Calculate Log Luminance
void main()
{
	vec3 color		= Sample4x4( g_Sampler1, vTexCoord, g_HalfTexelSize ).xyz;
	float luminance	= dot( color, vec3( 0.27, 0.67, 0.06 ) );
	outColor		= vec4( log( 1.0e-6 + luminance ) );
}



#elif defined( AVERAGE_LOG_LUMINANCE )

// Average Log Luminance
void main()
{
	float color	= Sample4x4( g_Sampler1, vTexCoord, g_HalfTexelSize ).x;
	color		= mix( color, exp(color), float(g_CalcExponential==1) );// 
	outColor	= vec4( color );
}



#elif defined( CALC_ADAPTIVE_LUMINANCE )

// Calculate Adaptive Luminance ( from DirectX9 SDK sample code )
const float fAdaptedLum = texture( g_Sampler1, vec2(0.5) ).x;// 直前の輝度値
const float fCurrentLum = texture( g_Sampler2, vec2(0.5) ).x;// 現行の輝度値

void main()
{
    // The user's adapted luminance level is simulated by closing the gap between
    // adapted luminance and current luminance by 2% every frame, based on a
    // 30 fps rate. This is not an accurate model of human adaptation, which can
    // take longer than half an hour.
//	float fNewAdaptation	= fAdaptedLum + (fCurrentLum - fAdaptedLum) * ( 1 - pow( 0.98f, g_TimeStep * SENSITIVITY ) );
	
float diff = abs( fAdaptedLum - fCurrentLum );
float fNewAdaptation	= fAdaptedLum + (fCurrentLum - fAdaptedLum) * clamp( g_TimeStep * SENSITIVITY*(exp( diff )-1.0), 0.0, 1.0 );

	outColor				= vec4( clamp( fNewAdaptation, LUM_AVG_MIN, LUM_AVG_MAX ) );


}



#elif defined( BRIGHT_PASS_FILTER )

// Bright Pass Filter
void main()
{
	vec3 lum_scaled	= texture( g_Sampler1, vTexCoord ).rgb;// ピクセルの輝度
	
	lum_scaled	*= g_LuminanceScale;
	lum_scaled	-= BRIGHT_PASS_THRESHOLD;
	lum_scaled	= max( lum_scaled, 0.0 );
	lum_scaled	/= (BRIGHT_PASS_OFFSET + lum_scaled);
	
	outColor	= vec4( lum_scaled, 0.0 );
}



#elif defined( GAUSSIAN_BLUR )

// Gaussian Blur
const vec2 dir	= g_BlurDirection / g_TexSize;

void main()
{
	outColor	= GaussianBlur13( g_Sampler1, vTexCoord, dir );
}



#elif defined( COMPOSITE )

// Composite
void main()
{
	vec3	lum_scaled	= texture( g_Sampler1, vTexCoord ).xyz;// ピクセルの輝度
	vec3	blur		= texture( g_Sampler2, vTexCoord ).xyz;
	
	lum_scaled			*= g_LuminanceScale;
	lum_scaled			/= ( 1.0 + lum_scaled );

	outColor.xyz		= lum_scaled + g_BloomWeight * blur;
}


#else

// Do Nothing
void main (void)
{
	discard;
}

#endif







#endif
