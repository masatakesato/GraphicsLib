#include	<oreore/common/TString.h>

#include	<graphics/authoring/controller/ControlPoint.h>
using namespace GraphicsLib;



int main()
{

	ControlPoint2D<float> cp;

	cp.SetOrigin( 0, 5 );
	cp.SetTarget( 5, -22 );
	//InitVec( cp.Origin(), 5.0f, 5.0f );

	tcout << cp.OriginChanged() << tendl;
	tcout << cp.TargetChanged() << tendl;

	tcout << cp.Origin() << tendl;
	tcout << cp.Target() << tendl;

	return 0;
}