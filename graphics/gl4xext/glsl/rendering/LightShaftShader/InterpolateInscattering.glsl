// Include Files
#pragma include "Common.glsl"
#pragma include "LightShaftShader/LightShaftShaderCommon.glsl"
#pragma include "LightAttribute.glsl"
#pragma include "ShadowMap.glsl"


// Uniform Locations


// Random Accessible Buffers
layout( binding=0, rg32f )		uniform readonly image2D	g_TexInterpolationSource;
layout( binding=1, rgba16f )	uniform readonly image2D	g_TexInitialInscattering;
layout( binding=2, rgba16f )	uniform writeonly image2D	g_TexInterpolatedInscattering;


// Constant Variables


// Workgroup Layout
layout( local_size_x=16, local_size_y=16, local_size_z=1 ) in;



void main()
{
	ivec2 storePos		= ivec2( gl_GlobalInvocationID.xy );

	//float sampleStenicil	= imageLoad( g_TexEpipolarDepthStencil, storePos ).x;


	imageStore( g_TexInterpolatedInscattering, storePos, vec4( 0.5, 0.25, 0.3, 0.0 ) );
}
