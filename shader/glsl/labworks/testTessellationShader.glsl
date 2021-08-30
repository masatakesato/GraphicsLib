#version 430



uniform sampler2D	g_Texture;	// texture
uniform float		g_Scale;	// color scale


#ifdef _VERTEX_SHADER_

// Shader Input
in vec3			POSITION;
in vec2			TEXCOORD;

// Uniform location


// Vertex Shader Output
out vec3		WorldPos_CS_in;//vPosition;
out vec2		TexCoord_CS_in;//vTexCoord;


// Vertex Shader
void main(void)
{
	gl_Position = vec4( POSITION, 1.0 );

	WorldPos_CS_in	= POSITION;//vPosition	= POSITION;
	TexCoord_CS_in	= TEXCOORD;//vTexCoord	= TEXCOORD;
}


#endif




//##########################################################################################//
//										Tessellation										//
//##########################################################################################//

//===================== Tessellation Control Shader =====================//
#ifdef _TESSELLATION_CONTROL_SHADER_

// 出力するパッチの頂点数を定義する
layout (vertices = 3) out; 

// Uniform Locations
uniform vec3 gEyeWorldPos; 


// TCS Input
in vec3 WorldPos_CS_in[];
in vec2 TexCoord_CS_in[];
//in vec3 Normal_CS_in[]; 


// TCS Output
out vec3 WorldPos_ES_in[]; 
out vec2 TexCoord_ES_in[]; 
//out vec3 Normal_ES_in[];



// 2点間の距離からテッセレーションレベルを計算する
float GetTessLevel( float Distance0, float Distance1 )
{  
	float AvgDistance = (Distance0 + Distance1) / 2.0;

	if( AvgDistance <= 2.0 )		return 10.0;
	else if( AvgDistance <= 5.0 )	return 7.0;
	else							return 3.0;
}


// Tessellation Control Shader
void main()
{
	// 出力パッチのコントロールポイントを設定する
	TexCoord_ES_in[ gl_InvocationID ]	= TexCoord_CS_in[ gl_InvocationID ];
	//Normal_ES_in[ gl_InvocationID ]		= Normal_CS_in[ gl_InvocationID ];
	WorldPos_ES_in[ gl_InvocationID ]	= WorldPos_CS_in[ gl_InvocationID ];

	// 視点から各コントロールポイントまでの距離を計算する
	float EyeToVertexDistance0	= distance( gEyeWorldPos, WorldPos_ES_in[0] );
	float EyeToVertexDistance1	= distance( gEyeWorldPos, WorldPos_ES_in[1] );
	float EyeToVertexDistance2	= distance( gEyeWorldPos, WorldPos_ES_in[2] );

	// 視点までの距離情報を使って、テッセレーションレベルを計算する
	gl_TessLevelOuter[0] = 1.0;//GetTessLevel( EyeToVertexDistance1, EyeToVertexDistance2 );
	gl_TessLevelOuter[1] = 1.0;//GetTessLevel( EyeToVertexDistance2, EyeToVertexDistance0 );
	gl_TessLevelOuter[2] = 1.0;//GetTessLevel( EyeToVertexDistance0, EyeToVertexDistance1 );
	gl_TessLevelInner[0] = 3.0;//gl_TessLevelOuter[2];
	//gl_TessLevelInner[1] = 2.0;//gl_TessLevelOuter[2];
}


#endif



//===================== Tessellation Evaluation Shader =====================//

#ifdef _TESSELLATION_EVALUATION_SHADER_


layout(triangles, equal_spacing, ccw) in;


// Uniform Variables
//uniform mat4 gVP;
//uniform sampler2D gDisplacementMap;
//uniform float gDispFactor;


// TES Input
in vec3 WorldPos_ES_in[];
in vec2 TexCoord_ES_in[];
//in vec3 Normal_ES_in[];


// TES Output
out vec3 WorldPos_FS_in;
out vec2 TexCoord_FS_in;
//out vec3 Normal_FS_in;



vec2 interpolate2D( vec2 v0, vec2 v1, vec2 v2 )
{
	return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}


vec3 interpolate3D( vec3 v0, vec3 v1, vec3 v2 )
{
	return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}


// Tessellation Evaluation Shader
void main()
{
	// Interpolate the attributes of the output vertex using the barycentric coordinates
	TexCoord_FS_in = interpolate2D( TexCoord_ES_in[0], TexCoord_ES_in[1], TexCoord_ES_in[2] ); 
	//Normal_FS_in = interpolate3D(Normal_ES_in[0], Normal_ES_in[1], Normal_ES_in[2]);
	//Normal_FS_in = normalize(Normal_FS_in);  
	WorldPos_FS_in = interpolate3D( WorldPos_ES_in[0], WorldPos_ES_in[1], WorldPos_ES_in[2] );

	// Displace the vertex along the normal  
	//float Displacement = texture(gDisplacementMap, TexCoord_FS_in.xy).x;
	//WorldPos_FS_in += Normal_FS_in * Displacement * gDispFactor;  
	gl_Position = /*gVP * */vec4(WorldPos_FS_in, 1.0);
}  

#endif


//##########################################################################################//
//##########################################################################################//



#ifdef _FRAGMENT_SHADER_

// Fragment Shader Input
in vec3		WorldPos_FS_in;
in vec2		TexCoord_FS_in;//in vec2		vTexCoord;

out vec4	outColor;


// Fragment Shader
void main (void)
{
	//vec4 color = texture2D( g_Texture, vTexCoord.xy ).xyzw;	
	//outColor = color * g_Scale;

	outColor	= vec4( TexCoord_FS_in, 0.0, 1.0 );
}

#endif
