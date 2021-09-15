#ifndef	GPU_PREFIX_SUM_H
#define	GPU_PREFIX_SUM_H

#include	<graphics/gl4x/shader/GLComputeShader.h>
#include	<graphics/gl4x/resource/GLShaderStorageBufferObject.h>



namespace OreOreLib
{


	class GPUPrefixSum
	{
	public:
		
		GPUPrefixSum();
		~GPUPrefixSum();

		void Init();
		void Release();
		
		void BindData( GLShaderStorageBufferObject *pInputData, GLShaderStorageBufferObject* pOutputData, int numelements );
		void UnbindData();

		void Execute();	// Non Recusive version
		void Execute_rec();// Recursive Version


	private:
		
		// Compute Shaders
		enum PASS{ PASS_SCAN, PASS_ACCUM_BLOCK_SUM, NUM_PASS };
		GLComputeShader	m_Pass[NUM_PASS];

		// PASS_SCAN shader params
		GLint						m_ulOutputBlockSums;
		GLuint						m_biInputData;		// ssbo block index for input array data
		GLuint						m_biOutputData;		// ssbo block index for output array data
		GLuint						m_biBlockSum; // ssbo block index for blocksum

		// PASS_ACCUM_BLOCK_SUM shader params
		GLuint						m_biInOutData;
		GLuint						m_biBlockSums;

		// Intermediate Buffers for Parallel Prefix Sum Computantion
		int							m_NumRecursiveLevels;
		GLShaderStorageBufferObject	**m_BlockScans;
		GLShaderStorageBufferObject	**m_BlockSums;
		int							*m_numThreadBlocks;
		int							*m_numElms;

		// Bind Points for Shader Storage Buffer Objects.
		static GLuint				m_BindPoints[4];
		
		void RecursivePrefixSum( int level );
		
	};



}// end of namespace



#endif	// GPU_PREFIX_SUM_H //