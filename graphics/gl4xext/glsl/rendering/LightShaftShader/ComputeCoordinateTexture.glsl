
// Include Files
#pragma include "LightShaftShader/LightShaftShaderCommon.glsl"


// Uniform Locations
uniform sampler1D	g_SliceEndPoints;
//uniform sampler2D	g_TexPosition;
uniform sampler2D	g_TexDepth;


// Random Accessible Buffers
layout( binding = TEX_UNIT_EPIPOLAR_COORDINATE, rg32f )	uniform writeonly image2D	destEpipolarCoordinate;
layout( binding = TEX_UNIT_EPIPOLAR_CAM_SPACE_Z, r32f )	uniform writeonly image2D	destEpipolarCameraSpaceZ;
layout( binding = TEX_UNIT_EPIPOLAR_DEPTH, r8 )			uniform writeonly image2D	destEpipolarDepthStencil;
//layout( binding = 3 ) uniform writeonly image2D destEpipolarWorldPos;

//const ivec2 g_ImageSize	= imageSize( destEpipolarCoordinate );// writeonlyはだめらしい


// Workgroup Layout
layout( local_size_x=16, local_size_y=16 ) in;// 2D layout. x: numSamplesInSlice dimension, y: numEpipolarSlices dimension



// Main Kernel
void main()
{	
	ivec2 storePos = ivec2( gl_GlobalInvocationID.xy );// データ格納先のアドレス
	if( storePos.x >= g_numSamplesInSlice || storePos.y >= g_numEpipolarSlices )	return;
	
	vec4 samplePoints	= texture( g_SliceEndPoints, float(storePos.y) / float(g_numEpipolarSlices-1) ).xyzw;
	
	// 端点を補間する
	float interpolationCoeff	= storePos.x / float(g_numSamplesInSlice-1);

	vec2 samplePoint	= mix( samplePoints.xy, samplePoints.zw, interpolationCoeff );

	if( any( greaterThan( abs(samplePoint), vec2(1.0+1e-4) ) ) )
	{
		imageStore( destEpipolarCoordinate, storePos, vec4(0.0) );
		imageStore( destEpipolarCameraSpaceZ, storePos, vec4(0.0) );
		imageStore( destEpipolarDepthStencil, storePos, vec4( 0.0 ) );
		return;
	}

	float depth			= texture( g_TexDepth, samplePoint*0.5 + vec2(0.5) ).r;

	imageStore( destEpipolarCoordinate, storePos, vec4(samplePoint, 0.0, 0.0) );//vec4(1) );//
	imageStore( destEpipolarCameraSpaceZ, storePos, vec4(depth) );//vec4(1) );//
	imageStore( destEpipolarDepthStencil, storePos, vec4(0.25) );

}
