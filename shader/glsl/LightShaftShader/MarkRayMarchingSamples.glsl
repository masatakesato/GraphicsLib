// MinMaxMimMap作成は２ステップ必要
//ステップ１：シャドウマップの8近傍ピクセルを探索して、奥行きの最大値/最小値を計算する
//ステップ2：奥行き最大最小テクスチャを縮小した1DMinMaxMipMapを作成する		←　このシェーダーはこっち


// Include Files
#pragma include "Common.glsl"
#pragma include "LightShaftShader/LightShaftShaderCommon.glsl"
#pragma include "LightAttribute.glsl"
#pragma include "ShadowMap.glsl"



// Uniform Locations


// Random Accessible Buffers
layout( binding=0, rg16ui )	uniform readonly uimage2D	g_TexInterpolationSource;
layout( binding=1, r8 )		uniform image2D				g_TexEpipolarDepthStencil;


// Constant Variables


// Workgroup Layout
layout( local_size_x=16, local_size_y=16, local_size_z=1 ) in;



void main()
{
	ivec2 storePos		= ivec2( gl_GlobalInvocationID.xy );

	uvec2 sampleSource		= imageLoad( g_TexInterpolationSource, storePos ).xy;
	float sampleStenicil	= imageLoad( g_TexEpipolarDepthStencil, storePos ).x;

	if( sampleSource.x != sampleSource.y )// if valid raymarching sample, just keep current depth value
	{
		//imageStore( g_TexEpipolarDepthStencil, storePos, vec4( 0 ) );
		return;
	}
	imageStore( g_TexEpipolarDepthStencil, storePos, vec4( 0.5 ) );//vec4( 0.25) );// if invalid sample, invalidate depth value.
}
