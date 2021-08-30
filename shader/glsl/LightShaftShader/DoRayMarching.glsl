
// Include Files
#pragma include "Common.glsl"
#pragma include "LightShaftShader/LightShaftShaderCommon.glsl"
#pragma include "LightAttribute.glsl"
#pragma include "ShadowMap.glsl"


// Uniform Locations


// Random Accessible Buffers
layout( binding=0, rg32f )		uniform readonly image2D	g_TexEpipolarCoordinate;
layout( binding=1, r32f )		uniform readonly image2D	g_TexEpipolarCameraSpaceZ;
layout( binding=1, rgba32f )	uniform readonly image2D	g_TexSliceUvOriginDirection;
layout( binding=2, rg16f )		uniform readonly image2D	g_Tex1DMinMaxMipMap;
layout( binding=3, r8 )			uniform readonly image2D	g_TexEpipolarDepthStencil;

layout( binding=4, rgba16f )	uniform writeonly image2D	g_TexInitialScattering;


// Constant Variables


// Workgroup Layout( num_groups_X: numSamplesInSlice dimension, num_groups_Y: numEpipolarSlices dimension )
layout( local_size_x=16, local_size_y=16, local_size_z=1 ) in;



void main()
{
	ivec2 storePos		= ivec2( gl_GlobalInvocationID.xy );

	//float sampleStenicil	= imageLoad( g_TexEpipolarDepthStencil, storePos ).x;

	//g_TexSliceUvOriginDirection();



	imageStore( g_TexInitialScattering, storePos, vec4( 0.5, 0.25, 0.3, 0.0 ) );
}


/*
// 

// TODO: レイマーチングの始点終点はシャドウマップ空間？それともワールド空間?
// TODO: 部分空間の長さをどうやって計算する？


// g_TexSliceUvOriginDirection.xy : 向き()
// g_TexSliceUvOriginDirection.zw : 始点 
// 終点は？？？　g_TexEpipolarCoordinate.xy(スクリーン座標) + g_TexEpipolarCameraSpaceZ.x


// Tex1DMinMaxMipMapの1テクセル -> 


float marchingDistance	= 0.0;
int Level				= 0;
int sampleIndex			= 0;


while( marchingDistance < )
{
	if( sampleIndex % pow(2,Level+1) == 0 )// 自分よりも上の
		Level += 1;
	
	while( Level > 0 )
	{
		//
		float dStart	= ;
		float dEnd		= ;
	
		//
		float dMin		= ;
		float dMiax		= ;
	
	
	}



}

*/

