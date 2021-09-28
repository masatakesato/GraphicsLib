// Include Files
#pragma include "Common.glsl"



//#################### Vertex Shader ##################//
#ifdef _VERTEX_SHADER_


// Variables


// Inputs
in vec4			POSITION;
in vec2			TEXCOORD;
in vec3			NORMAL;
in vec3			COLOR;

//in vec3			TANGENT;
//in vec3			BINORMAL;


// Outputs
out Vertex_Out
{
	vec2		TexCoord;
}OUT;


// Main function
void main()
{	
	gl_Position		= POSITION;
	OUT.TexCoord	= TEXCOORD;
}


#endif







//##################### Fragment Shader #################//
#ifdef _FRAGMENT_SHADER_


// Uniform variables
uniform sampler1D	g_Texture1D;	// texture1D
uniform sampler2D	g_Texture2D;	// texture2D
uniform float		g_Scale;	// color scale
uniform int			g_Mode;


// Inputs
in Vertex_Out
{
	vec2		TexCoord;
}IN;


// Outputs
out vec4 outColor;


// Main Function
void main()
{
	vec4 color = vec4(0);

	if( g_Mode==0 )		color = texture( g_Texture1D, IN.TexCoord.x ).xyzw;
	else if( g_Mode==1)	color = texture( g_Texture2D, IN.TexCoord.xy ).xyzw;

	outColor = color * g_Scale;
	//outColor	= pow( color * g_Scale, vec4(1.0/2.2) );

	//if( IN.TexCoord.x<=0.0009765625 )	outColor		= vec4(1,0,0,0);
	//else if( IN.TexCoord.x>=1.0 )	outColor	= vec4(0,1,0,0);
	//else if( IN.TexCoord.y<=0.0 )	outColor	= vec4(0,0,1,0);
	//else if( IN.TexCoord.y>=1.0 )	outColor	= vec4(1,0,1,0);
}


#endif
