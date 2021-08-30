
// Uniform Varianbles




#ifdef _VERTEX_SHADER_

// Shader Input
in vec3			POSITION;
in vec2			TEXCOORD;

// Uniform location
uniform mat4	g_MatTransform;


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

// Include files
#pragma include"../shader/glsl/ColorBlendMacros.glsl"


// Uniform Varianbles
uniform sampler2D	g_Texture0;
uniform sampler2D	g_Texture1;
uniform sampler2D	g_Texture2;
uniform sampler2D	g_Texture3;
uniform sampler2D	g_Texture4;
uniform sampler2D	g_Texture5;
uniform sampler2D	g_Texture6;
uniform sampler2D	g_Texture7;
uniform sampler2D	g_Texture8;
uniform sampler2D	g_Texture9;
uniform sampler2D	g_Texture10;
uniform sampler2D	g_Texture11;
uniform sampler2D	g_Texture12;
uniform sampler2D	g_Texture13;
uniform sampler2D	g_Texture14;
uniform sampler2D	g_Texture15;


//uniform float	g_BlendWeight[8];
uniform float	g_Opacity[8];
uniform vec3	g_Diffuse0[8];
uniform vec3	g_Diffuse1[8];



// Fragment Shader Input
in vec2		vTexCoord;

// Fragment Shader Output
out vec4	outColor;


// Fragment Shader
//void main (void)
//{
//	// WorleyNoise(複数解像度)
//	vec4 worley0	= texture( g_Texture0, vTexCoord ).rgba;
//	vec4 worley1	= texture( g_Texture1, vTexCoord ).rgba;
//	vec4 worley2	= texture( g_Texture2, vTexCoord ).rgba;
//	// SimplexNoise(複数解像度)
//	vec4 simplex0	= texture( g_Texture3, vTexCoord ).rgba;
//	vec4 simplex1	= texture( g_Texture4, vTexCoord ).rgba;
//	vec4 simplex2	= texture( g_Texture5, vTexCoord ).rgba;
//
//	//################################## スケールで重み付け加算テスト 2013.12.12 ###############################//
//	//outColor = ColorBlend_Overlay(worley0, simplex0)*0.6 + ColorBlend_Overlay(worley1,simplex1)*0.3 + ColorBlend_Overlay(worley2,simplex2 )*0.1;
//	//#########################################################################################################//
//
//	//################################## スケールで重み付け加算テスト 2013.12.13 ###############################//
//	//outColor = ColorBlend_Multiply(simplex0, worley0)*0.6 + ColorBlend_Multiply(simplex1,worley1)*0.3 + ColorBlend_Multiply(simplex2,worley2)*0.1;
//	//##########################################################################################################//
//
//
//	//################################## スケールで重み付け加算テスト 2013.12.16 ###############################//
//	//outColor	= ( worley0*0.6 + (worley1+simplex1)*0.2 + simplex2*0.2 );
//	//##########################################################################################################//
//
//
//
//
//
//	//outColor = ( color0 + color1 + color2 + color3 + color4 + color5 ) * 0.166666;//ColorBlend_Lighten( color1, color2 );
//	
//	//texture( g_Texture0, vTexCoord ).rgba - texture( g_Texture0, vTexCoord ).rgba;//vec4( vTexCoord, 0.0, 1.0 );
//}


//################################## スケールで重み付け加算テスト 2013.12.16 ###############################//
void main( void )
{
	vec4 noise0	= texture( g_Texture0, vTexCoord ).rgba;
	vec4 noise1	= texture( g_Texture1, vTexCoord ).rgba;
	vec4 noise2	= texture( g_Texture2, vTexCoord ).rgba;
	vec4 noise3	= texture( g_Texture3, vTexCoord ).rgba;
	vec4 noise4	= texture( g_Texture4, vTexCoord ).rgba;
	vec4 noise5	= texture( g_Texture5, vTexCoord ).rgba;


	if( length(noise0)==0.0 )	noise0 = vec4(1,1,1,1);
	if( length(noise1)==0.0 )	noise1 = vec4(1,1,1,1);


	vec4 blend_result00	= noise0;
	vec4 blend_result01 = ( ColorBlend_Overlay( noise1, noise0 )*(1.0-g_Opacity[0]) + noise1*g_Opacity[0] );
	vec4 blend_result12 = ( ColorBlend_Overlay( noise2, noise1 )*(1.0-g_Opacity[1]) + noise2*g_Opacity[1] );




	// ノイズレベル毎に、明暗に応じた色を渡す。
	vec4 noise0_color	= vec4( noise0.r * g_Diffuse1[0] + (1.0-noise0.r) * g_Diffuse0[0], 1.0 );
	vec4 noise1_color	= vec4( noise1.r * g_Diffuse1[1] + (1.0-noise1.r) * g_Diffuse0[1], 1.0 );
	vec4 noise2_color	= vec4( noise2.r * g_Diffuse1[2] + (1.0-noise2.r) * g_Diffuse0[2], 1.0 );

	blend_result01	= ( ColorBlend_Overlay( noise1_color, noise0_color )*(1.0-g_Opacity[0]) + noise1_color*g_Opacity[0] );
	blend_result12	= ( ColorBlend_Overlay( noise2_color, noise1_color )*(1.0-g_Opacity[1]) + noise2_color*g_Opacity[1] );


	outColor	= blend_result01*(1.0-g_Opacity[2]) + blend_result12 * g_Opacity[2];//( ColorBlend_Overlay( (blend_result01 + blend_result12)*0.5, noise0 ) + ((blend_result01 + blend_result12)*0.5) ) * 0.5;

	
	// オーバーレイで重ねるのはポイントか、、、
	// 低解像度は、高解像度の局所特徴を包含する
	// 現状一番まともに動く
}
//##########################################################################################################//


// 背景はデフォルトは真っ白にしておく


#endif
