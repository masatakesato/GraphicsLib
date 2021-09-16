#ifndef	BRDF_COMMON_GLSL
#define	BRDF_COMMON_GLSL



// Specular Component of Ashikhmin Shirley BRDF
vec3 AnisotropicPhong_Specular()
{
	vec3 result;


	return result;
}


// Diffuse Component of Ashikhmin Shirley BRDF. expression(5) in the paper
vec3 AnisotropicPhong_Diffuse( vec3 L, vec3 V, vec3 n, vec3 Rd, vec3 diffuseWight )
{
	const float dot_nl = clamp( dot(n,L), 0.0, 1.0 );
	const float dot_nv = clamp( dot(n,V), 0.0, 1.0 );// 論文とVの向き逆にするとうまくいく、なんで？？

	vec3 result	=	3.82454757828 * Rd * diffuseWight *
					( 1.0 - pow( 1.0 - 0.5 * dot_nl, 5.0 ) ) * 
					( 1.0 - pow( 1.0 - 0.5 * dot_nv, 5.0 ) );
	return result;
}


// G-buffer(LightBuffer)に書き込む値: BRDFの種類によって異なる

//	例えばAshikhmin Shirley BRDFの場合は、、、
//	N*L, N*V, H*LもしくはH*V, の3つが必要

//	cook-torranceの場合は、、、、、
//	N*H, N*V, H*LもしくはH*V, N*L,  の4つが必要


float Lambert( vec3 N, vec3 L )
{
	return max(dot(N,L),0.0);
}



float SimplifiedCookTorrance( float e, vec3 L, vec3 N, vec3 H )
{
	float	LdotH = max( dot(L,H), 1.0e-3 );
	float	NdotH = max( dot(N,H), 0.0 );
	float	NdotL = max( dot(N,L), 0.0 );
	
	return max( (1.0+e)/(8.0* pow(LdotH,3.0)) * pow(NdotH,e) * NdotL, 0.0 );
}



//vec3 ext_color = vec3(0.4, 0.55, 0.5);//vec3(0.0045, 0.75, 0.3);
vec3 VolumetricExtinction( float depth, vec3 ext_color )
{
	vec3 weight = clamp( depth/ext_color, vec3(0.0), vec3(1.0) );
	
	return vec3(1.0) - weight;
}















#endif	// BRDF_COMMON_GLSL //