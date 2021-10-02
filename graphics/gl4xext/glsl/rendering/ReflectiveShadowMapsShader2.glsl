// Include files
#pragma include "LightAttribute.glsl"



// Uniform Locations
uniform int		g_LightID;	// 光源インデックス



//#################### バーテックスシェーダ ##################//
#ifdef _VERTEX_SHADER_

// Include files
#pragma include "Common.glsl"


// Uniform Locations
uniform mat4	g_MatLight;
uniform vec3	g_LightPos;



// Vertex Shader Inputs
in vec4			POSITION;	// 頂点座標
in vec3			NORMAL;		// 法線ベクトル
in vec2			TEXCOORD;	// テクスチャ座標
in vec4			COLOR;		// 色


// Vertex Shader Outputs
out vec3		vWorldPos;	// ワールド座標系頂点座標
out vec4		vViewPos;	// ビュー座標系の頂点座標
out vec2		vTexCoord;	// テクスチャ座標
out vec3		vNormal;	// 法線ベクトル
out vec4		vColor;		// 頂点座標
out vec3		vSurfaceToLight;



// Main Function
void main()
{	
	vec4 wPos	= POSITION * g_MatWorld;

	gl_Position	= wPos * g_MatLight;//
	vWorldPos	= wPos.xyz;
	vViewPos	= gl_Position;
	vTexCoord	= TEXCOORD;
	vNormal		= normalize( NORMAL * mat3(g_MatWorldInverseTranspose) );
	vColor		= COLOR;

	vSurfaceToLight	= g_LightPosition(g_LightID) - wPos.xyz;//g_LightPos - wPos.xyz;//

}


#endif







//##################### フラグメントシェーダ #################//
#ifdef _FRAGMENT_SHADER_

// Include files



// Uniform Locations
uniform ivec2	g_RsmSize;	// RSMテクスチャの解像度
uniform float	g_TexelCoeff;


uniform vec4	g_Diffuse;	// diffuse material



// Fragment Inputs
in vec3		vWorldPos;	// ワールド座標系頂点座標
in vec4		vViewPos;	// ビュー座標系の頂点座標
in vec2		vTexCoord;	// テクスチャ座標
in vec3		vNormal;	// 法線ベクトル
in vec4		vColor;		// 頂点座標
in vec3		vSurfaceToLight;


// Fragment Outputs
out vec4	fPosition;	// ワールド座標
out vec3	fNormal;	// 法線ベクトル
out vec4	fFlux;		// xyz:放射束, w:テクセルの立体角
out float	fDepth;		// 奥行き



// Main Functions
void main()
{
	float surfLightDist	= length( vSurfaceToLight );
	vec3 surfaceToLight	= vSurfaceToLight / surfLightDist;
	float LdotN		= dot( surfaceToLight, vNormal );

	// 位置
	fPosition.xyz	= vWorldPos;
	fPosition.w		= vViewPos.w;

	// 法線ベクトル TODO: ポリゴン板の両面に対応できるようにする？
	fNormal			= vNormal;
	if( LdotN < 0.0 )
	{
		LdotN	= -LdotN;
		fNormal = -fNormal;
	}

	// 放射束. テクセルの立体角を出力
	const float cos_theta_3	= pow( max(dot( g_LightDir( g_LightID ), surfaceToLight ),0), 3.0 );
	const float texelSteradian	= g_TexelCoeff * max(cos_theta_3,0.0);
	const float texelArea		= surfLightDist * surfLightDist * texelSteradian;//(4.0*M_PI**r) * (texelSteradian/(4.0*M_PI));//

	fFlux		= vec4( g_LightIntensity( g_LightID ) * g_Diffuse.rgb * max( LdotN, 0.0 ), texelArea );


	// 奥行き値(実際の距離.正規化しない)
	fDepth		= vViewPos.z;
	
}


#endif
