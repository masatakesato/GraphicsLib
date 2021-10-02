// http://www.gamedev.net/topic/638661-voxelization-using-dominant-axis-voxel-cone-tracing/
// サンプルシェーダーコードのってる


#pragma include "../Common.glsl"
#pragma include "../ShadowMap.glsl"
#pragma include "VCTCommon.glsl"




// TODO: ボクセル空間をトレースする際のセルフオクルージョンを回避したい
// GPU Pro3のp.227記事によると、、、
// レイ方向に押し出すだけでは不十分。法線方向にも押し出しが必要


#ifndef	M_PI
#define M_PI	3.14159265358979323846
#endif



// ボクセル空間のトレース方向. y軸(0, 1, 0)が真上の座標空間
const vec3 g_TraceDir[]	=
{
	normalize( vec3( 0.0, 1.0, 0.0 ) ),			// w1
	normalize( vec3( 0.0, 0.5, 0.866025 ) ),		// w2
	normalize( vec3( 0.823639, 0.5, 0.267617 ) ),		// w3
	normalize( vec3( 0.509037, 0.5, -0.700629 ) ),	// w4
	normalize( vec3( -0.509037, 0.5, -0.700629 ) ),	// w5
	normalize( vec3( -0.823639, 0.5, 0.267617 ) )// w6
};



// 各トレース方向の重み(立体角 + 法線方向に対する傾き(余弦）で重み付け)
const float g_DirWeight[] =
{
	M_PI / 4.0,			// W1
	3.0 * M_PI / 20.0,	// W2
	3.0 * M_PI / 20.0,	// W3
	3.0 * M_PI / 20.0,	// W4
	3.0 * M_PI / 20.0,	// W5
	3.0 * M_PI / 20.0,	// W6
};


// コーン中心からの側面までの角度(rad). 正12面体の1平面が展開するコーンの角度は???
//const float g_ConeAngle[] =
//{
//	acos( 1.0 - 1.0471975512/*g_DirWeight[0]*/ / (2.0 * M_PI) ),	// W0 ConeAngle
//	acos( 1.0 - 1.0471975512/*g_DirWeight[1]*/ / (2.0 * M_PI) ),	// W1 ConeAngle
//	acos( 1.0 - 1.0471975512/*g_DirWeight[2]*/ / (2.0 * M_PI) ),	// W2 ConeAngle
//	acos( 1.0 - 1.0471975512/*g_DirWeight[3]*/ / (2.0 * M_PI) ),	// W3 ConeAngle
//	acos( 1.0 - 1.0471975512/*g_DirWeight[4]*/ / (2.0 * M_PI) ),	// W4 ConeAngle
//	acos( 1.0 - 1.0471975512/*g_DirWeight[5]*/ / (2.0 * M_PI) ),	// W5 ConeAngle
//};


const float g_ConeRatio[] =
{
	sin( acos( 1.0 - 1.0471975512 / ( 2.0 * M_PI ) ) ),	// Sine of W0 ConeAngle
	sin( acos( 1.0 - 1.0471975512 / ( 2.0 * M_PI ) ) ),	// Sine of W1 ConeAngle
	sin( acos( 1.0 - 1.0471975512 / ( 2.0 * M_PI ) ) ),	// Sine of W2 ConeAngle
	sin( acos( 1.0 - 1.0471975512 / ( 2.0 * M_PI ) ) ),	// Sine of W3 ConeAngle
	sin( acos( 1.0 - 1.0471975512 / ( 2.0 * M_PI ) ) ),	// Sine of W4 ConeAngle
	sin( acos( 1.0 - 1.0471975512 / ( 2.0 * M_PI ) ) ),	// Sine of W5 ConeAngle
};



//##############################################################################//
//								Vertex Shader									//
//##############################################################################//

#ifdef _VERTEX_SHADER_


// Uniform Locations



// Shader Input
in vec3			POSITION;
in vec2			TEXCOORD;


// Vertex Shader Output
out vec2		vTexCoord;


// Vertex Shader
void main(void)
{
	gl_Position = vec4( POSITION, 1.0 );//
	vTexCoord	= TEXCOORD;
}


#endif




//##############################################################################//
//								Fragment Shader									//
//##############################################################################//

#ifdef _FRAGMENT_SHADER_

// Uniform Locations
uniform sampler2D	g_TexPosition;
uniform sampler2D	g_TexNormal;
//uniform sampler3D	g_TexIrradianceVolume;
uniform sampler3D	g_TexIrradianceVolumeAniso[6];


// Const Variables
const vec3			g_TexIVResolution	= vec3( textureSize( g_TexIrradianceVolumeAniso[0], 0 ) );
const vec3			g_VoxelSize			= 1.0 / g_TexIVResolution;
const float			g_VoxSize			= min( min( g_VoxelSize.x, g_VoxelSize.y ), g_VoxelSize.z );//length( g_VoxelSize );//g_VoxelSize.x;//

// Octreeのノードをトレースして補間するときに必要. MipMapの場合はハードウェアが自動で処理してくれるので不要
//const float			g_VoxSizeLOD[]	=
//{
//	g_VoxSize,
//	g_VoxSize*2,
//	g_VoxSize*4,
//	g_VoxSize*8,
//	g_VoxSize*16,
//	g_VoxSize*32,
//	g_VoxSize*64,
//	g_VoxSize*128,
//	g_VoxSize*256,
//	g_VoxSize*512,
//	g_VoxSize*1024
//};


// Private Functions


// ボクセルをサンプリングする
vec4 SampleVoxel( vec3 pos, vec3 dir, vec3 normal, float lod )//, sampler3D voxTex )
{
	vec4 sampleX	= dir.x > 0.0 ? textureLod( g_TexIrradianceVolumeAniso[0], pos, lod ) : textureLod( g_TexIrradianceVolumeAniso[1], pos, lod );
	vec4 sampleY	= dir.y > 0.0 ? textureLod( g_TexIrradianceVolumeAniso[2], pos, lod ) : textureLod( g_TexIrradianceVolumeAniso[3], pos, lod );
	vec4 sampleZ	= dir.z > 0.0 ? textureLod( g_TexIrradianceVolumeAniso[4], pos, lod ) : textureLod( g_TexIrradianceVolumeAniso[5], pos, lod );

	vec3 wts		= abs( dir );

	float invSampleMag	= 1.0 / ( wts.x + wts.y + wts.z + 1.0e-3 );
	wts *= invSampleMag;

	vec4 filtered	= sampleX * wts.x + sampleY * wts.y + sampleZ * wts.z;

	return filtered;
}


//void correctAlpha( inout vec4 color, in float alphaCorrection )// Octree使用時(MipMapの線形補間が効かない場合)に、手動で色ブレンドする。MipMap使用時は不要
//{
//	const float oldColA = color.a;
//	color.a = 1.0 - pow( (1.0 - color.a), alphaCorrection );
//	color.xyz *= color.a / clamp(oldColA, 0.0001, 10000.0);
//}





// ボクセル空間をレイトレースする.
//	tsOrigin:テクスチャ空間のレイ始点
//	tsDirection:テクスチャ空間のレイ方向
//	coneRatio: sin(coneAngle * 0.5 )
//	tsMaxDistance:テクスチャ空間のレイ探索最大距離
vec4 ConeTrace( vec3 tsOrigin, vec3 tsDirection, vec3 tsNormal, float coneRatio, float tsMaxDistance )
{
	vec3 sampleOrigin		= tsOrigin;
	vec4 accum				= vec4( 0.0 );
	const float minDiam		= 0.5 * g_VoxSize;// コーン内接円の直径下限値. MipMapレベル0のボクセル幅
	const float startDist	= 0.0;//min( minDiam, g_VoxSize / coneRatio );//minDiam * 2.0;

	float currDist			= startDist;
	while( currDist <= tsMaxDistance && accum.w < 1.0 )
	{
		float sampleDiam	= max( coneRatio * currDist * 2.0, minDiam );	// レイ始点からの距離に応じた、サンプリング空間の直径を計算する
		float sampleLOD		= clamp( log2( sampleDiam / g_VoxSize ), 0.0, float(g_numCascadeLevels-1) );	// ボクセルのLODレベルを計算する
		vec3 samplePos		= sampleOrigin + currDist * tsDirection;
		vec4 sampleVal		= SampleVoxel( samplePos, -tsDirection, tsNormal, sampleLOD );//, g_TexIrradianceVolume );

		// Octreeのノードをトレースして補間するときに必要. MipMapの場合はハードウェアが自動で処理してくれるので不要
		//float d	= g_VoxSizeLOD[ int(ceil(sampleLOD)) ];
		//correctAlpha( sampleVal, currDist/d );

		accum				+= ( 1.0 - accum.w ) * sampleVal;
		currDist			+= max( sampleDiam*0.5, minDiam );
	}

	return accum;
}






vec4 quat_from_axis_angle(vec3 axis, float angle)
{ 
	vec4 qr;
	float half_angle = (angle * 0.5);// * 3.14159 / 180.0;
	qr.x = axis.x * sin(half_angle);
	qr.y = axis.y * sin(half_angle);
	qr.z = axis.z * sin(half_angle);
	qr.w = cos(half_angle);
	return qr;
}

vec4 quat_conj(vec4 q)
{ 
	return vec4(-q.x, -q.y, -q.z, q.w); 
}

vec4 quat_mult(vec4 q1, vec4 q2)
{ 
	vec4 qr;
	qr.x = (q1.w * q2.x) + (q1.x * q2.w) + (q1.y * q2.z) - (q1.z * q2.y);
	qr.y = (q1.w * q2.y) - (q1.x * q2.z) + (q1.y * q2.w) + (q1.z * q2.x);
	qr.z = (q1.w * q2.z) + (q1.x * q2.y) - (q1.y * q2.x) + (q1.z * q2.w);
	qr.w = (q1.w * q2.w) - (q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z);
	return qr;
}

vec3 rotate_vertex_position(vec3 position, vec3 axis, float angle)
{ 
	vec4 qr = quat_from_axis_angle(axis, angle);
	vec4 qr_conj = quat_conj(qr);
	vec4 q_pos = vec4(position.x, position.y, position.z, 0);

	vec4 q_tmp = quat_mult(qr, q_pos);
	qr = quat_mult(q_tmp, qr_conj);

	return vec3(qr.x, qr.y, qr.z);
}










// Fragment Shader Input
in vec2		vTexCoord;


// Fragment Shader Output
out vec4	outColor;



// Fragment Shader
void main (void)
{

	vec4 worldPosition	= texture( g_TexPosition, vTexCoord ).xyzw;
	if( worldPosition.w == 0.0 ) discard;

	vec4 worldNormal	= texture( g_TexNormal, vTexCoord ).xyzw;

	vec3 voxPosition	= ( worldPosition * g_MatTexSpace ).xyz;
	vec3 voxNormal		= normalize( ( vec4(worldNormal.xyz, 0.0) * g_MatTexSpace ).xyz );

	//vec3 voxelColor		= textureLod( g_TexIrradianceVolume, voxPosition, 0 ).xyz;
	//outColor			= vec4( voxelColor, 1.0 );


	// ボクセルをトレースする. とりあえずディフューズだけ

	vec4 diffuseIndirect	= vec4(0);
	float iMaxDist		= 0.5;


	//################### TODO: (0, 1, 0)からworldNormal.xyzへクォータニオンを計算する #################//

	// 回転軸の計算
	vec3 rot_axis		= cross( worldNormal.xyz, vec3(0,1,0) );
	float axis_length	= length( rot_axis );

	rot_axis			= mix( rot_axis, vec3(0,0,1), float(axis_length<1.0e-6) );

	// 回転角の計算(クォータニオンは半時計周りなので回転角をマイナスにして回転方向を反転)
	float rot_angle		= axis_length<1.0e-6 ? 0.0 : -acos( dot( vec3(0,1,0), worldNormal.xyz ) );

	vec4 quat			= vec4( rot_axis, rot_angle );


	//############################## Trace Diffuse Component #############################//
	for( int i=0; i<6; ++i )
	{
		vec3 wsSampleVec	= ( rotate_vertex_position( g_TraceDir[i], quat.xyz, quat.w ) );
		vec3 vsSampleVec	= normalize( ( vec4(wsSampleVec, 0.0) * g_MatTexSpace ).xyz );
		vec3 vsSamplePos	= voxPosition + g_VoxelSize * normalize(voxNormal + vsSampleVec) * 1.7320508;
		//vsSamplePos	= floor(vsSamplePos * g_TexIVResolution) * g_VoxelSize;
		
		vec4 gatherIrrad	= ConeTrace( vsSamplePos, vsSampleVec, voxNormal, g_ConeRatio[i], iMaxDist );
		gatherIrrad.xyz		*=	clamp( exp( gatherIrrad.w )-1.0, 0.0, 1.0 );//smoothstep( 0.1, 0.5, pow(exp(gatherIrrad.w), 0.85)-1.0 );
		//smoothstep( 0.1, 0.5, gatherIrrad.w );
		//clamp( exp(gatherIrrad.w)-1.0, 0.0, 1.0 );// ライトリークを目立たなくする重み付け.適当

		diffuseIndirect.xyz		+= g_DirWeight[i] * gatherIrrad.xyz;
		diffuseIndirect.w		+= gatherIrrad.w;
	}


	//############ TODO: Experimental. Trace Specular Component. 2015.05.15 ##############//
	vec3 wsSampleVec	= reflect( normalize(worldPosition.xyz - g_ViewAttrib.Position), worldNormal.xyz );
	vec3 vsSampleVec	= normalize( ( vec4(wsSampleVec, 0.0) * g_MatTexSpace ).xyz );
	vec3 vsSamplePos	= voxPosition + g_VoxelSize * normalize(voxNormal + vsSampleVec) * 1.7320508;

	diffuseIndirect += ConeTrace( vsSamplePos, vsSampleVec, voxNormal, /*0.12*/0.05, iMaxDist );


	//const vec3 bgColor = vec3( 0.25, 0.25, 0.25 );
	//outColor.xyz		= mix( bgColor,  diffuseIndirect.xyz, diffuseIndirect.w );// 実験

	outColor.xyz  = diffuseIndirect.xyz;// * clamp( exp( diffuseIndirect.w/6.0) - 1.0, 0.0, 1.0 );
	//outColor.xyz  = diffuseIndirect.xyz;// * (diffuseIndirect.w);

	//outColor	= vec4( worldPosition.xyz, 0 );
}

#endif

