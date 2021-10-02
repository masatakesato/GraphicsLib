// Include Files
#pragma include "Common.glsl"
#pragma include "vct/VCTCommon.glsl"
#pragma include "VectorFormatConversion.glsl"

// Uniform Locations
uniform sampler2D	g_RSMPosition;
uniform sampler2D	g_RSMNormal;
uniform sampler2D	g_RSMFlux;

uniform float		g_VPLAreaScale;		// VPL点群の点が，RSMテクスチャのピクセルいくつに相当するか(例：RSM:256^2, VPL:128^2なら，4.0)
uniform vec2		g_ViewportResolution;




#ifdef _VERTEX_SHADER_

// Uniform Locations


// Constant Variables
const float g_VoxelR		= pow( (3.0 * g_VoxelDim * g_VoxelDim * g_VoxelDim) / (4.0 * M_PI), 1.0/3.0 );
const float g_VoxelCrossArea	= M_PI * g_VoxelR * g_VoxelR;// ボクセルと同一体積の球の断面積


// Vertex Shader Input
in vec2			POSITION;	// 頂点座標. VPLスプライト座標 (0.0, 0.0) - (1.0, 1.0)


// Vertex Shader Output
out vec3		vTexSpaceNormal;	// 法線ベクトル
out vec3		vFlux;		// 頂点座標
out int			vLayer;


// Main Function
void main()
{
	//============= 法線ベクトルをg_NormalSamplerから取得する =================//
	//vNormal			= texture( g_RSMNormal, POSITION ).xyz;
	vTexSpaceNormal = normalize( texture( g_RSMNormal, POSITION ).xyz * mat3(g_MatTexSpace) );


	//=========== LPVスクリーン空間上での仮想点光源の位置を計算する ===========//
	// g_PositionSamplerからワールド座標を取得する
	vec4 worldpos	= texture( g_RSMPosition, POSITION ).xyzw;	//worldpos	= vec4(-1.9, 0.0, -0.6, 0);	//worldpos = vec4(1.4, POSITION.x-0.5, POSITION.y-0.7, 1);

	// 法線ベクトル方向に0.25セル分の距離だけ移動する
	//worldpos.xyz	+= 0.25 * g_CellSize * vNormal;
	
	// 光線ベクトル方向に0.25セル分の距離だけ移動
	//worldpos.xyz	+= 0.25 * g_CellSize * normalize( g_LightPos - worldpos.xyz );
	
	// ワールド座標をLPVスクリーン空間座標に変換する
	gl_Position		= vec4(worldpos.xyz, 1.0) * g_MatVoxelSpace;//g_matLPVScreen;


	//=============== g_FluxSamplerから仮想点光源の放射束を取得する ===========//
	vec4 rsmFlux	= texture( g_RSMFlux, POSITION ).xyzw;	// xyz: Flux, w: Texel Area
	const float rsmTexelContrib	= clamp( rsmFlux.w / g_VoxelCrossArea, 0.0, 1.0 );

	vFlux			= rsmFlux.xyz * rsmTexelContrib * g_VPLAreaScale;


	//===================== レイヤを計算する ==============================//
	// ワールド座標→LPV空間座標
	// lpvscreenpos.zは,[-1, 1]の間の数字
	// これを，[0,1]にしてLPVの高さをかければ,,,
	vLayer			= int( (gl_Position.z*0.5 + 0.5) * g_VoxelResolution.z );

}


#endif



#ifdef _GEOMETRY_SHADER_


// Input/Output Layouts
layout(points) in;
layout(points, max_vertices=1) out;


// Uniform Locations



// Geometry Shader Input
in vec3		vTexSpaceNormal[1];	// 法線ベクトル
in vec3		vFlux[1];	// 頂点座標
in int		vLayer[1];


// Geometry Shader Output
out vec3	gTexSpaceNormal;		// 法線ベクトル
out vec3	gFlux;			// 放射束


// Main Function
void main()
{
	//============ 明るさのある頂点だけ出力する =============//
	if(length(vFlux[0])!=0.0)
	{
		// レイヤの割り当て
		gl_Layer	= vLayer[0];// テクスチャレイヤーの指定. 必ずEmitVertexの前に実行する

		// VPL情報の書き出し
		gl_Position	= gl_in[0].gl_Position;
		gTexSpaceNormal		= vTexSpaceNormal[0];
		gFlux		= vFlux[0];

		EmitVertex();

		EndPrimitive();
	}
}



#endif




#ifdef _FRAGMENT_SHADER_

// Fragment Shader Input
in vec3		gTexSpaceNormal;		// 法線ベクトル
in vec3		gFlux;			// 放射束


// Fragment Shader Output
//out vec4	outColor;//out uint	outColor;//


out vec4	outColor0;
out vec4	outColor1;
out vec4	outColor2;
out vec4	outColor3;
out vec4	outColor4;
out vec4	outColor5;



// Main Function
void main()
{	
	//outColor	= vec4( gFlux, 1.0 );
	//outColor	= vec4( gFlux, length(gFlux) );
	//outColor	= convVec4ToRGBA8( vec4(gFlux, 1.0) * 255.0 );// TODO: データ圧縮どうする??
	
	outColor0	= vec4( gFlux * float(+gTexSpaceNormal.x>1.0e-3), 1.0 );//outColor * float(+gNormal.x>1.0e-3);//vec4( gFlux * max( dot(gNormal, vec3(+1,0,0)), 0 ), 1.0 );//outColor * max( dot( gNormal, vec3(+1,0,0) ), 0 );//float(+gNormal.x>0);//
	outColor1	= vec4( gFlux * float(-gTexSpaceNormal.x>1.0e-3), 1.0 );//outColor * float(-gNormal.x>1.0e-3);//vec4( gFlux * max( dot(gNormal, vec3(-1,0,0)), 0 ), 1.0 );//outColor * max( dot( gNormal, vec3(-1,0,0) ), 0 );//float(-gNormal.x>0);//
	outColor2	= vec4( gFlux * float(+gTexSpaceNormal.y>1.0e-3), 1.0 );//outColor * float(+gNormal.y>1.0e-3);//vec4( gFlux * max( dot(gNormal, vec3(0,+1,0)), 0 ), 1.0 );//outColor * max( dot( gNormal, vec3(0,+1,0) ), 0 );//float(+gNormal.y>0);//
	outColor3	= vec4( gFlux * float(-gTexSpaceNormal.y>1.0e-3), 1.0 );//outColor * float(-gNormal.y>1.0e-3);//vec4( gFlux * max( dot(gNormal, vec3(0,-1,0)), 0 ), 1.0 );//outColor * max( dot( gNormal, vec3(0,-1,0) ), 0 );//float(-gNormal.y>0);//
	outColor4	= vec4( gFlux * float(+gTexSpaceNormal.z>1.0e-3), 1.0 );//outColor * float(+gNormal.z>1.0e-3);//vec4( gFlux * max( dot(gNormal, vec3(0,0,+1)), 0 ), 1.0 );//outColor * max( dot( gNormal, vec3(0,0,+1) ), 0 );//float(+gNormal.z>0);//
	outColor5	= vec4( gFlux * float(-gTexSpaceNormal.z>1.0e-3), 1.0 );//outColor * float(-gNormal.z>1.0e-3);//vec4( gFlux * max( dot(gNormal, vec3(0,0,-1)), 0 ), 1.0 );//outColor * max( dot( gNormal, vec3(0,0,-1) ), 0 );//float(-gNormal.z>0);//
}


#endif
