#pragma include "Common.glsl"
#pragma include "ShadowMap.glsl"



uniform vec3		g_Diffuse;	// diffuse material
uniform vec3		g_Specular;	// specular material
uniform float		g_Shininess;
uniform float		g_Transparency	= 0.25;



#ifdef _VERTEX_SHADER_

// Shader Input
in vec3			POSITION;
in vec2			TEXCOORD;
in vec3			NORMAL;


// Uniform location



// Vertex Shader Output
out vec4		vWorldPos;
out vec2		vTexCoord;
out vec3		vNormal;
out vec4		vWorldViewProjPos;



// Vertex Shader
void main(void)
{
	vec4 wPos			= vec4( POSITION, 1.0 ) * g_MatWorld;
	gl_Position			= wPos * g_ViewTransform.MatViewProjection;
	vWorldPos			= wPos;
	vTexCoord			= TEXCOORD;
	vNormal				= normalize( NORMAL * mat3( g_MatWorldInverseTranspose ) );
	vWorldViewProjPos	= gl_Position;
}


#endif




#ifdef _FRAGMENT_SHADER_

// Include Files
#pragma include "LightAttribute.glsl"
#pragma include "BRDF_Common.glsl"



// Uniform Locations
uniform float	g_DepthScale	= 0.1;//1.0 / 200.0;


// Constant Variables



// Static Functions




// Fragment Shader Input
in vec4		vWorldPos;
in vec2		vTexCoord;
in vec3		vNormal;
in vec4		vWorldViewProjPos;


// Fragment Shader Output
out vec4	outAccum;
out vec4	outRevealage;



// Fragment Shader
void main( void )
{
	const vec3 V	= normalize( g_ViewAttrib.Position - vWorldPos.xyz );	// どっち向きが正しい? サーフェス点から離れる方向? サーフェス点に近付く方向?
	const vec3 L	= normalize(g_LightDir(0));//normalize( vec3(1,1,1) );
	const vec3 H	= normalize( (L + V) * 0.5 );
	const vec3 N	= normalize( vNormal );
	
	vec4 color;
	//============================================= 反射率の計算 ============================================//
	color.rgb	= g_Diffuse;///*g_Specular * SimplifiedCookTorrance( g_Shininess, L, N, H ) +*/ AnisotropicPhong_Diffuse( L, V, N, g_Diffuse, vec3( 0.31831, 0.31831, 0.31831 ) );
	color.rgb	*= g_LightIntensity( 0 );

	
	//==================================== シャドウマップをサンプリングする =================================//

	float islit			= 0.0;
	float viewDepth		= vWorldViewProjPos.z;// / vWorldViewProjPos.w;
	vec3 colorWeight	= vec3(1.0, 1.0, 1.0);

	// 光源系座標空間で影生成範囲を判別
	//vec2 lsTexCoord		= 0.5 * ( vLightSpacePos.xy / vLightSpacePos.w ) + vec2(0.5);
	//float bLitArea	= float( lsTexCoord.x>=0.0 && lsTexCoord.y>=0.0 && lsTexCoord.x<=1.0 && lsTexCoord.y<=1.0 && vLightSpacePos.w>0.0 );

	int shadowMapId		= g_ShadowCasterIndex( 0 );
	int	numCascades	= g_NumShadows( 0 );
	color.rgb		*= float( GetShadowDistance( vWorldPos, numCascades, shadowMapId, viewDepth ) < 0.0 );
	color.a			= g_Transparency;

	// Insert your favorite weighting function here. The color-based factor
	// avoids color pollution from the edges of wispy clouds. The z-based
	// factor gives precedence to nearer surfaces.
	float depth	= abs(/*vWorldViewProjPos.z*/1.0 / vWorldViewProjPos.w) * g_DepthScale;
	float weight = clamp(0.03 / (1e-5 + pow( depth /* 200*/, 4.0)), 1e-2, 3e3 );// Equation (9)

 
	// Blend Func: GL_ONE, GL_ONE
	// Switch to premultiplied alpha and weight
	outAccum = vec4( color.xyz * color.w, color.w ) * weight;
 
	// Blend Func: GL_ZERO, GL_ONE_MINUS_SRC_ALPHA
	outRevealage = vec4( color.w );


}

#endif
