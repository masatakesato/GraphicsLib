

// Uniform Varianbles

// noise params
//uniform float	g_Scale;
//uniform float	g_Stretch;
uniform float	g_Octaves;
uniform float	g_Lacunarity;
uniform float	g_Gain;


// output params





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
	// 入力座標のスケーリングと回転が必要か、、、
	gl_Position = vec4( POSITION, 1.0 );//
	vTexCoord	= ( g_MatTransform * vec4( TEXCOORD, 0.0, 1.0 ) ).xyz;//vec3( TEXCOORD, 0.0 ) * g_Scale * vec3( 1.0/g_Stretch, 1.0, 1.0 );//
}


#endif




#ifdef _FRAGMENT_SHADER_

// Include files
#pragma include"../shader/glsl/SimplexNoise.glsl"
#pragma include"../shader/glsl/OutputShader.glsl"

// Fragment Shader Input
in vec3		vTexCoord;

// Fragment Shader Output
out vec4	outColor;


// Fragment Shader
void main(void)
{
	float n=0.0;
		
	//n	= noise( vec2( vTexCoord.x*51.2, vTexCoord.y*51.2 ) );
	//outColor = vec4(vec3(n, n, n), 1.0);
	
	n	= Simplex_fBm( vTexCoord, g_Octaves, g_Lacunarity, g_Gain )*0.5 + 0.5;


// Output control
	n	= BrightnessContrast( n, g_Contrast, g_Brightness );
	n	= clamp( Threshold( n, g_Threshold, g_Smoothness ), 0.0, 1.0 );
	n	= g_Invert==1.0 ? Invert(n) : n;

	outColor = vec4( vec3(n, n, n), 1.0);
}


#endif
