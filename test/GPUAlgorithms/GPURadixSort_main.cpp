#include	<crtdbg.h>
#include	<Windows.h>
#include	<iostream>
#include	<iomanip>
using namespace std;

#include	<graphics/gl4x/common/GLHelperFunctions.h>
#include	<graphics/gl4x/resource/GLBindPointManager.h>
#include	<graphics/gl4x/resource/GLShaderStorageBufferObject.h>
#include	<graphics/gl4x/resource/GLTextureBufferObject.h>
#include	<graphics/gl4x/other/GLPrimitives.h>

#include	<graphics/gl4xext/algorithm/GPURadixSort.h>

using namespace OreOreLib;


#include	<gl/freeglut.h>


GPURadixSort	g_GPURadixSort;
GLShaderStorageBufferObject	g_SSBO;



const int numElm = 125;//4096*4096;//


// http://stereopsis.com/radix.html

#define _2(x)	(x >> 22 )

static inline uint32 FloatFlip32( uint32 f )
{
	uint32 mask = -int( f >> 31 ) | 0x80000000;
	return f ^ mask;
}


static inline uint32 IFloatFlip32( uint32 f )
{
	uint32 mask = ( ( f >> 31 ) - 1 ) | 0x80000000;
	return f ^ mask;
}


void initialize()
{
	GLBindPointManager::InitBindPoints();
	
	TCHAR dir[_MAX_PATH];
	GetCurrentDirectory( _MAX_PATH, dir );
	SetCurrentDirectory( _T("../../../graphics/gl4xext/glsl/") );
	
	g_GPURadixSort.Init();

	// back to current directory
	SetCurrentDirectory( dir );

	uint32 *fArray	= new uint32[numElm];
	uint32 *fArray2	= new uint32[numElm];

	for( int i=0; i<numElm; ++i )
	{
		float signRand = rand()%100 * 100 > 50 ? +1.0f : -1.0f;
		float a = float( numElm - i ) *signRand;
		fArray[i]	= FloatFlip32( (uint32 &)a );
		fArray2[i]	= 0;
		//tcout << val._u32 << tendl;
	}
	
	g_SSBO.Init( sizeof( uint32 ) * numElm, GL_DYNAMIC_COPY, GLBindPointManager::Reserve( GL_SHADER_STORAGE_BUFFER ) );
	g_SSBO.UpdateSubresource( fArray, sizeof( uint32 ) * numElm );
	
	g_GPURadixSort.BindData( &g_SSBO, numElm );

	LARGE_INTEGER freq, start, end;
	QueryPerformanceFrequency( &freq );
	QueryPerformanceCounter( &start );

	g_GPURadixSort.Execute();

	QueryPerformanceCounter( &end );

	tcout << "duration = " << 1000.0 * double( end.QuadPart - start.QuadPart ) / double(freq.QuadPart) << " [ms]." << tendl;

	
	//===================== Print Final Sorted Result	==================//
	tcout << _T( "//===================== Final Sorted Result	==================//" ) << tendl;

	g_SSBO.Readback2CPU( fArray2, sizeof( uint32 ) * numElm );

	for( int i=0; i < numElm; ++i )
	{
		if( i<numElm-1 && fArray2[i] > fArray2[i+1] )
			tcout << _T("Warning: former element is greater than latter one...") << fArray2[i] << _T( "->" ) << fArray2[i+1] << tendl;
	}

	float min_val	= 0,
		  max_val	= 0;
	uint32& imin_val	= (uint32 &)min_val;
	uint32& imax_val	= (uint32 &)max_val;
	imin_val		= IFloatFlip32( fArray2[0] );
	imax_val		= IFloatFlip32( fArray2[numElm-1] );

	//float min_val = float( IFloatFlip32( fArray2[0] ) );
	//float max_val = float( IFloatFlip32( fArray2[numElm-1] ) );

	tcout << "Min = " << min_val << tendl;
	tcout << "Max = " << max_val << tendl;

	SafeDeleteArray( fArray );
	SafeDeleteArray( fArray2 );
}





int main( int argc, char **argv )
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	// initialize and run program
	glutInit( &argc, argv );                                      // GLUT initialization
	glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS );
	glutCreateWindow( "GPU Bitonic Sort" );						// create Window
	glewInit();


	initialize();


	// release all allocated data
	g_GPURadixSort.Release();
	g_SSBO.Release();

	GLBindPointManager::ReleaseBindPoints();

	return 0;
}