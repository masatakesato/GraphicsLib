// Include Files


// Uniform Locations
uniform uint	g_BitShift;
uniform uint	g_NumElements;


// Shader Storage Buffers
layout( std430, binding = 0 ) buffer inputdata
{
	uint g_idata[];
};

layout( std430, binding = 1 ) writeonly buffer radixOffsets
{
	uvec2 g_LocalRadixOffsets[];
};

layout( std430, binding = 2 ) writeonly buffer radixCounts
{
	uint g_LocalRadixCounts[];
};


// Workgroup Layout
layout( local_size_x=THREADS_PER_BLOCK ) in;


// Constant Variables
const uvec4 g_HexadecimalRadix[] = { uvec4( 0x0, 0x1, 0x2, 0x3 ), uvec4( 0x4, 0x5, 0x6, 0x7 ), uvec4( 0x8, 0x9, 0xA, 0xB ), uvec4( 0xC, 0xD, 0xE, 0xF ), };
const int g_PartialArrayLength = int( 2 * gl_WorkGroupSize.x );
const uint g_MaxUnsignedLong = 4294967295;


// Shared Variables
shared uvec2	s_Keys[g_PartialArrayLength];
shared uint		s_PrefixSum[g_PartialArrayLength+1];	// Last element keeps Summation from [0] to [g_PartialArrayLength-1].
shared uvec4	s_RadixCount4[g_PartialArrayLength+1];	// buffer for local offset calculation. Last element keeps Summation from [0] to [g_PartialArrayLength-1].



// Local Functions
void LocalScan( in int thid, inout shared uint array[g_PartialArrayLength+1] )
{
	int offset	= 1;

	//===================	build sum in place up the tree	===================//
	for( int d=g_PartialArrayLength >> 1; d > 0; d>>=1 )
	{
		barrier();// __syncthreads();

		if( thid < d )
		{
			int ai = offset*( 2 * thid + 1 ) - 1;
			int bi = offset*( 2 * thid + 2 ) - 1;

			array[bi] += array[ai];
		}
		offset *= 2;
	}

	if( thid == 0 )
	{
		array[g_PartialArrayLength]	= array[g_PartialArrayLength-1];
		array[g_PartialArrayLength - 1] = 0;// clear the last element 
	}
	
	//===================	traverse down tree & build scan	===================//
	for( int d = 1; d < g_PartialArrayLength; d *= 2 )
	{
		offset >>= 1;
		barrier();

		if( thid < d )
		{
			int ai = offset*( 2 * thid + 1 ) - 1;
			int bi = offset*( 2 * thid + 2 ) - 1;

			uint t		= array[ai];
			array[ai]	= array[bi];
			array[bi]	+= t;	
		}
	}
	
	barrier();
}


void LocalScan4( in int thid, inout shared uvec4 array[g_PartialArrayLength+1] )
{
	int offset	= 1;

	//===================	build sum in place up the tree	===================//
	for( int d=g_PartialArrayLength >> 1; d > 0; d>>=1 )
	{
		barrier();// __syncthreads();

		if( thid < d )
		{
			int ai = offset*( 2 * thid + 1 ) - 1;
			int bi = offset*( 2 * thid + 2 ) - 1;

			array[bi] += array[ai];
		}
		offset *= 2;
	}

	if( thid == 0 )
	{
		array[g_PartialArrayLength]	= array[g_PartialArrayLength - 1];
		array[g_PartialArrayLength - 1] = uvec4(0);// clear the last element 
	}

	//===================	traverse down tree & build scan	===================//
	for( int d = 1; d < g_PartialArrayLength; d *= 2 )
	{
		offset >>= 1;
		barrier();

		if( thid < d )
		{
			int ai = offset*( 2 * thid + 1 ) - 1;
			int bi = offset*( 2 * thid + 2 ) - 1;

			uvec4 t		= array[ai];
			array[ai]	= array[bi];
			array[bi]	+= t;
		}
	}

	barrier();
}



// Main Kernel
void main()
{
	uvec2 destIdx = uvec2( 0 );

	const int thid	= int( gl_LocalInvocationID.x );//threadIdx.x;
	const uint globalThreadIdx0	= gl_GlobalInvocationID.x * 2;
	const uint globalThreadIdx1	= gl_GlobalInvocationID.x * 2 + 1;
	const uint localThreadIdx0	= gl_LocalInvocationID.x * 2;
	const uint localThreadIdx1	= gl_LocalInvocationID.x * 2 + 1;

	uint	Src = 0,
			Dst = 1,
			falseTotal = 0;

	//================ Copy Input Array Keys to shared memory ===========//
	s_Keys[localThreadIdx0][Src]	= globalThreadIdx0 < g_NumElements ? g_idata[globalThreadIdx0] : g_MaxUnsignedLong;// g_idata[globalThreadIdx0];//
	s_Keys[localThreadIdx1][Src]	= globalThreadIdx1 < g_NumElements ? g_idata[globalThreadIdx1] : g_MaxUnsignedLong;// g_idata[globalThreadIdx1];//

	
	//=========================	Sort Keys	=============================//
	uint uBitMask0, uBitMask1;
	for( uint i=0, maskbit=1<<g_BitShift; i<4; ++i, maskbit<<=1 )
	{
		// Create Bitamask array
		uBitMask0	= s_PrefixSum[localThreadIdx0]	= uint( bool( s_Keys[localThreadIdx0][Src] & maskbit ) );// 指定ビット目が1の場合にフラグを立てる
		uBitMask1	= s_PrefixSum[localThreadIdx1]	= uint( bool( s_Keys[localThreadIdx1][Src] & maskbit ) );// 指定ビット目が1の場合にフラグを立てる

		// Scan Bitmask Array
		LocalScan( thid, s_PrefixSum );

		// Calculate Destination Index
		falseTotal	= g_PartialArrayLength - s_PrefixSum[g_PartialArrayLength];//( s_PrefixSum[g_PartialArrayLength - 1] + s_BitMask[g_PartialArrayLength - 1] );
		destIdx[0]	= uBitMask0 > 0 ? s_PrefixSum[localThreadIdx0] + falseTotal : localThreadIdx0 - s_PrefixSum[localThreadIdx0];
		destIdx[1]	= uBitMask1 > 0 ? s_PrefixSum[localThreadIdx1] + falseTotal : localThreadIdx1 - s_PrefixSum[localThreadIdx1];

		// Shuffle Keys
		s_Keys[destIdx[0]][Dst]	= s_Keys[localThreadIdx0][Src];
		s_Keys[destIdx[1]][Dst]	= s_Keys[localThreadIdx1][Src];
		
		barrier();// Syncronize

		// Swap Ping-Pong Buffer
		Dst ^=1;
		Src ^=1;

	}// end of bitShift loop


	//=========================	Calculate localOffset	=============================//
	uvec4 u4BitMask0, u4BitMask1;
	uint radixOffset0 = 0, radixOffset1 = 0;

	for( uint i=0; i < 4; ++i )// キー配列に含まれる基数の数をカウントする(1ループあたり基数４つ分いっぺんに処理)
	{
		// Create 4 Bitmask Array
		u4BitMask0	= s_RadixCount4[localThreadIdx0]	= uvec4( equal( uvec4( (s_Keys[localThreadIdx0][Src]>>g_BitShift) & 0xF ), g_HexadecimalRadix[i] ) );
		u4BitMask1	= s_RadixCount4[localThreadIdx1]	= uvec4( equal( uvec4( (s_Keys[localThreadIdx1][Src]>>g_BitShift) & 0xF ), g_HexadecimalRadix[i] ) );
		
		// Scan Local Bitmask Array
		LocalScan4( thid, s_RadixCount4 );// s_RadixCount4[g_PartialArrayLength]に、基数4つ分のカウント合計数が入ってる

		// Output to Local Radix Offset
		radixOffset0	+= uint( dot( s_RadixCount4[localThreadIdx0], u4BitMask0 ) );
		radixOffset1	+= uint( dot( s_RadixCount4[localThreadIdx1], u4BitMask1 ) );
		
		// Output Radix Counting Value. TODO: 注意!!!! THREADS_PER_BLOCKを4以上に設定すること
		if( thid<4 )
		{
			g_LocalRadixCounts[( 4*i + thid ) * gl_NumWorkGroups.x + gl_WorkGroupID.x]	= s_RadixCount4[g_PartialArrayLength][thid];//gl_WorkGroupID.x;//

		}

		barrier();
	}
	

	//================ Copy shared memory to Output Array  ===========//
	g_LocalRadixOffsets[globalThreadIdx0]	= uvec2( s_Keys[localThreadIdx0][Src], radixOffset0 );
	g_LocalRadixOffsets[globalThreadIdx1]	= uvec2( s_Keys[localThreadIdx1][Src], radixOffset1 );

}


// 

// キー値4ビット分だけ並べ替え


// スレッド数=要素数でカーネルをディスパッチする

// Significant Bitを調べて


// 注意! シェアードメモリの上限は64kb = 32bit浮動小数データ16384個分まで

// 一部のスレッドだけ使ってPrefixSumやろうとしたけど無理だった（スレッドIDで処理を条件分岐させた）
//　→PrefixSum()内で、barrier()使ってスレッド間の同期を取っている
//　→barrierはグループ内の全スレッド（PrefixSumで使ってないやつも含む）に作用してしまう
//  →PrefixSumするしないの条件分岐を挟んで、スレッドの同期タイミングがおかしくなる
//　→対策として、待ちスレッドにもPrefixSum()と同じ回数だけbarrier()かませばタイミング合わせられるけど、、、、
//　→別の対策として、PrefixSumにあわせたスレッド設計（1スレッドで要素2個分処理）に変える方法もあり
//  →更に別の対策として、ビット判定処理とPrefixSumを別カーネルに分割する方法もあり