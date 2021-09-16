//uniform sampler2D	g_Texture;	// texture
uniform vec4		g_Diffuse;	// diffuse material


//=================== Control Parameter ======================//
const vec4	g_CoarseColor		= vec4( 0.9098, 0.7803, 0.5803, 0.0 );	// 疎な領域の色
const vec4	g_DenseColor		= vec4( 0.5647, 0.3411, 0.2666, 0.0 );	// 密な領域の色
const vec4	g_HeartwoodColor	= vec4( 0.137, 0.0862, 0.062, 0.0 );	// 心材部分の色



uniform float	g_RingInterval;	//const float g_RingInterval		= 0.05;	// 年輪の幅
const float g_HeartwoodRad		= 0.5;	// 心材の半径
uniform float	g_Smoothness;	//const float	g_Smoothness		= 0.1;

const vec3	g_LocalOffset		= vec3( 0, 0, 0 );// 中心座標オフセット
const mat4	g_MatLocal			= mat4( vec4(1,0,0,0), vec4(0,1,0,0), vec4(0,0,1,0), vec4(0,0,0,1) );
//const vec3	g_LocalDirection	= vec3( 0, 1, 0 );// 向き



// 年輪の揺らぎ
uniform float	g_Amplitude1;	//	= 0.5;//const vec3	g_Scale_Pertubation1= vec3( 1.0, 2.0, 1.0 );//uniform vec3 g_GlobalPertubation;	
uniform float	g_Stretch1;		//	= 0.01;	// 引き伸ばし
uniform float	g_Roughness1;	//const float	g_Finess_Pertubation1	= 20.0;	// ノイズの粗さ

uniform float	g_Amplitude2;	//	= 0.5;//const vec3	g_Scale_Pertubation1= vec3( 1.0, 2.0, 1.0 );//uniform vec3 g_GlobalPertubation;	
uniform float	g_Stretch2;		//	= 0.01;	// 引き伸ばし
uniform float	g_Roughness2;	//const float	g_Finess_Pertubation1	= 20.0;	// ノイズの粗さ





//const float	g_LocalPertubationAmount	= 0.15;						// 局所ノイズの量
//const vec3	g_LocalPertubation			= vec3( 0.001, 0.05, 0.001 );//uniform vec3 g_LocalPertubation;	//	// 局所ノイズのスケール

//const float	g_LocalPertubationStretch	= 1.0;	// 引き伸ばし
//const float	g_LocalPertubationFiness	= 1.0;	// ノイズの粗さ


#ifdef _VERTEX_SHADER_

// Shader Input
in vec3			POSITION;
in vec2			TEXCOORD;
in vec3			NORMAL;

// Uniform location
uniform mat4	g_MatWorld;
uniform mat4	g_MatViewProj;
uniform mat4	g_MatWorldViewProj;

// Vertex Shader Output
out vec2		vTexCoord;
out vec3		vWorldPos;
out vec3		vNormal;
out vec3		vLocalPos;
out vec3		vLocalPos0;
out vec3		vLocalPos1;
out vec3		vLocalPos2;


// Vertex Shader
void main(void)
{
	gl_Position = vec4( POSITION, 1.0 );//g_MatViewProj * g_MatWorld * vec4( POSITION, 1.0 );//
	vWorldPos	= (g_MatWorld * vec4( POSITION, 1.0 )).xyz;
	vTexCoord	= TEXCOORD;
	vNormal		= NORMAL;

	// const variables
	mat3 matScale1 = mat3( g_MatLocal[0].xyz, g_MatLocal[1].xyz / g_Stretch1, g_MatLocal[2].xyz );
	mat3 matScale2 = mat3( g_MatLocal[0].xyz, g_MatLocal[1].xyz / g_Stretch2, g_MatLocal[2].xyz );

	vLocalPos	= vWorldPos.xyz;//(g_MatLocal * vec4( POSITION, 1.0 )).xyz;
	vec3 lops = (g_MatLocal * vec4( POSITION, 1.0 )).xyz + vec3(500.0, 500.0, 500.0);
	vLocalPos0	= lops;
	vLocalPos1	= matScale1 * (lops * g_Roughness1) + vec3(500.0, 500.0, 500.0);
	vLocalPos2	= matScale2 * (lops * g_Roughness2) + vec3(500.0, 500.0, 500.0);
}


#endif




#ifdef _FRAGMENT_SHADER_

// Inclue files
//#pragma include "Functions.glsl"
//#pragma include "NoiseFunction.glsl"


#pragma include "./Functions.glsl"
//#pragma include "../shader/NoiseFunction.glsl"

#pragma include "./SimplexNoise.glsl"



// uniform variables
//uniform vec3 g_Scale		= vec3(10, 10, 10);
const float g_Octaves		= 1.0;
const float g_Lacunarity	= 1.0;
const float g_Gain			= 1.0;
uniform float g_Invert		= 0.0;



// Fragment Shader Input
in vec2	vTexCoord;
in vec3	vWorldPos;
in vec3	vNormal;
in vec3	vLocalPos;
in vec3	vLocalPos0;
in vec3	vLocalPos1;
in vec3	vLocalPos2;

out vec4 outColor;



// Fragment Shader
void main (void)
{
	// 木中心からの距離を計算
	vec3 direction = g_MatLocal[1].xyz;	// 上方向を取得
	float d = Distance_Point2Line( g_LocalOffset, direction, vLocalPos );
	
	// 距離値に大局的な揺らぎを加える
	float noise_global_pertubation = Simplex( vLocalPos1 );//inoise( vLocalPos1 );
	d += noise_global_pertubation * g_Amplitude1 * g_RingInterval;// / g_Roughness1;
	
	// 距離値に局所的な揺らぎを加える
	float noise_local_pertubation = Simplex( vLocalPos2 );//inoise( vLocalPos2 );
	d += noise_local_pertubation * g_Amplitude2 * g_RingInterval;// / g_Roughness2;

	// mod計算で距離場を[0-g_RingInterval]サイクルにする
	float factor = mod( d, g_RingInterval ) / g_RingInterval;


	// 角を丸くする
	if(factor>=0.95) factor = smoothstep(1.0, 0.95, factor ) * 0.95;


	// 年輪の色分布
	vec4 color1 = mix( g_CoarseColor, g_DenseColor, factor );
	
	// 心材/辺材の色分布
	float weight = smoothstep( -g_Smoothness, +g_Smoothness, d-g_HeartwoodRad )*0.65 + 0.35;//d < g_HeartwoodRad ? 0.35 : 1.0;//

	outColor = max( min( color1*weight, vec4(1,1,1,1) ), vec4(0,0,0,0) );
	
	//	gl_FragColor = vec4( vTexCoord, 0.0, 0.0 );
	//	gl_FragColor = vec4(texture1D( permTexture, vTexCoord.x ).xyz, 0);
	//gl_FragColor	= texture2D( permTexture2d, vTexCoord );
	//	gl_FragColor = vec4( abs(texture1D( permGradTexture, vTexCoord.x ).xyz), 0.0 );
	//  gl_FragColor = vec4( 1.0, 0.73, 0.407, 0.0 ) * intensity;// * d;//vec4( intensity, intensity, intensity, 0.0 ) * weight;//vec4(vNormal, 0.0);// g_Diffuse;//
}

#endif
