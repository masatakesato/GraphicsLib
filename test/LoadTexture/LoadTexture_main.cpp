#include	<oreore/Texture.h>
using namespace OreOreLib;





int main( int argc, char *argv[] )
{

#if defined( FREEIMAGE_SUPPORT )
	InitFreeImage();

#elif defined( RESIL_SUPPORT )
	InitIL();

#endif

	Texture2D aaa;

	aaa.Allocate( 256, 256, 0, 0, DATA_FORMAT::FORMAT_R32G32B32A32_FLOAT );

	aaa.Load( _T( "c:/test.jpg" ) );


	aaa.Write( _T("../textures/output.hdr") );


#if defined( FREEIMAGE_SUPPORT )
	DeinitFreeImage();
#endif

	return 0;
}