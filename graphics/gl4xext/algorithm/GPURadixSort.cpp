#include	"GPURadixSort.h"

#include	<oreore/common/Utility.h>
#include	<oreore/mathlib/MathLib.h>

#include	<graphics/gl4x/resource/GLBindPointManager.h>


#define		PER_BLOCK_ELEMENTS	256	// Must be EVEN number!! 1024 at maximum. must be graeter equal 4(THREADS_PER_BLOCK>=4 at LocalScan Kernel)
#define		THREADS_PER_BLOCK	(PER_BLOCK_ELEMENTS / 2)
#define		NUM_RADIX	16 // number of radix, 0000, 0001, 0010, 0011, ..., 1111



namespace OreOreLib
{

	GLuint	GPURadixSort::m_BindPoints[5]	={ 0, 0, 0, 0, 0 };


	GPURadixSort::GPURadixSort()
	{
		m_ArrayLength				= 0;
		m_refInOut					= NULL;

		m_NumRadixRecursiveLevels	= 0;
		m_GlobalRadixOffsets		= NULL;
		m_LocalRadixCounts			= NULL;
		m_numRadixThreadBlocks		= NULL;
		m_numRadixElms				= NULL;
	}



	GPURadixSort::~GPURadixSort()
	{
		Release();
	}



	void GPURadixSort::Init()
	{
		GLuint	program_id;

		GL_SHADER_MACRO Defines[] =
		{
			GL_SHADER_MACRO( _T( "THREADS_PER_BLOCK" ), to_tstring( THREADS_PER_BLOCK ) ),
			GL_SHADER_MACRO( _T( "" ), _T( "" ) ),
		};


		//===========================	PASS_LOCAL_SORT	=========================//
		// Create shader
		m_Pass[PASS_LOCAL_SORT].Init( _T( "algorithm/GPURadixSort_LocalSort.glsl" ), GLSL_VERSION::GLSL_430, Defines );
		program_id	= m_Pass[PASS_LOCAL_SORT].ID();

		// Init Uniform Locations
		m_p0ulBitShift		= glGetUniformLocation( program_id, "g_BitShift" );
		m_p0ulNumElements	= glGetUniformLocation( program_id, "g_NumElements" );

		// Init Block Indices for SSBO
		m_p0biInputArray	= glGetProgramResourceIndex( program_id, GL_SHADER_STORAGE_BLOCK, "inputdata" );
		m_p0biRadixOffsets	= glGetProgramResourceIndex( program_id, GL_SHADER_STORAGE_BLOCK, "radixOffsets" );
		m_p0biRadixCounts	= glGetProgramResourceIndex( program_id, GL_SHADER_STORAGE_BLOCK, "radixCounts" );

		// Link shader
		m_Pass[PASS_LOCAL_SORT].Link();


		//=============================	PASS_SCAN	=============================//
		// Create shader
		m_Pass[PASS_SCAN].Init( _T( "algorithm/GPURadixSort_Scan.glsl" ), GLSL_VERSION::GLSL_430, Defines );
		program_id	= m_Pass[PASS_SCAN].ID();

		// Init Uniform Locations
		m_p1ulOutputBlockSums	= glGetUniformLocation( program_id, "g_OutputBlockSum" );

		// Init Block Indices for SSBO
		m_p1biInputData		= glGetProgramResourceIndex( program_id, GL_SHADER_STORAGE_BLOCK, "inputdata" );
		m_p1biOutputData		= glGetProgramResourceIndex( program_id, GL_SHADER_STORAGE_BLOCK, "outputdata" );
		m_p1biBlockSum		= glGetProgramResourceIndex( program_id, GL_SHADER_STORAGE_BLOCK, "blockSum" );

		// Link shader
		m_Pass[PASS_SCAN].Link();


		//======================	PASS_ACCUM_BLOCK_SUM	======================//
		Defines[0].Definition = to_tstring( PER_BLOCK_ELEMENTS );

		// Create shader
		m_Pass[PASS_ACCUM_BLOCK_SUM].Init( _T( "algorithm/GPURadixSort_AccumBlockSum.glsl" ), GLSL_VERSION::GLSL_430, Defines );
		program_id	= m_Pass[PASS_ACCUM_BLOCK_SUM].ID();

		// Init Uniform Locations

		// Init Block Indices for SSBO
		m_p2biInOutData		= glGetProgramResourceIndex( program_id, GL_SHADER_STORAGE_BLOCK, "inoutdata" );
		m_p2biBlockSums		= glGetProgramResourceIndex( program_id, GL_SHADER_STORAGE_BLOCK, "blocksumdata" );

		// Link shader
		m_Pass[PASS_ACCUM_BLOCK_SUM].Link();


		//=========================	PASS_GLOBAL_SORT	=========================//
		//Defines[0].Definition = std::to_string( PER_BLOCK_ELEMENTS );

		// Create Shader
		m_Pass[PASS_GLOBAL_SORT].Init( _T( "algorithm/GPURadixSort_GlobalSort.glsl" ), GLSL_VERSION::GLSL_430, Defines );
		program_id	= m_Pass[PASS_GLOBAL_SORT].ID();

		// Init Uniform Locations
		m_p3ulBitShift		= glGetUniformLocation( program_id, "g_BitShift" );
		m_p3ulNumElements	= glGetUniformLocation( program_id, "g_NumElements" );

		// Init Block Indices for SSDO
		m_p3biLocalOffset	= glGetProgramResourceIndex( program_id, GL_SHADER_STORAGE_BLOCK, "localOffset" );
		m_p3biGlobalOffset	= glGetProgramResourceIndex( program_id, GL_SHADER_STORAGE_BLOCK, "globalOffset" );
		m_p3biOutData		= glGetProgramResourceIndex( program_id, GL_SHADER_STORAGE_BLOCK, "outdata" );

		// Link Shader
		m_Pass[PASS_GLOBAL_SORT].Link();


		//=======================	Reserve BindPoints for SSBO	==================//
		for( int i=0; i < 5; ++i )
			if( m_BindPoints[i] == 0 )	m_BindPoints[i] = GLBindPointManager::Reserve( GL_SHADER_STORAGE_BUFFER );

	}



	void GPURadixSort::Release()
	{
		UnbindData();

		for( int i=0; i < NUM_PASS; ++i )	m_Pass[i].Release();

	}



	void GPURadixSort::BindData( GLShaderStorageBufferObject *pInOutData, int numelements )
	{
		m_ArrayLength	= numelements;
		m_refInOut		= pInOutData;

		tcout << _T( "m_ArrayLength = " ) << m_ArrayLength << tendl;
		tcout << _T( "Elements Per Block = " ) << PER_BLOCK_ELEMENTS << tendl;


		//===================	Allocate Radix Offset Buffer	=========================//
		m_LocallySortedKeyAndRadixOffsets.Init( m_ArrayLength *sizeof( Vec2ui ), GL_DYNAMIC_COPY, m_BindPoints[4] );
		m_Pass[PASS_LOCAL_SORT].SetWorkGroupSize( DivUp( DivUp( m_ArrayLength, 2 ), THREADS_PER_BLOCK ), 1, 1 );
		m_Pass[PASS_GLOBAL_SORT].SetWorkGroupSize( DivUp( m_ArrayLength, PER_BLOCK_ELEMENTS ), 1, 1 );


		//===============	Allocate Buffers for Recursive Global Radix Scan ===============//
		uint32 radixCountArrayLength	= NUM_RADIX * m_Pass[PASS_LOCAL_SORT].GetWorkGroupSizeX();// number of radix * threadblock(workgroup) size of local sort pass
		m_NumRadixRecursiveLevels	= int( ceil( Log( (float64)PER_BLOCK_ELEMENTS, (float64)radixCountArrayLength ) ) );

		tcout << _T( "Radix Count Array Length = " ) << radixCountArrayLength << tendl;
		tcout << _T( "Number of Radix Scan Recursive Levels = " ) << m_NumRadixRecursiveLevels << tendl;

		if( m_NumRadixRecursiveLevels == 0 )
			return;

		m_LocalRadixCounts		= new GLShaderStorageBufferObject[m_NumRadixRecursiveLevels];
		m_GlobalRadixOffsets	= new GLShaderStorageBufferObject[m_NumRadixRecursiveLevels];
		m_numRadixThreadBlocks	= new int[m_NumRadixRecursiveLevels];
		m_numRadixElms			= new int[m_NumRadixRecursiveLevels];

		//===============	Allocate Rest of Buffers for Recursive Scan	===============//
		for( int level=0; level < m_NumRadixRecursiveLevels; ++level )
		{
			int arrayLength					= int( ceil( radixCountArrayLength / pow( PER_BLOCK_ELEMENTS, level ) ) );
			m_numRadixElms[level]			= int( Ceil( arrayLength, PER_BLOCK_ELEMENTS ) );
			m_numRadixThreadBlocks[level]	= DivUp( DivUp( arrayLength, 2 ), THREADS_PER_BLOCK );// 処理に必要なスレッドブロックの数

			m_LocalRadixCounts[level].Init( m_numRadixElms[level] * sizeof( uint32 ), GL_DYNAMIC_COPY, m_BindPoints[level % 2 + 0] );// repeatedly assign bindpoints  m_BindPoints[0],m_BindPoints[1],m_BindPoints[0],m_BindPoints[1]...
			m_GlobalRadixOffsets[level].Init( m_numRadixElms[level] * sizeof( uint32 ), GL_DYNAMIC_COPY, m_BindPoints[level % 2 + 2] );// repeatedly assign bindpoints  m_BindPoints[2],m_BindPoints[3],m_BindPoints[2],m_BindPoints[3]...

			tcout << _T( "Level:" ) << level << tendl;
			tcout << _T( "   actual arrayLength = " ) << arrayLength << _T( " " ) << _T( " (aligned to " ) << m_numRadixElms[level] << _T( ")" ) << tendl;
			tcout << _T( "   # RadixThreadBlocks..." ) << m_numRadixThreadBlocks[level] << tendl;
		}

	}



	void GPURadixSort::UnbindData()
	{
		m_ArrayLength		= 0;
		m_refInOut			= NULL;
		
		m_LocallySortedKeyAndRadixOffsets.Release();

		m_NumRadixRecursiveLevels	= 0;
		SafeDeleteArray( m_LocalRadixCounts );
		SafeDeleteArray( m_GlobalRadixOffsets );
		SafeDeleteArray( m_numRadixThreadBlocks );
		SafeDeleteArray( m_numRadixElms );
	}



	void GPURadixSort::Execute()
	{
		tcout << _T( "Begin Sorting..." ) << tendl;

		// 32bitキー値をソートする
		for( int i=0; i<8; ++i )
		{
//#ifdef _DEBUG
//			tcout << tendl<< _T( "Iteration......" ) << i << tendl;
//#endif // _DEBUG

			uint32 bitShift = 4*i;
			LocalSort( bitShift );
			ScanRadixOffset();
			GlobalSort( bitShift );

			glFlush();
		}

		tcout << _T( "End Sorting..." ) << tendl;
	}



	void GPURadixSort::LocalSort( uint32 bitShift )
	{

		GLuint	program_id;

		program_id	= m_Pass[PASS_LOCAL_SORT].ID();

		m_Pass[PASS_LOCAL_SORT].Bind();
		{
			// Bind SSBOs
			m_refInOut->BindToShaderByIndex( program_id, m_p0biInputArray );
			m_LocallySortedKeyAndRadixOffsets.BindToShaderByIndex( program_id, m_p0biRadixOffsets );
			m_LocalRadixCounts[0].BindToShaderByIndex( program_id, m_p0biRadixCounts );

			// Set Uniforms
			glUniform1ui( m_p0ulBitShift, bitShift );
			glUniform1ui( m_p0ulNumElements, m_ArrayLength );

			// Dispatch Compute Shader
			m_Pass[PASS_LOCAL_SORT].Dispatch();
			//glFlush();
			glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
		}
		m_Pass[PASS_LOCAL_SORT].Unbind();


#ifdef _DEBUG

		////===================== Print Locally sorted Array	==================//
		//tcout << _T( "//===================== Locally sorted Array	==================//" ) << tendl;

		//Vec2ui *cpuBuffer	= new Vec2ui[m_ArrayLength];

		//m_refInOut->Readback2CPU( cpuBuffer, sizeof( Vec2ui )*m_ArrayLength );

		//for( int i=0; i < m_ArrayLength; ++i )
		//{
		//	ieee754 val;
		//	val._f32	= cpuBuffer[i].x;
		//	tcout << val._f32 << _T( " " );
		//	if( i==m_ArrayLength-1 || ( i+1 )%16==0 )
		//		tcout << tendl;
		//}
		////tcout << tendl;

		//SafeDeleteArray( cpuBuffer );


		////====================	Print Local Radix Offset	====================//
		//tcout << _T( "//===================== Local Radix Offset	==================//" ) << tendl;

		//Vec2ui  *radixOffsets	= new Vec2ui[m_ArrayLength];

		//m_LocallySortedKeyAndRadixOffsets.Readback2CPU( radixOffsets, sizeof( Vec2ui )*m_ArrayLength );
		//for( int i=0; i < m_ArrayLength; ++i )
		//{
		//	tcout << radixOffsets[i].y << _T( " " );
		//	if( i==m_ArrayLength-1 || ( i+1 )%16==0 )
		//		tcout << tendl;
		//}

		//SafeDeleteArray( radixOffsets );


		////====================	Print Local Radix Count	====================//
		//tcout << _T( "//===================== Local Radix Count	==================//" ) << tendl;
		//tcout << m_numRadixThreadBlocks[0] << _T( "[threadBlocks] * 16[radixes]..." ) << tendl;

		//uint32  *radixCounts	= new uint32[m_numRadixElms[0]];

		//m_LocalRadixCounts[0].Readback2CPU( radixCounts, sizeof( uint32 )*m_numRadixElms[0] );
		//for( int i=0; i < m_numRadixElms[0]; ++i )
		//{
		//	tcout << radixCounts[i] << _T( " " );
		//	if( i==m_numRadixThreadBlocks[0]-1 || ( i+1 )%16==0 )
		//		tcout << "..." << ( i+0 )/16 << tendl;
		//	if( (i+1)%256==0 && i>0 )
		//		tcout << tendl;
		//}

		//SafeDeleteArray( radixCounts );
#endif

	}



	void GPURadixSort::ScanRadixOffset()
	{
//		RecursivePrefixSum( 0 );

		GLuint	program_id;

		//====================== Iterate Partial Scan	========================//
		uint32 bOutputBlockSums;
		program_id	= m_Pass[PASS_SCAN].ID();
		m_Pass[PASS_SCAN].Bind();
		{
			for( int level=0; level<m_NumRadixRecursiveLevels; ++level )
			{
				bOutputBlockSums = uint32( level < m_NumRadixRecursiveLevels - 1 );

				// Bind SSBOs
				m_LocalRadixCounts[level].BindToShaderByIndex( program_id, m_p1biInputData );
				m_GlobalRadixOffsets[level].BindToShaderByIndex( program_id, m_p1biOutputData );// Output Scan
				if( bOutputBlockSums )	m_LocalRadixCounts[level + 1].BindToShaderByIndex( program_id, m_p1biBlockSum );// Output BlockSums

				// Set Uniforms
				glUniform1ui( m_p1ulOutputBlockSums, bOutputBlockSums );

				// Dispatch Compute Shader
				glDispatchCompute( m_numRadixThreadBlocks[level], 1, 1 );
				//glFlush();
				glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
			}
		}
		m_Pass[PASS_SCAN].Unbind();

		//=============== Accumulate Blocksums to all elements ==================//
		program_id	= m_Pass[PASS_ACCUM_BLOCK_SUM].ID();
		m_Pass[PASS_ACCUM_BLOCK_SUM].Bind();
		{
			for( int level=m_NumRadixRecursiveLevels-1; level>=1; --level )
			{
				// Bind SSBOs
				m_GlobalRadixOffsets[level - 1].BindToShaderByIndex( program_id, m_p2biInOutData );
				m_GlobalRadixOffsets[level].BindToShaderByIndex( program_id, m_p2biBlockSums );

				uint32 group_size_x	= m_numRadixThreadBlocks[level - 1];
				// Dispatch Compute Shader
				glDispatchCompute( group_size_x, 1, 1 );
				//glFlush();
				glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
			}
		}
		m_Pass[PASS_ACCUM_BLOCK_SUM].Unbind();


#ifdef _DEBUG
		//uint32 *cpuBuffer	= new uint32[m_numRadixElms[0]];

		//tcout << "m_GlobalRadixOffsets[" << 0 << "]:  ";
		//m_GlobalRadixOffsets[0].Readback2CPU( cpuBuffer, sizeof( uint32 )*m_numRadixElms[0] );

		////for( int i=0; i < m_numRadixElms[0]; ++i )	tcout << cpuBuffer[i] << _T( " " );
		////tcout << tendl;
		//for( int i=0; i < m_numRadixElms[0]; ++i )
		//{
		//	tcout << cpuBuffer[i] << _T( " " );
		//	if( i==m_numRadixThreadBlocks[0]-1 || ( i+1 )%16==0 )
		//		tcout << "..." << ( i+0 )/16 << tendl;
		//	if( ( i+1 )%256==0 && i>0 )
		//		tcout << tendl;
		//}

		//SafeDeleteArray( cpuBuffer );
#endif

	}



	void GPURadixSort::GlobalSort( uint32 bitShift )
	{
		GLuint	program_id	= m_Pass[PASS_GLOBAL_SORT].ID();

		m_Pass[PASS_GLOBAL_SORT].Bind();
		{
			// Bind SSBOs
			m_LocallySortedKeyAndRadixOffsets.BindToShaderByIndex( program_id, m_p3biLocalOffset );
			m_GlobalRadixOffsets[0].BindToShaderByIndex( program_id, m_p3biGlobalOffset );
			m_refInOut->BindToShaderByIndex( program_id, m_p3biOutData );

			// Set Uniforms
			glUniform1ui( m_p3ulBitShift, bitShift );
			glUniform1ui( m_p3ulNumElements, m_ArrayLength );

			// Dispatch Compute Shader
			m_Pass[PASS_GLOBAL_SORT].Dispatch();
			//glFlush();
			glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
		}
		m_Pass[PASS_GLOBAL_SORT].Unbind();
	

	}



//	void GPURadixSort::RecursivePrefixSum( int level )
//	{
//#ifdef _DEBUG
//		tcout << "RecursivePrefixSum(" << level << ")" << tendl;
//#endif
//
//		GLuint	program_id;
//
//		program_id	= m_Pass[PASS_SCAN].ID();
//
//		uint32 bOutputBlockSums = uint32( level < m_NumRadixRecursiveLevels - 1 );
//
//		//=================	Scan each block independently	============//
//		m_Pass[PASS_SCAN].Bind();
//		{
//			// Bind SSBOs
//			m_RadixBlockSums[level].BindToShaderByIndex( program_id, m_biInputData );
//			m_RadixBlockScans[level].BindToShaderByIndex( program_id, m_biOutputData );// Output Scan
//			if( bOutputBlockSums )	m_RadixBlockSums[level + 1].BindToShaderByIndex( program_id, m_biBlockSum );// Output BlockSums
//
//			// Set Uniform Locations
//			glUniform1ui( m_ulOutputBlockSums, bOutputBlockSums );
//
//			// Dispatch Compute Shader
//			glDispatchCompute( m_numRadixThreadBlocks[level], 1, 1 );
//			glFlush();
//			glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
//		}
//		m_Pass[PASS_SCAN].Unbind();
//
//
//		if( bOutputBlockSums == 0 )
//			return;
//
//		RecursivePrefixSum( ++level );
//
//#ifdef _DEBUG
//		tcout << "Add(" << level << ")" << tendl;
//#endif
//
//		//=============== Accumulate Blocksums to all elements ==================//
//		program_id	= m_Pass[PASS_ACCUM_BLOCK_SUM].ID();
//		m_Pass[PASS_ACCUM_BLOCK_SUM].Bind();
//		{
//			// Bind SSBOs
//			m_RadixBlockScans[level - 1].BindToShaderByIndex( program_id, m_biInOutData );
//			m_RadixBlockScans[level].BindToShaderByIndex( program_id, m_biBlockSums );
//
//			uint32 group_size_x	= m_numRadixThreadBlocks[level - 1];
//			// Dispatch Compute Shader
//			glDispatchCompute( group_size_x, 1, 1 );
//			glFlush();
//			glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
//		}
//		m_Pass[PASS_ACCUM_BLOCK_SUM].Unbind();
//
//	}


}// end of namespace
