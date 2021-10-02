#ifndef	SHADOWMAP_GLSL
#define	SHADOWMAP_GLSL



#define MAX_SHADOW_CASCADES	16
#define	MAX_SHADOW_MAPS		1024

#ifndef FLT_MAX
#define	FLT_MAX	3.402823466e+38F
#endif // !FLT_MAX




// Shadowmap Array
uniform sampler2DArray	g_ShadowMapArray;


// Shadow Matrices
uniform ubMatShadows
{
	mat4	g_MatShadows[MAX_SHADOW_MAPS];
};


// SplitDepths
uniform ubSplitDepths
{
	float	g_SplitDespths[MAX_SHADOW_MAPS];
};


// ShadowIndex linked list
uniform ubShadowIDLinkedList
{
	int		g_ShadowIDLinkedList[MAX_SHADOW_MAPS];
};


// ShadowMapSize
const vec2	g_ShadowMapResolution	= vec2( textureSize( g_ShadowMapArray, 0 ) );
const vec2	g_ShadowMapTexelSize	= 1.0 / g_ShadowMapResolution;



const float g_Bias[] =
{
	0.15,
	0.15,//0.0015,
};



#define g_MatShadow( shadow_id )	g_MatShadows[ shadow_id ];
#define	g_SplitDepth( shadow_id )	g_SplitDespths[ shadow_id ];
#define	g_NextShadowID( shadow_id )	g_ShadowIDLinkedList[ shadow_id ];

#define g_ShadowMapDepth( texcoord, layer )	texture( g_ShadowMapArray, vec3( texcoord.xy, float(layer) ) ).r
#define	g_ShadowMapDepthGather( texcoord, layer )	textureGather( g_ShadowMapArray, vec3( (texcoord).xy, float(layer) ) )

#define IsInsideTexture( texcoord )	bool( texcoord.x>=0.0 && texcoord.y>=0.0 && texcoord.x<1.0 && texcoord.y<1.0 )




// 遮蔽物までの距離を返す. ゼロ未満の値なら遮蔽物なし
float GetShadowDistance( in vec4 worldPos, in int numShadows, in int shadow_id, in float camera_depth )
{
	float shadowDistance		= FLT_MAX;
	int layer					= shadow_id;
	
	for( int i=0; i<numShadows; ++i )
	{
		// フラグメントのワールド座標を、シャドウマップ空間の座標値に変換する
		mat4 matShadow			= g_MatShadows[ layer ];
		const float	bOrthoProj	= float( matShadow[3][3] == 1.0 );
		const float bias		= g_Bias[int( bOrthoProj )];
		vec4 ssPos				= worldPos * matShadow;
		ssPos.z					*= mix( 1.0, abs( 2.0 / matShadow[2][2] ), bOrthoProj );
		vec2 ssTexCoord			= ssPos.xy / ssPos.w * 0.5 + vec2( 0.5 );
		
		float shadowMapDepth	= g_ShadowMapDepth( ssTexCoord, layer );
		bool inRange			= camera_depth < g_SplitDepth( layer );

		if( inRange && IsInsideTexture( ssTexCoord ) && ssPos.w>0.0 )
		{
			//bool shadowd = ssPos.z - 0.0015 > shadowMapDepth;
			shadowDistance = ssPos.z - shadowMapDepth - bias;
			return shadowDistance;
		}

		layer	= g_NextShadowID( layer );

	}// end of i loop

	return shadowDistance;
}

//
//
//float isShadowed( int shadowmap_id, int numCascades, vec4 worldPos, float cameraDepth ) //, float l_dot_n )
//{
//	float islit	= 1.0;
//
//	for( int i=0; i<numCascades; ++i )
//	{
//		mat4 matShadow	= g_MatShadow( shadowmap_id );
//		vec4 ssPos		= worldPos * matShadow;
//		ssPos.z			*= mix( 1.0, abs( 2.0 / matShadow[2][2] ), float( matShadow[3][3] == 1.0 ) );
//
//		vec2 texCoordShadow	= 0.5 * (ssPos.xy / ssPos.w) + vec2( 0.5 );
//		bool bInsideCascade	= texCoordShadow.x >= 0.0 && texCoordShadow.y >= 0.0 && texCoordShadow.x <= 1.0 && texCoordShadow.y <= 1.0 && ssPos.w>0.0;
//
//		// 最初に見つかったカスケードレベルのシャドウマップを使う
//		if( cameraDepth < g_SplitDepth( shadowmap_id ) && bInsideCascade )
//		{
//			//float bias = 0.0015 * tan( acos( l_dot_n ) ); // cosTheta is dot( n,l ), clamped between 0 and 1
//			//bias = clamp(bias, 0.0, 1.0 );
//			//bias = max( bias, vCascadeSpacePos[i].z * 0.02 );
//
//			float depth_Threshold	= ssPos.z - 0.15;//bias; //* 0.98;// - bias;// 影に0.98かけるとアクネが落ち着く
//			float depth_ShadowMap;
//
//			depth_ShadowMap	= texture( g_ShadowMapArray, vec3( texCoordShadow, float( shadowmap_id ) ) ).r;
//			depth_ShadowMap = mix( depth_ShadowMap, DISTANCE_FARTHEST, float( depth_ShadowMap == 0.0 ) );
//
//			islit	= (depth_ShadowMap < depth_Threshold ? 0.0 : 1.0);
//
//			break;
//		}
//
//		shadowmap_id	= g_NextShadowID( shadowmap_id );
//	}// end of i loop
//
//	return islit;
//}



#endif
