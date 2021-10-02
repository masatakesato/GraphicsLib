#ifdef _VERTEX_SHADER_

// Include files
#pragma include "Common.glsl"
#pragma include "ShadowMap.glsl"


// Uniform location
uniform int		g_ShadowIndex;


// Shader Input
in vec3			POSITION;


// Vertex Shader Output
out Vertex_Out
{
	//vec4		Position;	// 頂点座標
	float		Depth;
}OUT;


// Vertex Shader
void main(void)
{
	mat4 matShadow	= g_MatShadow(g_ShadowIndex);
	gl_Position		= vec4( POSITION, 1.0 ) * g_MatWorld * matShadow;
	//OUT.Position	= gl_Position;
	OUT.Depth		= gl_Position.z	* abs( 2.0 / matShadow[2][2] );
}


#endif




#ifdef _FRAGMENT_SHADER_

// Fragment Shader Input
in Vertex_Out
{
	//vec4		Position;	// 頂点座標
	float	Depth;
}IN;


// Fragment Shader Output
out float	outDepth;	// デプス
//out vec3	outPosition;// 座標
//out vec3	outNormal;	// 法線
//out vec3	outFlux;	// 放射束


// Fragment Shader
void main (void)
{
	outDepth	= IN.Depth;//IN.Position.z;
}

#endif
