// Include Files
#pragma include "exp/GPUErosion/GPUErosion_Common.glsl"


// Uniform Locations
uniform sampler2D		g_TexTerrain;


// Random Accessible Buffers
layout( binding = 0 )	uniform writeonly image2D	g_TexDest;


// Constant Variables
#define	g_TexSize	g_ErosionAttribs.TexSize;


// Workgroup Layout



// Main Kernel
void main()
{
	ivec2 texelPos	= ivec2( gl_GlobalInvocationID.xy );
	if( texelPos.x >= int(g_TexSize.x) || texelPos.y >= int(g_TexSize.y) )
		return;

	float terrainHeight	= texelFetch( g_TexTerrain, texelPos, 0 ).x;

	imageStore( g_TexDest, texelPos, vec4(terrainHeight) );

}
