#ifndef GPU_RADIX_SORT_H
#define	GPU_RADIX_SORT_H

#include	<graphics/gl4x/shader/GLComputeShader.h>
#include	<graphics/gl4x/resource/GLShaderStorageBufferObject.h>




namespace OreOreLib
{


	class GPURadixSort
	{
	public:

		GPURadixSort();
		~GPURadixSort();

		void Init();
		void Release();

		void BindData( GLShaderStorageBufferObject *pInOutData, int numelements );
		void UnbindData();
		
		// Sorting Pass
		void Execute();


	private:

		// Compute Shaders
		enum PASS{ PASS_LOCAL_SORT, PASS_SCAN, PASS_ACCUM_BLOCK_SUM, PASS_GLOBAL_SORT, NUM_PASS };
		GLComputeShader	m_Pass[NUM_PASS];


		// PASS_LOCALSORT shader params
		GLuint						m_p0ulBitShift;
		GLuint						m_p0ulNumElements;
		GLuint						m_p0biInputArray;		// ssbo block index for input array data
		GLuint						m_p0biRadixOffsets;
		GLuint						m_p0biRadixCounts;

		// PASS_SCAN shader params
		GLint						m_p1ulOutputBlockSums;
		GLuint						m_p1biInputData;		// ssbo block index for input array data
		GLuint						m_p1biOutputData;		// ssbo block index for output array data
		GLuint						m_p1biBlockSum;		// ssbo block index for blocksum

		// PASS_ACCUM_BLOCK_SUM shader params
		GLuint						m_p2biInOutData;
		GLuint						m_p2biBlockSums;

		// PASS_GLOBAL_SORT shader params
		GLuint						m_p3ulBitShift;
		GLuint						m_p3ulNumElements;
		GLuint						m_p3biLocalOffset;
		GLuint						m_p3biGlobalOffset;
		GLuint						m_p3biOutData;


		int							m_ArrayLength;
		GLShaderStorageBufferObject	*m_refInOut;

		// Local Radix Offset
		GLShaderStorageBufferObject	m_LocallySortedKeyAndRadixOffsets;// Vec2ui. x: locally sorted key value, y: prefix sum per radix within single workgroup

		// Global Radix Offset and Intermediate Buffers for Recursive Scan
		int							m_NumRadixRecursiveLevels;
		GLShaderStorageBufferObject	*m_LocalRadixCounts;	// 0番目の要素に、Scanしたい配列データをセットする
		GLShaderStorageBufferObject	*m_GlobalRadixOffsets;	// 0番目の要素にScan結果が入る
		int							*m_numRadixThreadBlocks;
		int							*m_numRadixElms;

		// Bind Points for Shader Storage Buffer Objects.
		static GLuint				m_BindPoints[5];


		void LocalSort( uint32 bitShift );
		void ScanRadixOffset();
		void GlobalSort( uint32 bitShift );

		//void RecursivePrefixSum( int level );


// 参考コード. キー値毎のプレフィックスサムだけ別カーネルに分割する...	https://github.com/ekpyron/gpualg/blob/master/shaders/radixsort/counting.glsl

	};



}// end of namespace


#endif // !GPU_RADIX_SORT_H
