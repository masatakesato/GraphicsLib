// Include Files
//#pragma include "Common.glsl"



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
uniform sampler2D	g_Texture;	// texture2D
uniform int			g_FilterRadius;	// filter radius

//uniform ubAttribute
//{
//	int	FilterRadius;
//};


// Const Variables
const vec2 g_TexelSize = 1.0 / vec2(textureSize( g_Texture, 0 ));


// Functions
float SobelFilter( sampler2D tex, vec2 uv, int radius )
{
	vec2 sum = vec2( 0.0, 0.0 );
		
	for( int i=-radius; i<=radius; i++ )
	{
		for( int j=-radius; j<=radius; j++ )
		{
			vec2 offset = uv + vec2(float(i), float(j)) * g_TexelSize;
			float len_inv = (i==0&&j==0) ? 0.0 : 1.0 / float(i*i + j*j);
			vec2 G_ij = vec2( float(i), float(j) ) * len_inv;//

			sum += texture( tex, offset ).w * G_ij;
		}
	}
	
	return length( sum );
}


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
	float result = SobelFilter( g_Texture, IN.TexCoord.xy, g_FilterRadius );
	outColor = vec4(result);
}


#endif
