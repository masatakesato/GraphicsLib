uniform sampler2D	g_Texture;	// texture
uniform float		g_Scale;	// color scale


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

// Constant Variables
#define	NVERTS 4;

const vec3 rects[] =
{
	vec3( -5.0, 10.0, -5.0 ),
	vec3( +5.0, 10.0, -5.0 ),
	vec3( +5.0, 10.0, +5.0 ),
	vec3( -5.0, 10.0, +5.0 )
};



// Uniform locations




// Fragment Shader Input
in vec2		vTexCoord;


// Fragment Shader Output
out vec4	outColor;


// Fragment Shader
void main (void)
{

	for( int i=0; i<NVERTS; ++i )
	{
		
		
	}// end of i loop


	vec4 color = texture2D( g_Texture, vTexCoord.xy ).xyzw;

	//if( color >=0.0)
	//	gl_FragColor = vec4(color ,color, color, 0.0) / 50.0;
	//else
	//	gl_FragColor = vec4(1.0, 0.0, 0.0, 0.0);
	
	outColor = color * g_Scale;

	//outColor = abs(texture2D( g_Texture, vTexCoord.xy ).xyzw * g_Scale );///255.0f;//0.00000381469; //vec4(color ,color, color, 0.0);//vec4( vTexCoord.xy, 0.0, 0.0 );//vec4(vNormal, 0.0);//
}

#endif
