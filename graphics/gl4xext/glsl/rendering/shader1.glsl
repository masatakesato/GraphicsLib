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



uniform sampler2D	g_Texture;	// texture
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
    gl_FragColor = vec4( vTexCoord.xy, 0.0, 0.0 );//vec4(vNormal, 0.0);// g_Diffuse;//
}

#endif
