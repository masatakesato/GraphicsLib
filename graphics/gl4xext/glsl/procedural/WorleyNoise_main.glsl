

// Uniform Varianbles

// noise params
//uniform float	g_Scale;
//uniform float	g_Stretch;
uniform float	g_Octaves;
uniform float	g_Lacunarity;
uniform float	g_Gain;



#ifdef _VERTEX_SHADER_

// Shader Input
in vec3			POSITION;
in vec2			TEXCOORD;

// Uniform location
uniform mat4	g_MatTransform;


// Vertex Shader Output
out vec3		vTexCoord;


// Vertex Shader
void main(void)
{
	gl_Position = vec4( POSITION, 1.0 );//
	vTexCoord	= ( g_MatTransform * vec4( TEXCOORD, 0.0, 1.0 ) ).xyz;//vec3( TEXCOORD, 0.0 ) * g_Scale;
}


#endif




#ifdef _FRAGMENT_SHADER_

// Include files
#pragma include"../shader/glsl/WorleyNoise.glsl"
#pragma include"../shader/glsl/OutputShader.glsl"

// Uniform Varianbles
uniform int	g_Mode;

// Fragment Shader Input
in vec3		vTexCoord;

// Fragment Shader Output
out vec4	outColor;


// Fragment Shader
void main (void)
{
	vec2 noise = Worley_fBm( vTexCoord, g_Octaves, g_Lacunarity, g_Gain, g_Mode );

// Output control
	noise.x	= BrightnessContrast( noise.x, g_Contrast, g_Brightness );
	noise.x	= clamp( Threshold( noise.x, g_Threshold, g_Smoothness ), 0.0, 1.0 );
	noise.x	= g_Invert==1.0 ? Invert( noise.x ) : noise.x;

	outColor = vec4( vec3( noise.x), noise.y );
}


#endif
