// Include Files
#pragma include "Common.glsl"
#pragma include "LightAttribute.glsl"
#pragma include "ShadowMap.glsl"
#pragma include "vct/VCTCommon.glsl"
#pragma include "VectorFormatConversion.glsl"


// Uniform Locations



// Random Accessible Buffers
layout( binding=0 )			uniform atomic_uint				g_SurfaceVoxelCount;
//layout( binding=1, r32ui )	uniform readonly uimageBuffer	g_SurfaceVoxelList;// TBO verion
layout( std430, binding = 0 ) readonly buffer surfaceVoxelList// SSBO version
{
	uint g_SurfaceVoxelList[];
};


layout( binding=2, r32ui )	uniform readonly uimage3D		g_TexColor;
layout( binding=3, r32ui )	uniform readonly uimage3D		g_TexNormal;

layout( binding=4, rgba16f )	uniform writeonly image3D	g_TexIrradiance;


// Constant Variables
//const ivec3 g_TexResolution		= imageSize( g_TexColor );
const uint	g_NumVoxelFragments	= atomicCounter( g_SurfaceVoxelCount );


// Workgroup Layout
layout( local_size_x=256, local_size_y=1, local_size_z=1 ) in;



// Main Kernel
void main()
{
	uint threadIndex	= gl_GlobalInvocationID.x;

	// メモリ領域外のスレッドは除外する
	if( threadIndex >= g_NumVoxelFragments )
		return;

	// ボクセルのインデックスを取得する
	//ivec3 voxelPos	= ivec3( convUINTXYZ10ToVec3( imageLoad( g_SurfaceVoxelList, int( threadIndex ) ).x ) );// TBO version
	ivec3 voxelPos	= ivec3( convUINTXYZ10ToVec3( g_SurfaceVoxelList[threadIndex] ) );// SSBO version
	
	
	// テスト実装1. 法線ベクトルを取得する.動作確認済
	vec4 texNormal	= convRGBA8ToVec4( imageLoad( g_TexNormal, voxelPos ).x ) / 255.0;
	vec3 wsNormal	= normalize( texNormal.xyz * 2.0 - vec3(1.0) );
	
	// テスト実装2. ボクセル毎の色情報を取得する.動作確認済
	vec4 voxelColor	= convRGBA8ToVec4( imageLoad( g_TexColor, voxelPos ).x ) / 255.0;
	
	// テスト実装3. ボクセルインデックスからワールド空間座標に変換する. 動作確認済
	vec4 wsPosition	= vec4( voxelPos, 1.0 ) * g_MatCellToWorldSpace;
	
	wsPosition.xyz += wsNormal * g_VoxelDim * 0.5;// ちょっとだけ法線ベクトル方向に移動.シャドウマップのサンプリングでアーティファクト減らす


	const int lightId	= 0;


	float n_dot_l	= dot( g_LightDir( lightId ), wsNormal );

	
	float viewDepth	= max( distance( g_ViewAttrib.Position, wsPosition.xyz ) - g_ViewAttrib.zNear, 0.0 );
	int shadowMapId	= g_ShadowCasterIndex( lightId );
	int	numShadows	= g_NumShadows( lightId );
	float islit		= float( GetShadowDistance( wsPosition, numShadows, shadowMapId, viewDepth ) < 0.0 );
	
	vec4 irrad		= vec4( 0, 0, 0, voxelColor.w );// サーフェスと交差するボクセルを黒く塗りつぶすだけ.
	//vec4 irrad	= vec4( voxelColor.xyz * clamp( n_dot_l, 0.0, 1.0 ) * islit, voxelColor.w );// サーフェスと交差するボクセルに陰影付け(影あり)する
	
	imageStore( g_TexIrradiance, voxelPos, irrad );


}