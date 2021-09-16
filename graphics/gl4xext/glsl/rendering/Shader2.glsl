#version 150

#pragma include "Shader/Common.glsl"



uniform vec3 L = vec3(1,0.7,2);

//#################### バーテックスシェーダ ##################//
#ifdef _VERTEX_SHADER_



// 変数
//uniform mat4	g_matMVP;
uniform vec3	g_LightDir;
uniform vec3	g_EyePos;


// 入力
in vec4			POSITION;	//
in vec2			TEXCOORD0;	//
in vec3			NORMAL;		//
in vec3			TANGENT;	//
in vec3			BINORMAL;	//


// 出力
out Vertex_Out
{
	vec4		Position;	// 頂点座標
	vec2		TexCoord;	// テクスチャ座標
	
//	vec3		Tangent;	// 接線ベクトル
//	vec3		Binormal;	// 従法線ベクトル
	vec3		Normal;		// 法線ベクトル
	
}OUT;


// メイン関数
void main()
{	
	//vec4 pos = vec4(POSITION.xyz*0.1, POSITION.w);
	gl_Position		= POSITION * camera.g_matMVP;
	OUT.Position	= POSITION * camera.g_matMVP;
	OUT.TexCoord	= TEXCOORD0;
	OUT.Normal		= NORMAL;//
}


#endif







//##################### フラグメントシェーダ #################//
#ifdef _FRAGMENT_SHADER_

// 入力
in Vertex_Out
{
	vec4		Position;	// 頂点座標
	vec4		TexCoord;	// テクスチャ座標
	
//	vec3		Tangent;	// 接線ベクトル
//	vec3		Binormal;	// 従法線ベクトル
	vec3		Normal;		// 法線ベクトル
	
}IN;


// 出力
out vec4 Color_out;


// メイン関数
void main()
{
	float I = max( dot(IN.Normal, normalize(L)), 0.0);
	Color_out.rgb = vec3(I);//IN.Normal;//color_gray().rgb;//
	Color_out.a = 0.0;	
}


#endif
