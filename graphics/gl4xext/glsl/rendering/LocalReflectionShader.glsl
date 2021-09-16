// Uniform Locations
uniform sampler2D	g_TexPosition;	// position texture
uniform sampler2D	g_TexDepth;		// depth texture
uniform sampler2D	g_TexNormal;	// normal texture
uniform sampler2D	g_TexColor;		// color texture



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


// Uniform Locations
#pragma include "Common.glsl"


// Fragment Shader Input
in vec2		vTexCoord;


// Fragment Shader Output
out vec4	outColor;



// Fragment Shader
void main (void)
{
	vec3 wsPos		= texture2D( g_TexPosition, vTexCoord.xy ).xyz;
	vec3 wsNormal	= texture2D( g_TexNormal, vTexCoord.xy ).xyz ;

	if( length(wsNormal)==0.0 )	discard;


	//=================== 視線ベクトルに対する反射ベクトルの計算 =====================//
	vec3 ws_eye_to_surface	= normalize( wsPos - g_ViewAttrib.Position );
	vec3 ws_refl			= reflect( ws_eye_to_surface, wsNormal );

	// test output
	//outColor.xyz	= ws_refl.xyz;
	//return;
	
	
	//================== レイマーチングでデプスバッファを探索 ======================//
	const float delta = 1.25;
	const vec3 wsStartPos	= wsPos + 0.5 * wsNormal;
	const int numSteps = 256;
	const float depthThreshold	= 0.03;
	
	
	float blocked	= 0.0;
	vec3 blockedColor = vec3( 0.2, 0.2, 0.2 );

	// レイマーチングのサンプル点座標(ワールド空間)を計算する
	vec3 wsSamplePos	= wsStartPos;

	for( int i=0; i<numSteps; ++i )
	{
		// サンプル点座標を視点系に変換する
		vec4 hsSamplePos	= vec4( wsSamplePos, 1.0 ) * g_ViewTransform.MatViewProjection;

		// hsSamplePosからスクリーンスペースのテクスチャ座標に変換する
		vec2 depthSampleCoord	= 0.5 * ( hsSamplePos.xy / hsSamplePos.w ) + vec2(0.5);

		if( depthSampleCoord.x<0.0 || depthSampleCoord.x>=1.0 || depthSampleCoord.y<0.0 || depthSampleCoord.y>=1.0 )	break;

		// デプス値を比較する
		float samplePosDepth	= distance( g_ViewAttrib.Position, wsSamplePos );
		float texDepth			= texture2D( g_TexDepth, depthSampleCoord ).x;
		float depthDiff			= abs( samplePosDepth - texDepth );

		
		blocked	= float( (depthDiff<depthThreshold) && (texDepth>0.0) );
		
		if( blocked>0.0 )
		{
			blockedColor = texture2D( g_TexColor, depthSampleCoord ).xyz;
			break;
		}

		float ddelta = max( delta/samplePosDepth, 0.05 );
		wsSamplePos += ddelta * ws_refl;

	}// end of i loop
	

	vec4 color = texture2D( g_TexColor, vTexCoord.xy ).xyzw;
	outColor = color;

	if( blocked > 0.0 )
		outColor.xyz = blockedColor * 0.75 + outColor.xyz * 0.25 ;


}

#endif
