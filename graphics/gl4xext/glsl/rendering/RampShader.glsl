// Include Files
#pragma include "Common.glsl"



//#################### バーテックスシェーダ ##################//
#ifdef _VERTEX_SHADER_


// 変数


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
	vec2		TexCoord;	// テクスチャ座標
}OUT;


// メイン関数
void main()
{	
	gl_Position		= POSITION;
	OUT.TexCoord	= TEXCOORD;
}


#endif







//##################### フラグメントシェーダ #################//
#ifdef _FRAGMENT_SHADER_


// Uniform Variables
uniform sampler1D	g_TexRamp;	// texture1D
uniform sampler2D	g_TexScalar;	// scalar texture
uniform float		g_Scale;	// color scale


// Fragment Shader Input
in Vertex_Out
{
	vec2		TexCoord;	// テクスチャ座標	
}IN;


// Fragment Shader Output
out vec4 outColor;


// Main Function
void main()
{
	float scalar = clamp( texture( g_TexScalar, IN.TexCoord.xy ).x, 0.0, 0.999 );

	vec4 rampColor = texture( g_TexRamp, scalar ).xyzw;

	outColor = vec4( rampColor );//rampColor * g_Scale;
	//outColor	= pow( color * g_Scale, vec4(1.0/2.2) );

	//if( IN.TexCoord.x<=0.0009765625 )	outColor		= vec4(1,0,0,0);
	//else if( IN.TexCoord.x>=1.0 )	outColor	= vec4(0,1,0,0);
	//else if( IN.TexCoord.y<=0.0 )	outColor	= vec4(0,0,1,0);
	//else if( IN.TexCoord.y>=1.0 )	outColor	= vec4(1,0,1,0);
}


#endif
