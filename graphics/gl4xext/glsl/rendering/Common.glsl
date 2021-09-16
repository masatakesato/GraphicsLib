#ifndef COMMON_H
#define COMMON_H




#define	MAX_MATERIALS			8

#define	MAX_DIRECTIONAL_LIGHTS	256
#define	MAX_POINT_LIGHTS		256
#define	MAX_SPOT_LIGHTS			256
#define	MAX_AREA_LIGHTS			256


#ifndef MAT_IDENTITY
#define MAT_IDENTITY mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 )
#endif


#ifndef M_PI
#define M_PI 3.141592
#endif



struct ViewTransform
{
	mat4	MatView;
	mat4	MatProjection;
	mat4	MatViewProjection;
	mat4	MatInvViewProjection;
};


struct ViewAttribute
{
	vec3	Position;
	vec3	Forward;
	vec3	Vertical;
	vec3	Horizontal;
	float	Fov;
	float	AspectRatio;
	float	zNear;
	float	zFar;
};



uniform ubViewTransform
{
	ViewTransform	g_ViewTransform;
};


uniform ubViewAttribute
{
	ViewAttribute	g_ViewAttrib;
};


uniform mat4	g_MatWorld			= MAT_IDENTITY;
//uniform mat4	g_MatWorldViewProj	= MAT_IDENTITY;

const mat3		g_MatWorldInverseTranspose		= mat3( transpose( inverse( g_MatWorld ) ) );


// マテリアルパラメータ. マテリアル構成要素をまとめた. CPU側での変数セットアップが大変
//struct MaterialParam
//{
//	vec3	Ambient;
//	float	Transparency;
//	vec3	Diffuse;
//	float	Shininess;
//	vec3	Specular;
//	float	RefractiveIndex;// 屈折率
//};

//layout(location = 0) uniform MaterialParam g_Materials[ MAX_MATERIALS ];


// マテリアルパラメータ. マテリアル構成要素毎に配列化
struct MaterialParam
{
	vec3	Ambient[ MAX_MATERIALS ];
	vec3	Diffuse[ MAX_MATERIALS ];
	vec3	Specular[ MAX_MATERIALS ];

	float	Shiness[ MAX_MATERIALS ];
	float	RefractiveIndex[ MAX_MATERIALS ];
	float	Opacity[ MAX_MATERIALS ];
};

uniform MaterialParam	g_Materials;




// UnoformBufferObjectでこうやって配列は作れない
//layout(location = 6) uniform DirectionalLightParam g_DirectionalLights[ MAX_DIRECTIONAL_LIGHTS ];

// UniformBufferObjectでこれも駄目。配列長の制限が厳しい
//uniform LightSourceBlock
//{
//	vec4	Direction;
//	vec4	Intensity;
//}g_DirectionalLights[ MAX_DIRECTIONAL_LIGHTS ];



#endif	// COMMON_H //
