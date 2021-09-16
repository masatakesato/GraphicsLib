#ifndef FUNCTIONS_GLSLH
#define	FUNCTIONS_GLSLH



// 中心線ベクトルからの距離を計算する
// x1,x2: 直線上の2点
// x0:   点
float Distance_Point2Line( in vec3 x1, in vec3 x2, in vec3 x0 )
{
	float x1x0	= distance( x1, x0 );
	float x2x1	= distance( x2, x1 );
	float dot_	= dot( x1-x0, x2-x1 );

	return sqrt( ( (x1x0*x1x0) * (x2x1*x2x1) - (dot_*dot_) ) / (x2x1*x2x1) );
}






#endif	// FUNCTIONS_GLSLH //
