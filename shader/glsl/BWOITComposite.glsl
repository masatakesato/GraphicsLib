#ifdef _VERTEX_SHADER_

// Shader Input
in vec3			POSITION;
in vec2			TEXCOORD;

// Uniform location


// Vertex Shader Output
out vec2		vTexCoord;


// Vertex Shader
void main(void)
{
	gl_Position = vec4( POSITION, 1.0 );//
	vTexCoord	= TEXCOORD;
}


#endif




#ifdef _FRAGMENT_SHADER_


// Uniform Locations
uniform sampler2D	g_TexAccum;	// Accumulation Texture
uniform sampler2D	g_TexRevealage;	// Revealage Texture
uniform vec3		g_BackgroundColor;



// Constant Variables



// Fragment Shader Input
in vec2		vTexCoord;


// Fragement Shader Output
out vec4	outColor;


// Fragment Shader
void main (void)
{
	vec4 accum		= texture( g_TexAccum, vTexCoord ).xyzw;//texelFetch(RT0, int2(gl_FragCoord.xy), 0);
	float reveal	= texture( g_TexRevealage, vTexCoord ).x;//texelFetch(RT1, int2(gl_FragCoord.xy), 0).r;
 
	vec3 averageColor	=  accum.xyz / max(accum.w, 0.00001);

	// 自前アルファブレンド処理
	outColor.xyz	= averageColor * (1.0 - reveal) + g_BackgroundColor * reveal;

	// Blend Func: GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA. OpenGLアルファブレンド使用時の処理.
	//outColor		= vec4( accum.xyz / max( accum.w, 0.00001 ), reveal );
}

#endif



















