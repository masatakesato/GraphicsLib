// Inlude Files
#pragma include "Common.glsl"

// Constant Variables
const float g_Near	= 0.001;
const float g_Far	= 1000000.0;
const float g_C		= 0.1;
const float Fcoef	= 2.0 / log( g_Far + 1.0 );
const float Fcoef_half	= 0.5 * Fcoef;


#ifndef LOGARITHMIC_DEPTH
#define	LOGARITHMIC_DEPTH( w ) ( log2(max(1.0e-6, 1.0+w)) * Fcoef - 1.0 )
#endif






#ifdef _VERTEX_SHADER_

// Include files
#pragma include "ShadowMap.glsl"


// Uniform location
uniform int		g_ShadowIndex;


// Shader Input
in vec3			POSITION;


// Vertex Shader Output
out Vertex_Out
{
	vec4	Position;	// 頂点座標
	float	flogz;
}OUT;


// Vertex Shader
void main(void)
{
	vec4 wPos		= vec4( POSITION, 1.0 ) * g_MatWorld;
	gl_Position		= wPos * g_MatShadow( g_ShadowIndex );//g_ViewTransform.MatViewProjection;//
	OUT.Position	= gl_Position;

	gl_Position.z	= LOGARITHMIC_DEPTH( gl_Position.w );
	OUT.flogz		= 1.0 + gl_Position.w;
}


#endif




#ifdef _FRAGMENT_SHADER_

// Fragment Shader Input
in Vertex_Out
{
	vec4	Position;	// 頂点座標
	float	flogz;
}IN;


// Fragment Shader Output
out float	outDepth;	// デプス
//out vec3	outPosition;// 座標
//out vec3	outNormal;	// 法線
//out vec3	outFlux;	// 放射束


// Fragment Shader
void main (void)
{
	outDepth	= IN.Position.z;
	gl_FragDepth	= log2(IN.flogz) * Fcoef_half;
}

#endif
