// Include Files
#pragma include "VectorFormatConversion.glsl"
#pragma include "VoxelConeTracing/VCTCommon.glsl"


// Uniform Locations
layout( binding=0, rgba16f )	uniform readonly image3D	g_TexIrradiance;	// TODO: テクスチャデータ読めない.原因調査 2015.04.22-> VCTVoxelData->m_TexIrradianceのフォーマットがRGBA32Fになってた.RGBA16Fに変更後、正常動作確認
layout( binding=1, r32ui )		uniform readonly uimage3D	g_TexNormal;

layout( binding=2, rgba16f )	uniform writeonly image3D	g_TexIrradianceAnisoXPos;
layout( binding=3, rgba16f )	uniform writeonly image3D	g_TexIrradianceAnisoXNeg;
layout( binding=4, rgba16f )	uniform writeonly image3D	g_TexIrradianceAnisoYPps;
layout( binding=5, rgba16f )	uniform writeonly image3D	g_TexIrradianceAnisoYNeg;
layout( binding=6, rgba16f )	uniform writeonly image3D	g_TexIrradianceAnisoZPos;
layout( binding=7, rgba16f )	uniform writeonly image3D	g_TexIrradianceAnisoZNeg;


// Constant Variables
const ivec3 g_TexResolution	= imageSize( g_TexIrradiance );


// Workgroup Layout
layout( local_size_x=8, local_size_y=8, local_size_z=8 ) in;



// Main Kernel
void main()
{
	ivec3 voxelPos	= ivec3( gl_GlobalInvocationID.xyz );
	
	// メモリ領域外のスレッドは除外する
	if( any( greaterThanEqual(voxelPos, g_TexResolution) ) )
		return;
	
	// ボクセルの色を取得する
	// TODO: テクスチャデータ読めない.原因調査 2015.04.22-> VCTVoxelData->m_TexIrradianceのフォーマットがRGBA32Fになってた.RGBA16Fに変更後、正常動作確認
	vec4 texIrrad	= imageLoad( g_TexIrradiance, voxelPos ).xyzw;

	// 中身の入ってないボクセルは処理中止する
	if( texIrrad.w==0 )
		return;
	
	// ボクセルの法線ベクトルを取得し、テクスチャ空間に変換する
	vec4 texNormal	= convRGBA8ToVec4( imageLoad( g_TexNormal, voxelPos ).x ) / 255.0;
	vec3 wsNormal	= texNormal.xyz * 2.0 - vec3(1.0);
	vec3 tsNormal	= normalize( wsNormal * mat3(g_MatTexSpace) );
	

	imageStore( g_TexIrradianceAnisoXPos, voxelPos,
		vec4( texIrrad.xyz, /*max(dot(tsNormal,vec3(+1,0,0)),0) */ texIrrad.w ) * float( +tsNormal.x>1.0e-2 ) );//outColor * float(+gNormal.x>1.0e-3);//vec4( gFlux * max( dot(gNormal, vec3(+1,0,0)), 0 ), 1.0 );//outColor * max( dot( gNormal, vec3(+1,0,0) ), 0 );//float(+gNormal.x>0);//
	
	imageStore( g_TexIrradianceAnisoXNeg, voxelPos,
		vec4( texIrrad.xyz, /*max(dot(tsNormal,vec3(-1,0,0)),0) */ texIrrad.w ) * float( -tsNormal.x>1.0e-2 ) );//outColor * float(-gNormal.x>1.0e-3);//vec4( gFlux * max( dot(gNormal, vec3(-1,0,0)), 0 ), 1.0 );//outColor * max( dot( gNormal, vec3(-1,0,0) ), 0 );//float(-gNormal.x>0);//
	
	imageStore( g_TexIrradianceAnisoYPps, voxelPos,
		vec4( texIrrad.xyz, /*max(dot(tsNormal,vec3(0,+1,0)),0) */ texIrrad.w ) * float( +tsNormal.y>1.0e-2 ) );//outColor * float(+gNormal.y>1.0e-3);//vec4( gFlux * max( dot(gNormal, vec3(0,+1,0)), 0 ), 1.0 );//outColor * max( dot( gNormal, vec3(0,+1,0) ), 0 );//float(+gNormal.y>0);//
	
	imageStore( g_TexIrradianceAnisoYNeg, voxelPos,
		vec4( texIrrad.xyz, /*max(dot(tsNormal,vec3(0,-1,0)),0) */ texIrrad.w ) * float( -tsNormal.y>1.0e-2 ) );//outColor * float(-gNormal.y>1.0e-3);//vec4( gFlux * max( dot(gNormal, vec3(0,-1,0)), 0 ), 1.0 );//outColor * max( dot( gNormal, vec3(0,-1,0) ), 0 );//float(-gNormal.y>0);//
	
	imageStore( g_TexIrradianceAnisoZPos, voxelPos,
		vec4( texIrrad.xyz, /*max(dot(tsNormal,vec3(0,0,+1)),0) */ texIrrad.w ) * float( +tsNormal.z>1.0e-2 ) );//outColor * float(+gNormal.z>1.0e-3);//vec4( gFlux * max( dot(gNormal, vec3(0,0,+1)), 0 ), 1.0 );//outColor * max( dot( gNormal, vec3(0,0,+1) ), 0 );//float(+gNormal.z>0);//
	
	imageStore( g_TexIrradianceAnisoZNeg, voxelPos,
		vec4( texIrrad.xyz, /*max(dot(tsNormal,vec3(0,0,-1)),0) */ texIrrad.w ) * float( -tsNormal.z>1.0e-2 ) );//outColor * float(-gNormal.z>1.0e-3);//vec4( gFlux * max( dot(gNormal, vec3(0,0,-1)), 0 ), 1.0 );//outColor * max( dot( gNormal, vec3(0,0,-1) ), 0 );//float(-gNormal.z>0);//
}
