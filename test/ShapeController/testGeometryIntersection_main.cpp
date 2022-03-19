#include	<graphics/common/Intersection.h>
using namespace GraphicsLib;

// https://www.slac.stanford.edu/exp/glast/ground/obsolete-LATSoft-old/old_checkout_package_docs/volume04/tbsim_v3r1/geometry/v1r1/files.html


int main()
{
	// Triangle / AABB intersection
	{
		tcout << _T("//=============== Triangle / AABB intersection... ================//\n");

		Vec3f triVerts[3] = { {0, 0, 0}, {1, 0, 0}, {1, 1, 0} };
		Vec3f boxCenter, boxHalfSize( 1, 1, 1);

		TriangleAABBIntersection( boxCenter, boxHalfSize.xyz, triVerts, {0, 0, 1} );

		tcout << tendl;
	}

	// Ray Triangle intersection 
	{
		tcout << _T("//=============== Ray / Triangle intersection... ================//\n");

		tcout << tendl;
	}

	return 0;
}