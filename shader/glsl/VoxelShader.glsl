// Include Files
#pragma include "Common.glsl"
#pragma include "VectorFormatConversion.glsl"



// Uniform Variables
uniform sampler3D	g_Texture3D;
uniform int			g_Lod	= 0;


// Constant Variables
const ivec3 g_TexSize	= textureSize( g_Texture3D, g_Lod );
const vec3 g_TexelSize	= 1.0 / vec3( g_TexSize );





//##############################################################################//
//								Vertex Shader									//
//##############################################################################//

#ifdef _VERTEX_SHADER_

// Shader Input
in vec3			POSITION;
//in vec3			TEXCOORD;
in vec3			NORMAL;


// Uniform Locations
uniform mat4	g_MatTransform	= MAT_IDENTITY;
uniform float	g_Scale			= 1.0;


// Constant Variables
const uint g_TexSliceSize	= g_TexSize.x * g_TexSize.y;


// Vertex Shader Output
out vec4		vWorldPos;
out vec3		vTexCoord;
out vec3		vNormal;



// Vertex Shader
void main(void)
{
	// インスタンスIDからワールド空間上の中心位置を計算する
	uvec3 wsCellIdx;
	wsCellIdx.z			= gl_InstanceID / g_TexSliceSize;
	uint zSliceOffset	= g_TexSliceSize * wsCellIdx.z;
	wsCellIdx.y			= ( gl_InstanceID - zSliceOffset ) / g_TexSize.x;
	wsCellIdx.x			= gl_InstanceID - zSliceOffset - g_TexSize.x * wsCellIdx.y;


	vec4 wsCenterPos	= vec4( vec3(wsCellIdx), 1.0 ) * g_MatTransform;
	vec4 wPos			= vec4( wsCenterPos.xyz + POSITION * g_Scale, 1.0 );
	gl_Position			= wPos * g_ViewTransform.MatViewProjection;
	vWorldPos			= wsCenterPos;//wsCellPos;
	vTexCoord			= (vec3(wsCellIdx) + vec3(0.5)) / vec3(g_TexSize);// テクスチャ座標はインスタンスID毎のセル中心点から計算
	vNormal				= normalize( NORMAL * mat3(g_MatWorldInverseTranspose) );
}


#endif




//##############################################################################//
//								Geometry Shader									//
//##############################################################################//

#ifdef _GEOMETRY_SHADER_

// Input/Output Layout
layout( triangles )	in;
layout( triangle_strip, max_vertices=3 )	out;


// Geometry Shader Input
in vec4		vWorldPos[3];
in vec3		vTexCoord[3];
in vec3		vNormal[3];


// Geometry Shader Output
out vec4	gWorldPos;
out vec3	gTexCoord;
out vec3	gNormal;



// Main Function
void main()
{

	if( any( notEqual(textureLod( g_Texture3D, vTexCoord[0], g_Lod ).xyzw, vec4(0) ) ) )
	//if( any( notEqual(texture( g_Texture3D, vTexCoord[0] ).xyzw, vec4(0) ) ) )
	{
		gl_Position	= gl_in[0].gl_Position;
		gWorldPos	= vWorldPos[0];
		gTexCoord	= vTexCoord[0];
		gNormal		= vNormal[0];
		EmitVertex();

		gl_Position	= gl_in[1].gl_Position;
		gWorldPos	= vWorldPos[1];
		gTexCoord	= vTexCoord[1];
		gNormal		= vNormal[1];
		EmitVertex();

		gl_Position	= gl_in[2].gl_Position;
		gWorldPos	= vWorldPos[2];
		gTexCoord	= vTexCoord[2];
		gNormal		= vNormal[2];
		EmitVertex();

		EndPrimitive();
	}

}



#endif




//##############################################################################//
//								Fragment Shader									//
//##############################################################################//

#ifdef _FRAGMENT_SHADER_

// Include Files



// Constant Variables



// Fragment Shader Input
in vec4		gWorldPos;
in vec3		gTexCoord;
in vec3		gNormal;


// Fragment Shader Output
out vec4	outColor;


// Fragment Shader
void main( void )
{

	//==================== Extract Normalized[-1, +1] Value from RGBA8 =====================//
	vec4 color	= convRGBA8ToVec4( floatBitsToUint(texture( g_Texture3D, gTexCoord ).x) ) / 255.0;
	color.xyz	= color.xyz * 2.0 - vec3(1.0);
	outColor	= color;

	////==================== Extract RGB from RGBA8 =====================//
	//vec4 color	= convRGBA8ToVec4( floatBitsToUint(texture( g_Texture3D, gTexCoord ).x) ) / 255.0;
	//outColor	= color;


	////======================= Extract 10bit XYZ Voxel Coortinates ============================//
	//uvec3 color		= convUINTXYZ10ToVec3( floatBitsToUint(texture( g_Texture3D, gTexCoord ).x) );
	//outColor.xyz	= vec3(color) * g_TexelSize;


	////======================= Just Output Sampled Texture Value ==============================//
	//outColor	= textureLod( g_Texture3D, gTexCoord, g_Lod ).xyzw;
	//outColor	= texture( g_Texture3D, gTexCoord ).xyzw;


	// Debug Output
	//outColor	= vec4( gTexCoord, 0.0 );


}

#endif
