// Inclide Files
#pragma include "Common.glsl"
#pragma include "BRDF_Common.glsl"
#pragma include "LightAttribute.glsl"
#pragma include "ShadowMap.glsl"



//#################### バーテックスシェーダ ##################//
#ifdef _VERTEX_SHADER_


// 変数


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
	float		Depth;

//	vec3		Tangent;	// 接線ベクトル
//	vec3		Binormal;	// 従法線ベクトル

}OUT;



// メイン関数
void main()
{	
	vec4 wPos		= POSITION * g_MatWorld;
	gl_Position		= wPos * g_ViewTransform.MatViewProjection;//g_MatViewProj;//
	OUT.Position	= wPos;
	OUT.TexCoord	= TEXCOORD0;
	OUT.Normal		= normalize( NORMAL * g_MatWorldInverseTranspose );
	OUT.Color		= COLOR;

	OUT.Depth		= gl_Position.z;
}


#endif







//##################### フラグメントシェーダ #################//
#ifdef _FRAGMENT_SHADER_


//const vec3 color_table[7] =
//{
//	{1,0,0},
//	{0,1,0},
//	{0,0,1},
//	{0,1,1},
//	{1,0,1},
//	{1,1,0},
//	{1,1,1}
//};

// Uniform Locations
uniform int	g_MaterialID	= 0;


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


// Fragment SHader Output
out vec4 outColor;


// メイン関数
void main()
{
	const vec3 diff			= g_Materials.Diffuse[g_MaterialID].xyz;
	const vec3 spec			= g_Materials.Specular[g_MaterialID].xyz;
	const float shininess	= g_Materials.Shiness[g_MaterialID];


	int lightId	= 0;

	const int lightType	= g_LightType(lightId);
	const vec3 V	= normalize( g_ViewAttrib.Position - IN.Position.xyz );	// どっち向きが正しい? サーフェス点から離れる方向? サーフェス点に近付く方向?
	const vec3 L	= mix( normalize(g_LightPosition(lightId) - IN.Position.xyz), g_LightDir( lightId ), float(lightType==LIGHT_TYPE_DIRECTIONAL) );//normalize( vec3(1,1,1) );
	const vec3 H	= (L + V) * 0.5;
	const vec3 N	= normalize( IN.Normal );

	outColor.rgb	= spec.xyz * SimplifiedCookTorrance( shininess, L, N, H ) + AnisotropicPhong_Diffuse( L, V, N, diff, vec3( 0.31831, 0.31831, 0.31831 ) );

	int shadow_id			= g_ShadowCasterIndex(lightId);
	int nunshadows			= g_NumShadows(lightId);
	float shadowDistance	= GetShadowDistance( IN.Position, nunshadows, shadow_id, IN.Depth );
	float isLit				= float( shadowDistance < 0.0 ? 1.0 : 0.0 );

	outColor.rgb	*= isLit * g_LightIntensity(lightId);
	outColor.a		= 0.0;

	//Color_out.rgb *= color;
}


#endif
