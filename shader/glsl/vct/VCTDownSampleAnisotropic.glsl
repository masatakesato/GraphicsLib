
// Include Files


// Uniform Locations
uniform sampler3D	g_TexSource3D;
uniform int			g_SrcMipMapLevel;


// Random Accessible Buffers
//layout( binding = 0, rgba32f )	uniform readonly image3D	g_TexSource;
layout( binding = 1, rgba32f )	uniform writeonly image3D	g_TexDest;


// Constant Variables
const ivec3	g_SourceResolution	= textureSize( g_TexSource3D, g_SrcMipMapLevel );
const vec3	g_SourceVoxelSize	= 1.0 / vec3( g_SourceResolution );
const ivec3	g_DestResolution	= g_SourceResolution / 2;// half size of Source Texture


// Workgroup Layout
layout( local_size_x=8, local_size_y=8, local_size_z=8 ) in;// 3D layout. xyx = uvw



// Main Kernel
void main()
{	
	ivec3 storePos = ivec3( gl_GlobalInvocationID.xyz );// データ格納先のアドレス
	if( storePos.x >= g_DestResolution.x || storePos.y >= g_DestResolution.y || storePos.z >= g_DestResolution.z )	return;
	
	ivec3 centerPos	= storePos * 2;

	vec4 outColor;


#if DIRECTION == 0

	vec4 srcColor0	= texelFetch( g_TexSource3D, centerPos + ivec3(-1,0,0), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor1	= texelFetch( g_TexSource3D, centerPos + ivec3(0,0,0), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor2	= texelFetch( g_TexSource3D, centerPos + ivec3(-1,1,0), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor3	= texelFetch( g_TexSource3D, centerPos + ivec3(0,1,0), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor4	= texelFetch( g_TexSource3D, centerPos + ivec3(-1,0,1), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor5	= texelFetch( g_TexSource3D, centerPos + ivec3(0,0,1), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor6	= texelFetch( g_TexSource3D, centerPos + ivec3(-1,1,1), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor7	= texelFetch( g_TexSource3D, centerPos + ivec3(0,1,1), g_SrcMipMapLevel ).xyzw;

	// +X
	outColor =
		srcColor0 + srcColor1 * (1.0 - srcColor0.w)
		+ srcColor2 + srcColor3 * (1.0 - srcColor2.w)
		+ srcColor4 + srcColor5 * (1.0 - srcColor4.w)
		+ srcColor6 + srcColor7 * (1.0 - srcColor6.w);

#elif DIRECTION == 1

	vec4 srcColor0	= texelFetch( g_TexSource3D, centerPos + ivec3(1,0,0), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor1	= texelFetch( g_TexSource3D, centerPos + ivec3(2,0,0), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor2	= texelFetch( g_TexSource3D, centerPos + ivec3(1,1,0), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor3	= texelFetch( g_TexSource3D, centerPos + ivec3(2,1,0), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor4	= texelFetch( g_TexSource3D, centerPos + ivec3(1,0,1), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor5	= texelFetch( g_TexSource3D, centerPos + ivec3(2,0,1), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor6	= texelFetch( g_TexSource3D, centerPos + ivec3(1,1,1), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor7	= texelFetch( g_TexSource3D, centerPos + ivec3(2,1,1), g_SrcMipMapLevel ).xyzw;

	// -X
	outColor =
		srcColor1 + srcColor0 * (1.0 - srcColor1.w)
		+ srcColor3 + srcColor2 * (1.0 - srcColor3.w)
		+ srcColor5 + srcColor4 * (1.0 - srcColor5.w)
		+ srcColor7 + srcColor6 * (1.0 - srcColor7.w);

#elif DIRECTION == 2

	vec4 srcColor0	= texelFetch( g_TexSource3D, centerPos + ivec3(0,-1,0), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor1	= texelFetch( g_TexSource3D, centerPos + ivec3(1,-1,0), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor2	= texelFetch( g_TexSource3D, centerPos + ivec3(0,0,0), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor3	= texelFetch( g_TexSource3D, centerPos + ivec3(1,0,0), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor4	= texelFetch( g_TexSource3D, centerPos + ivec3(0,-1,1), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor5	= texelFetch( g_TexSource3D, centerPos + ivec3(1,-1,1), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor6	= texelFetch( g_TexSource3D, centerPos + ivec3(0,0,1), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor7	= texelFetch( g_TexSource3D, centerPos + ivec3(1,0,1), g_SrcMipMapLevel ).xyzw;

	// +Y
	outColor =
		srcColor0 + srcColor2 * (1.0 - srcColor0.w)
		+ srcColor1 + srcColor3 * (1.0 - srcColor1.w)
		+ srcColor4 + srcColor6 * (1.0 - srcColor4.w)
		+ srcColor5 + srcColor7 * (1.0 - srcColor5.w);

#elif DIRECTION == 3

	vec4 srcColor0	= texelFetch( g_TexSource3D, centerPos + ivec3(0,1,0), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor1	= texelFetch( g_TexSource3D, centerPos + ivec3(1,1,0), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor2	= texelFetch( g_TexSource3D, centerPos + ivec3(0,2,0), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor3	= texelFetch( g_TexSource3D, centerPos + ivec3(1,2,0), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor4	= texelFetch( g_TexSource3D, centerPos + ivec3(0,1,1), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor5	= texelFetch( g_TexSource3D, centerPos + ivec3(1,1,1), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor6	= texelFetch( g_TexSource3D, centerPos + ivec3(0,2,1), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor7	= texelFetch( g_TexSource3D, centerPos + ivec3(1,2,1), g_SrcMipMapLevel ).xyzw;

	// -Y
	outColor =
		srcColor2 + srcColor0 * (1.0 - srcColor2.w)
		+ srcColor3 + srcColor1 * (1.0 - srcColor3.w)
		+ srcColor6 + srcColor4 * (1.0 - srcColor6.w)
		+ srcColor7 + srcColor5 * (1.0 - srcColor7.w);

#elif DIRECTION == 4

	vec4 srcColor0	= texelFetch( g_TexSource3D, centerPos + ivec3(0,0,-1), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor1	= texelFetch( g_TexSource3D, centerPos + ivec3(1,0,-1), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor2	= texelFetch( g_TexSource3D, centerPos + ivec3(0,1,-1), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor3	= texelFetch( g_TexSource3D, centerPos + ivec3(1,1,-1), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor4	= texelFetch( g_TexSource3D, centerPos + ivec3(0,0,0), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor5	= texelFetch( g_TexSource3D, centerPos + ivec3(1,0,0), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor6	= texelFetch( g_TexSource3D, centerPos + ivec3(0,1,0), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor7	= texelFetch( g_TexSource3D, centerPos + ivec3(1,1,0), g_SrcMipMapLevel ).xyzw;

	// +Z
	outColor =
		srcColor0 + srcColor4 * (1.0 - srcColor0.w)
		+ srcColor1 + srcColor5 * (1.0 - srcColor1.w)
		+ srcColor2 + srcColor6 * (1.0 - srcColor2.w)
		+ srcColor3 + srcColor7 * (1.0 - srcColor3.w);

#elif DIRECTION == 5

	vec4 srcColor0	= texelFetch( g_TexSource3D, centerPos + ivec3(0,0,1), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor1	= texelFetch( g_TexSource3D, centerPos + ivec3(1,0,1), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor2	= texelFetch( g_TexSource3D, centerPos + ivec3(0,1,1), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor3	= texelFetch( g_TexSource3D, centerPos + ivec3(1,1,1), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor4	= texelFetch( g_TexSource3D, centerPos + ivec3(0,0,2), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor5	= texelFetch( g_TexSource3D, centerPos + ivec3(1,0,2), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor6	= texelFetch( g_TexSource3D, centerPos + ivec3(0,1,2), g_SrcMipMapLevel ).xyzw;
	vec4 srcColor7	= texelFetch( g_TexSource3D, centerPos + ivec3(1,1,2), g_SrcMipMapLevel ).xyzw;

	// -Z
	outColor =
		srcColor4 + srcColor0 * (1.0 - srcColor4.w)
		+ srcColor5 + srcColor1 * (1.0 - srcColor5.w)
		+ srcColor6 + srcColor2 * (1.0 - srcColor6.w)
		+ srcColor7 + srcColor3 * (1.0 - srcColor7.w);
#else
# error shouldn't be here
#endif

	outColor *= 0.25;


	imageStore( g_TexDest, storePos, outColor );//vec4( vec3(storePos) / vec3(g_DestResolution), 1.0) );//

}
