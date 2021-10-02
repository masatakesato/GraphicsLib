#version 420




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

// Include files
#pragma include"../shader/GeometricalNoise.glsl"


// Uniform Varianbles
uniform vec3	g_Scale;
uniform float	g_Octaves;
uniform float	g_Lacunarity;
uniform float	g_Gain;


// Fragment Shader Input
in vec2		vTexCoord;


// Fragment Shader Output
out vec4	outColor;


// Fragment Shader
void main(void)
{
	float n;
		
	//n	= noise( vec2( vTexCoord.x*51.2, vTexCoord.y*51.2 ) );
	//outColor = vec4(vec3(n, n, n), 1.0);
	
	//n	= clamp( Geometrical_fBm( vec3(vTexCoord.xy, 0.0 )*g_Scale.xyy, g_Octaves, g_Lacunarity, g_Gain ), 0.4, 0.6 );
	n	= clamp( Geometrical_fBm( vec3(vTexCoord.xy, 0.0 )*g_Scale.xyy, g_Octaves, g_Lacunarity, g_Gain ), g_Min, g_Max );
	n	= (n - g_Min) / (g_Max - g_Min);
	outColor = vec4(vec3(n, n, n), 1.0);



	// TODO: 2013.10.10: まずはノイズ関数の出力値をそのまま使う
	// TODO: 2013.10.15: ノイズ関数の出力値にポストプロセスをかけられるようにする (クランプ、正規化、重み付けetc..)
}


#endif
