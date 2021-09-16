// Include Files
#pragma include "lpv/LPVHelperFunctions.glslh"



// RSM
//uniform vec2		g_RsmTexelDim = {1/2048, 1/2048};	// テクセル1個の幅
uniform mat4		g_matLight;		// 光源系の座標変換マトリクス
uniform sampler2D	g_ShadowMap;	// シャドウマップ

// LPV
uniform sampler3D	g_LPV_SH_R;
uniform sampler3D	g_LPV_SH_G;
uniform sampler3D	g_LPV_SH_B;



uniform vec3	g_EyePos;
uniform vec3	g_LightPos;
uniform vec3	g_LightIntensity;


uniform float	g_Magnitude = 1.0;// TODO:　間接照明の強度をユーザーが調整するためのパラメータ．外部からシェーダに値を取り込む
uniform float	g_LpvCellDim;
uniform vec3	g_LpvCellSize;


#define M_PI 3.141592




vec2 neighbors[25] = vec2[25]
(
	vec2(-2, -2),
	vec2(-1, -2),
	vec2(0,  -2),
	vec2(+1, -2),
	vec2(+2, -2),

	vec2(-2, -1),
	vec2(-1, -1),
	vec2(0,  -1),
	vec2(+1, -1),
	vec2(+2, -1),

	vec2(-2, 0),
	vec2(-1, 0),
	vec2(0,  0),
	vec2(+1, 0),
	vec2(+2, 0),

	vec2(-2, +1),
	vec2(-1, +1),
	vec2(0,  +1),
	vec2(+1, +1),
	vec2(+2, +1),

	vec2(-2, +2),
	vec2(-1, +2),
	vec2(0,  +2),
	vec2(+1, +2),
	vec2(+2, +2)
);



//#################### バーテックスシェーダ ##################//
#ifdef _VERTEX_SHADER_

// 変数
uniform mat4	g_matMVP;
uniform mat4	g_MatWorld;


// 入力
in vec4		POSITION;
in vec3		NORMAL;
in vec4		COLOR;


// 出力
out vec4	vPosition;	// 頂点座標
out vec4	vLightPos;	// 光源系の頂点座標
out vec3	vNormal;	// 法線ベクトル
out vec4	vColor;		// 色


// メイン関数
void main()
{	
	vec4 wPos	= POSITION * g_MatWorld;

	gl_Position	= wPos * g_matMVP;
	vPosition	= wPos;
	vLightPos	= wPos * g_matLight;
	vNormal		= NORMAL * mat3( g_MatWorld );
	vColor		= COLOR;
}


#endif







//##################### フラグメントシェーダ #################//
#ifdef _FRAGMENT_SHADER_

// 変数
uniform vec3		g_Diffuse;	// diffuse material
uniform vec3		g_Specular;	// specular material
uniform float		g_Shininess;
uniform mat4		g_MatTexCoord;



// 入力
in vec4		vPosition;	// 頂点座標
in vec4		vLightPos;	// 光源系の頂点座標
in vec3		vNormal;	// 法線ベクトル
in vec4		vColor;		// 色


// 出力
out vec4 Color_out;





// Variance Shadow map
float VSM_Filter(vec2 texcoord, float fragDepth)
{/*
	float depth = texture(g_ShadowMap, texcoord).x;
	
	float depth_sq = depth * depth;
	float variance = depth - depth_sq;
	float md = fragDepth - depth;
	float p = -variance / (variance + (md * md));

	return 1.0 - max(p, float(depth<=fragDepth));
	*/
	
	float occluder = texture(g_ShadowMap, texcoord).r;
	float overdark = 0.001;
	float    lit = exp(overdark * (occluder - fragDepth));
	lit = 2*(clamp(lit, 0, 1) - 0.5);

	return lit;
	
}



// メイン関数
void main()
{
	//================= 影付け =================//
	vec2 rsmtexcoord = 0.5*(vLightPos.xy/vLightPos.w) + vec2(0.5);
	float Depth_RSM = texture(g_ShadowMap, rsmtexcoord).r;
	float Depth_Frag = vLightPos.z/*vLightPos.w*/ - 0.05;//
	float islit = 1.0;

	if( rsmtexcoord.x<0.0 || rsmtexcoord.x>1.0 || rsmtexcoord.y<0.0 || rsmtexcoord.y>1.0 || vLightPos.z<0.0 )	islit = 0.0;
	if( Depth_RSM < Depth_Frag ) islit = 0.0;

	//float diff = abs( Depth_RSM - Depth_Frag );
	//if( diff> 1.0 ) 
	//	Color_out.rgb = vec3( 1.0, 0.0, 0.0 );
	//else
	//	Color_out.rgb = vec3( diff, diff, diff );


	
	//================= 照明計算 ===============//
	vec3 L				= g_LightPos - vPosition.xyz;
	float LightDistance = length(L);
	L /= LightDistance;
	vec3 V				= normalize(g_EyePos - vPosition.xyz);
	vec3 N				= vNormal;
	
	
	//if(dot(V, N) < 0.0)	N = -N;// 面の法線が常に視点を向くようにする
	vec3 Nsh			= dot(V, N) < 0.0 ? -N : N;
	vec4 SampleDirSH	= CalcSHCoeff(-Nsh);
	vec3 lpvtexcoord	= ( vPosition * g_MatTexCoord ).xyz;// ワールド座標空間からLPVのテクスチャ座標を計算する
	vec4 SH_R			= texture( g_LPV_SH_R, lpvtexcoord );
	vec4 SH_G			= texture( g_LPV_SH_G, lpvtexcoord );
	vec4 SH_B			= texture( g_LPV_SH_B, lpvtexcoord );


	
	vec3 LpvIntensity = vec3(
							max(dot(SampleDirSH, SH_R),0),
							max(dot(SampleDirSH, SH_G),0),
							max(dot(SampleDirSH, SH_B),0)
							);
	
	
	//=============== 裏面に光が漏れ出さないようにする =================//
	vec4 SH_Gradient_R = SH_Gradient( g_LPV_SH_R, Nsh, lpvtexcoord, 1.0/g_LpvCellSize.xyz );
	vec4 SH_Gradient_G = SH_Gradient( g_LPV_SH_G, Nsh, lpvtexcoord, 1.0/g_LpvCellSize.xyz );
	vec4 SH_Gradient_B = SH_Gradient( g_LPV_SH_B, Nsh, lpvtexcoord, 1.0/g_LpvCellSize.xyz );

	vec4 grad_avg		= normalize( max( max(SH_Gradient_R, SH_Gradient_G), SH_Gradient_B ) );//normalize( SH_Gradient_R + SH_Gradient_G + SH_Gradient_B );//
	float Visibility	= dot( SampleDirSH, grad_avg );// サンプル点SHと、近傍のSH変化から計算した勾配との内積を計算する。0未満なら向きが逆
	

	// 内積判定に適当に重み付け。アーティファクト減らす目的。物理的な意味はない
	//Visibility	= clamp( exp( Visibility /* 2.0*/ ) - 1.0, 0.0, 1.0 );
	//Visibility	= smoothstep( -0.70710678118, 0.70710678118, Visibility );
	//Visibility		= pow( Visibility*0.5+0.5, 2.5 );
	//Visibility	= smoothstep( 0.0, 1.0, Visibility );
	//Visibility	= Visibility * 0.5 + 0.5;
	Visibility		= pow(  Visibility*0.5 + 0.5, 2.0 );


	vec3 lightRadiance = g_LightIntensity / (4 * M_PI) / LightDistance / LightDistance;// 光源からの距離に比例して減衰
	vec3 LpvRadiance = LpvIntensity * 4.0 / g_LpvCellDim / g_LpvCellDim * g_Magnitude;

	vec3 result	= g_Diffuse.rgb * 
					(
						( islit * lightRadiance * max(dot(L,N),0) / M_PI )	// diffuse
						+ LpvRadiance / M_PI * Visibility					// irradiance volume
					);
	
	// temporary gamma correction. TODO: reimplement int the post process shader
	Color_out.rgb = result;//vec3(Visibility,Visibility,Visibility);//
	Color_out.a	= 1.0;

	// デバッグ
	//Color_out.rgb = vec3( lpvtexcoord.xz, 0.0 );
	//Color_out.rgb	= LpvIntensity;
}


#endif
