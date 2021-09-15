#include	"GPUBitonicSort.h"

#include	<oreore/common/Utility.h>
#include	<oreore/mathlib/MathLib.h>


#define		THREADS_PER_BLOCK	1024



namespace OreOreLib
{

	GPUBitonicSort::GPUBitonicSort()
	{


	}



	GPUBitonicSort::~GPUBitonicSort()
	{


	}



	void GPUBitonicSort::Init()
	{
		GLuint	program_id;
		
		GL_SHADER_MACRO Defines[] =
		{
			GL_SHADER_MACRO( _T( "THREADS_PER_BLOCK" ), to_tstring( THREADS_PER_BLOCK ) ),
			GL_SHADER_MACRO( _T( "" ), _T( "" ) ),
		};

		// Create shader
		m_Shader.Init( _T( "algorithm/GPUBitonicSort.glsl" ), GLSL_VERSION::GLSL_430, Defines );
		program_id	= m_Shader.ID();

		// Init Uniform Location
		m_ulThreads	 = glGetUniformLocation( program_id, "threads" );
		m_ulStride	 = glGetUniformLocation( program_id, "stride" );
		m_ulSize	 = glGetUniformLocation( program_id, "size" );

		// Link shader
		m_Shader.Link();

	}



	void GPUBitonicSort::Release()
	{
		m_Shader.Release();

	}



	void GPUBitonicSort::BindData( GLShaderStorageBufferObject *pSsbo )
	{
		pSsbo->BindToShaderByName( m_Shader.ID(), "ssbFloatArray" );

	}



	void GPUBitonicSort::BindUnbindData()
	{


	}



		void GPUBitonicSort::Execute( uint32 arraySize )
		{
	
			uint32 threads	= uint32(arraySize) >> 1;
			uint32 blocks	= DivUp( threads, (uint32)THREADS_PER_BLOCK );
	
	//		val, key, threads, size, stride
	
	
			m_Shader.Bind();
			{
	
				for( uint32 size=2; size<=arraySize; size<<=1 )
				{
					for( uint32 stride=size>>1; stride>0; stride>>=1 )
					{
						// Set Uniforms
						glUniform1ui( m_ulThreads, threads );
						glUniform1ui( m_ulStride, stride );
						glUniform1ui( m_ulSize, size );

						//tcout << "threads = " << threads << tendl;
						//tcout << "stride = " << stride << tendl;
						//tcout << "size = " << size << tendl<<tendl;
						
						// Dispatch Compute Shader
						glDispatchCompute( blocks, 1, 1 );
						glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
					}
				}
	
			}
			m_Shader.Unbind();
		
		}






	//void GPUBitonicSort::Execute( uint32 arraySize )
	//{

	//	m_Shader.Bind();
	//	{
	//		// Dispatch Compute Shader
	//		glDispatchCompute( 1024, 1, 1 );
	//	}
	//	m_Shader.Unbind();

	//}




}// end of namespace