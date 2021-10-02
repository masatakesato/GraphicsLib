#define GL_1_2

#ifdef GL_1_2
#define IN         attribute
#define OUT        varying
#define LOWP       lowp
#define MEDIUMP    mediump
//precision MEDIUMP float;
//precision MEDIUMP int;
#else
#define IN         in
#define OUT        out
#version 150
#define LOWP    
#define MEDIUMP   
#endif



const  vec4		g_TexSize = vec4( 512.0, 512.0, 1.0/512.0, 1.0/512.0 );	// x, y, 1/x, 1/y
uniform sampler2D	g_Texture;	// texture(normal)
uniform vec4		g_Diffuse;	// diffuse material




#ifdef _VERTEX_SHADER_

// Shader Input
IN vec3			POSITION;
IN vec2			TEXCOORD;
IN vec3			NORMAL;

// Uniform location
uniform mat4	g_MatWorld;
uniform mat4	g_MatViewProj;
uniform mat4	g_MatWorldViewProj;

// Vertex Shader Output
OUT vec2		vTexCoord;
OUT vec3		vNormal;

// Vertex Shader
void main(void)
{
	gl_Position = g_MatViewProj * g_MatWorld * vec4( POSITION, 1.0 );//

	vTexCoord	= TEXCOORD;
	vNormal		= NORMAL;
}


#endif




#ifdef _FRAGMENT_SHADER_

// Fragment Shader Input
varying vec2	vTexCoord;
varying vec3	vNormal;


// Fragment Shader
void main (void)
{
	vec3	top_normal		= normalize( ( texture2D( g_Texture, vTexCoord.xy + vec2(0.0, g_TexSize.w) ).xyz-vec3(0.5,0.5,0.5) ) * 2.0 );
	vec3	bottom_normal	= normalize( ( texture2D( g_Texture, vTexCoord.xy - vec2(0.0, g_TexSize.w) ).xyz-vec3(0.5,0.5,0.5) ) * 2.0 );
	vec3	right_normal	= normalize( ( texture2D( g_Texture, vTexCoord.xy + vec2(g_TexSize.z, 0.0) ).xyz-vec3(0.5,0.5,0.5) ) * 2.0 );
	vec3	left_normal		= normalize( ( texture2D( g_Texture, vTexCoord.xy - vec2(g_TexSize.z, 0.0) ).xyz-vec3(0.5,0.5,0.5) ) * 2.0 );

	vec3	center_normal	= normalize( ( texture2D( g_Texture, vTexCoord.xy							).xyz-vec3(0.5,0.5,0.5) ) * 2.0 );
	
	const vec3	right_vec	= vec3( 1.0, 0.0, 0.0 );
	const vec3	top_vec		= vec3( 0.0, 1.0, 0.0 );

	float div = 0.002 * ( ( left_normal.x - right_normal.x ) / ( 2.0 * g_TexSize.z ) + ( top_normal.y - bottom_normal.y ) / ( 2.0 * g_TexSize.w ) );

	div	= exp(-div)*0.5;

//float ao_weight = max( (right_normal.x - center_normal.x),  - (left_normal.x - center_normal.x) );// + max(top_normal.y - center_normal.y, 0.0);
	
	gl_FragColor = vec4( div, div, div, 0.0 );////vec4(vNormal, 0.0);// g_Diffuse;//
}

#endif
