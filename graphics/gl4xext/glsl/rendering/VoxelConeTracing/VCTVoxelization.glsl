#pragma include "Common.glsl"
#pragma include "vct/VCTCommon.glsl"



//##############################################################################//
//								Vertex Shader									//
//##############################################################################//

#ifdef _VERTEX_SHADER_

// Shader Input
in vec3			POSITION;
in vec2			TEXCOORD;
in vec3			NORMAL;


// Uniform location



// Vertex Shader Output
out vec4		vWorldPos;
out vec2		vTexCoord;
out vec3		vWorldNormal;



// Vertex Shader
void main( void )
{
	vec4 wPos		= vec4( POSITION, 1.0 ) * g_MatWorld;
	gl_Position		= wPos;//
	vWorldPos		= wPos;
	vTexCoord		= TEXCOORD;
	vWorldNormal	= normalize( NORMAL * mat3( g_MatWorldInverseTranspose ) );
}


#endif




//##############################################################################//
//								Geometry Shader									//
//##############################################################################//

#ifdef _GEOMETRY_SHADER_

// 入出力プリミティブタイプ. 3角形ポリゴンに限定
layout( triangles ) in;
layout( triangle_strip, max_vertices=3 ) out;

// Defines
#define	X	0
#define	Y	1
#define	Z	2


// Uniform Locations
uniform vec2	g_ViewportResolution;


// Constant Variables
const vec2		g_HalfTexelSize	= 0.25 / g_ViewportResolution;


// Geometry Shader Input
in vec4		vWorldPos[3];
in vec3		vTexCoord[3];	// テクスチャ座標
in vec3		vWorldNormal[3];


// Geometry Shader Output
out vec3	gTexCoord;	// テクスチャ座標
out vec3	gWorldNormal;
out vec2	gAxiSpacePos;
out vec4	gAxisSpaceAABB;


// Helper Functions
void expandTriangle( inout vec4 screenPos[3] )
{
	// 三角形のエッジを計算する
	vec2 edge[3];
	edge[0]	= screenPos[1].xy - screenPos[0].xy;
	edge[1]	= screenPos[2].xy - screenPos[1].xy;
	edge[2]	= screenPos[0].xy - screenPos[2].xy;

	// エッジ法線ベクトルを計算する
	vec2 edgeNormal[3];
	edgeNormal[0]	= normalize( edge[0] );
	edgeNormal[1]	= normalize( edge[1] );
	edgeNormal[2]	= normalize( edge[2] );
	edgeNormal[0]	= vec2( -edgeNormal[0].y, edgeNormal[0].x );// rotate 90 [deg]
	edgeNormal[1]	= vec2( -edgeNormal[1].y, edgeNormal[1].x );// rotate 90 [deg]
	edgeNormal[2]	= vec2( -edgeNormal[2].y, edgeNormal[2].x );// rotate 90 [deg]

	// ポリゴンが裏面向いてる場合は、エッジ法線が
	vec3 a					= normalize( screenPos[1].xyz - screenPos[0].xyz );
	vec3 b					= normalize( screenPos[2].xyz - screenPos[0].xyz );
	vec3 clipSpaceNormal	= cross( a, b );
	float invertFlag		= mix( 1.0, -1.0, float( clipSpaceNormal.z < 0.0 ) );
	edgeNormal[0]			*= invertFlag;
	edgeNormal[1]			*= invertFlag;
	edgeNormal[2]			*= invertFlag;


	vec3 edgeDist;
	edgeDist.x	= dot( edgeNormal[0], screenPos[0].xy );
	edgeDist.y	= dot( edgeNormal[1], screenPos[1].xy );
	edgeDist.z	= dot( edgeNormal[2], screenPos[2].xy );


	screenPos[0].xy = screenPos[0].xy - g_HalfTexelSize * (edge[2] / dot( edge[2], edgeNormal[0] ) + edge[0] / dot( edge[0], edgeNormal[2] ));
	screenPos[1].xy = screenPos[1].xy - g_HalfTexelSize * (edge[0] / dot( edge[0], edgeNormal[1] ) + edge[1] / dot( edge[1], edgeNormal[0] ));
	screenPos[2].xy = screenPos[2].xy - g_HalfTexelSize * (edge[1] / dot( edge[1], edgeNormal[2] ) + edge[2] / dot( edge[2], edgeNormal[1] ));

}



// Main Functions
void main()
{
	// Calc World space Face normal
	vec3 wsFaceNormal	= normalize( cross( vWorldPos[2].xyz - vWorldPos[0].xyz, vWorldPos[1].xyz - vWorldPos[0].xyz ) );

	// Choose Dominant Axis
	vec3 n_dot_v		= abs( vec3( dot( g_DominantAxis( X ), wsFaceNormal ), dot( g_DominantAxis( Y ), wsFaceNormal ), dot( g_DominantAxis( Z ), wsFaceNormal ) ) );
	uint axis_idx		= uint( mix( X, Y, uint( n_dot_v[X] < n_dot_v[Y] ) ) );					// X/Y軸で投影面積の大きい軸方向を選択
	axis_idx			= uint( mix( axis_idx, Z, uint( n_dot_v[axis_idx] < n_dot_v[Z] ) ) );	// Z軸


	//################################## Conservative Rasterization #####################################//

	vec4 axisSpaceVerts[3];

	// 軸空間の頂点を計算する
	axisSpaceVerts[0]	= vWorldPos[0] * g_MatAxisSpace( axis_idx );
	axisSpaceVerts[1]	= vWorldPos[1] * g_MatAxisSpace( axis_idx );
	axisSpaceVerts[2]	= vWorldPos[2] * g_MatAxisSpace( axis_idx );

	// バウンディングボックスを計算する
	gAxisSpaceAABB.xy	= min( min( axisSpaceVerts[0].xy, axisSpaceVerts[1].xy ), axisSpaceVerts[2].xy );
	gAxisSpaceAABB.zw	= max( max( axisSpaceVerts[0].xy, axisSpaceVerts[1].xy ), axisSpaceVerts[2].xy );

	// バウンディングボックスをテクセル半分幅だけ拡張する
	gAxisSpaceAABB.xy	-= g_HalfTexelSize;
	gAxisSpaceAABB.zw	+= g_HalfTexelSize;


	// ポリゴンをエッジ外方向に拡張する
	expandTriangle( axisSpaceVerts );

	//###################################################################################################//

	// Project Triangle vertices to Dominant Axis Space
	gl_Position		= axisSpaceVerts[0];
	gAxiSpacePos	= axisSpaceVerts[0].xy;
	gTexCoord		= (vWorldPos[0] * g_MatTexSpace).xyz;
	gWorldNormal	= vWorldNormal[0];
	EmitVertex();

	gl_Position		= axisSpaceVerts[1];
	gAxiSpacePos	= axisSpaceVerts[1].xy;
	gTexCoord		= (vWorldPos[1] * g_MatTexSpace).xyz;
	gWorldNormal	= vWorldNormal[1];
	EmitVertex();

	gl_Position		= axisSpaceVerts[2];
	gAxiSpacePos	= axisSpaceVerts[2].xy;
	gTexCoord		= (vWorldPos[2] * g_MatTexSpace).xyz;
	gWorldNormal	= vWorldNormal[2];
	EmitVertex();

	EndPrimitive();
}


#endif




//##############################################################################//
//								Fragment Shader									//
//##############################################################################//
#ifdef _FRAGMENT_SHADER_

// Include Files
#pragma include "LightAttribute.glsl"
#pragma include "BRDF_Common.glsl"
#pragma include "VectorFormatConversion.glsl"


// Defines
#define MAX_NUM_AVG_ITERATIONS	100
#define	TEX_POSITION			0
#define	TEX_NORMAL				1
#define	TEX_COLOR				2


// Uniform Locations
//uniform vec4	g_Diffuse	= vec4( 0.5 );// diffuse material
//uniform vec4	g_Specular	= vec4( 0.0 );// specular material
//uniform float	g_Shininess	= 0.0;		// shininess

// Uniform Locations
uniform vec3	g_Diffuse	= vec3(0.5);// diffuse material
uniform vec3	g_Specular	= vec3(0.0);// specular material
uniform float	g_Shininess	= 0.0;		// shininess
uniform float	g_Opacity	= 1.0;		// opacity




// Random Accessible Buffers
layout( binding = TEX_POSITION, r32ui )	uniform writeonly uimage3D	g_TexPosition;//layout( binding = TEX_POSITION, rgba32f )	uniform writeonly image3D	g_TexPosition;//
layout( binding = TEX_NORMAL, r32ui )	uniform volatile uimage3D	g_TexNormal;
layout( binding = TEX_COLOR, r32ui )	uniform volatile uimage3D	g_TexColor;


//layout(binding = 3)					uniform atomic_uint			g_VoxelIndex;
//layout( binding = 3, r32ui)			uniform coherent uimageBuffer g_VoxelFragmentList;


// Private Functions
uint imageAtomicRGBA8Avg( layout( r32ui ) coherent volatile uimage3D imgUI, ivec3 coords, vec4 val )
{
	val.rgb		*= 255.0;	// Optimise following calculations
	uint newVal	= convVec4ToRGBA8( val );

	uint prevStoredVal = 0;
	uint curStoredVal;


	// Loop as long as destination value gets changed by other threads
	uint numIter = 0;
	while( (curStoredVal = imageAtomicCompSwap( imgUI, coords, prevStoredVal, newVal )) != prevStoredVal && numIter < MAX_NUM_AVG_ITERATIONS )
	{
		prevStoredVal	= curStoredVal;
		vec4 rval		= convRGBA8ToVec4( curStoredVal );
		rval.xyz		= (rval.xyz * rval.w); // Denormalize

		vec4 curValF	= rval + val; // Add new value
		curValF.xyz		/= (curValF.w); // Renormalize

		newVal			= convVec4ToRGBA8( curValF );

		++numIter;
	}

	val		= convRGBA8ToVec4( newVal );
	val.a	= 255.0;
	newVal	= convVec4ToRGBA8( val );

	imageStore( imgUI, coords, uvec4( newVal ) );

	return newVal;
}



// Fragment Shader Input
in vec3		gTexCoord;
in vec3		gWorldNormal;
in vec2		gAxiSpacePos;
in vec4		gAxisSpaceAABB;



// Fragment Shader Output
out vec4	outColor;



// Fragment Shader
void main( void )
{
	// ボクセル領域外のフラグメントは無視する
	if( any( lessThan( gTexCoord, vec3( 0 ) ) ) || any( greaterThanEqual( gTexCoord, vec3( 1.0 + 1e-4 ) ) ) )
		discard;

	if( any( lessThan( gAxiSpacePos, gAxisSpaceAABB.xy ) ) || any( greaterThan( gAxiSpacePos, gAxisSpaceAABB.zw ) ) )
		discard;


	vec4 diffColor = vec4( g_Diffuse, g_Opacity );// texture( diffuseTex, vec2(In.uv.x, 1.0 - In.uv.y) );

	vec4 normal	= vec4( normalize( gWorldNormal ) * 0.5 + vec3( 0.5 ), 1.0 );
	normal.xyz	*= diffColor.a;
	normal.a	= diffColor.a;

	ivec3 iVoxelPos	= ivec3( gTexCoord * g_VoxelResolution );
	//uint voxelIndex	= atomicCounterIncrement( g_VoxelIndex );

	memoryBarrier();

	uint voxelPosXYZ10	= convVec3ToUINTXYZ10( uvec3( iVoxelPos ) );

	// Store Voxel Fragment List
	//imageStore( g_VoxelFragmentList, int(voxelIndex), uvec4(voxelPosXYZ10) );

	// Store XYZ Voxel Coordinate
	imageStore( g_TexPosition, iVoxelPos, uvec4( voxelPosXYZ10 ) ); //imageStore( g_TexPosition, iVoxelPos, vec4(iVoxelPos.xyz, 0.0) ); //

	// Store Averaged Normal
	imageAtomicRGBA8Avg( g_TexNormal, iVoxelPos, normal );

	// Store Averaged Color
	imageAtomicRGBA8Avg( g_TexColor, iVoxelPos, diffColor );

}


#endif
