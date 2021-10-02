#pragma include "Common.glsl"
//#pragma include "LightAttribute.glsl"



//#################### バーテックスシェーダ ##################//
#ifdef _VERTEX_SHADER_


// Fragment Shader Input
in vec4			POSITION;
in vec2			TEXCOORD0;
in vec3			NORMAL;
in vec3			COLOR;

in vec3			TANGENT;
in vec3			BINORMAL;


// Vertex Shader Output
out Vertex_Out
{
	vec4		Position;	// 頂点座標
	vec2		TexCoord;	// テクスチャ座標
	vec3		Normal;		// 法線ベクトル
	vec3		Color;		// 頂点色
	float		Depth;

//	vec3		Tangent;	// 接線ベクトル
//	vec3		Binormal;	// 従法線ベクトル
	
	

}OUT;


// メイン関数
void main()
{
	vec4 worldPos	= POSITION * g_MatWorld;
	
	gl_Position		= worldPos * g_ViewTransform.MatViewProjection;
	OUT.Position	= worldPos;
	OUT.TexCoord	= TEXCOORD0;
	OUT.Normal		= normalize( NORMAL * g_MatWorldInverseTranspose );// convert local-space normal to world space
	OUT.Color		= COLOR;
	OUT.Depth		= gl_Position.z;
}


#endif







//##################### フラグメントシェーダ #################//
#ifdef _FRAGMENT_SHADER_

// Uniform Locations
uniform vec3	g_Diffuse;	// diffuse material


// Fragment Shader Input
in Vertex_Out
{
	vec4		Position;	// 頂点座標
	vec2		TexCoord;	// テクスチャ座標
	vec3		Normal;		// 法線ベクトル
	vec3		Color;		// 頂点色
	float		Depth;
//	vec3		Tangent;	// 接線ベクトル
//	vec3		Binormal;	// 従法線ベクトル
	
}IN;


// Fragment Shader Output
out vec4	outPosition;
out vec3	outNormal;
out float	outDepth;
out vec4	outColor;



// Main Function
void main()
{
	outPosition		= IN.Position;
	outNormal		= IN.Normal;
	outDepth		= IN.Depth;	// IN.Position.wで割ると[0,1]に正規化

	//const vec3 L	= g_LightDir(0);
	//float dot_nl	= max( dot(IN.Normal, L), 0.0 );
	outColor		= vec4( g_Diffuse, 1.0 );//vec4( g_DirectionalLights[idx].Intensity.xyz * dot_nl, 0.0 );

}


#endif
