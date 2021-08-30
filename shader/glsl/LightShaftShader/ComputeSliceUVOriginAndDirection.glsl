

// EpipolarSliceの「向き」と「原点」を、シャドウマップUV空間上に投影する


// Include Files
#pragma include "Common.glsl"
#pragma include "LightShaftShader/LightShaftShaderCommon.glsl"
#pragma include "LightAttribute.glsl"
#pragma include "ShadowMap.glsl"


// Helper Functions

// Transform Screen Space Position[-1,1] to World Space
vec4 UnProjectFromScreenSpaceToWorldSpace( in vec3 ssPos, in mat4 matInvViewProj )
{
	vec4 ssInvPos = vec4(0);

	ssInvPos = vec4( ssPos, 1.0 ) * matInvViewProj;
	ssInvPos.xyz /= ssInvPos.w;

	return ssInvPos;
}




// Constant Variables
const vec4 g_IncorrectSliceUVDirAndStart = vec4( 0, 0, 0, 0 );


// Uniform Locations


// Random Accessible Buffers
layout( binding = TEX_UNIT_SLICE_END_POINTS, rgba32f )	uniform readonly image1D	g_TexSliceEndPoints;
//layout( binding = TEX_UNIT_EPIPOLAR_COORDINATE, rg32f )	uniform readonly image2D	g_TexEpipolarCoordinate;

layout( binding = 3, rgba32f )							uniform writeonly image2D	g_TexSliceUvOriginDirection;// xy: shadowmap space slice direction(per texel length), zw: shadowmap space slice origin



// Workgroup Layout
layout( local_size_x=16, local_size_y=16, local_size_z=1 ) in;



// Main Kernel
void main()
{
	int shadowCasterIndex	= g_ShadowCasterIndex( g_EpipolarLightParam.LightID );// TODO: ワークグループのy次元毎に、カスケード毎のシャドウマップIDを割り当てる
	int numCascades			= g_NumShadows( g_EpipolarLightParam.LightID );


	//========================= 無効なスレッドは除外する ================================//
	ivec2 storePos	= ivec2( gl_GlobalInvocationID.xy );
	if( storePos.x >= g_numEpipolarSlices || storePos.y >= numCascades )
		return;

	
	//================ 画面の外側にはみ出たスライスのスレッドも除外する =================//
	vec4 sliceEndPoints	= imageLoad( g_TexSliceEndPoints, storePos.x ).xyzw;

	if( any( greaterThan( abs(sliceEndPoints), vec4(1.0+1e-4) ) ) )
	{
		imageStore( g_TexSliceUvOriginDirection, storePos, g_IncorrectSliceUVDirAndStart );
		return;
	}

	
	//====== ExitPointをシャドウマップ空間に写像して、スライス毎のベクトルを計算する =====//
	// ExitPointをスクリーン空間からワールド空間に変化する
	vec4 wsExitPoint	= UnProjectFromScreenSpaceToWorldSpace( vec3( sliceEndPoints.zw, 1.0 ), g_ViewTransform.MatInvViewProjection );
	
	// ワールド空間に変換したExitPintをLightSpaceに変換する
	vec4 lsExitPoint	= wsExitPoint * g_MatShadow( shadowCasterIndex );
	
	// LightSpaceのExitPointをシャドウマップのテクスチャ座標に変換する
	vec2 luvExitPoint	= lsExitPoint.xy / lsExitPoint.w * 0.5 +vec2(0.5);
	
	
	//=================== カメラ位置をシャドウマップ空間に写像する =====================//
	vec4 lsSliceOrigin	= vec4( g_ViewAttrib.Position, 1.0 ) * g_MatShadow( shadowCasterIndex );
	vec2 luvSliceOrigin	= 0.5 * ( lsSliceOrigin.xy / lsSliceOrigin.w ) + vec2(0.5);

	
	//========================= スライスのベクトルを計算する ==========================//
	vec2 luvSliceDir	= luvExitPoint - luvSliceOrigin;
	luvSliceDir			/= max( abs(luvSliceDir.x), abs(luvSliceDir.y) );// テクセルを探索するので、取りこぼしがないよう単位ベクトルの長さを十分に細かく設定する

	
	//========================== スライスの始点を修正する ==============================//
	// シャドウマップテクセル境界
	vec4 uvBoundaryMinMax	= vec4( 0.0, 0.0, 1.0, 1.0 ) + g_ShadowMapTexelSize.xyxy * vec4( 0.5, 0.5, -0.5, 0.5 ); 
	
	
	// テクセル境界の外側にあるluvSliceOriginを検出し、位置を境界線上まで移動する
	if( any( lessThan( (luvSliceOrigin.xyxy - uvBoundaryMinMax) * vec4(1.0,1.0,-1.0,-1.0), vec4(0) ) ) )
	{
		// uv境界に対するスライス始点の相対位置を計算する
		bvec4 b4IsValidIntersection	= greaterThan( abs(luvSliceDir.xyxy), vec4(1.0e-6) );
		vec4 f4DistToBoundaries		= ( uvBoundaryMinMax - luvSliceOrigin.xyxy ) / ( luvSliceDir.xyxy + vec4(!b4IsValidIntersection) );// !b4ValidIntersectionFlagで、ゼロ除算(luvSliceDirがu軸/v軸いずれかに平行な場合)を回避する

		// レイ進行方向の衝突判定があるかどうか判別する
		b4IsValidIntersection		= b4IsValidIntersection && greaterThan( f4DistToBoundaries, vec4(0.0) );

		// Compute the second intersection coordinate
		vec4 f4IsecYXYX				= luvSliceOrigin.yxyx + f4DistToBoundaries * luvSliceDir.yxyx;
		
		// Select only these coordinates that fall onto the boundary
        b4IsValidIntersection		= b4IsValidIntersection && greaterThanEqual(f4IsecYXYX, uvBoundaryMinMax.yxyx) && lessThanEqual(f4IsecYXYX, uvBoundaryMinMax.wzwz);

        // Replace distances to all incorrect boundaries with the large value
        f4DistToBoundaries			= vec4(b4IsValidIntersection) * f4DistToBoundaries + 
									  vec4(!b4IsValidIntersection) * vec4( +FLT_MAX, +FLT_MAX, +FLT_MAX, +FLT_MAX );
		
		// 一番近い衝突点を選択する
		float minDistance			= min( min( min( f4DistToBoundaries.x, f4DistToBoundaries.y), f4DistToBoundaries.z), f4DistToBoundaries.w );

		// スライス原点の位置を更新する
		luvSliceOrigin				= luvSliceOrigin + minDistance * luvSliceDir;
		
	}
	

	luvSliceDir *= g_ShadowMapTexelSize;
	
	imageStore( g_TexSliceUvOriginDirection, storePos, vec4( luvSliceDir, luvSliceOrigin ) );
}
