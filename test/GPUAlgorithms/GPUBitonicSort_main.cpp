#include	<crtdbg.h>
#include	<Windows.h>
#include	<iostream>
using namespace std;


#include	<graphics/gl4x/common/GLHelperFunctions.h>
#include	<graphics/gl4x/resource/GLBindPointManager.h>
#include	<graphics/gl4x/resource/GLBufferObject.h>
#include	<graphics/gl4x/resource/GLShaderStorageBufferObject.h>
#include	<graphics/gl4x/resource/GLTextureBufferObject.h>
#include	<graphics/gl4x/other/GLPrimitives.h>

#include	<graphics/gl4xext/algorithm/GPUBitonicSort.h>


using namespace OreOreLib;


#include	<gl/freeglut.h>



GPUBitonicSort	g_GPUBitonicSort;
GLShaderStorageBufferObject	g_SSBO;



const int numElm = 1024 * 1024;

void initialize() 
{
	GLBindPointManager::InitBindPoints();

	TCHAR dir[_MAX_PATH];
	GetCurrentDirectory( _MAX_PATH, dir );
	SetCurrentDirectory( _T("../../../graphics/gl4xext/glsl/") );

	g_GPUBitonicSort.Init();

	// back to current directory
	SetCurrentDirectory( dir );

	float *fArray	= new float[numElm];
	float *fArray2	= new float[numElm];

	for( int i=0; i<numElm; ++i )
	{
		fArray[i]	= float( numElm-i );//float( rand()%numElm );// float( 1023-i );
		//tcout << fArray[i] << tendl;
	}
	
	g_SSBO.Init( sizeof( float ) * numElm, GL_DYNAMIC_COPY, GLBindPointManager::Reserve(GL_SHADER_STORAGE_BUFFER) );
	g_SSBO.UpdateSubresource( fArray, sizeof( float ) * numElm );

	g_GPUBitonicSort.BindData( &g_SSBO );

	g_GPUBitonicSort.Execute( numElm );


	memset( fArray2, 0, sizeof( float ) * numElm );
	g_SSBO.Readback2CPU( fArray2, sizeof( float ) * numElm );
	
	//for( int i=0; i<numElm; ++i )
	//	tcout << fArray2[i] << tendl;
	tcout << int(fArray2[numElm-1]) << tendl;

	SafeDeleteArray( fArray );
	SafeDeleteArray( fArray2 );
}





int main(int argc, char **argv) 
{
	 _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// initialize and run program
	glutInit(&argc, argv);                                      // GLUT initialization
	glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS );
	glutCreateWindow( "GPU Bitonic Sort" );						// create Window
	glewInit();


	initialize();


	// release all allocated data
	g_GPUBitonicSort.Release();
	g_SSBO.Release();

	GLBindPointManager::ReleaseBindPoints();

	return 0;
}