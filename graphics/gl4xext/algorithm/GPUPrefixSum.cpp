#include	"GPUPrefixSum.h"

#include	<oreore/common/Utility.h>
#include	<oreore/mathlib/MathLib.h>

#include	<graphics/gl4x/resource/GLBindPointManager.h>



#define		PER_BLOCK_ELEMENTS	8	// Must be EVEN number!!
#define		THREADS_PER_BLOCK	(PER_BLOCK_ELEMENTS / 2)// 1024, 2048, ?


namespace OreOreLib
{

	GLuint	GPUPrefixSum::m_BindPoints[4]	={ 0, 0, 0, 0 };


	GPUPrefixSum::GPUPrefixSum()
	{
		m_NumRecursiveLevels	= 0;
		m_BlockScans			= NULL;
		m_BlockSums				= NULL;
		m_numThreadBlocks		= NULL;
		m_numElms				= NULL;
	}



	GPUPrefixSum::~GPUPrefixSum()
	{
		Release();
	}



	void GPUPrefixSum::Init()
	{
		GLuint	program_id;

		GL_SHADER_MACRO Defines[] =
		{
			GL_SHADER_MACRO( _T( "THREADS_PER_BLOCK" ), to_tstring( THREADS_PER_BLOCK ) ),
			GL_SHADER_MACRO( _T( "" ), _T( "" ) ),
		};

		//=============================	PASS_SCAN	=============================//
		// Create shader
		m_Pass[PASS_SCAN].Init( _T( "algorithm/GPUPrefixSum_Scan.glsl" ), GLSL_VERSION::GLSL_430, Defines );
		program_id	= m_Pass[PASS_SCAN].ID();

		// Init Uniform Locations
		m_ulOutputBlockSums	= glGetUniformLocation( program_id, "g_OutputBlockSum" );

		// Init Block Indices for SSBO
		m_biInputData		= glGetProgramResourceIndex( program_id, GL_SHADER_STORAGE_BLOCK, "inputdata" );
		m_biOutputData		= glGetProgramResourceIndex( program_id, GL_SHADER_STORAGE_BLOCK, "outputdata" );
		m_biBlockSum		= glGetProgramResourceIndex( program_id, GL_SHADER_STORAGE_BLOCK, "blockSum" );

		// Link shader
		m_Pass[PASS_SCAN].Link();


		//======================	PASS_ACCUM_BLOCK_SUM	======================//
		// Create shader
		Defines[0].Definition = to_tstring( PER_BLOCK_ELEMENTS );

		m_Pass[PASS_ACCUM_BLOCK_SUM].Init( _T( "algorithm/GPUPrefixSum_AccumBlockSum.glsl" ), GLSL_VERSION::GLSL_430, Defines );
		program_id	= m_Pass[PASS_ACCUM_BLOCK_SUM].ID();

		// Init Uniform Locations

		// Init Block Indices for SSBO
		m_biInOutData		= glGetProgramResourceIndex( program_id, GL_SHADER_STORAGE_BLOCK, "inoutdata" );
		m_biBlockSums		= glGetProgramResourceIndex( program_id, GL_SHADER_STORAGE_BLOCK, "blocksumdata" );

		// Link shader
		m_Pass[PASS_ACCUM_BLOCK_SUM].Link();


		//=======================	Reserve BindPoints for SSBO	==================//
		for( int i=0; i < 4; ++i )
			if( m_BindPoints[i] == 0 )	m_BindPoints[i] = GLBindPointManager::Reserve( GL_SHADER_STORAGE_BUFFER );

	}



	void GPUPrefixSum::Release()
	{
		UnbindData();

		for( int i=0; i < NUM_PASS; ++i )	m_Pass[i].Release();

	}



	void GPUPrefixSum::BindData( GLShaderStorageBufferObject *pInputData, GLShaderStorageBufferObject* pOutputData, int numelements )
	{
		m_NumRecursiveLevels	= int( ceil( Log( (float64)PER_BLOCK_ELEMENTS, (float64)numelements ) ) ) + 1;
		tcout << _T( "Number of Recursive Levels = " ) << m_NumRecursiveLevels << tendl;

		if( m_NumRecursiveLevels == 0 )
			return;

		tcout << _T( "numElements = " ) << numelements << tendl;
		tcout << _T( "Elements Per Block = " ) << PER_BLOCK_ELEMENTS << tendl;

		//===============	Allocate Buffers for Prefix Sum Calculation	===============//
		m_BlockSums				= new GLShaderStorageBufferObject*[m_NumRecursiveLevels];
		m_BlockScans			= new GLShaderStorageBufferObject*[m_NumRecursiveLevels];
		m_numThreadBlocks		= new int[m_NumRecursiveLevels];
		m_numElms				= new int[m_NumRecursiveLevels];

		//=============== Init Level-0 Elements using Input/Output Buffers	===============//
		m_BlockSums[0]			= pInputData;	// Set Input array's pointer to m_BlockSums[0]
		m_BlockScans[0]			= pOutputData;	// Set Output array's pointer to m_BlockScans[0]
		m_numThreadBlocks[0]	= DivUp( DivUp( numelements, 2 ), THREADS_PER_BLOCK );
		m_numElms[0]			= numelements;


		//===============	Allocate Rest of Buffers for Recursive Scan	===============//
		for( int level=1; level < m_NumRecursiveLevels; ++level )
		{
			int arrayLength				= int( ceil( numelements / pow( PER_BLOCK_ELEMENTS, level ) ) );
			m_numElms[level]			= int( Ceil( arrayLength, PER_BLOCK_ELEMENTS ) );
			m_numThreadBlocks[level]	= DivUp( DivUp( arrayLength, 2 ), THREADS_PER_BLOCK );// 処理に必要なスレッドブロックの数

			m_BlockSums[level]	= new GLShaderStorageBufferObject();
			m_BlockSums[level]->Init( m_numElms[level] * sizeof( float ), GL_DYNAMIC_COPY, m_BindPoints[level % 2 + 0] );// repeatedly assign bindpoints  m_BindPoints[0],m_BindPoints[1],m_BindPoints[0],m_BindPoints[1]...

			m_BlockScans[level]	= new GLShaderStorageBufferObject();
			m_BlockScans[level]->Init( m_numElms[level] * sizeof( float ), GL_DYNAMIC_COPY, m_BindPoints[level % 2 + 2] );// repeatedly assign bindpoints  m_BindPoints[2],m_BindPoints[3],m_BindPoints[2],m_BindPoints[3]...

			tcout << _T( "Level:" ) << level << tendl;
			tcout << _T( "   actual arrayLength = " ) << arrayLength << _T( " " ) << _T( " (aligned to " ) << m_numElms[level] << _T( ")" ) << tendl;
			tcout << _T( "   # ThreadBlocks..." ) << m_numThreadBlocks[level] << tendl;

			tcout << tendl;
		}

		tcout << tendl;
	}



	void GPUPrefixSum::UnbindData()
	{
		if( m_BlockSums )
		{
			m_BlockSums[0]	= NULL;
			for( int i=1; i < m_NumRecursiveLevels; ++i )	SafeDelete( m_BlockSums[i] );
			SafeDeleteArray( m_BlockSums );
		}
		
		if( m_BlockScans )
		{
			m_BlockScans[0]	= NULL;
			for( int i=1; i < m_NumRecursiveLevels; ++i )	SafeDelete( m_BlockScans[i] );
			SafeDeleteArray( m_BlockScans );
		}

		SafeDeleteArray( m_numThreadBlocks );
		SafeDeleteArray( m_numElms );

		m_NumRecursiveLevels	= 0;

	}



	void GPUPrefixSum::Execute()
	{

		GLuint	program_id;

		//====================== Iterate Partial Scan	========================//
		uint32 bOutputBlockSums;
		program_id	= m_Pass[PASS_SCAN].ID();
		m_Pass[PASS_SCAN].Bind();
		{
			for( int level=0; level<m_NumRecursiveLevels; ++level )
			{
				bOutputBlockSums = uint32( level < m_NumRecursiveLevels - 1 );

				// Bind SSBOs
				m_BlockSums[level]->BindToShaderByIndex( program_id, m_biInputData );
				m_BlockScans[level]->BindToShaderByIndex( program_id, m_biOutputData );// Output Scan
				if( bOutputBlockSums )	m_BlockSums[level + 1]->BindToShaderByIndex( program_id, m_biBlockSum );// Output BlockSums

				// Set Uniform Locations
				glUniform1ui( m_ulOutputBlockSums, bOutputBlockSums );

				// Dispatch Compute Shader
				glDispatchCompute( m_numThreadBlocks[level], 1, 1 );
				glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
			}
		}
		m_Pass[PASS_SCAN].Unbind();

		//=============== Accumulate Blocksums to all elements ==================//
		program_id	= m_Pass[PASS_ACCUM_BLOCK_SUM].ID();
		m_Pass[PASS_ACCUM_BLOCK_SUM].Bind();
		{
			for( int level=m_NumRecursiveLevels-1; level>=1; --level )
			{
				// Bind SSBOs
				m_BlockScans[level - 1]->BindToShaderByIndex( program_id, m_biInOutData );
				m_BlockScans[level]->BindToShaderByIndex( program_id, m_biBlockSums );

				uint32 group_size_x	= m_numThreadBlocks[level - 1];
				// Dispatch Compute Shader
				glDispatchCompute( group_size_x, 1, 1 );
				glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
			}
		}
		m_Pass[PASS_ACCUM_BLOCK_SUM].Unbind();


#ifdef _DEBUG

		float *cpuBuffer	= new float[m_numElms[0]];

		for( int level=0; level < m_NumRecursiveLevels; ++level )
		{
			int numelms;

			tcout << "m_BlockScans[" << level << "]:  ";
			numelms	= m_numElms[level];
			m_BlockScans[level]->Readback2CPU( cpuBuffer, sizeof( float )*numelms );

			for( int i=0; i < numelms; ++i )	tcout << cpuBuffer[i] << _T( " " );
			tcout << tendl;

			tcout << "m_BlockSums[" << level << "]:   ";
			numelms	= m_numElms[level];
			m_BlockSums[level]->Readback2CPU( cpuBuffer, sizeof( float )*numelms );

			for( int i=0; i < numelms; ++i )	tcout << cpuBuffer[i] << _T( " " );
			tcout << tendl;

		}

		tcout << _T("Result = ") << int(cpuBuffer[0]) << tendl;
		
		tcout << tendl;

		SafeDeleteArray( cpuBuffer );
#endif

	}




	void GPUPrefixSum::Execute_rec()
	{
		RecursivePrefixSum( 0 );


#ifdef _DEBUG

		float *cpuBuffer	= new float[m_numElms[0]];

		for( int level=0; level < m_NumRecursiveLevels; ++level )
		{
			int numelms;

			tcout << "m_BlockSums[" << level << "]:   ";
			numelms	= m_numElms[level];
			m_BlockSums[level]->Readback2CPU( cpuBuffer, sizeof( float )*numelms );

			for( int i=0; i < numelms; ++i )	tcout << cpuBuffer[i] << _T( " " );
			tcout << tendl;


			tcout << "m_BlockScans[" << level << "]:  ";
			numelms	= m_numElms[level];
			m_BlockScans[level]->Readback2CPU( cpuBuffer, sizeof( float )*numelms );

			for( int i=0; i < numelms; ++i )	tcout << cpuBuffer[i] << _T( " " );
			tcout << tendl;
		}

		SafeDeleteArray( cpuBuffer );
#endif

	}



	void GPUPrefixSum::RecursivePrefixSum( int level )
	{
#ifdef _DEBUG
		tcout << "RecursivePrefixSum(" << level << ")" << tendl;
#endif

		GLuint	program_id;

		program_id	= m_Pass[PASS_SCAN].ID();

		uint32 bOutputBlockSums = uint32( level < m_NumRecursiveLevels - 1 );

		//=================	Scan each block independently	============//
		m_Pass[PASS_SCAN].Bind();
		{
			// Bind SSBOs
			m_BlockSums[level]->BindToShaderByIndex( program_id, m_biInputData );
			m_BlockScans[level]->BindToShaderByIndex( program_id, m_biOutputData );// Output Scan
			if( bOutputBlockSums )	m_BlockSums[level + 1]->BindToShaderByIndex( program_id, m_biBlockSum );// Output BlockSums

			// Set Uniform Locations
			glUniform1ui( m_ulOutputBlockSums, bOutputBlockSums );

			// Dispatch Compute Shader
			glDispatchCompute( m_numThreadBlocks[level], 1, 1 );
			glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
		}
		m_Pass[PASS_SCAN].Unbind();


		if( bOutputBlockSums == 0 )
			return;

		RecursivePrefixSum( ++level );

#ifdef _DEBUG
tcout << "Add(" << level << ")" << tendl;
#endif

		//=============== Accumulate Blocksums to all elements ==================//
		program_id	= m_Pass[PASS_ACCUM_BLOCK_SUM].ID();
		m_Pass[PASS_ACCUM_BLOCK_SUM].Bind();
		{
			// Bind SSBOs
			m_BlockScans[level-1]->BindToShaderByIndex( program_id, m_biInOutData );
			m_BlockScans[level]->BindToShaderByIndex( program_id, m_biBlockSums );

			uint32 group_size_x	= m_numThreadBlocks[level-1];
			// Dispatch Compute Shader
			glDispatchCompute( group_size_x, 1, 1 );
		}
		m_Pass[PASS_ACCUM_BLOCK_SUM].Unbind();
		
	}



}// end of namespace