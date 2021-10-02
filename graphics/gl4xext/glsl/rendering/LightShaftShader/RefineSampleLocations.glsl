// Include Files
#pragma include "LightShaftShader/LightShaftShaderCommon.glsl"


// Defines

// 可変
#ifndef INITIAL_SAMPLE_STEP
#define INITIAL_SAMPLE_STEP		128
#endif


#ifndef	MAX_SAMPLES_IN_SLICE
#define	MAX_SAMPLES_IN_SLICE	512
#endif


#ifndef THREAD_GROUP_SIZE

#if INITIAL_SAMPLE_STEP>32
#define THREAD_GROUP_SIZE	INITIAL_SAMPLE_STEP

#else
#define THREAD_GROUP_SIZE	32

#endif

#endif


// Constant Variables
const uint		g_uiNumPackedFlags	= THREAD_GROUP_SIZE / 32;


// Shared Variables
shared uint		g_uiPackedCamSpaceDiffFlags[ g_uiNumPackedFlags ];// 自分とその隣とのデプスが不連続かどうかを保持する


// Uniform Locations



// Random Accessible Buffers
layout( binding = TEX_UNIT_EPIPOLAR_COORDINATE, rg32f )	uniform readonly image2D	g_TexEpipolarCoordinate;
layout( binding = TEX_UNIT_EPIPOLAR_CAM_SPACE_Z, r32f )	uniform readonly image2D	g_TexEpipolarCameraSpaceZ;

layout( binding = 0, rg16ui )							uniform writeonly uimage2D	destInterpolationSource;




// Workgroup Layout
layout( local_size_x=INITIAL_SAMPLE_STEP, local_size_y=1, local_size_z=1 ) in;



// Main Kernel
void main()
{	
	// ワークグループ1つがスライス1つ分の処理を担当する
	uint uiSliceInd				= gl_WorkGroupID.y;
	// スレッドグループ内最初のサンプル点のグローバルインデックスを計算する
	// 各グループはTHREAD_GROUOP_SIZE個のサンプル点を処理する
	uint uiGroupStartGlobalInd	= gl_WorkGroupID.x * THREAD_GROUP_SIZE;
	// グループ内のローカルインデックスを取得する
	uint uiSampleInd			= gl_LocalInvocationID.x;

	// サンプル点のグローバルインデックスを計算する. CoordinateTextureのピクセル参照に必要
	uint uiGlobalSampleInd		= uiGroupStartGlobalInd + uiSampleInd;

	// サンプル点のスクリーン座標を取得する
	vec2 f2SampleLocationPS		= imageLoad( g_TexEpipolarCoordinate, ivec2( uiGlobalSampleInd, uiSliceInd ) ).xy; //g_tex2DCoordinates.Load( uint3(uiGlobalSampleInd, uiSliceInd, 0) );

	bool bIsValidThread			= all( lessThan( abs(f2SampleLocationPS), vec2(1.0+1.0e-4) ) );


	// シェアードメモリをゼロで初期化する
	if( gl_LocalInvocationID.x < g_uiNumPackedFlags )
		g_uiPackedCamSpaceDiffFlags[ gl_LocalInvocationID.x ]	= 0;

	// シェアードメモリ全要素の更新が完了するまで全スレッド待機
	barrier();


	// ここから、スレッド毎に対象フラグメントの計算を開始する

	if( bIsValidThread )
	{
		// デプス値(サンプル位置とその右隣ピクセル)を取得する
		float fCameraSpaceZ				= imageLoad( g_TexEpipolarCameraSpaceZ, ivec2( uiGlobalSampleInd, uiSliceInd ) ).x;
		float fRightNeighbCameraSpaceZ	= imageLoad( g_TexEpipolarCameraSpaceZ, ivec2( uiGlobalSampleInd+1, uiSliceInd ) ).x;
		float fMaxZ						= max( max( fCameraSpaceZ, fRightNeighbCameraSpaceZ ), 1.0 );
		// 不連続かどうかを閾値と比較して判別する
		bool bFlag						= abs( fCameraSpaceZ - fRightNeighbCameraSpaceZ ) / fMaxZ < 0.2 * g_EpipolarParam.RefinementThreshold;

		// シェアードメモリにフラグを書き込む. 複数スレッドから書き込みがある.どれか一つでもbFlagがtrueなら,シェアードメモリにtrueを残す
		atomicOr( g_uiPackedCamSpaceDiffFlags[ uiSampleInd / 32 ], uint( bFlag << ( uiSampleInd % 32 )) );
	}


	// シェアードメモリ全要素の更新が完了するまで全スレッド待機
	barrier();


	// Skip invalid threads. This can be done only after the synchronization
    if( !bIsValidThread )
        return;

	//                                 uiInitialSampleStep
	//       uiSampleInd             |<--------->|
	//          |                    |           |
	//       X  *  *  *  X  *  *  *  X  *  *  *  X           X - 初期サンプルの位置
	//       |           |
	//       |           uiInitialSample1Ind
	//      uiInitialSample0Ind
	//

	// レイマーチングする近傍サンプル2点を探す
	uint uiInitialSampleStep	= INITIAL_SAMPLE_STEP;
	uint uiInitialSample0Ind	= ( uiSampleInd / uiInitialSampleStep ) * uiInitialSampleStep;

	// エピポーラー近傍はサンプリング密度を上げる
	uint uiInitialSample0GlobalInd	= uiInitialSample0Ind + uiGroupStartGlobalInd;
	vec2 f2InitialSample0Coords		= imageLoad( g_TexEpipolarCoordinate, ivec2( uiInitialSample0GlobalInd, uiSliceInd ) ).xy;

	if( float(uiInitialSample0GlobalInd) / float(MAX_SAMPLES_IN_SLICE ) < 0.05 && length( f2InitialSample0Coords - g_ScreenLightPos ) < 0.1 )
	{
		uiInitialSampleStep	= uint( max( INITIAL_SAMPLE_STEP / g_EpipolarParam.EpipoleSamplingDensityFactor, 1.0 ) );
		uiInitialSample0Ind	= ( uiSampleInd / uiInitialSampleStep ) * uiInitialSampleStep;
	}
	uint uiInitialSample1Ind	= uiInitialSample0Ind + uiInitialSampleStep;

	// Remeber that the last sample in each epipolar slice must be ray marching one
	uint uiInterpolationTexWidth, uiInterpolationTexHeight;
	uiInterpolationTexWidth		= g_EpipolarParam.NumSamplesInSlice;
	uiInterpolationTexHeight	= g_EpipolarParam.NumEpipolarSlices;
	if( gl_WorkGroupID.x == uiInterpolationTexWidth / THREAD_GROUP_SIZE - 1 )
		uiInitialSample1Ind = min( uiInitialSample1Ind, THREAD_GROUP_SIZE - 1 );

    uint uiLeftSrcSampleInd		= uiSampleInd;
    uint uiRightSrcSampleInd	= uiSampleInd;


	// サンプル点が初期サンプルのいずれでもない場合にのみ、探索
	if( uiSampleInd > uiInitialSample0Ind && uiSampleInd < uiInitialSample1Ind )
	{
		// スレッド毎のローカルメモリにシェアードメモリの内容をコピーする
		uint uiPackedCamSpaceDiffFlags[ g_uiNumPackedFlags ];
		for( uint i=0; i<g_uiNumPackedFlags; ++i )
			uiPackedCamSpaceDiffFlags[i] = g_uiPackedCamSpaceDiffFlags[i];
	

		// カメラ奥行きの不連続があるかどうか調べる
		bool bNoDepthBreaks	= true;

#if INITIAL_SAMPLE_STEP < 32
		{
			// Check if all uiInitialSampleStep flags starting from
			// position uiInitialSample0Ind are set:
			int iFlagPackOrder		= uiInitialSample0Ind / 32;
			int iFlagOrderInPack	= uiInitialSample0Ind % 32;
			uint uiFlagPack			= uiPackedCamSpaceDiffFlags[ iFlagPackOrder ];
			uint uiAllFlagsMask		= ( (1<<uiInitialSampleStep) - 1 );
			if( ((uiFlagPack >> iFlagOrderInPack) & uiAllFlagsMask) != uiAllFlagsMask )
				bNoDepthBreaks	= false;
		}
#else
		for( uint i=0; i<g_uiNumPackedFlags; ++i )
		{
			if( uiPackedCamSpaceDiffFlags[i] != 0xFFFFFFFFu )
				bNoDepthBreaks	= false;// If at least one flag is not set, there is a depth break on this section
		}
#endif

		if( bNoDepthBreaks == true )	// 奥行き不連続がない場合は、最初に計算したレイマーチング端点インデックスをそのまま使う
		{
			uiLeftSrcSampleInd	= uiInitialSample0Ind;
			uiRightSrcSampleInd	= uiInitialSample1Ind;
		}
		else						// 奥行き不連続ありの場合は、
		{
			// uiSampleIndから左側のサンプル領域で、補間に使うサンプルを探す
			{
				// Note that i-th flag reflects the difference between i-th and (i+1)-th samples:
				// Flag[i] = abs(fCamSpaceZ[i] - fCamSpaceZ[i+1]) < g_PPAttribs.m_fRefinementThreshold;
				// We need to find first depth break starting from iFirstDepthBreakToTheLeftInd sample
				// and going to the left up to uiInitialSample0Ind
				int iFirstDepthBreakToTheLeftInd	= int( uiSampleInd - 1 );
				//                                                              iFirstDepthBreakToTheLeftInd
				//                                                                     |
				//                                                                     V
				//      0  1  2  3                       30 31   32 33     ....   i-1  i  i+1 ....  63   64
				//   |                                         |                           1  1  1  1  |
				//          uiPackedCamSpaceDiffFlags[0]             uiPackedCamSpaceDiffFlags[1]
				//
				//   iFlagOrderInPack == i % 32

				int iFlagPackOrder		= int(iFirstDepthBreakToTheLeftInd) / 32;
				int iFlagOrderInPack	= int(iFirstDepthBreakToTheLeftInd) % 32;
				uint uiFlagPack			= uiPackedCamSpaceDiffFlags[ iFlagPackOrder ];
				// To test if there is a depth break in the current flag pack,
				// we must check all flags starting from the iFlagOrderInPack
				// downward to 0 position. We must skip all flags from iFlagOrderInPack+1 to 31
				if( iFlagOrderInPack < 31 )
				{
					// Set all higher flags to 1, so that they will be skipped
					// Note that if iFlagOrderInPack == 31, there are no flags to skip
					// Note also that (U << 32) != 0 as it can be expected. (U << 32) == U instead
					uiFlagPack |= ( uint(0x0FFFFFFFFU) << uint(iFlagOrderInPack + 1) );
				}
				
				// Find first zero flag starting from iFlagOrderInPack position. Since all
				// higher bits are set, they will be effectivelly skipped
				int iFirstUnsetFlagPos	= findMSB( uint(~uiFlagPack) );//firstbithigh( uint(~uiFlagPack) );
				// firstbithigh(0) == +INT_MAX
				if( !(0 <= iFirstUnsetFlagPos && iFirstUnsetFlagPos < 32) )
					// There are no set flags => proceed to the next uint flag pack
					iFirstUnsetFlagPos	= -1;
				iFirstDepthBreakToTheLeftInd	-= iFlagOrderInPack - iFirstUnsetFlagPos;
				
#if INITIAL_SAMPLE_STEP > 32
				// Check the remaining full flag packs
				iFlagPackOrder--;
				while( iFlagPackOrder >= 0 && iFirstUnsetFlagPos == -1 )
				{
					uiFlagPack	= uiPackedCamSpaceDiffFlags[ iFlagPackOrder ];
					iFirstUnsetFlagPos	= findMSB( uint(~uiFlagPack) );//firstbithigh( uint(~uiFlagPack) );
					if( !(0 <= iFirstUnsetFlagPos && iFirstUnsetFlagPos < 32) )
						iFirstUnsetFlagPos	= -1;
					iFirstDepthBreakToTheLeftInd	-= 31 - iFirstUnsetFlagPos;
					iFlagPackOrder--;
				}
#endif
				// Ray marching sample is located next to the identified depth break:
				uiLeftSrcSampleInd	= max( uint(iFirstDepthBreakToTheLeftInd + 1), uiInitialSample0Ind );
				
			}
			
			
			// uiSampleIndから右側のサンプル領域で、補間に使うサンプルを探す
			{
				// We need to find first depth break starting from iRightSrcSampleInd and
				// going to the right up to the uiInitialSample1Ind
				uiRightSrcSampleInd		= uiSampleInd;
				int iFlagPackOrder		= int( uiRightSrcSampleInd ) / 32;
				uint iFlagOrderInPack	= uiRightSrcSampleInd % 32;
				uint uiFlagPack			= uiPackedCamSpaceDiffFlags[ iFlagPackOrder ];
				// We need to find first unset flag in the current flag pack
				// starting from iFlagOrderInPack position and up to the 31st bit
				// Set all lower order bits to 1 so that they are skipped during
				// the test:
				if( iFlagOrderInPack > 0 )
					uiFlagPack |= ( (1 << uint(iFlagOrderInPack))-1 );
				// Find first zero flag:
				int iFirstUnsetFlagPos	= findLSB( uint(~uiFlagPack) );//firstbitlow( uint(~uiFlagPack) );
				if( !(0 <= iFirstUnsetFlagPos && iFirstUnsetFlagPos < 32) )
					iFirstUnsetFlagPos	= 32;
				uiRightSrcSampleInd	+= iFirstUnsetFlagPos - iFlagOrderInPack;

#if INITIAL_SAMPLE_STEP > 32
				// Check the remaining full flag packs
				iFlagPackOrder++;
				while( iFlagPackOrder < int(g_uiNumPackedFlags) && iFirstUnsetFlagPos == 32 )
				{
					uiFlagPack = uiPackedCamSpaceDiffFlags[iFlagPackOrder];
					iFirstUnsetFlagPos	= findLSB( uint(~uiFlagPack) );//firstbitlow( uint(~uiFlagPack) );
					if( !(0 <= iFirstUnsetFlagPos && iFirstUnsetFlagPos < 32) )
						iFirstUnsetFlagPos	= 32;
					uiRightSrcSampleInd	+= iFirstUnsetFlagPos;
					iFlagPackOrder++;
				}
#endif
				uiRightSrcSampleInd = min( uiRightSrcSampleInd, uiInitialSample1Ind );
			}
		}

		// If at least one interpolation source is the same as the sample itself, the
		// sample is ray marching sample and is interpolated from itself:
		if(uiLeftSrcSampleInd == uiSampleInd || uiRightSrcSampleInd == uiSampleInd )
			uiLeftSrcSampleInd = uiRightSrcSampleInd = uiSampleInd;
	}

	//g_rwtex2DInterpolationSource[ uint2(uiGlobalSampleInd, uiSliceInd) ] = uint2(uiGroupStartGlobalInd + uiLeftSrcSampleInd, uiGroupStartGlobalInd + uiRightSrcSampleInd);
	imageStore( destInterpolationSource,
				ivec2( int(uiGlobalSampleInd), int(uiSliceInd) ),
				uvec4( uiGroupStartGlobalInd + uiLeftSrcSampleInd, uiGroupStartGlobalInd + uiRightSrcSampleInd, 0, 0 ) );

}
