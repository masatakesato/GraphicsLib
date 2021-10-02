

// Include Files
#pragma include "LightShaftShader/LightShaftShaderCommon.glsl"


// Uniform Buffers
uniform ubEndPoints
{
	vec2 g_EndPoints[2048];
};


// Uniform Locations


// Random Accessible Buffers
layout( binding = 0 )	uniform writeonly image1D	g_TexSliceEndPoints;


// Workgroup Layout
layout( local_size_x=16, local_size_y=1 ) in;// ワークグループあたり16スレッド. 1次元インデックス



#define INSIDE_SCREEN(p)	( p.x>=0.0 && p.x<1.0 && p.y>=0.0 && p.y<1.0 )




//             <------
//   +1   0,1___________0.75
//          |     3     |
//        | |           | A
//        | |0         2| |
//        V |           | |
//   -1     |_____1_____|
//       0.25  ------>  0.5
//
//         -1          +1
//
//vec2 GetEpipolarLineExitPoint( in float fEpipolarSlice )
//{
//	uint32 uiBoundary			= uint32( clamp( floor( fEpipolarSlice * 4.0 ), 0.0, 3.0 ) );
//	float fPosOnBoundary	= fract( fEpipolarSlice * 4.0 );
//	float fBoundaryCoord	= -1.0 + 2.0 * fPosOnBoundary;
//	//                                   Left             Bttom           Right              Top   
//	vec4 f4BoundaryXCoord	= vec4(     -1,		  fBoundaryCoord,       1,			-fBoundaryCoord );
//	vec4 f4BoundaryYCoord	= vec4( -fBoundaryCoord,      -1,		    fBoundaryCoord,       1 );
//	bvec4 b4BoundaryFlags	= equal( uiBoundary.xxxx, uvec4(0,1,2,3) );	//bool4( uiBoundary.xxxx == uint4(0,1,2,3) );
//	// Select the right coordinates for the boundary
//	vec2 f2ExitPoint		= vec2( dot( f4BoundaryXCoord, vec4(b4BoundaryFlags) ), dot( f4BoundaryYCoord, vec4(b4BoundaryFlags) ) );
//
//	return f2ExitPoint;
//}



vec2 GetEpipolarLineEntryPoint( in vec2 f2ExitPoint, in vec2 ssLightPos )
{
	if( INSIDE_SCREEN(ssLightPos) )	// 光源がスクリーン内に写っている場合はそのまま出力
	{
		return ssLightPos;
	}
	else							// スクリーンの外側にはみ出ている場合は交点計算
	{
		// 光源からスクリーン端点までのレイを計算する:
		vec2 f2RayDir				= f2ExitPoint.xy - ssLightPos;
		float fDistToExitBoundary	= length(f2RayDir);
		f2RayDir					/= fDistToExitBoundary;

		// Compute signed distances along the ray from the light position to all four boundaries
		bvec4 b4IsCorrectIntersectionFlag	= greaterThan( abs(f2RayDir.xyxy), vec4(1e-5) );	//bvec4( abs(f2RayDir.xyxy) > vec4(1e-5) );/*
		vec4 f4DistToBoundaries				= ( vec4(-1,-1,1,1) - ssLightPos.xyxy ) / ( f2RayDir.xyxy + vec4(!b4IsCorrectIntersectionFlag) );
		// Addition of !b4IsCorrectIntersectionFlag is required to prevent divison by zero
		// Note that such incorrect lanes will be masked out

		// We now need to find first intersection BEFORE the intersection with the exit boundary
		// This means that we need to find maximum intersection distance which is less than fDistToBoundary
		// We thus need to skip all boundaries, distance to which is greater than the distance to exit boundary
		// Using -FLT_MAX as the distance to these boundaries will result in skipping them:
		b4IsCorrectIntersectionFlag			= b4IsCorrectIntersectionFlag && lessThan( f4DistToBoundaries, vec4(fDistToExitBoundary - 1e-4) );	//( f4DistToBoundaries < (fDistToExitBoundary - 1e-4) );
		f4DistToBoundaries					= vec4( b4IsCorrectIntersectionFlag ) * f4DistToBoundaries + 
			vec4( !b4IsCorrectIntersectionFlag ) * vec4(-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);

		float fFirstIntersecDist	= 0.0;
		fFirstIntersecDist			= max( fFirstIntersecDist, f4DistToBoundaries.x );
		fFirstIntersecDist			= max( fFirstIntersecDist, f4DistToBoundaries.y );
		fFirstIntersecDist			= max( fFirstIntersecDist, f4DistToBoundaries.z );
		fFirstIntersecDist			= max( fFirstIntersecDist, f4DistToBoundaries.w );
		// Now we can compute entry point:
		vec2 f2EntryPoint			= ssLightPos + f2RayDir * fFirstIntersecDist;

		return f2EntryPoint;
	
	}
}




void main()
{	
	int storePos	= int( gl_GlobalInvocationID.x );// データ格納先のアドレス
	if( storePos >= g_numEpipolarSlices )	return;
	
	//======================= 画面フレーム上の終点を取得する =======================//
	vec2 exitPoint	= g_EndPoints[ storePos ];


	//============ スライス線分が画面フレーム内を通過するかチェックする ============//
	// スクリーン上の光源が、上下左右どのフレームからはみ出ているかをチェックする(+x,-x,+y,-yの順序)
	vec4 bOutsideLightPos	= vec4( float(g_ScreenLightPos.x>=1.0), float(g_ScreenLightPos.x<=-1.0), float(g_ScreenLightPos.y>=1.0), float(g_ScreenLightPos.y<=-1.0) );
	// EndPointが、上下左右どのフレームからはみ出ているかをチェックする(+x,-x,+y,-yの順序)
	vec4 bOutsideExitPoint	= vec4( float(exitPoint.x>=1.0), float(exitPoint.x<=-1.0), float(exitPoint.y>=1.0), float(exitPoint.y<=-1.0) );
	//
	if( dot( bOutsideLightPos, bOutsideExitPoint ) > 0.0 )
	{
		imageStore( g_TexSliceEndPoints, storePos, vec4(-10000.0) );
		return;
	}


	//====================== フレーム内への進入点座標を計算する ====================//
	vec2 entryPoint	= GetEpipolarLineEntryPoint( exitPoint, g_ScreenLightPos );


	//====== スライスの長さが短すぎる（サンプル数が密すぎる）場合は引き伸ばし ======//
	float epipolarSliceScreenLen	= length( (exitPoint - entryPoint) * g_ScreenResolution/2.0 );
	exitPoint	= entryPoint + ( exitPoint - entryPoint ) * max( float(g_numSamplesInSlice) / epipolarSliceScreenLen, 1.0 );
	
	imageStore( g_TexSliceEndPoints, storePos, vec4( entryPoint, exitPoint ) );
}
