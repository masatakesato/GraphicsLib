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

// Include Files
#pragma include "vct/VCTCommon.glsl"
#pragma include "VectorFormatConversion.glsl"

// Uniform Locations
uniform sampler2D	g_TexPosition;
uniform sampler2D	g_TexNormal;
//uniform sampler3D	g_TexInput;
//uniform isampler3D	g_TexInput;// for integer format texture
uniform usampler3D	g_TexInput;// for integer format texture


// Fragment Shader Input
in vec2		vTexCoord;


// Fragment Shader Output
out vec4	outColor;



// Fragment Shader
void main(void)
{
	vec4 worldPos		= texture( g_TexPosition, vTexCoord ).xyzw;
		
	if( worldPos.w == 0.0 ) discard;
	
	vec3 vctTexCoord	= ( vec4(worldPos) * g_MatTexSpace ).xyz;
	
	
	//vec3 voxelColor		= textureLod( g_TexInput, vctTexCoord, 0 ).xyz;
	//outColor			= vec4( voxelColor, 1.0 );
	
	
	// Extract Volume Cell Index[0,1] from xyz10 compressed format
	//vec3 voxelColor	= vec3( convUINTXYZ10ToVec3( (texture( g_TexInput, vctTexCoord ).x) ) ) / vec3(g_VoxelResolution);
	//outColor		= vec4( voxelColor, 1.0 );
		
	// Extract Normal Vector form RGB8 To Vec4
	vec4 color	= convRGBA8ToVec4( texture( g_TexInput, vctTexCoord ).x ) / 255.0;
	color.xyz	= color.xyz * 2.0 - vec3(1.0);
	outColor	= color;


	// Extract Color form RGB8 To Vec4
	//vec4 color	= convRGBA8ToVec4( texture( g_TexInput, vctTexCoord ).x ) / 255.0;
	//outColor	= color;


}

#endif
