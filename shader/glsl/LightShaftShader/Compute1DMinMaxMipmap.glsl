
// MinMaxMimMap作成は２ステップ必要
//ステップ１：シャドウマップの8近傍ピクセルを探索して、奥行きの最大値/最小値を計算する
//ステップ2：奥行き最大最小テクスチャを縮小した1DMinMaxMipMapを作成する		←　このシェーダーはこっち


// Include Files
#pragma include "Common.glsl"
#pragma include "LightShaftShader/LightShaftShaderCommon.glsl"
#pragma include "LightAttribute.glsl"
#pragma include "ShadowMap.glsl"



// Uniform Locations
uniform int	g_MinMaxSampleOffset;
uniform int	g_MinMaxStoreOffset;


// Random Accessible Buffers
layout( binding=0, rg16f )	uniform image2D	g_Tex1DMinMaxMipMap;// r: g: 


// Constant Variables
//const uvec2	g_TexResolution	= imageSize( g_Tex1DMinMaxMipMap );


// Workgroup Layout
layout( local_size_x=16, local_size_y=16, local_size_z=1 ) in;




// Main Kernel
//void main()// Alternative code. calculate all mip levels at once. )
//{
//	ivec2 storePos	= ivec2( gl_GlobalInvocationID.xy );
////	if( storePos.x >= g_ShadowMapTexelSize.x || storePos.y >= g_numEpipolarSlices )	return;
//
//	vec2 sampleDepth1	= imageLoad( g_Tex1DMinMaxMipMap, ivec2( storePos.x*2, storePos.y ) ).xy;
//	vec2 sampleDepth2	= imageLoad( g_Tex1DMinMaxMipMap, ivec2( storePos.x*2 + 1, storePos.y ) ).xy;
//
//	float minDepth		= min( sampleDepth1.x, sampleDepth2.x );
//	float maxDepth		= max( sampleDepth1.y, sampleDepth2.y );
//
//	storePos.x += g_MinMaxLevelOffset;
//
//	imageStore( g_Tex1DMinMaxMipMap, storePos, vec4( minDepth, maxDepth, 0.0, 0.0 ) );
//}




void main()
{
	//if( any( greaterThanEqual( gl_GlobalInvocationID.xy, g_TexResolution) ) )
	//	return;

	ivec2 samplePos		= ivec2( gl_GlobalInvocationID.x * 2 + g_MinMaxSampleOffset, gl_GlobalInvocationID.y );
	vec2 sampleDepth1	= imageLoad( g_Tex1DMinMaxMipMap, samplePos ).xy;
	vec2 sampleDepth2	= imageLoad( g_Tex1DMinMaxMipMap, ivec2( samplePos.x + 1, samplePos.y ) ).xy;

	float minDepth		= min( sampleDepth1.x, sampleDepth2.x );
	float maxDepth		= max( sampleDepth1.y, sampleDepth2.y );

	ivec2 storePos		= ivec2( gl_GlobalInvocationID.x + g_MinMaxStoreOffset, gl_GlobalInvocationID.y );
	imageStore( g_Tex1DMinMaxMipMap, storePos, vec4( minDepth, maxDepth, 0.0, 0.0 ) );
}
