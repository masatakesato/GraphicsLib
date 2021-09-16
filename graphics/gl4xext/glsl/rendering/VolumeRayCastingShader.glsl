




// Inclide Files
#pragma include "Common.glsl"
#pragma include "BRDF_Common.glsl"
#pragma include "LightAttribute.glsl"
#pragma include "ShadowMap.glsl"



//#################### Vertex Shader ##################//
#ifdef _VERTEX_SHADER_


// 


// Vertex Shader Input
in vec4	POSITION;
//in vec2	TEXCOORD0;
//in vec3	NORMAL;


// Vertex Shader Output
out Vertex_Out
{
	vec4	wsPosition;
	vec4	ssPosition;	// スクリーン空間座標
}OUT;



// Vertex Shader Main Function
void main()
{
	//gl_Position		= POSITION * g_MatWorld * g_ViewTransform.MatViewProjection;
	//OUT.ssPosition	= gl_Position;

	OUT.wsPosition	= POSITION * g_MatWorld;
	OUT.ssPosition	= OUT.wsPosition * g_ViewTransform.MatViewProjection;

	gl_Position		= OUT.ssPosition;
}


#endif




//#################### Fragment Shader ##################//
#ifdef _FRAGMENT_SHADER_



// Uniform Locations
uniform mat4	g_MatVoxelSpace;
//uniform mat4 Modelview;
//uniform float FocalLength;
//uniform vec2 WindowSize;
//uniform vec3 RayOrigin;
uniform int		g_numEyeRaySamples	= 128;



// Constant Variables
//const float	g_FocalLength	= 1.0 / tan( g_ViewAttrib.Fov / 2.0 );
const vec3	g_vsRayOrigin	= ( vec4(g_ViewAttrib.Position, 1.0) * g_MatVoxelSpace ).xyz;
const float g_MaxDist		= sqrt(12.0);
const float g_StepSize		= g_MaxDist / float( g_numEyeRaySamples );

//const vec2 WindowSize	= vec2(1280,720);



// Fragment Shader Input
in Vertex_Out
{
	vec4	wsPosition;
	vec4	ssPosition;	// スクリーン空間座標
}IN;


// Fragment Shader Output
out vec4 outColor;



struct Ray
{
	vec3 Origin;
	vec3 Dir;
};

struct AABB
{
	vec3 Min;
	vec3 Max;
};


// Static Functions
bool IntersectBox( Ray r, AABB aabb, out float t0, out float t1 )
{
	vec3 invR = 1.0 / r.Dir;
	vec3 tbot = invR * ( aabb.Min-r.Origin );
	vec3 ttop = invR * ( aabb.Max-r.Origin );
	vec3 tmin = min( ttop, tbot );
	vec3 tmax = max( ttop, tbot );
	vec2 t = max( tmin.xx, tmin.yz );
	t0 = max( t.x, t.y );
	t = min( tmax.xx, tmax.yz );
	t1 = min( t.x, t.y );
	return t0 <= t1;
}



// Fragment Shader Main Function
void main()
{
	outColor	= vec4(0);

	vec3 vsRayDir;
	//rayDirection.xy = IN.ssPosition.xy;//2.0 * gl_FragCoord.xy / WindowSize - 1.0;// gl_FragCoordがテクセルの座標. WindowSizeで割って[0,1]に正規化後、[-1,+1]にする
	//rayDirection.z	= -g_FocalLength;
	//rayDirection	= ( vec4( rayDirection, 0 ) * g_MatVoxelSpace ).xyz;// スクリーン座標 -> ワールド空間座標 -> 正規化ボクセル空間座標

	vsRayDir	= ( vec4( IN.wsPosition.xyz - g_ViewAttrib.Position, 0 ) * g_MatVoxelSpace ).xyz;

	Ray eye = Ray( g_vsRayOrigin, normalize( vsRayDir ) );
	AABB aabb = AABB( vec3( -1.0 ), vec3( +1.0 ) );// 正規化ボクセル空間[-1,+1]上のボクセル空間バウンディングボックス


	float tnear, tfar;
	IntersectBox( eye, aabb, tnear, tfar );
	if( tnear < 0.0 ) tnear = 0.0;

	vec3 rayStart = eye.Origin + eye.Dir * tnear;
	vec3 rayStop = eye.Origin + eye.Dir * tfar;

	// Transform from object space to texture coordinate space:
	rayStart = 0.5 * ( rayStart + 1.0 );
	rayStop = 0.5 * ( rayStop + 1.0 );

	// Perform the ray marching:
	vec3 pos = rayStart;
	vec3 step = normalize( rayStop-rayStart ) * g_StepSize;
	float travel = distance( rayStop, rayStart );
	for( int i=0; i < g_numEyeRaySamples && travel > 0.0; ++i, pos += step, travel -= g_StepSize )
	{
		outColor.xyz += vec3(1.0/float(g_numEyeRaySamples));
		// ...lighting and absorption stuff here...

	}

	//outColor.xyz	= vec3( tfar - tnear/*g_vsRayOrigin.xyz*/) / 3.46410161514;
	//outColor.w= 1.0;
	

}

#endif
