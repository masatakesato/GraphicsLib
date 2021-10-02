#version 430



#pragma include "Common.glsl"



//#################### バーテックスシェーダ ##################//
#ifdef _VERTEX_SHADER_


// 変数
//uniform mat4	g_MatWorld;
//uniform mat4	g_MatView;
//uniform mat4	g_MatViewProj;


uniform vec3	g_LightDir;
uniform vec3	g_EyePos;



// 入力
in vec4			POSITION;
in vec2			TEXCOORD0;
in vec3			NORMAL;
in vec3			COLOR;

//in vec3			TANGENT;
//in vec3			BINORMAL;


// 出力
out Vertex_Out
{
	vec4		Position;	// 頂点座標
	vec2		TexCoord;	// テクスチャ座標
	vec3		Normal;		// 法線ベクトル
	vec3		Color;		// 頂点色

//	vec3		Tangent;	// 接線ベクトル
//	vec3		Binormal;	// 従法線ベクトル
	
	

}OUT;


// メイン関数
void main()
{	
	gl_Position		= POSITION * g_MatWorld * g_MatViewProj;
	OUT.Position	= gl_Position;
	OUT.TexCoord	= TEXCOORD0;
	OUT.Normal		= NORMAL;
	OUT.Color		= COLOR;
}


#endif







//##################### フラグメントシェーダ #################//
#ifdef _FRAGMENT_SHADER_

// 入力
in Vertex_Out
{
	vec4		Position;	// 頂点座標
	vec2		TexCoord;	// テクスチャ座標
	vec3		Normal;		// 法線ベクトル
	vec3		Color;		// 頂点色
	
//	vec3		Tangent;	// 接線ベクトル
//	vec3		Binormal;	// 従法線ベクトル
	
	
}IN;


// 出力
out vec4 Color_out;


// メイン関数
void main()
{
	Color_out	= vec4( 0.3, 0.6, 0.8, 0.0 ) * dot( IN.Normal, normalize( vec3( 1.0, 1.0, 1.0 ) ));//dot( IN.);
	Color_out	= vec4( IN.Color, 0.0 );
/*
int idx = 53;// 光源リストの53番目だけ有効にしてある。仮措置 2013.11.08

	const vec3 diff = g_Materials.Diffuse[0].xyz;
	const vec3 spec = g_Materials.Specular[0].xyz;

	//int i=0;
	float I = 0.0;
	//for( i=0; i<1000; i++ )
	//{
		I = max( dot(IN.Normal, g_DirectionalLights[idx].Direction.xyz ), 0.0);
	//}
	Color_out.rgb = g_DirectionalLights[idx].Intensity.xyz * I * diff.xyz;	// g_DirectionalLights[idx].Intensity.xyz * 
	//Color_out.a = 0.0;
*/	
}


#endif
