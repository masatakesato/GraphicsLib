// Include Files
#pragma include "Common.glsl"



//#################### バーテックスシェーダ ##################//
#ifdef _VERTEX_SHADER_


// 変数


uniform vec3	g_LightDir;
uniform vec3	g_EyePos;


// 入力
in vec4			POSITION;
in vec2			TEXCOORD;
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
	gl_Position		= POSITION;
	OUT.Position	= gl_Position;
	OUT.TexCoord	= TEXCOORD;
	OUT.Normal		= NORMAL;
	OUT.Color		= COLOR;
}


#endif







//##################### フラグメントシェーダ #################//
#ifdef _FRAGMENT_SHADER_


// uniform変数
uniform sampler2DArray	g_TextureArray;
uniform int				g_Layer;
uniform float			g_Magnitude = 0.1;


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
	vec3 texCoordArray = vec3( IN.TexCoord, g_Layer );
	Color_out = texture( g_TextureArray, texCoordArray ).xyzw * g_Magnitude;// vec4(1.0, 0.0, 1.0, 0.0);
}


#endif
