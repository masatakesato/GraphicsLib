// Include Files
#pragma include "Common.glsl"
#pragma include "LightShaftShader/LightShaftShaderCommon.glsl"
#pragma include "LightAttribute.glsl"
#pragma include "ShadowMap.glsl"

// Uniform Locations


// Random Accessible Buffers
layout( binding=5, rgba32f ) uniform readonly image2D	g_TexSliceUvOriginDirection;// xy: シャドウマップ空間のスライスベクトル, zw: シャドウマップ空間のスライス原点, 

layout( binding=1, rg16f )	uniform writeonly image2D	g_TexMinMaxShadowMap;



// Constant Variables
const ivec2	g_TexMinMaxShadowMapSize	= ivec2( int( g_ShadowMapResolution.x ), int( g_numEpipolarSlices * g_NumShadows( g_EpipolarLightParam.LightID ) ) );
const vec2	g_SliceOrigDirTexelSize		= 1.0 / vec2( imageSize( g_TexSliceUvOriginDirection ) );




// Workgroup Layout
layout( local_size_x=32, local_size_y=32, local_size_z=1 ) in;




//// 以下のようなデータを出力したい
//// x軸方向に、スライス1個分のシャドウマップサンプルデータ(EpipolarSliceベクトル上の)が並んでいる。
//// y軸方向に、カスケード毎のEpilplarSliceのブロックが並んでいる
//+------------	cascade 1	------------+
//|	epipolar slice1 minmax depth		|					
//|	epipolar slice2 minmax depth		|
//|	...									|
//|										|
//+------------	cascade 2	------------+
//|	epipolar slice1 minmax depth		|
//|	epipolar slice2 minmax depth		|
//|	...									|
//|										|
//+------------	cascade 3	------------+



// Main Kernel
void main()
{

	int numCascades			= g_NumShadows( g_EpipolarLightParam.LightID );
	int shadowCasterIndex	= g_ShadowCasterIndex( g_EpipolarLightParam.LightID );

	//========================= 無効なスレッドは除外する ================================//
	ivec2 storePos	= ivec2( gl_GlobalInvocationID.xy );
	//uint sharedIdx	= gl_GlobalInvocationID.x % LOCAL_SIZE_X;
	if( storePos.x >= g_TexMinMaxShadowMapSize.x/2 || storePos.y >= g_TexMinMaxShadowMapSize.y )	return;

	
	// テクスチャのサンプル情報
	uint cascadeIndex	= int( gl_GlobalInvocationID.y / g_numEpipolarSlices );//
	int sliceIndex		= int( gl_GlobalInvocationID.y - cascadeIndex * g_ShadowMapResolution.x );
	
	vec4 uvOriginDirection	= imageLoad( g_TexSliceUvOriginDirection, ivec2( sliceIndex, cascadeIndex ) ).xyzw;
	vec2 currUV				= uvOriginDirection.zw + uvOriginDirection.xy * float(gl_GlobalInvocationID.x) * 2.0;// uvスライス始点 + uv移動ベクトル. 2.0は、解像度半分でシャドウマップダウンサンプリングするため
	
	if( length( uvOriginDirection.xy )==0  || currUV.x<0.0 || currUV.x>1.0 || currUV.y<0.0 || currUV.y>1.0 )
	{
		imageStore( g_TexMinMaxShadowMap, storePos, vec4( 0.0 ) );
		return;
	}

	vec4 minDepths4	= vec4( FLT_MAX );
	vec4 maxDepths4	= vec4( 0.0 );


	// スライス上の近傍8ピクセルをサンプリングして、奥行きの最大値/最小値を計算する
	for( int i=0; i < 1; ++i )
	{
		vec4 depths4	= abs( g_ShadowMapDepthGather( currUV + uvOriginDirection.xy * float(i), shadowCasterIndex ) );
		minDepths4		= min( minDepths4, depths4 );
		maxDepths4		= max( maxDepths4, depths4 );
	}

	minDepths4.xy		= min( minDepths4.xy, minDepths4.zw );
	minDepths4.x		= min( minDepths4.x, minDepths4.y );

	maxDepths4.xy		= max( maxDepths4.xy, maxDepths4.zw );
	maxDepths4.x		= max( maxDepths4.x, maxDepths4.y );
	
	imageStore( g_TexMinMaxShadowMap, storePos, vec4( minDepths4.x, maxDepths4.x, 0.0, 0.0 ) );
	//imageStore( g_TexMinMaxShadowMap, storePos, vec4( 0.5 ) );
	
}

