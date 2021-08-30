// Include Files
#pragma include "Common.glsl"
#pragma include "ShadowMap.glsl"


// Uniform Locations
uniform sampler2D	g_TexDiffuse;	// Diffuse Gbuffer
uniform sampler2D	g_TexPosition;	// Position Gbuffer
uniform sampler2D	g_TexDepth;		// Depth Gbuffer
uniform sampler2D	g_TexNormal;	// Normal Gbuffer
uniform sampler2D	g_TexIrradiance;// Irradiance Buffer





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
#pragma include "LightAttribute.glsl"
#pragma include "BRDF_Common.glsl"


// Uniform Locations
uniform int	g_UseIrradianceMap;


// Fragment Shader Input
in vec2		vTexCoord;


// Fragment Shader Output
out vec4	outColor;



// Fragment Shader
void main (void)
{
	const vec3 diffColor	= texture( g_TexDiffuse, vTexCoord ).xyz;
	const vec4 wsPosition	= texture( g_TexPosition, vTexCoord ).xyzw;
	const float viewDepth	= texture( g_TexDepth, vTexCoord ).x;
	const vec3 wsNormal		= texture( g_TexNormal, vTexCoord ).xyz;
	const vec3 irrad		= texture( g_TexIrradiance, vTexCoord ).xyz;


	const int lightId		= 0;

	const int lightType			= g_LightType( lightId );
	const vec3 lightIntensity	= g_LightIntensity( lightId );
	const vec3 wsLightDir		= mix( normalize(g_LightPosition(lightId) - wsPosition.xyz), g_LightDir( lightId ), float(lightType==LIGHT_TYPE_DIRECTIONAL) );
	float n_dot_l				= max( dot( wsNormal, wsLightDir ), 0.0 );
	
	int shadowMapId				= g_ShadowCasterIndex( lightId );
	int	numShadows				= g_NumShadows( lightId );
	float islit					= float( GetShadowDistance( wsPosition, numShadows, shadowMapId, viewDepth ) < 0.0 );


	vec3 result	= diffColor * (n_dot_l * lightIntensity * islit + irrad * float(g_UseIrradianceMap) );

	//outColor = vec4( result, 1.0 );
	outColor = vec4( pow( result, vec3(1.0/2.2) ), 1.0 );
}

#endif
