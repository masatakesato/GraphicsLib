// Include Files


// Uniform Locations
uniform uint	g_BitShift;
uniform uint	g_NumElements;


// Shader Storage Buffers
layout( std430, binding = 0 ) buffer localOffset
{
	uvec2 g_LocalOffset[];
};

layout( std430, binding = 1 ) buffer globalOffset
{
	uint g_GlobalOffset[];
};

layout( std430, binding = 2 ) buffer outdata
{
	uint g_odata[];
};



// Workgroup Layout
layout( local_size_x=THREADS_PER_BLOCK ) in;



// Main Kernel
void main()
{
	if( gl_GlobalInvocationID.x >= g_NumElements )
		return;

	// キー値をローカル変数に退避する
	const uint keyValue = g_LocalOffset[gl_GlobalInvocationID.x].x;//g_idata[gl_GlobalInvocationID.x];

	// データ格納先インデックスを計算する
	const uint radix	= ( keyValue>>g_BitShift ) & 0xF;	// キー値から4ビット基数を計算する
	const uint destIdx	= g_LocalOffset[gl_GlobalInvocationID.x].y + g_GlobalOffset[radix * gl_NumWorkGroups.x + gl_WorkGroupID.x];

	// グローバルメモリに値を書き込む
	g_odata[destIdx]	= keyValue;
	

	// TODO: 注意!! 最初はg_idata[]の要素を直接入れ替えようとしたけどダメだった。2015.10.26
	//	-> ワークグループをまたいで配列要素を入れ替えようとしたため。
	//  -> barrier()は、単一ワークグループないのスレッドを同期させるだけ。別ワークグループに対しては無効
	//  -> memoryBarrier()はどう？-> ダメっぽい

	// 結論: 複数ワークグループにまたがる配列データのシャッフルは無理。出力用のバッファを用意する

	// 参考： ワークグループ同士の同期取りたいなら、glDispatchComputeで分割しておく
	// http://stackoverflow.com/questions/24094469/any-better-way-to-synchronize-all-invocations-of-compute-shader

}