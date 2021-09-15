#include	<crtdbg.h>
#include	<Windows.h>
#include	<iostream>
using namespace std;


#include	<oreore/GLPrimitives.h>
#include	<oreore/GLHelperFunctions.h>
#include	<oreore/GLBindPointManager.h>
#include	<oreore/GLShaderStorageBufferObject.h>
#include	<oreore/GLTextureBufferObject.h>
#include	<oreore/QuadShader.h>
#include	<labworks/GPUPrefixSum.h>


using namespace OreOreLib;


#include	<gl/freeglut.h>



GPUPrefixSum	g_GPUPrefixSum;
GLShaderStorageBufferObject	g_SSBO, g_SSBO2;



const int numElm = 64;//4096*4096;

void initialize()
{
	GLBindPointManager::InitBindPoints();
	

	TCHAR dir[_MAX_PATH];
	GetCurrentDirectory( _MAX_PATH, dir );
	SetCurrentDirectory( _T( "../shader/glsl/" ) );

	g_GPUPrefixSum.Init();


	// back to current directory
	SetCurrentDirectory( dir );



	float *fArray	= new float[numElm];
	float *fArray2	= new float[numElm];

	for( int i=0; i<numElm; ++i )
	{
		fArray[i]	= /*float(i%2==1) */ 1.0f;//1.0f;//float( numElm-i );//float( rand()%numElm );// float( 1023-i );
		fArray2[i]	= 0.0f;
		//tcout << fArray[i] << tendl;
	}

	g_SSBO.Init( sizeof( float ) * numElm, GL_DYNAMIC_COPY, GLBindPointManager::Reserve( GL_SHADER_STORAGE_BUFFER ) );
	g_SSBO.UpdateSubresource( fArray, sizeof( float ) * numElm );

	g_SSBO2.Init( sizeof( float ) * numElm, GL_DYNAMIC_COPY, GLBindPointManager::Reserve( GL_SHADER_STORAGE_BUFFER ) );
	//g_SSBO2.UpdateSubresource( fArray2, sizeof( float ) * numElm );
	

	g_GPUPrefixSum.BindData( &g_SSBO, &g_SSBO2, numElm );
	
	g_GPUPrefixSum.Execute();
	//g_GPUPrefixSum.Execute_rec();

	//g_SSBO2.Readback2CPU( fArray2, sizeof( float ) * numElm );

	//for( int i=0; i<numElm; ++i )
	//	tcout << fArray2[i] << tendl;

	SafeDeleteArray( fArray );
	SafeDeleteArray( fArray2 );
}





int main( int argc, char **argv )
{

	//const int numelements = 13;
	//const int threadsperblock = 3;

	//tcout << _T( "numElements = " ) << numelements << tendl;

	//int	numLevels	= int( floor( Log( threadsperblock, numelements ) ) );
	//tcout << _T( "Recursive Levels = " ) << numLevels << tendl;
	//for( int level=0; level < numLevels; ++level )
	//{
	//	int arrayLength = ceil( numelements / pow( threadsperblock, level + 1 ) );
	//	tcout << _T( "Level " ) << level << _T( " arrayLength = " ) << arrayLength << tendl;
	//}






	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	// initialize and run program
	glutInit( &argc, argv );                                      // GLUT initialization
	glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS );
	glutCreateWindow( "GPU Bitonic Sort" );						// create Window
	glewInit();


	initialize();


	// release all allocated data
	g_GPUPrefixSum.Release();
	g_SSBO.Release();
	g_SSBO2.Release();

	GLBindPointManager::ReleaseBindPoints();

	return 0;
}