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


	// Distance to line segment
	{
		tcout << _T("//=============== Distance to line segment... ================//\n");

		Vec3f P(10, 10, 10), Q1(0, 0, 0), Q2(10, 10, 10);

		tcout << _T("P: ") << P << tendl;
		tcout << _T("Line Q: ") << Q1 << _T(" / ") << Q2 << tendl;

		auto d = DistanceToLineSegment( P, Q1, Q2, false );
		tcout << _T("Distance: ") << d << tendl;

		tcout << tendl;
	}



	// Line segment / Line segment intersection
	{
		tcout << _T("//=============== Line segment / Line segment intersection... ================//\n");

		// non-parallel case
		//Vec3f P1(0, 0, 0), P2(10, 10, 0), Q1(0, 10, 5),  Q2(10, 0, 5);
		Vec3f P1(0, 0, 0), P2(10, 0, 0), Q1(0, 1, 0),  Q2(-1, 0, 0);
		Vec3f Hp, Hq;

		//tcout << _T("Line P: ") << P1 << _T(" / ") << P2 << tendl;
		//tcout << _T("Line Q: ") << Q1 << _T(" / ") << Q2 << tendl;

		//LineSegmentIntersection( P1, P2, Q1, Q2, Hp, Hq );

		//tcout << _T("Hp: ") << Hp << tendl;
		//tcout << _T("Hq: ") << Hq << tendl;

		//tcout << tendl;

		// parallel case
		InitVec( P1, 0.0f, 0.0f, 0.0f );
		InitVec( P2, 10.0f, 10.0f, 0.0f );
		InitVec( Q1, 15.0f, 15.0f, 5.0f );
		InitVec( Q2, 16.0f, 16.0f, 5.0f );

		tcout << _T("Line P: ") << P1 << _T(" / ") << P2 << tendl;
		tcout << _T("Line Q: ") << Q1 << _T(" / ") << Q2 << tendl;

		LineSegmentIntersection( P1, P2, Q1, Q2, Hp, Hq, true, true, false, true );

		tcout << _T("Hp: ") << Hp << tendl;
		tcout << _T("Hq: ") << Hq << tendl;

		tcout << tendl;
	}

	return 0;
}