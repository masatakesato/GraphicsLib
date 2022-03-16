#include	<oreore/common/TString.h>

#include	<graphics/authoring/controller/ControlPoint.h>
#include	"ControlPointExt.h"
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



	RadialControlPoint2D<float> rcp( 0, 5, 0, 6, 1 );


	rcp.SetRadius( 55.6f );

	tcout << rcp.RadiusChanged() << tendl;

	rcp.ResetRadiusChanged();

	tcout << rcp.RadiusChanged() << tendl;

	tcout << rcp.Radius() << tendl;

	return 0;
}