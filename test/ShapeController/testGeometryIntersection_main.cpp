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

	// Ray / Triangle intersection 
	{
		tcout << _T("//=============== Ray / Triangle intersection... ================//\n");

		tcout << tendl;
	}


	// Line segment / Line segment intersection
	{
		tcout << _T("//=============== Line segment / Line segment intersection... ================//\n");

		Vec3f P1(0, 0, 0), P2(10, 10, 0), Q1(0, 10, 5),  Q2(10, 0, 5);
		Vec3f p_result, q_result;

		LineSegmentIntersection( P1, P2, Q1, Q2, p_result, q_result );

		tcout << _T("p_result: ") << p_result << tendl;
		tcout << _T("q_result: ") << q_result << tendl;

		// parallel case
		InitVec( P1, 0.0f, 0.0f, 0.0f );
		InitVec( P2, 10.0f, 10.0f, 0.0f );
		InitVec( Q1, 0.0f, 0.0f, 5.0f );
		InitVec( Q2, 10.0f, 10.0f, 5.0f );

		LineSegmentIntersection( P1, P2, Q1, Q2, p_result, q_result );

		tcout << _T("p_result: ") << p_result << tendl;
		tcout << _T("q_result: ") << q_result << tendl;

		tcout << tendl;
	}

	return 0;
}