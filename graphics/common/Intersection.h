#ifndef INTERSECTION_H
#define	INTERSECTION_H

#include	<oreore/mathlib/GraphicsMath.h>



namespace GraphicsLib
{

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////// Fast Box-Triangle intersection by GraphicsGems //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

// https://github.com/autonomousvision/occupancy_flow/blob/master/im2mesh/utils/libvoxelize/tribox2.h


//#define FINDMINMAX(x0,x1,x2,minimum,maximum) \
//  minimum = maximum = x0;                    \
//  if(x1<minimum) minimum=x1;                 \
//  if(x1>maximum) maximum=x1;                 \
//  if(x2<minimum) minimum=x2;                 \
//  if(x2>maximum) maximum=x2;


// planeNormal: plane normal
// v: vector from AABB center to plane surface point
// maxbox: half ranges of origin-centered AABB
	namespace detail
	{

		template < typename T >
		static inline bool PlaneOriginedAABBIntersection( const Vec3<T>& planeNormal, Vec3<T>& vecToSurface, const T range[3] )
		{
			T d = -DotProduct( planeNormal, vecToSurface );
			Vec3<T> vmin, vmax;
			for( int i=0; i<3; ++i )
			{
				vmin.xyz[i] = -range[i];
				vmax.xyz[i] = range[i];
			}

			if( DotProduct( planeNormal, vmin ) + d > 0 )	return false;
			if( DotProduct( planeNormal, vmax ) + d >= 0 )	return true;

			return false;
		}


		 //======================== X-tests ========================//
		#define AXISTEST_X01( a, b, fa, fb )         \
    p0 = a*v0.y - b*v0.z;                    \
    p2 = a*v2.y - b*v2.z;                    \
        if( p0 < p2 ){ minimum = p0; maximum = p2; } else { minimum = p2; maximum = p0; } \
    rad = fa * boxhalfsize[1] + fb * boxhalfsize[2];   \
    if( minimum > rad || maximum < -rad ) return 0;


		#define AXISTEST_X2(a, b, fa, fb)              \
    p0 = a*v0.y - b*v0.z;                    \
    p1 = a*v1.y - b*v1.z;                    \
        if(p0<p1) {minimum=p0; maximum=p1;} else {minimum=p1; maximum=p0;} \
    rad = fa * boxhalfsize[1] + fb * boxhalfsize[2];   \
    if(minimum>rad || maximum<-rad) return 0;


//======================== Y-tests ========================//
		#define AXISTEST_Y02(a, b, fa, fb)             \
    p0 = -a*v0.x + b*v0.z;                   \
    p2 = -a*v2.x + b*v2.z;                   \
        if(p0<p2) {minimum=p0; maximum=p2;} else {minimum=p2; maximum=p0;} \
    rad = fa * boxhalfsize[0] + fb * boxhalfsize[2];   \
    if(minimum>rad || maximum<-rad) return 0;

		#define AXISTEST_Y1(a, b, fa, fb)              \
    p0 = -a*v0.x + b*v0.z;                   \
    p1 = -a*v1.x + b*v1.z;                   \
        if(p0<p1) {minimum=p0; maximum=p1;} else {minimum=p1; maximum=p0;} \
    rad = fa * boxhalfsize[0] + fb * boxhalfsize[2];   \
    if(minimum>rad || maximum<-rad) return 0;


//======================== Z-tests ========================//
		#define AXISTEST_Z12(a, b, fa, fb)             \
    p1 = a*v1.x - b*v1.y;                    \
    p2 = a*v2.x - b*v2.y;                    \
        if(p2<p1) {minimum=p2; maximum=p1;} else {minimum=p1; maximum=p2;} \
    rad = fa * boxhalfsize[0] + fb * boxhalfsize[1];   \
    if(minimum>rad || maximum<-rad) return 0;

		#define AXISTEST_Z0(a, b, fa, fb)              \
    p0 = a*v0.x - b*v0.y;                    \
    p1 = a*v1.x - b*v1.y;                    \
        if(p0<p1) {minimum=p0; maximum=p1;} else {minimum=p1; maximum=p0;} \
    rad = fa * boxhalfsize[0] + fb * boxhalfsize[1];   \
    if(minimum>rad || maximum<-rad) return 0;

	}


// boxcenter: AABB中心位置
// boxhalfsize: AABBの半分の大きさ
// triverts: トライアングル座標
// normal: トライアングルの法線ベクトル
	template < typename T >
	static inline bool TriangleAABBIntersection( const Vec3<T>& boxcenter, const T boxhalfsize[3], const Vec3<T> triverts[3], const Vec3<T>& normal )
	{
		Vec3<T> v0, v1, v2;
		Subtract( v0, triverts[0], boxcenter );
		Subtract( v1, triverts[1], boxcenter );
		Subtract( v2, triverts[2], boxcenter );

		Vec3<T> e0, e1, e2;
		Subtract( e0, v1, v0 );
		Subtract( e1, v2, v1 );
		Subtract( e2, v0, v2 );

		T rad, p0, p1, p2, minimum, maximum;
		Vec3<T> fe;
		Abs( fe, e0 );
		AXISTEST_X01( e0.z, e0.y, fe.z, fe.y );
		AXISTEST_Y02( e0.z, e0.x, fe.z, fe.x );
		AXISTEST_Z12( e0.y, e0.x, fe.y, fe.x );

		Abs( fe, e1 );
		AXISTEST_X01( e1.z, e1.y, fe.z, fe.y );
		AXISTEST_Y02( e1.z, e1.x, fe.z, fe.x );
		AXISTEST_Z0( e1.y, e1.x, fe.y, fe.x );

		Abs( fe, e2 );
		AXISTEST_X2( e2.z, e2.y, fe.z, fe.y );
		AXISTEST_Y1( e2.z, e2.x, fe.z, fe.x );
		AXISTEST_Z12( e2.y, e2.x, fe.y, fe.x );


		//FINDMINMAX( v0[X],v1[X],v2[X], minimum, maximum );
		minimum = Min( Min( v0.x, v1.x ), v2.x );
		maximum = Max( Max( v0.x, v1.x ), v2.x );
		if( minimum > boxhalfsize[0] || maximum < -boxhalfsize[0] )	return false;

		//FINDMINMAX( v0[Y],v1[Y],v2[Y], minimum, maximum );
		minimum = Min( Min( v0.y, v1.y ), v2.y );
		maximum = Max( Max( v0.y, v1.y ), v2.y );
		if( minimum > boxhalfsize[1] || maximum < -boxhalfsize[1] )	return false;

		//FINDMINMAX( v0[Z],v1[Z],v2[Z], minimum, maximum );
		minimum = Min( Min( v0.z, v1.z ), v2.z );
		maximum = Max( Max( v0.z, v1.z ), v2.z );
		if( minimum > boxhalfsize[2] || maximum < -boxhalfsize[2] )	return false;

		if( !detail::PlaneOriginedAABBIntersection( normal, v0, boxhalfsize ) )
			return false;

		return true;
	}





	//################ \hogehogeproject\programs\GlobalIllumination\Source\MonteCalroRayTracing\Triangle.h ####################//


	// t: distance from ray origin to intersection point
	// u: weight of v01
	// v: weight of v02
	template < typename T >
	static inline int RayTriangleIntersection( T *orig, T *dir, Vec3<T> triverts[3], T *t, T *u, T *v )
	{
		Vec3<T> e1, e2, pvec, qvec, tvec;//float e1[3],e2[3], pvec[3], qvec[3], tvec[3];
		T det, inv_det;

		Subtract( e1, triverts[1], triverts[0] );
		Subtract( e2, triverts[2], triverts[0] );
		CrossProduct( pvec, dir, e2 );
		det = DotProduct( e1, pvec );

		if( det > -EPSILON_E6 && det < EPSILON_E6 ) return 0;
		inv_det = 1 / det;
		Subtract( tvec, orig, triverts[0] );
		*u = DotProduct( tvec, pvec ) * inv_det;

		if( *u < 0 || *u > 1 )	return 0;
		CrossProduct( qvec, tvec, e1 );
		*v = DotProduct( dir, qvec ) * inv_det;

		if( *v < 0 || *u + *v > 1 )	return 0;
		*t = DotProduct( e2, qvec ) * inv_det;

		if( *t <= 0 )	return 0;//tが0の時も位置が全く動いてないからだめ
		*t *= 0.999999f;
		Scale( tvec, dir, *t );
		//pvec = orig;
		Add( orig, /*pvec,*/ tvec );

		return 1;//ぶつかったら1を返す
	}//end of method




	////static int rayTriIntersection_sse(double *orig, double *dir, const float triverts[3][3], double *t, double *u, double *v)//ノード中のポリゴンとレイとの交点計算。交点が見つかった場合、type,id,u,v,t,Pを更新する
	////{
	////	// 0～3の数字を返す．
	////	
	////	register double e1[3],e2[3], pvec[3], qvec[3], tvec[3];
	////	register double det, inv_det;
	////	
	////	// SSE用バッファ //
	////	//double e1_x[4], e1_y[4], e1_z[4];
	////	//double e2_x[4], e2_y[4], e2_z[4];
	////	//double pvec_x[4], pvec_y[4], pvec_z[4];
	////	//double det[4], inv_det[4];
	////	//double tvec_x[3], tvec_y[4], tvec_z[4];
	////	
	////	//register unsigned int flag[4];
	////	      
	////	Subtract(e1,triverts[1],triverts[0]);// 4つのトライアングルについていっぺんに計算
	////	//_0_e1[x] = _0_triverts_1[x] - _0_triverts_0[x];
	////	//_1_e1[x] = _1_triverts_1[x] - _1_triverts_0[x];
	////	//_2_e1[x] = _2_triverts_1[x] - _2_triverts_0[x];
	////	//_3_e1[x] = _3_triverts_1[x] - _3_triverts_0[x];
	////	
	////	//_0_e1[y] = _0_triverts_1[y] - _0_triverts_0[y];
	////	//_1_e1[y] = _1_triverts_1[y] - _1_triverts_0[y];
	////	//_2_e1[y] = _2_triverts_1[y] - _2_triverts_0[y];
	////	//_3_e1[y] = _3_triverts_1[y] - _3_triverts_0[y];
	////	
	////	//_0_e1[z] = _0_triverts_1[z] - _0_triverts_0[z];
	////	//_1_e1[z] = _1_triverts_1[z] - _1_triverts_0[z];
	////	//_2_e1[z] = _2_triverts_1[z] - _2_triverts_0[z];
	////	//_3_e1[z] = _3_triverts_1[z] - _3_triverts_0[z];
	////	
	////	
	////	Subtract(e2,triverts[2],triverts[0]);// 4つのトライアングルについていっぺんに計算
	////	//_0_e2[x] = _0_triverts_2[x] - _0_triverts_0[x];
	////	//_1_e2[x] = _1_triverts_2[x] - _1_triverts_0[x];
	////	//_2_e2[x] = _2_triverts_2[x] - _2_triverts_0[x];
	////	//_3_e2[x] = _3_triverts_2[x] - _3_triverts_0[x];
	////
	////	//_0_e2[y] = _0_triverts_2[y] - _0_triverts_0[y];
	////	//_1_e2[y] = _1_triverts_2[y] - _1_triverts_0[y];
	////	//_2_e2[y] = _2_triverts_2[y] - _2_triverts_0[y];
	////	//_3_e2[y] = _3_triverts_2[y] - _3_triverts_0[y];
	////
	////	//_0_e2[z] = _0_triverts_2[z] - _0_triverts_0[z];
	////	//_1_e2[z] = _1_triverts_2[z] - _1_triverts_0[z];
	////	//_2_e2[z] = _2_triverts_2[z] - _2_triverts_0[z];
	////	//_3_e2[z] = _3_triverts_2[z] - _3_triverts_0[z];
	////
	////
	////    CrossProduct(pvec,dir,e2);				// 4つのトライアングルについていっぺんに計算
	////	//_0_pvec[x] = (dir[y]*_0_e2[z]) - (dir[z]*_0_e2[y]);
	////	//_1_pvec[x] = (dir[y]*_1_e2[z]) - (dir[z]*_1_e2[y]);
	////	//_2_pvec[x] = (dir[y]*_2_e2[z]) - (dir[z]*_2_e2[y]);
	////	//_3_pvec[x] = (dir[y]*_3_e2[z]) - (dir[z]*_3_e2[y]);
	////
	////    //_0_pvec[y] = (dir[z]*_0_e2[x]) - (dir[x]*_0_e2[z]);
	////	//_1_pvec[y] = (dir[z]*_1_e2[x]) - (dir[x]*_1_e2[z]);
	////	//_2_pvec[y] = (dir[z]*_2_e2[x]) - (dir[x]*_2_e2[z]);
	////	//_3_pvec[y] = (dir[z]*_3_e2[x]) - (dir[x]*_3_e2[z]);
	////
	////    //_0_pvec[z] = (dir[x]*_0_e2[y]) - (dir[y]*_0_e2[x]); 
	////    //_1_pvec[z] = (dir[x]*_1_e2[y]) - (dir[y]*_1_e2[x]); 
	////    //_2_pvec[z] = (dir[x]*_2_e2[y]) - (dir[y]*_2_e2[x]); 
	////    //_3_pvec[z] = (dir[x]*_3_e2[y]) - (dir[y]*_3_e2[x]); 
	////	
	////	
	////	det=DotProduct(e1,pvec);				// 4つのトライアングルについていっぺんに計算
	////	//_0_det = _0_e1[x]*_0_pvec[x] + _0_e1[y]*_0_pvec[y] + _0_e1[z]*_0_pvec[z];
	////	//_1_det = _1_e1[x]*_1_pvec[x] + _1_e1[y]*_1_pvec[y] + _1_e1[z]*_1_pvec[z];
	////	//_2_det = _2_e1[x]*_2_pvec[x] + _2_e1[y]*_2_pvec[y] + _2_e1[z]*_2_pvec[z];
	////	//_3_det = _3_e1[x]*_3_pvec[x] + _3_e1[y]*_3_pvec[y] + _3_e1[z]*_3_pvec[z];
	////	
	////	
	////    if(det>-0.000001 && det<0.000001) return 0;
	////	inv_det=1.0/det;				// 4つのトライアングルについていっぺんに計算
	////	//_0_inv_det = 1.0 / _0_det;
	////	//_1_inv_det = 1.0 / _1_det;
	////	//_2_inv_det = 1.0 / _2_det;
	////	//_3_inv_det = 1.0 / _3_det;
	////
	////
	////	Subtract(tvec,orig,triverts[0]);		// 4つのトライアングルについていっぺんに計算
	////	//_0_tvec[x] = orig[x] - _0_triverts_0[x];
	////	//_1_tvec[x] = orig[x] - _1_triverts_0[x];
	////	//_2_tvec[x] = orig[x] - _2_triverts_0[x];
	////	//_3_tvec[x] = orig[x] - _3_triverts_0[x];
	////	
	////	//_0_tvec[y] = orig[y] - _0_triverts_0[y];
	////	//_1_tvec[y] = orig[y] - _1_triverts_0[y];
	////	//_2_tvec[y] = orig[y] - _2_triverts_0[y];
	////	//_3_tvec[y] = orig[y] - _3_triverts_0[y];
	////	
	////	//_0_tvec[z] = orig[z] - _0_triverts_0[z];
	////	//_1_tvec[z] = orig[z] - _1_triverts_0[z];
	////	//_2_tvec[z] = orig[z] - _2_triverts_0[z];
	////	//_3_tvec[z] = orig[z] - _3_triverts_0[z];
	////	
	////	
	////	*u=DotProduct(tvec,pvec)*inv_det;// uを更新. 4つのトライアングルについていっぺんに計算
	////	//_0_u = _0_inv_det * (_0_tvec[x]*_0_pvec[x] + _0_tvec[y]*_0_pvec[y] + _0_tvec[z]*_0_pvec[z]);
	////	//_1_u = _1_inv_det * (_1_tvec[x]*_1_pvec[x] + _1_tvec[y]*_1_pvec[y] + _1_tvec[z]*_1_pvec[z]);
	////	//_2_u = _2_inv_det * (_2_tvec[x]*_2_pvec[x] + _2_tvec[y]*_2_pvec[y] + _2_tvec[z]*_2_pvec[z]);
	////	//_3_u = _3_inv_det * (_3_tvec[x]*_3_pvec[x] + _3_tvec[y]*_3_pvec[y] + _3_tvec[z]*_3_pvec[z]);
	////	
	////	
	////    if(*u<0.0 || *u>1.0) return 0;
	////	CrossProduct(qvec,tvec,e1);
	////	//_0_qvec[x] = (_0_tvec[y]*_0_e1[z]) - (_0_tvec[z]*_0_e1[y]);
	////	//_1_qvec[x] = (_1_tvec[y]*_1_e1[z]) - (_1_tvec[z]*_1_e1[y]);
	////	//_2_qvec[x] = (_2_tvec[y]*_2_e1[z]) - (_2_tvec[z]*_2_e1[y]);
	////	//_3_qvec[x] = (_3_tvec[y]*_3_e1[z]) - (_3_tvec[z]*_3_e1[y]);
	////	
	////    //_0_qvec[y] = (_0_tvec[z]*_0_e1[x]) - (_0_tvec[x]*_0_e1[z]);
	////	//_1_qvec[y] = (_1_tvec[z]*_1_e1[x]) - (_1_tvec[x]*_1_e1[z]);
	////	//_2_qvec[y] = (_2_tvec[z]*_2_e1[x]) - (_2_tvec[x]*_2_e1[z]);
	////	//_3_qvec[y] = (_3_tvec[z]*_3_e1[x]) - (_3_tvec[x]*_3_e1[z]);
	////	
	////    //_0_qvec[z] = (_0_tvec[x]*_0_e1[y]) - (_0_tvec[y]*_0_e1[x]);
	////    //_1_qvec[z] = (_1_tvec[x]*_1_e1[y]) - (_1_tvec[y]*_1_e1[x]);
	////    //_2_qvec[z] = (_2_tvec[x]*_2_e1[y]) - (_2_tvec[y]*_2_e1[x]);
	////    //_3_qvec[z] = (_3_tvec[x]*_3_e1[y]) - (_3_tvec[y]*_3_e1[x]);
	////	
	////	
	////	*v=DotProduct(dir,qvec)*inv_det;// vを更新. 4つのトライアングルについていっぺんに計算
	////	//_0_v = _0_inv_det * (dir[x]*_0_qvec[x] + dir[y]*_0_qvec[y] + dir[z]*_0_qvec[z]);
	////	//_1_v = _1_inv_det * (dir[x]*_1_qvec[x] + dir[y]*_1_qvec[y] + dir[z]*_1_qvec[z]);
	////	//_2_v = _2_inv_det * (dir[x]*_2_qvec[x] + dir[y]*_2_qvec[y] + dir[z]*_2_qvec[z]);
	////	//_3_v = _3_inv_det * (dir[x]*_3_qvec[x] + dir[y]*_3_qvec[y] + dir[z]*_3_qvec[z]);
	////	
	////	
	////    if(*v<0.0 || *u+*v>1.0) return 0;
	////	*t=DotProduct(e2,qvec)*inv_det;//tを更新
	////	//_0_t = _0_inv_det * (_0_e2[x]*_0_qvec[x] + _0_e2[y]*_0_qvec[y] + _0_e2[z]*_0_qvec[z]);
	////	//_1_t = _1_inv_det * (_1_e2[x]*_1_qvec[x] + _1_e2[y]*_1_qvec[y] + _1_e2[z]*_1_qvec[z]);
	////	//_2_t = _2_inv_det * (_2_e2[x]*_2_qvec[x] + _2_e2[y]*_2_qvec[y] + _2_e2[z]*_2_qvec[z]);
	////	//_3_t = _3_inv_det * (_3_e2[x]*_3_qvec[x] + _3_e2[y]*_3_qvec[y] + _3_e2[z]*_3_qvec[z]);
	////	
	////	
	////	if(*t<=0.0) return 0;//tが0の時も位置が全く動いてないからだめ
	////	Scale(tvec,dir,*t);
	////	//_0_tvec[x] = dir[x] * _0_t;
	////	//_1_tvec[x] = dir[x] * _1_t;
	////	//_2_tvec[x] = dir[x] * _2_t;
	////	//_3_tvec[x] = dir[x] * _3_t;
	////
	////	//_0_tvec[y] = dir[y] * _0_t;
	////	//_1_tvec[y] = dir[y] * _1_t;
	////	//_2_tvec[y] = dir[y] * _2_t;
	////	//_3_tvec[y] = dir[y] * _3_t;
	////
	////	//_0_tvec[z] = dir[z] * _0_t;
	////	//_1_tvec[z] = dir[z] * _1_t;
	////	//_2_tvec[z] = dir[z] * _2_t;
	////	//_3_tvec[z] = dir[z] * _3_t;
	////
	////
	////	// _0_t～_3_tで，flagが1のやつだけ比較．最大値
	////	SET(pvec,orig);
	////	Add(orig,pvec,tvec);// レイの始点を更新
	////	
	////	
	//// return 1;//ぶつかったら1を返す
	////}//end of method




	//###########################################################################################################//
	//
	//									RaySphereIntersection
	//
	//###########################################################################################################//

	template < typename T >
	static inline bool RaySphereIntersection( const Vec3<T>& rayOrigin, const Vec3<T>& rayDirection, const Vec3<T>& sphereCenter, const T& sphereRadius )
	{
		Vec3<T> sphereCenter2RayOrigin;
		Subtract( sphereCenter2RayOrigin, rayOrigin, sphereCenter );
		T distanceSqrd = LengthSqrd( sphereCenter2RayOrigin );

		T B = DotProduct( rayDirection, sphereCenter2RayOrigin );//rayDirection * ( rayOrigin - sphereCenter );
		T C = distanceSqrd - sphereRadius * sphereRadius; //abs( (rayOrigin - sphereCenter ) * (rayOrigin - sphereCenter) ) - Radius * Radius;
		T D = B * B - C;

		if( D >= 0 )
		{
			T t = -B - sqrt( D );
			if( t >= 0 )
			{
				//	ray=Ray(ray.V*t+ray.P,ray.V,t,2);
				//	ray.t=t;
				//	ray.P=ray.V*t+ray.P;
				return true;
			}
		}

		return false;
	}




	//	[関数名]	ParticleBoxIntersection
	//	[引数]		origin：レイ始点，direction：レイ向き
	//	[戻り値]	交点までの距離
	//　[内容]		移動する粒子と境界(BoundingBox)との衝突点を求める(正方向の衝突点のみ検出)
	double RayAABBIntersection( const Vec3d& rayOrigin, const Vec3d& rayDirection, const Vec3d& aabbMin, const Vec3d& aabbMax )
	{
		double tmin, tmax, tymin, tymax, tzmin, tzmax;

		if( rayDirection.x >= 0 )
		{
			tmin = ( aabbMin.x - rayOrigin.x ) / rayDirection.x;
			tmax = ( aabbMax.x - rayOrigin.x ) / rayDirection.x;
		}
		else
		{
			tmin = ( aabbMax.x - rayOrigin.x ) / rayDirection.x;
			tmax = ( aabbMin.x - rayOrigin.x ) / rayDirection.x;
		}
		if( rayDirection.y >= 0 )
		{
			tymin = ( aabbMin.y - rayOrigin.y ) / rayDirection.y;
			tymax = ( aabbMax.y - rayOrigin.y ) / rayDirection.y;
		}
		else
		{
			tymin = ( aabbMax.y - rayOrigin.y ) / rayDirection.y;
			tymax = ( aabbMin.y - rayOrigin.y ) / rayDirection.y;
		}

		if( ( tmin > tymax ) || ( tymin > tmax ) )
			return 0.0;
		if( tymin > tmin )
			tmin = tymin;
		if( tymax < tmax )
			tmax = tymax;

		if( rayDirection.z >= 0 )
		{
			tzmin = ( aabbMin.z - rayOrigin.z ) / rayDirection.z;
			tzmax = ( aabbMax.z - rayOrigin.z ) / rayDirection.z;
		}
		else
		{
			tzmin = ( aabbMax.z - rayOrigin.z ) / rayDirection.z;
			tzmax = ( aabbMin.z - rayOrigin.z ) / rayDirection.z;
		}
		if( ( tmin > tzmax ) || ( tzmin > tmax ) )
			return 0.0;
		if( tzmin > tmin )
			tmin = tzmin;
		if( tzmax < tmax )
			tmax = tzmax;

		//cout << "tmin = "<< tmin <<endl;
		//cout << "tmax = "<< tmax <<endl;
		return abs( Min( tmin, DBL_MAX ) ) < abs( Max( tmax, 0.0 ) ) ?  Min( tmin, DBL_MAX ) : Max( tmax, 0.0 );

	}



	// Distance to line segment
	template < typename T >
	static inline T DistanceToLineSegment( const Vec3<T>& P, const Vec3<T>& Q1, const Vec3<T>& Q2 )
	{
		if( IsSame( Q1, Q2 ) )
			return Distance( P, Q1 );

		Vec3<T> Q12, O=Q1;// O = closest point on line segment(Q1, Q2)
		Subtract( Q12, Q2, Q1 );

		T	Q12_Q12	= DotProduct( Q12, Q12 );
			Q12_Q1	= DotProduct( Q12, Q1 );

		T s = -Q12_Q1 / Q12_Q12;

		AddScaled( O, Clamp(s, 0, 1), Q12 );

		return Distance( P, O );
	}



	// Line segment intersection
	template < typename T >
	static inline bool LineSegmentIntersection( const Vec3<T>& P1, const Vec3<T>& P2, const Vec3<T>& Q1, const Vec3<T>& Q2, Vec3<T>&p_result, Vec3<T>& q_result )
	{
		Vec3<T> P12, Q12, W;// W=Q1->P1 vector

		Subtract( P12, P2, P1 );
		Subtract( Q12, Q2, Q1 );
		Subtract( W, P1, Q1 );

		// Dot products
		T	P_Q = DotProduct( P12, Q12 ),
			P_P = DotProduct( P12, P12 ),
			Q_Q = DotProduct( Q12, Q12 ),
			P_W = DotProduct( P12, W ),
			Q_W = DotProduct( Q12, W );

		T denom = P_P * Q_Q - P_Q * P_Q;
		T s, t;
		if( denom <= EPSILON_E9 )// 線分同士が平行な場合 -> P1/Q1Q2, P2/Q1Q2, Q1/P1P2, Q2/P1P2 の距離を求めて最小のケースを出力する
		{
			s = 0.0;
			t = ( P_P * s - P_Q ) / P_Q;
		}
		else
		{
			s = ( P_Q * Q_W - Q_Q * P_W ) / denom;
			t = ( P_P * Q_W - P_W * P_Q ) / denom;
		}

// TODO: 0 <= s <= 1,  0 <= t <= 1, が満たされていない場合はどうする?
		// s>1 && t>1: P2-Q2間の距離
		// s>1 && t<0: P2-Q1間の距離
		// s<0 && t<0: P1-Q1間の距離
		// s<0 && t>1: P1-Q2間の距離


		AddScaled( p_result, P1, s, P12 );
		AddScaled( q_result, Q1, t, Q12 );

		return true;
	}





}// end of namespace


#endif // !INTERSECTION_H