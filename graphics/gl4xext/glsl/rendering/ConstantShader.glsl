#pragma include "Common.glsl"




//#################### Vertex Shader ##################//
#ifdef _VERTEX_SHADER_


// 


// Vertex Shader Input
in vec4			POSITION;
in vec2			TEXCOORD0;
in vec3			NORMAL;
in vec3			COLOR;

//in vec3			TANGENT;
//in vec3			BINORMAL;


// Vertex Shader Output
out Vertex_Out
{
	vec4	Color;
}OUT;


//out 


// Main Function
void main()
{	
	vec4 wPos		= POSITION * g_MatWorld;
	gl_Position		= wPos * g_ViewTransform.MatViewProjection;//g_MatViewProj;//
	OUT.Color		= vec4( COLOR, 0.0 );
}


#endif







//##################### Fragment Shader #################//
#ifdef _FRAGMENT_SHADER_


// Uniform Locations
uniform int		g_UseVertexColor;
uniform vec4	g_Color;


// Framgment Shader Input
in Vertex_Out
{
	vec4	Color;		// ???_?F	
}IN;


// Fragment Shader Output
out vec4 Color_out;


// Main Function
void main()
{
	Color_out	= mix( g_Color, IN.Color, float(g_UseVertexColor) );
}


#endif
