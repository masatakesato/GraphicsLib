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




#ifdef _GEOMETRY_SHADER_


// Input/Output Layouts
layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;


// Uniform Locations



// Geometry Shader Input
in vec2		vTexCoord[3];	// 法線ベクトル


// Geometry Shader Output
out vec2	gTexCoord;	// テクスチャ座標


// Main Function
void main()
{
	gl_Position	= gl_in[0].gl_Position;
	gTexCoord	= vTexCoord[0];
	EmitVertex();

	gl_Position	= gl_in[1].gl_Position;;
	gTexCoord	= vTexCoord[1];
	EmitVertex();

	gl_Position	= gl_in[2].gl_Position;;
	gTexCoord	= vTexCoord[2];
	EmitVertex();

	EndPrimitive();
}


#endif



#ifdef _FRAGMENT_SHADER_


// Uniform Locations
layout( binding = 0, rgba32f )	uniform coherent imageBuffer	g_TboTexelCount;
layout( binding = 0 )			uniform atomic_uint				g_TexelCount;


// Constant Variables


// Fragment Shader Input
in vec2		gTexCoord;


// Fragment Shader Output
out vec4	outColor;



// Fragment Shader
void main (void)
{
	float dest		= 16 * (16 * gTexCoord.y) + gTexCoord.x * 16;
	uint counter	= atomicCounterIncrement(g_TexelCount);
	memoryBarrier();

	vec4 color		= counter>255 ? vec4(1,0,0,0) : vec4( float(counter)/255.0 );

	imageStore( g_TboTexelCount, int(counter), vec4(dest/255.0) );
	//outColor		= color;

	//outColor	= vec4( vTexCoord, 0, 0 ) * float(g_ImageSize);
}

#endif
