#ifndef	GPU_BITONIC_SORT_H
#define	GPU_BITONIC_SORT_H


#include	<oreore/GLComputeShader.h>
#include	<oreore/GLShaderStorageBufferObject.h>



namespace OreOreLib
{


	class GPUBitonicSort
	{
	public:
		
		GPUBitonicSort();
		~GPUBitonicSort();

		void Init();
		void Release();
		
		void BindData( GLShaderStorageBufferObject *pSsbo );
		void BindUnbindData();

		void Execute( uint32 arraySize );


	private:

		GLComputeShader	m_Shader;

		GLint			m_ulThreads;
		GLint			m_ulStride;
		GLint			m_ulSize;

	};



}// end of namespace



#endif	// GPU_BITONIC_SORT_H //