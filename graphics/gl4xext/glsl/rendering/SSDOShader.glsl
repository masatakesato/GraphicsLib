// Include Files
#pragma include "Common.glsl"


struct SSDOParams
{
	float	OcclusionRadius;
	float	MaxOcclusionDistance;
	vec2	TexSize;
	//vec3	ViewPosition;
};



//#################### バーテックスシェーダ ##################//
#ifdef _VERTEX_SHADER_


// Uniforms


// Inputs
in vec4			POSITION;
in vec2			TEXCOORD;


// Outputs
out Vertex_Out
{
	vec2		TexCoord;	// テクスチャ座標
}OUT;


// Vertex Shader Main Function
void main()
{	
	gl_Position		= POSITION;
	OUT.TexCoord	= TEXCOORD;
}


#endif







//##################### フラグメントシェーダ #################//
#ifdef _FRAGMENT_SHADER_

#pragma include "LightPropagationVolumes/LPVHelperFunctions.glslh"


// Uniforms
uniform ubSSDOParams
{
	SSDOParams	g_Params;
};

uniform int			g_BlurDir;
uniform sampler2D	g_TexPosition;
uniform sampler2D	g_TexNormal;
uniform sampler2D	g_TexNoise;
uniform sampler2D	g_TexOcclusion;
uniform sampler2D	g_TexFinal;


// Input
in Vertex_Out
{
	vec2	TexCoord;	// テクスチャ座標	
}IN;


// Output
out vec4	outColor;



// Static Functions


vec4 dssdo_accumulate( vec2 texcoord )
{
	vec3 points[] =
	{
		vec3(-0.134, 0.044, -0.825),
		vec3(0.045, -0.431, -0.529),
		vec3(-0.537, 0.195, -0.371),
		vec3(0.525, -0.397, 0.713),
		vec3(0.895, 0.302, 0.139),
		vec3(-0.613, -0.408, -0.141),
		vec3(0.307, 0.822, 0.169),
		vec3(-0.819, 0.037, -0.388),
		vec3(0.376, 0.009, 0.193),
		vec3(-0.006, -0.103, -0.035),
		vec3(0.098, 0.393, 0.019),
		vec3(0.542, -0.218, -0.593),
		vec3(0.526, -0.183, 0.424),
		vec3(-0.529, -0.178, 0.684),
		vec3(0.066, -0.657, -0.570),
		vec3(-0.214, 0.288, 0.188),
		vec3(-0.689, -0.222, -0.192),
		vec3(-0.008, -0.212, -0.721),
		vec3(0.053, -0.863, 0.054),
		vec3(0.639, -0.558, 0.289),
		vec3(-0.255, 0.958, 0.099),
		vec3(-0.488, 0.473, -0.381),
		vec3(-0.592, -0.332, 0.137),
		vec3(0.080, 0.756, -0.494),
		vec3(-0.638, 0.319, 0.686),
		vec3(-0.663, 0.230, -0.634),
		vec3(0.235, -0.547, 0.664),
		vec3(0.164, -0.710, 0.086),
		vec3(-0.009, 0.493, -0.038),
		vec3(-0.322, 0.147, -0.105),
		vec3(-0.554, -0.725, 0.289),
		vec3(0.534, 0.157, -0.250),
	};

	const int num_samples = 32;

	vec2 noise_texture_size	= vec2( 4.0, 4.0 );
	vec3 center_pos			= texture( g_TexPosition, texcoord ).xyz;
	float center_depth		= distance( g_ViewAttrib.Position, center_pos );

	float radius						= max( g_Params.OcclusionRadius / center_depth, 1.0e-2);
	float max_distance_inv				= 1.0 / g_Params.MaxOcclusionDistance;
	float attenuation_angle_threshold	= 0.1;
	
	vec3 noise = texture( g_TexNoise, texcoord*g_Params.TexSize.xy/noise_texture_size).xyz;

	//radius = min(radius, 0.1);

	vec3 center_normal = texture( g_TexNormal, texcoord ).xyz;

	vec4 occlusion_sh2 = vec4( 0.0 );

	const float fudge_factor_l0 = 2.0;
	const float fudge_factor_l1 = 10.0;

	const float sh2_weight_l0 = fudge_factor_l0 * 0.28209; //0.5*sqrt(1.0/pi);
	const vec3 sh2_weight_l1 = vec3( fudge_factor_l1, fudge_factor_l1, fudge_factor_l1 ) * 0.48860; //0.5*sqrt(3.0/pi);

	const vec4 sh2_weight = vec4(sh2_weight_l1, sh2_weight_l0) / num_samples;

	//[unroll] // compiler wants to branch here by default and this makes it run nearly 2x slower on PC and 1.5x slower on 360!
	for( int i=0; i<num_samples; ++i )
	{
		vec2 textureOffset		= reflect( points[ i ].xy, noise.xy ).xy * radius;
		vec2 sample_tex			= texcoord + textureOffset;
		vec3 sample_pos			= textureLod( g_TexPosition, sample_tex, 0.0 ).xyz;
		vec3 center_to_sample	= sample_pos - center_pos;
		float dist				= max( length( center_to_sample ), 1.0e-3 );
		vec3 center_to_sample_normalized = center_to_sample / dist;
		float attenuation		= 1.0 - clamp( dist * max_distance_inv, 0.0, 1.0 );
		float dp				= dot( center_normal, center_to_sample_normalized );

		attenuation				= attenuation*attenuation * step( attenuation_angle_threshold, dp );

		occlusion_sh2 += attenuation * CalcSHCoeff( center_to_sample_normalized );
	}

	return occlusion_sh2 * sh2_weight;

}


vec4 dssdo_blur( vec2 texcoord, vec2 dir )
{
	float weights[9] =
	{
		0.013519569015984728,
		0.047662179108871855,
		0.11723004402070096,
		0.20116755999375591,
		0.240841295721373,
		0.20116755999375591,
		0.11723004402070096,
		0.047662179108871855,
		0.013519569015984728
	};

	float indices[9]	= { -4.0, -3.0, -2.0, -1.0, 0.0, +1.0, +2.0, +3.0, +4.0 };

	vec2 step			= dir / g_Params.TexSize.xy;

	vec3 normal[9];

	normal[0] = texture( g_TexNormal, texcoord + indices[0] * step ).xyz;
	normal[1] = texture( g_TexNormal, texcoord + indices[1] * step ).xyz;
	normal[2] = texture( g_TexNormal, texcoord + indices[2] * step ).xyz;
	normal[3] = texture( g_TexNormal, texcoord + indices[3] * step ).xyz;
	normal[4] = texture( g_TexNormal, texcoord + indices[4] * step ).xyz;
	normal[5] = texture( g_TexNormal, texcoord + indices[5] * step ).xyz;
	normal[6] = texture( g_TexNormal, texcoord + indices[6] * step ).xyz;
	normal[7] = texture( g_TexNormal, texcoord + indices[7] * step ).xyz;
	normal[8] = texture( g_TexNormal, texcoord + indices[8] * step ).xyz;
	
	float total_weight = 1.0;
	float discard_threshold = 0.85;

	int i;

	for( i=0; i<9; ++i )
	{
		if( dot(normal[i], normal[4]) < discard_threshold )
		{
			total_weight -= weights[i];
			weights[i] = 0;
		}
	}

	//

	vec4 res = vec4( 0.0, 0.0, 0.0, 0.0 );

	for( i=0; i<9; ++i )
	{
		res += texture( g_TexOcclusion, texcoord + indices[i] * step ) * weights[i];
	}

	res /= total_weight;

	return res;
}



// Fragment Shader Main Functions

void main()
{

#if defined( PASS_OCCLUSION_SH )

	outColor	= dssdo_accumulate( IN.TexCoord );

#elif defined( PASS_BLUR )

	const vec2 blur_dir[2] = { vec2( 1.0, 0.0 ), vec2( 0.0, 1.0 ) };
	outColor	= dssdo_blur( IN.TexCoord, blur_dir[ g_BlurDir ] );

#else

	vec3 L				= normalize( g_ViewAttrib.Position - texture( g_TexPosition, IN.TexCoord ).xyz );
	vec4 SampleDirSH	= CalcSHCoeff(L);
	vec4 occlusionSH	= texture( g_TexFinal, IN.TexCoord ).xyzw;

	float occul			= clamp( dot(SampleDirSH, occlusionSH), 0.0, 1.0 );

	outColor			= pow( (1.0 - occul), 2.0 )  * vec4( 1.0, 1.0, 1.0, 0.0 );//
	//outColor			= occlusionSH;//vec4( L, 0.0 );

#endif

}






#endif

