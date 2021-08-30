#pragma include "Common.glsl"


#ifdef _VERTEX_SHADER_

// Shader Input
in vec4			POSITION;
in vec2			TEXCOORD;
in vec3			NORMAL;

// Vertex Shader Output
out vec2		vTexCoord;


// Vertex Shader
void main(void)
{
	gl_Position = POSITION;
	vTexCoord	= TEXCOORD;
}



#endif






#ifdef _FRAGMENT_SHADER_

// Include files
#pragma include "LightAttribute.glsl"
#pragma include "ShadowMap.glsl"


// Const Variables
uniform vec4 sample_kernel[17] =
{
    { 0.536343, 0.624624, 0.748867, 0},
    { 0.00317394, 0.000134823, 3.77269e-005, -2},
    { 0.0100386, 0.000914679, 0.000275702, -1.53125},
    { 0.0144609, 0.00317269, 0.00106399, -1.125},
    { 0.0216301, 0.00794618, 0.00376991, -0.78125},
    { 0.0347317, 0.0151085, 0.00871983, -0.5},
    { 0.0571056, 0.0287432, 0.0172844, -0.28125},
    { 0.0582416, 0.0659959, 0.0411329, -0.125},
    { 0.0324462, 0.0656718, 0.0532821, -0.03125},
    { 0.0324462, 0.0656718, 0.0532821, 0.03125},
    { 0.0582416, 0.0659959, 0.0411329, 0.125},
    { 0.0571056, 0.0287432, 0.0172844, 0.28125},
    { 0.0347317, 0.0151085, 0.00871983, 0.5},
    { 0.0216301, 0.00794618, 0.00376991, 0.78125},
    { 0.0144609, 0.00317269, 0.00106399, 1.125},
    { 0.0100386, 0.000914679, 0.000275702, 1.53125},
    { 0.00317394, 0.000134823, 3.77269e-005, 2},
};


//uniform vec4 sample_kernel[] =
//{
//    { 0.05882352941, 0.05882352941, 0.05882352941, 0},
//    { 0.05882352941, 0.05882352941, 0.05882352941, -2},
//    { 0.05882352941, 0.05882352941, 0.05882352941, -1.53125},
//    { 0.05882352941, 0.05882352941, 0.05882352941, -1.125},
//    { 0.05882352941, 0.05882352941, 0.05882352941, -0.78125},
//    { 0.05882352941, 0.05882352941, 0.05882352941, -0.5},
//    { 0.05882352941, 0.05882352941, 0.05882352941, -0.28125},
//    { 0.05882352941, 0.05882352941, 0.05882352941, -0.125},
//    { 0.05882352941, 0.05882352941, 0.05882352941, -0.03125},
//    { 0.05882352941, 0.05882352941, 0.05882352941, 0.03125},
//    { 0.05882352941, 0.05882352941, 0.05882352941, 0.125},
//    { 0.05882352941, 0.05882352941, 0.05882352941, 0.28125},
//    { 0.05882352941, 0.05882352941, 0.05882352941, 0.5},
//    { 0.05882352941, 0.05882352941, 0.05882352941, 0.78125},
//    { 0.05882352941, 0.05882352941, 0.05882352941, 1.125},
//    { 0.05882352941, 0.05882352941, 0.05882352941, 1.53125},
//    { 0.05882352941, 0.05882352941, 0.05882352941, 2},
//};





// Uniform Variables
uniform sampler2D	camera_depth_texture;//g_TexDepth;
uniform sampler2D	color_texture;

//uniform sampler2D	skin_texture;
uniform sampler2D	g_TexPosition;
uniform sampler2D	g_TexNormal;


const vec2			g_ScreenSize	= vec2(textureSize( color_texture, 0 ));//vec2( 1280.0, 720.0 );

uniform float		translucency;// = 0.0;//99;
uniform float		scale_separable = 2.5;//0.025;//0.75;//2.5;//
uniform float		depth_threshold	= 5.5;
//uniform float		scale_separable = 0.05;//1.8; //
//uniform float		depth_threshold	= 0.15;//5.0;//0.2;// * (far - near);//0.3;	// 

uniform vec2		blur_dir;

const float			near = 1.0e-2;//1.0e-3;	// camera z-near
const float			far	= 1.0e+3;//1.0e+2;	// camera z-far







vec3 SSSSTransmittance(
	int light_id,
	/**
	* This parameter allows to control the transmittance effect. Its range
	* should be 0..1. Higher values translate to a stronger effect.
	*/
	float translucency,
	/**
	* This parameter should be the same as the 'SSSSBlurPS' one. See below
	* for more details.
	*/
	float sssWidth,
	/**
	* Position in world space.
	*/
	vec3 worldPosition,
	/**
	* Normal in world space.
	*/
	vec3 worldNormal,
	/**
	* Light vector: lightWorldPosition - worldPosition.
	*/
	vec3 light,
/**
* Linear 0..1 shadow map.
*/
//	sampler2D shadowMap,
/**
* Regular world to light space matrix.
*/
//	mat4 lightViewProjection,
/**
* Far plane distance used in the light projection matrix.
*/
//float lightFarPlane 
	float dist
)
{
	/**
	* Calculate the scale of the effect.
	*/
	float scale = 8.25 * (1.0 - translucency) / sssWidth;
	/**
	* First we shrink the position inwards the surface to avoid artifacts:
	* (Note that this can be done once for all the lights)
	*/
	vec4 shrinkedPos = vec4( worldPosition - 0.05/*0.025/*0.005*//*0.02*/ * worldNormal, 1.0 );// アーティファクト減らす
	/**
	* Now we calculate the thickness from the light point of view:
	*/

	int layer			= g_ShadowCasterIndex( light_id );
int	numCascades		= g_NumShadows( light_id );
float lightFarPlane	= 1.0;
//float l_dot_n		= clamp( dot( worldNormal, light ), 0.0, 1.0 );
//float shadowdepth	= abs( shadowDistance( layer, numCascades, shrinkedPos, l_dot_n ) );//abs( min( shadowDistance( layer, numCascades, shrinkedPos, l_dot_n ), -0.05) );//
float shadowdepth	= GetShadowDistance( shrinkedPos, numCascades, layer, dist );


float d	= scale * shadowdepth;//abs( shadowDistance( layer, numCascades, shrinkedPos, l_dot_n ) ) * lightFarPlane;
//vec4 shadowPosition	= shrinkedPos * lightViewProjection;// SSSSMul(shrinkedPos, lightViewProjection);
//float d1			= texture( shadowMap, shadowPosition.xy/shadowPosition.w ).r; // 'd1' has a range of 0..1
//float d2			= shadowPosition.z; // 'd2' has a range of 0..'lightFarPlane'
//d1					*= lightFarPlane; // So we scale 'd1' accordingly:
//float d				= scale * abs( d1 - d2 );


	/**
	* Armed with the thickness, we can now calculate the color by means of the
	* precalculated transmittance profile.
	* (It can be precomputed into a texture, for maximum performance):
	*/
	float dd = -d * d;
	vec3 profile =	vec3( 0.233, 0.455, 0.649 )	* exp( dd / 0.0064 ) +
					vec3( 0.1, 0.336, 0.344 )	* exp( dd / 0.0484 ) +
					vec3( 0.118, 0.198, 0.0 )	* exp( dd / 0.187 ) +
					vec3( 0.113, 0.007, 0.007 )	* exp( dd / 0.567 ) +
					vec3( 0.358, 0.004, 0.0 )	* exp( dd / 1.99 ) +
					vec3( 0.078, 0.0, 0.0 )		* exp( dd / 7.41 );
	float prof_scale	= length(profile);
	profile /= mix( 1.0, prof_scale, float(prof_scale>1.0) );

	/**
	* Using the profile, we finally approximate the transmitted lighting from
	* the back of the object:
	*/
	return profile * clamp( 0.3 + dot(light, -worldNormal), 0.0, 1.0 );
}






















// TODO: レンダリングパスを整理する. 混ぜない!!! 2015.06.09


// Fragment Shader Input
in vec2	vTexCoord;

// Fragment Shader Output
out vec4	outColor;


// Fragment Shader
void main (void)
{
	const float kernel_range = 2.0;
	vec2 texcoords	= vTexCoord;//vec2(	(gl_FragCoord.x - 0.5) / (window_width-1.0),
						//	(gl_FragCoord.y - 0.5) / (window_height-1.0) ); 
	
	float depth	= texture( camera_depth_texture, texcoords ).r;
// manual calculation of nonlinear depth
//depth = depth * (near + far) / (near - far);
//depth += 2.0 * near * far / (near - far);
//depth = -depth;

	vec4 color	= texture( color_texture, texcoords );
	vec3 final_color = /*color.rgb;*/vec3(0.0, 0.0, 0.0);
	
	bool is_skin	= color.a==1.0;//length( color.rgb )>0.0;//true;//texture( skin_texture, texcoords ).r==1.0;
	vec2 final_step	= vec2( 0.0, 0.0 );

	if( !is_skin )
		discard;// reject non-subsurface pixels

	vec4 worldPosition		= texture( g_TexPosition, texcoords ).xyzw;
	vec3 worldNormal		= texture( g_TexNormal, texcoords ).xyz;
	
	float islit				= 0.0;
	float dist				= depth;
	
	int light_id			= 0;
	int shadowmap_id		= g_ShadowCasterIndex( light_id );
	int numShadows			= g_NumShadows( light_id );
	float shadowDistance	= GetShadowDistance( worldPosition, numShadows, shadowmap_id, dist );
	islit					= float( shadowDistance < 0.0 );	
	

	final_color		= final_color * sample_kernel[0].rgb;
	final_step		= scale_separable * blur_dir /* 0.0025*/ * 1.0 / depth;// * 1.0/kernel_range;
	//final_step		/= 3.0;


	float eye_depth	= depth;//-(depth * (far-near) + near);//


	for( int i=0; i<17; ++i )
	{
		vec2 offset	= texcoords + sample_kernel[i].a * final_step;

		vec4 sample_color		= texture( color_texture, offset );
		float sample_depth		= texture( camera_depth_texture, offset ).r;
		float sample_eye_depth	= sample_depth;//-( sample_depth * (far-near) + near );//

		vec3 color_inside	= sample_color.rgb * sample_kernel[i].rgb;
		vec3 color_outside	= color.rgb * sample_kernel[i].rgb;
		//float blend_weight	= clamp( ( depth_threshold - abs(eye_depth - sample_eye_depth) + 1.0 ), 0.0, 1.0 );
		//final_color.rgb += mix( color_outside, color_inside, blend_weight );

		float blend_weight	= clamp( /*300.0 */ depth_threshold * abs(eye_depth - sample_eye_depth) / depth, 0.0, 1.0 );
		final_color.rgb += mix( color_inside, color_outside, blend_weight );

	}// end of i loop


	//if( blur_dir.x>0.0 )//==1.0 
	//	final_color.rgb *= islit;

	
	//if( blur_dir.y>0.0 )//==1.0 )	// 最初にx方向、最後にy方向を処理。SSS処理が全部終わってからスペキュラを加算する
	//{
	//	final_color += texture( specular_texture, texcoords ).rgb;
	//}



	if( blur_dir.y>0.0 )//==1.0 
	{
		vec3 lightDir	= g_LightType(light_id)==LIGHT_TYPE_DIRECTIONAL ? g_LightDir(light_id) : normalize(g_LightPosition(light_id) - worldPosition.xyz);
		
		//int layer		= g_LightShadowMapID( light_id );
		//int	numCascades	= g_LightNumCascades( light_id );
		//float shadowed	= isShadowed( layer, numCascades, worldPosition );
		
		final_color.rgb	*= islit>0.0 ? 1.0 : 1.0-translucency;
		final_color.rgb += g_LightIntensity(light_id) /* M_PI*/ * SSSSTransmittance( shadowmap_id, translucency, scale_separable, worldPosition.xyz, worldNormal, lightDir, dist );
	}

	outColor = mix( color, vec4( final_color, 1.0 ), color.a );//color.a==0.0 ? vec4( final_color, /*color.a*/0.0 ) : color;
}


#endif
