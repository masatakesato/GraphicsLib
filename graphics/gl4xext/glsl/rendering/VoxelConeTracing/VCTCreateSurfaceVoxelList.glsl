// Include Files
#pragma include "VectorFormatConversion.glsl"


// Uniform Locations
layout( binding=0, r32ui )	uniform readonly uimage3D		g_TexColor;

layout( binding=0 )			uniform atomic_uint				g_SurfaceVoxelCount;
//layout( binding=1, r32ui )	uniform coherent uimageBuffer	g_SurfaceVoxelList;// TBO version
layout( std430, binding = 0 ) coherent buffer surfaceVoxelList// SSBO version
{
	uint g_SurfaceVoxelList[];
};



// Constant Variables
const ivec3 g_TexResolution	= imageSize( g_TexColor );


// Workgroup Layout
layout( local_size_x=8, local_size_y=8, local_size_z=8 ) in;


// Main Kernel
void main()
{
	ivec3 voxelPos	= ivec3( gl_GlobalInvocationID.xyz );
	
	// メモリ領域外のスレッドは除外する
	if( any( greaterThanEqual(voxelPos, g_TexResolution) ) )
		return;

	// 色情報をuint(rgba8)からvec4に展開する
	vec4 texelColor		= convRGBA8ToVec4( imageLoad( g_TexColor, voxelPos ).x );
	
	// 透明なボクセルは除外する
	if( texelColor.w==0.0 )
		return;

	// アトミックカウンタのインクリメント
	uint counter	= atomicCounterIncrement( g_SurfaceVoxelCount );
	memoryBarrier();
	
	// texelPosをR10G10B10A2フォーマットに圧縮する
	uint voxelPosXYZ10	= convVec3ToUINTXYZ10( uvec3(voxelPos) );
	//imageStore( g_SurfaceVoxelList, int(counter), uvec4(voxelPosXYZ10) );// TBO version
	g_SurfaceVoxelList[counter] = voxelPosXYZ10;// SSBO version
}