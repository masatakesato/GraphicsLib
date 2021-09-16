// Uniform Locations
uniform sampler2D	g_RustMap;	// 錆分布のアルファテクスチャ
//uniform sampler2D	g_RustLocalShape;	// texture
//uniform sampler2D	g_SpotDistribution;
//uniform sampler2D	g_InnerRust;

uniform float		g_Aging;
uniform float		g_Scale;
uniform vec3		g_Offset;

uniform float		g_DistributionAmount;
uniform float		g_DistributionSmoothness;

uniform float		g_SpotAmount;
uniform float		g_SpotSmoothness;

uniform float		g_BleedAmount;
uniform float		g_BleedSmoothness;

uniform float		g_BaseAmount;
uniform float		g_BaseSmoothness;


uniform float		g_OuterBleedAmount;
uniform float		g_OuterBleedSmoothness;

uniform float		g_OuterBaseAmount;
uniform float		g_OuterBaseSmoothness;








// 赤錆の色
const vec4 background_col	= vec4( 0.75, 0.75, 0.75, 0.0 );	// バックグラウンドの色
const vec4 rust_spot_col	= vec4( 0.18823529411, 0.15294117647, 0.16470588235, 0.0 );	// 錆発生点近傍の色 (48, 39, 42)
const vec4 rust_bleed_col	= vec4( 0.61176470588, 0.24705882352, 0.07843137254, 0.0 );	// 発生点から流れ出す色 (156, 63, 20)
const vec4 rust_base_col	= vec4( 0.89019607843, 0.63529411764, 0.38431372549, 0.0 );	// デフォルトの錆色 (227, 162, 98)


/*
// 青錆の色
const vec4 background_col	= vec4( 0.3, 0.25, 0.1, 0.0 );	// バックグラウンドの色
const vec4 rust_spot_col	= vec4( 0.018823529411, 0.25882352941, 0.16666666666, 0.0 );	// 錆発生点近傍の色 (56, 66, 68)
const vec4 rust_bleed_col	= vec4( 0.49019607843, 0.73333333333, 0.71372549019, 0.0 );	// 発生点から流れ出す色 (125, 187, 182)
const vec4 rust_base_col	= vec4( 0.30980392156, 0.65098039215, 0.52549019607, 0.0 )*0.85;	// デフォルトの錆色 (79, 166, 134)
*/


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


#define RUST_SHAPE_SCALE	7.5
#define RUST_INNER_SCALE	150.0
#define RUST_SPOT_SCALE		350.0


// Include files
//#pragma include"../shader/SimplexNoise2d.glsl"
//#pragma include"../shader/SimplexNoise3d.glsl"
//#pragma include"../shader/SimplexNoise4d.glsl"
#pragma include"../shader/SimplexNoise.glsl"
#pragma include"../shader/WorleyNoise.glsl"


// Fragment Shader Input
in vec2		vTexCoord;

out vec4	outColor;


#define EPS 1.0e-2

const float g_RustDistribThreshold_start	= 1.0 - g_DistributionAmount;
const float g_RustDistribThreshold_end		= g_RustDistribThreshold_start + g_DistributionSmoothness;

const float	g_RustSpotThreshold_start	= 1.0 - g_SpotAmount;
const float	g_RustSpotThreshold_end		= g_RustSpotThreshold_start + g_SpotSmoothness;

const float g_BleedThreashold_start		= g_BleedAmount;//1.0 - g_BleedAmount;//g_RustSpotThreshold_end - g_BleedAmount;
const float g_BleedThreashold_end		= g_BleedThreashold_start + g_BleedSmoothness;//g_RustSpotThreshold_end;

const float g_BaseThreashold_start		= 1.0 - g_BaseAmount;
const float g_BaseThreashold_end		= g_BaseThreashold_start + g_BaseSmoothness;


const float g_OuterBleedThreashold_start	= 1.0 - g_OuterBleedAmount;//1.0 - g_BleedAmount;//g_RustSpotThreshold_end - g_BleedAmount;
const float g_OuterBleedThreashold_end		= g_OuterBleedThreashold_start + g_OuterBleedSmoothness;//g_RustSpotThreshold_end;

const float g_OuterBaseThreashold_start		= 1.0 - g_OuterBaseAmount;
const float g_OuterBaseThreashold_end		= g_OuterBaseThreashold_start + g_OuterBaseSmoothness;




//void calcRustInnerColor( inout vec4 rust_col )
//{
//	float rust_inner_mat	= texture( g_InnerRust, vTexCoord.xy ).x;
//
//	// 黄色い部分の計算
//	float base_weight	= smoothstep( g_BaseThreashold_start, g_BaseThreashold_end, rust_inner_mat );
//	rust_col	= mix( rust_col, rust_base_col, base_weight );
//
//	// 黒い部分の計算
//	float spot_distrib	= texture( g_SpotDistribution, vTexCoord.xy ).x * 0.8 + rust_inner_mat * 0.2;
//	float spot_weight	= smoothstep( g_RustSpotThreshold_start, g_RustSpotThreshold_end, spot_distrib );
//	rust_col	= mix( rust_col, rust_spot_col, spot_weight );
//
//	// 錆流れ部分の計算
//	float bleed_weight = smoothstep( g_BleedThreashold_end, g_BleedThreashold_start, rust_inner_mat );
//	rust_col		= mix( rust_col, rust_bleed_col, bleed_weight );
//}


void calcRustInnerColor( inout vec4 rust_col )
{
	float rust_inner_mat	= //texture( g_InnerRust, vTexCoord.xy ).x;
								0.5 * Worley_fBm( (g_Offset + vec3(vTexCoord, 0.0) )*RUST_INNER_SCALE * g_Scale, 4, 1.5, 0.5 ) +
								0.5 * ( Simplex_fBm( (g_Offset + vec3(vTexCoord, 0.0) )*RUST_INNER_SCALE * g_Scale, 3, 2.5, 0.75 ) * 0.5 + 0.5 );

	// 黄色い部分の計算
	float base_weight	= smoothstep( g_BaseThreashold_start, g_BaseThreashold_end, rust_inner_mat );
	rust_col	= mix( rust_col, rust_base_col, base_weight );

	// 黒い部分の計算
	float spot_distrib	= //texture( g_SpotDistribution, vTexCoord.xy ).x * 0.8 + rust_inner_mat * 0.2;
							Worley_fBm( (g_Offset + vec3(vTexCoord, 0.0)) * RUST_SPOT_SCALE * g_Scale, 4, 1.25, 0.85 ) * 0.25 + 0.25;
	spot_distrib	= spot_distrib * 0.8 + rust_inner_mat * 0.2;

	float spot_weight	= smoothstep( g_RustSpotThreshold_start, g_RustSpotThreshold_end, spot_distrib );//rust_inner_mat );
	rust_col	= mix( rust_col, rust_spot_col, spot_weight );

	// 錆流れ部分の計算
	float bleed_weight = smoothstep( g_BleedThreashold_end, g_BleedThreashold_start, rust_inner_mat );
	rust_col		= mix( rust_col, rust_bleed_col, bleed_weight );
}



// Fragment Shader
//void main (void)
//{
//	// サーフェス全体の錆分布の重みを取得
//	float rust_map	= texture( g_RustMap, vTexCoord.xy ).x * g_Aging;
//	//outColor = vec4( rust_map, rust_map, rust_map, 0.0 );
//
//	// 錆そのものの分布形状
//	float rust_shape	= texture( g_RustLocalShape, vTexCoord.xy ).x;
//	const float rust_amount	= rust_shape * rust_map;
//
//
//	//========================= 錆領域内部の色を計算する ==========================//
//	vec4 rust_col_inner = background_col;
//	calcRustInnerColor( rust_col_inner );
//
//	//========================= 錆領域の外側の色を計算する ========================//
//	vec4 rust_col_outer = background_col;
//
//	// 黄色い部分の計算
//	float base_weight	= smoothstep( g_OuterBaseThreashold_start, g_OuterBaseThreashold_end, rust_amount );
//	rust_col_outer	= mix( rust_col_outer, rust_base_col, base_weight );
//
//	// 錆流れ部分の計算
//	float bleed_weight = smoothstep( g_OuterBleedThreashold_start, g_OuterBleedThreashold_end, rust_amount );
//	rust_col_outer		= mix( rust_col_outer, rust_bleed_col, bleed_weight );
//
////outColor	= vec4( rust_localdistrib, rust_localdistrib, rust_localdistrib, 0.0 );
//
//
//	//============================ 色を合成する ===================================//
//	float rust_blend_weight	= smoothstep( g_RustDistribThreshold_start, g_RustDistribThreshold_end, rust_amount );
////outColor				= vec4( rust_blend_weight, rust_blend_weight, rust_blend_weight, 0.0 );
//
//	outColor	= mix( rust_col_outer, rust_col_inner, rust_blend_weight );
//	
//
////outColor	= rust_base_col;//vec4( rust_inner_mat, rust_inner_mat, rust_inner_mat, 0 );
//
//
//
//
//
//
///*
//// 黄色い部分の計算
//float base_weight	= smoothstep( g_BaseThreashold_start, g_BaseThreashold_end, rust_inner_mat );
//rust_col	= mix( rust_col, rust_base_col, base_weight );
//
//// 黒い部分の計算
//float spot_distrib	= texture( g_SpotDistribution, vTexCoord.xy ).x * 0.8 + rust_inner_mat * 0.2;
//float spot_weight	= smoothstep( g_RustSpotThreshold_start, g_RustSpotThreshold_end, spot_distrib );//rust_inner_mat );
//rust_col	= mix( rust_col, rust_spot_col, spot_weight );
//
//// 錆流れ部分の計算
//float bleed_weight = smoothstep( g_BleedThreashold_end, g_BleedThreashold_start, rust_inner_mat );
//rust_col		= mix( rust_col, rust_bleed_col, bleed_weight );
//*/
//
//
//
//
////outColor	= rust_col;
//
//
////outColor	= vec4( spot_distrib, spot_distrib, spot_distrib, 0 );
//
//
///*
//// 後で修正する
//float base_weight	= smoothstep( g_BaseThreashold_start, g_BaseThreashold_end, rustdensity ) * g_BaseAmount;
//vec4 rust_col	= mix( background_col, rust_base_col, base_weight );
//
//
//
//
//	// bleed_colのエリアを決める
//	float bleed_weight	= smoothstep( g_BleedThreashold_start, g_BleedThreashold_end, rustdensity );
//	rust_col		= mix( rust_col, rust_bleed_col, bleed_weight );
//
//	// 錆発生点と近傍の色を計算する
//	rust_col = mix( rust_col, rust_spot_col, spot_weight );
//
//	outColor = rust_col;
//	*/
//	
//	//outColor = vec4(vec3(rust_shape, rust_shape, rust_shape), 1.0);
//	
//
//
//}


void main (void)
{

	//================================= test output ==========================================================//
	//float	n	= Simplex( vec3( vTexCoord, 0.0 )*g_Scale );
	//loat	n	= Worley_fBm( vec3( vTexCoord, 0.0 )*g_Scale, 1, 1.0, 1.0 );
	//outColor = vec4(vec3(n, n, n), 1.0);

	
	// サーフェス全体の錆分布の重みを取得
	float rust_map		= texture( g_RustMap, vTexCoord.xy ).x * g_Aging;
	//outColor = vec4( rust_map, rust_map, rust_map, 0.0 );

	// 錆そのものの分布形状
	float rust_shape;//	= texture( g_RustLocalShape, vTexCoord.xy ).x;
	
	//================================================= 錆の詳細形状マップ ===================================================//
	rust_shape	=	clamp(
							g_Aging *
							(
								0.35 * ( Simplex_fBm( (g_Offset+vec3(vTexCoord.xy,0.0)) * RUST_SHAPE_SCALE * g_Scale, 4, 2.5, 0.75 ) * 0.5 + 0.5 ) + 
								0.65 * Worley_fBm( (g_Offset+vec3(vTexCoord.xy,0.0)) * RUST_SHAPE_SCALE * g_Scale, 4, 1.5, 0.5 )
							),
							0.0,
							1.0 );
	const float rust_amount	= rust_shape * rust_map;
	float rust_blend_weight	= smoothstep( g_RustDistribThreshold_start, g_RustDistribThreshold_end, rust_amount );


	vec4 rust_col_inner = background_col;
	vec4 rust_col_outer = background_col;

	//========================= 錆領域内部の色を計算する ==========================//
	if( rust_blend_weight>1.0e-2 )
	{
		calcRustInnerColor( rust_col_inner );
	}
	//========================= 錆領域の外側の色を計算する ========================//
	// 黄色い部分の計算
		float base_weight	= smoothstep( g_OuterBaseThreashold_start, g_OuterBaseThreashold_end, rust_amount );
		rust_col_outer	= mix( rust_col_outer, rust_base_col, base_weight );

		// 錆流れ部分の計算
		float bleed_weight = smoothstep( g_OuterBleedThreashold_start, g_OuterBleedThreashold_end, rust_amount );
		rust_col_outer		= mix( rust_col_outer, rust_bleed_col, bleed_weight );
	

	//============================ 色を合成する ===================================//
	outColor	= mix( rust_col_outer, rust_col_inner, rust_blend_weight );
	
}





#endif
