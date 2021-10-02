#version 430




#ifdef _VERTEX_SHADER_

// Shader Input
in vec3			POSITION;
in vec2			TEXCOORD;

// Uniform Locations


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
layout( binding=0, rgba32f )	uniform readonly imageBuffer	g_TBO;


// Constant Variables
const int g_ImageSize	= imageSize( g_TBO );



// Fragment Shader Input
in vec2		vTexCoord;


// Fragment Shader Output
out vec4	outColor;



// Fragment Shader
void main (void)
{
	vec4 color	= imageLoad( g_TBO, int(vTexCoord.x*g_ImageSize) ).xyzw;
	outColor	= color;

	//outColor	= vec4( vTexCoord, 0, 0 ) * float(g_ImageSize);
}

#endif
