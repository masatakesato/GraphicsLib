
//void FAST32_hash_2D( vec2 gridcell, out vec4 hash_0, out vec4 hash_1 )	// generates 2 random numbers for each of the 4 cell corners
//{
//    // gridcell is assumed to be an integer coordinate
//    const vec2 OFFSET = vec2( 26.0, 161.0 );
//    const float DOMAIN = 71.0;
//    const vec2 SOMELARGEFLOATS = vec2( 951.135664, 642.949883 );
//    vec4 P = vec4( gridcell.xy, gridcell.xy + 1.0 );
//    P = P - floor(P * ( 1.0 / DOMAIN )) * DOMAIN;
//    P += OFFSET.xyxy;
//    P *= P;
//    P = P.xzxz * P.yyww;
//    hash_0 = fract( P * ( 1.0 / SOMELARGEFLOATS.x ) );
//    hash_1 = fract( P * ( 1.0 / SOMELARGEFLOATS.y ) );
//}
//
//
//void FAST32_hash_3D( vec3 gridcell,
//                        out vec4 lowz_hash_0,
//                        out vec4 lowz_hash_1,
//                        out vec4 lowz_hash_2,
//                        out vec4 highz_hash_0,
//                        out vec4 highz_hash_1,
//                        out vec4 highz_hash_2	)	// generates 3 random numbers for each of the 8 cell corners
//{
//    // gridcell is assumed to be an integer coordinate
//
//    // TODO: these constants need tweaked to find the best possible noise.
//    // probably requires some kind of brute force computational searching or something....
//    const vec2 OFFSET = vec2( 50.0, 161.0 );
//    const float DOMAIN = 69.0;
//    const vec3 SOMELARGEFLOATS = vec3( 635.298681, 682.357502, 668.926525 );
//    const vec3 ZINC = vec3( 48.500388, 65.294118, 63.934599 );
//
//    // truncate the domain
//    gridcell.xyz = gridcell.xyz - floor(gridcell.xyz * ( 1.0 / DOMAIN )) * DOMAIN;
//    vec3 gridcell_inc1 = step( gridcell, vec3( DOMAIN - 1.5 ) ) * ( gridcell + 1.0 );
//
//    // calculate the noise
//    vec4 P = vec4( gridcell.xy, gridcell_inc1.xy ) + OFFSET.xyxy;
//    P *= P;
//    P = P.xzxz * P.yyww;
//    vec3 lowz_mod = vec3( 1.0 / ( SOMELARGEFLOATS.xyz + gridcell.zzz * ZINC.xyz ) );
//    vec3 highz_mod = vec3( 1.0 / ( SOMELARGEFLOATS.xyz + gridcell_inc1.zzz * ZINC.xyz ) );
//    lowz_hash_0 = fract( P * lowz_mod.xxxx );
//    highz_hash_0 = fract( P * highz_mod.xxxx );
//    lowz_hash_1 = fract( P * lowz_mod.yyyy );
//    highz_hash_1 = fract( P * highz_mod.yyyy );
//    lowz_hash_2 = fract( P * lowz_mod.zzzz );
//    highz_hash_2 = fract( P * highz_mod.zzzz );
//}
//
//
//// convert a 0.0->1.0 sample to a -1.0->1.0 sample weighted towards the extremes
//vec4 Cellular_weight_samples( vec4 samples )
//{
//	samples = samples * 2.0 - 1.0;
//	//return (1.0 - samples * samples) * sign(samples); // square
//	return (samples * samples * samples) - sign(samples);	// cubic (even more variance)
//}
//
//
//
//
////
//// Cellular Noise 2D
//// Based off Stefan Gustavson's work at http://www.itn.liu.se/~stegu/GLSL-cellular
//// http://briansharpe.files.wordpress.com/2011/12/cellularsample.jpg
////
//// Speed up by using 2x2 search window instead of 3x3
//// produces a range of 0.0->1.0
////
//float Cellular2D(vec2 P)
//{
//	// establish our grid cell and unit position
//	vec2 Pi = floor(P);
//	vec2 Pf = P - Pi;
//
//	// calculate the hash.
//	// ( various hashing methods listed in order of speed )
//	vec4 hash_x, hash_y;
//	FAST32_hash_2D( Pi, hash_x, hash_y );
//	//SGPP_hash_2D( Pi, hash_x, hash_y );
//
//	// generate the 4 random points
//#if 1
//	// restrict the random point offset to eliminate artifacts
//	// we'll improve the variance of the noise by pushing the points to the extremes of the jitter window
//	const float JITTER_WINDOW = 0.25;	// 0.25 will guarentee no artifacts. 0.25 is the intersection on x of graphs f(x)=( (0.5+(0.5-x))^2 + (0.5-x)^2 ) and f(x)=( (0.5+x)^2 + x^2 )
//		hash_x = Cellular_weight_samples( hash_x ) * JITTER_WINDOW + vec4(0.0, 1.0, 0.0, 1.0);
//	hash_y = Cellular_weight_samples( hash_y ) * JITTER_WINDOW + vec4(0.0, 0.0, 1.0, 1.0);
//#else
//	// non-weighted jitter window. jitter window of 0.4 will give results similar to Stefans original implementation
//	// nicer looking, faster, but has minor artifacts. ( discontinuities in signal )
//	const float JITTER_WINDOW = 0.4;
//	hash_x = hash_x * JITTER_WINDOW * 2.0 + vec4(-JITTER_WINDOW, 1.0-JITTER_WINDOW, -JITTER_WINDOW, 1.0-JITTER_WINDOW);
//	hash_y = hash_y * JITTER_WINDOW * 2.0 + vec4(-JITTER_WINDOW, -JITTER_WINDOW, 1.0-JITTER_WINDOW, 1.0-JITTER_WINDOW);
//#endif
//
//	// return the closest squared distance
//	vec4 dx = Pf.xxxx - hash_x;
//	vec4 dy = Pf.yyyy - hash_y;
//	vec4 d = dx * dx + dy * dy;
//	d.xy = min(d.xy, d.zw);
//	return min(d.x, d.y) * ( 1.0 / 1.125 );	// scale return value from 0.0->1.125 to 0.0->1.0 ( 0.75^2 * 2.0 == 1.125 )
//}
//
//
//
////
//// Cellular Noise 3D
//// Based off Stefan Gustavson's work at http://www.itn.liu.se/~stegu/GLSL-cellular
//// http://briansharpe.files.wordpress.com/2011/12/cellularsample.jpg
////
//// Speed up by using 2x2x2 search window instead of 3x3x3
//// produces range of 0.0->1.0
////
//float Cellular3D(vec3 P)
//{
//	// establish our grid cell and unit position
//	vec3 Pi = floor(P);
//	vec3 Pf = P - Pi;
//
//	// calculate the hash.
//	// ( various hashing methods listed in order of speed )
//	vec4 hash_x0, hash_y0, hash_z0, hash_x1, hash_y1, hash_z1;
//	FAST32_hash_3D( Pi, hash_x0, hash_y0, hash_z0, hash_x1, hash_y1, hash_z1 );
//	//SGPP_hash_3D( Pi, hash_x0, hash_y0, hash_z0, hash_x1, hash_y1, hash_z1 );
//
//	// generate the 8 random points
//#if 1
//	// restrict the random point offset to eliminate artifacts
//	// we'll improve the variance of the noise by pushing the points to the extremes of the jitter window
//	const float JITTER_WINDOW = 0.166666666;	// 0.166666666 will guarentee no artifacts. It is the intersection on x of graphs f(x)=( (0.5 + (0.5-x))^2 + 2*((0.5-x)^2) ) and f(x)=( 2 * (( 0.5 + x )^2) + x * x )
//		hash_x0 = Cellular_weight_samples( hash_x0 ) * JITTER_WINDOW + vec4(0.0, 1.0, 0.0, 1.0);
//	hash_y0 = Cellular_weight_samples( hash_y0 ) * JITTER_WINDOW + vec4(0.0, 0.0, 1.0, 1.0);
//	hash_x1 = Cellular_weight_samples( hash_x1 ) * JITTER_WINDOW + vec4(0.0, 1.0, 0.0, 1.0);
//	hash_y1 = Cellular_weight_samples( hash_y1 ) * JITTER_WINDOW + vec4(0.0, 0.0, 1.0, 1.0);
//	hash_z0 = Cellular_weight_samples( hash_z0 ) * JITTER_WINDOW + vec4(0.0, 0.0, 0.0, 0.0);
//	hash_z1 = Cellular_weight_samples( hash_z1 ) * JITTER_WINDOW + vec4(1.0, 1.0, 1.0, 1.0);
//#else
//	// non-weighted jitter window. jitter window of 0.4 will give results similar to Stefans original implementation
//	// nicer looking, faster, but has minor artifacts. ( discontinuities in signal )
//	const float JITTER_WINDOW = 0.4;
//	hash_x0 = hash_x0 * JITTER_WINDOW * 2.0 + vec4(-JITTER_WINDOW, 1.0-JITTER_WINDOW, -JITTER_WINDOW, 1.0-JITTER_WINDOW);
//	hash_y0 = hash_y0 * JITTER_WINDOW * 2.0 + vec4(-JITTER_WINDOW, -JITTER_WINDOW, 1.0-JITTER_WINDOW, 1.0-JITTER_WINDOW);
//	hash_x1 = hash_x1 * JITTER_WINDOW * 2.0 + vec4(-JITTER_WINDOW, 1.0-JITTER_WINDOW, -JITTER_WINDOW, 1.0-JITTER_WINDOW);
//	hash_y1 = hash_y1 * JITTER_WINDOW * 2.0 + vec4(-JITTER_WINDOW, -JITTER_WINDOW, 1.0-JITTER_WINDOW, 1.0-JITTER_WINDOW);
//	hash_z0 = hash_z0 * JITTER_WINDOW * 2.0 + vec4(-JITTER_WINDOW, -JITTER_WINDOW, -JITTER_WINDOW, -JITTER_WINDOW);
//	hash_z1 = hash_z1 * JITTER_WINDOW * 2.0 + vec4(1.0-JITTER_WINDOW, 1.0-JITTER_WINDOW, 1.0-JITTER_WINDOW, 1.0-JITTER_WINDOW);
//#endif
//
//	// return the closest squared distance
//	vec4 dx1 = Pf.xxxx - hash_x0;
//	vec4 dy1 = Pf.yyyy - hash_y0;
//	vec4 dz1 = Pf.zzzz - hash_z0;
//	vec4 dx2 = Pf.xxxx - hash_x1;
//	vec4 dy2 = Pf.yyyy - hash_y1;
//	vec4 dz2 = Pf.zzzz - hash_z1;
//	vec4 d1 = dx1 * dx1 + dy1 * dy1 + dz1 * dz1;
//	vec4 d2 = dx2 * dx2 + dy2 * dy2 + dz2 * dz2;
//	d1 = min(d1, d2);
//	d1.xy = min(d1.xy, d1.wz);
//	return min(d1.x, d1.y) * ( 9.0 / 12.0 );	// scale return value from 0.0->1.333333 to 0.0->1.0 (2/3)^2 * 3 == (12/9) == 1.333333
//}



uniform usampler1D	g_WorleyPermTexture;
uniform float		g_Randomness = 0.5;


#define perm_worley(i) int( texture( g_WorleyPermTexture, float(i)/255.0 ).r )


float random( int x )
{
	int r1 = perm_worley( x );
	return	float( r1 )/256.0;
}


float random( int x, int y, int z )
{
	int r1 = perm_worley( x + perm_worley( y + perm_worley(z) ) );

	return	float(r1)/256.0;
}


float random( int x, int y, int z, int w )
{
	int r1 = perm_worley( x + perm_worley( y + perm_worley( z + perm_worley(w) ) ) );

	return	float(r1)/256.0;
}


// random value lookup with weighted randomness([0.0, 1.0])
float random( int x, int y, int z, float randomness )
{
	int r1 = perm_worley( x + perm_worley( y + perm_worley(z) ) );

	return	float(r1)/256.0 * randomness;
}

// random value lookup with weighted randomness([0.0, 1.0])
float random( int x, int y, int z, int w, float randomness )
{
	int r1 = perm_worley( x + perm_worley( y + perm_worley( z + perm_worley(w) ) ) );

	return	float(r1)/256.0 * randomness;
}







float euclidean( vec2 p1, vec2 p2 )
{
	return distance(p1, p2);


	//float	dif_x = p1.x - p2.x,
	//		dif_y = p1.y - p2.y;

	//return (dif_x * dif_x + dif_y * dif_y);
}


float euclidean( vec3 p1, vec3 p2 )
{
	return distance(p1, p2);

	//#######################################  試験実装. 2013.10.07
	//return sqrt( 0.15 * ( p1.x - p2.x ) * ( p1.x - p2.x ) + ( p1.y - p2.y ) * ( p1.y - p2.y ) + ( p1.z - p2.z ) * ( p1.z - p2.z ) );

	//float	dif_x = p1.x - p2.x,
	//		dif_y = p1.y - p2.y,
	//		dif_z = p1.z - p2.z;

	//return dif_x * dif_x + dif_y * dif_y + dif_z * dif_z;
}


float manhattan( vec2 p1, vec2 p2 )
{
	float	dif_x = p1.x - p2.x,
			dif_y = p1.y - p2.y;

	return abs(dif_x) + abs(dif_y);
}


float manhattan( vec3 p1, vec3 p2 )
{
	float	dif_x = p1.x - p2.x,
			dif_y = p1.y - p2.y,
			dif_z = p1.z - p2.z;

	return abs(dif_x) + abs(dif_y) + abs(dif_z);
}



float chebychev( vec2 p1, vec2 p2 )
{
	float	dif_x = abs( p1.x - p2.x ),
			dif_y = abs( p1.y - p2.y );

	return max( dif_x, dif_y );

	//if (dif_x > dif_y)
	//	return (dif_x);
	//else
	//	return (dif_y);
}


float chebychev( vec3 p1, vec3 p2 )
{
	float	dif_x = abs( p1.x - p2.x ),
			dif_y = abs( p1.y - p2.y ),
			dif_z = abs( p1.z - p2.z );

	//#######################################  試験実装. 2013.10.07
	//return max( max(dif_x*0.5, dif_y), dif_z );

	return max( max(dif_x, dif_y), dif_z );

	//if (dif_x > dif_y)
	//	return (dif_x);
	//else
	//	return (dif_y);
}





float quadratic( vec2 p1, vec2 p2 )
{
	float	dif_x = p1.x - p2.x,
			dif_y = p1.y - p2.y;

	return dif_x*dif_x + dif_x*dif_y + dif_y*dif_y;
}


// これから実装
float quadratic( vec3 p1, vec3 p2 )
{
	float	dif_x = p1.x - p2.x,
			dif_y = p1.y - p2.y;

	return dif_x*dif_x + dif_x*dif_y + dif_y*dif_y;
}





// 図形同士の距離計算.試験実装 2013.10.08
float distance_point_line_euclidean( vec2 v, vec2 w, vec2 p )
{
	// Return minimum distance between line segment vw and point p
	const float l2 = pow( distance(v, w), 2.0 );  // i.e. |w-v|^2 -  avoid a sqrt
	if( l2==0.0 ) return distance(p, v);   // v == w case
	// Consider the line extending the segment, parameterized as v + t (w - v).
	// We find projection of point p onto the line. 
	// It falls where t = [(p-v) . (w-v)] / |w-v|^2
	const float t = dot( p-v, w-v) / l2;
	if( t<0.0 )		return distance(p, v);       // Beyond the 'v' end of the segment
	else if( t>1.0 )	return distance(p, w);  // Beyond the 'w' end of the segment
	const vec2 projection = v + t * (w - v);  // Projection falls on the segment
	return distance( p, projection );
}



float distance_point_line_euclidean( vec3 v, vec3 w, vec3 p )
{
	// Return minimum distance between line segment vw and point p
	const float l2 = pow( distance(v, w), 2.0 );  // i.e. |w-v|^2 -  avoid a sqrt
	if( l2==0.0 ) return distance(p, v);   // v == w case
	// Consider the line extending the segment, parameterized as v + t (w - v).
	// We find projection of point p onto the line. 
	// It falls where t = [(p-v) . (w-v)] / |w-v|^2
	const float t = dot( p-v, w-v) / l2;
	if( t<0.0 )		return distance(p, v);       // Beyond the 'v' end of the segment
	else if( t>1.0 )	return distance(p, w);  // Beyond the 'w' end of the segment
	const vec3 projection = v + t * (w - v);  // Projection falls on the segment
	return distance( p, projection );
}



float distance_point_line_manhattan( vec2 v, vec2 w, vec2 p )
{
	// Return minimum distance between line segment vw and point p
	const float l2 = pow( manhattan(v, w), 2.0 );  // i.e. |w-v|^2 -  avoid a sqrt
	if( l2==0.0 ) return manhattan(p, v);   // v == w case
	// Consider the line extending the segment, parameterized as v + t (w - v).
	// We find projection of point p onto the line. 
	// It falls where t = [(p-v) . (w-v)] / |w-v|^2
	const float t = dot( p-v, w-v) / l2;
	if( t<0.0 )		return manhattan(p, v);       // Beyond the 'v' end of the segment
	else if( t>1.0 )	return manhattan(p, w);  // Beyond the 'w' end of the segment
	const vec2 projection = v + t * (w - v);  // Projection falls on the segment
	return manhattan( p, projection );
}



float distance_point_line_manhattan( vec3 v, vec3 w, vec3 p )
{
	// Return minimum distance between line segment vw and point p
	const float l2 = pow( manhattan(v, w), 2.0 );  // i.e. |w-v|^2 -  avoid a sqrt
	if( l2==0.0 ) return manhattan(p, v);   // v == w case
	// Consider the line extending the segment, parameterized as v + t (w - v).
	// We find projection of point p onto the line. 
	// It falls where t = [(p-v) . (w-v)] / |w-v|^2
	const float t = dot( p-v, w-v) / l2;
	if( t<0.0 )		return manhattan(p, v);       // Beyond the 'v' end of the segment
	else if( t>1.0 )	return manhattan(p, w);  // Beyond the 'w' end of the segment
	const vec3 projection = v + t * (w - v);  // Projection falls on the segment
	return manhattan( p, projection );
}




float distance_point_line_chebychev( vec2 v, vec2 w, vec2 p )
{
	return max( chebychev( p, v ), chebychev( p, w ) );
	/*
	// Return minimum distance between line segment vw and point p
	const float l2 = pow( chebychev(v, w), 2.0 );  // i.e. |w-v|^2 -  avoid a sqrt
	if( l2==0.0 ) return chebychev(p, v);   // v == w case
	// Consider the line extending the segment, parameterized as v + t (w - v).
	// We find projection of point p onto the line. 
	// It falls where t = [(p-v) . (w-v)] / |w-v|^2
	const float t = dot( p-v, w-v) / l2;
	if( t<0.0 )		return chebychev(p, v);       // Beyond the 'v' end of the segment
	else if( t>1.0 )	return chebychev(p, w);  // Beyond the 'w' end of the segment
	const vec2 projection = v + t * (w - v);  // Projection falls on the segment
	return chebychev( p, projection );
	*/
}



//float distance_point_line_chebychev( vec3 v, vec3 w, vec3 p )
//{
//	// Return minimum distance between line segment vw and point p
//	const float l2 = pow( chebychev(v, w), 2.0 );  // i.e. |w-v|^2 -  avoid a sqrt
//	if( l2==0.0 ) return chebychev(p, v);   // v == w case
//	// Consider the line extending the segment, parameterized as v + t (w - v).
//	// We find projection of point p onto the line. 
//	// It falls where t = [(p-v) . (w-v)] / |w-v|^2
//	const float t = dot( p-v, w-v) / l2;
//	if( t<0.0 )		return chebychev(p, v);       // Beyond the 'v' end of the segment
//	else if( t>1.0 )	return chebychev(p, w);  // Beyond the 'w' end of the segment
//	const vec3 projection = v + t * (w - v);  // Projection falls on the segment
//	return chebychev( p, projection );
//}





float distance_point_line_chebychev( vec3 v, vec3 w, vec3 p )
{
	//return max( chebychev( p, v ), chebychev( p, w ) );
	
	// Return minimum distance between line segment vw and point p
	const float l2 = pow( chebychev(v, w), 2.0 );  // i.e. |w-v|^2 -  avoid a sqrt
	if( l2==0.0 ) return chebychev(p, v);   // v == w case
	// Consider the line extending the segment, parameterized as v + t (w - v).
	// We find projection of point p onto the line. 
	// It falls where t = [(p-v) . (w-v)] / |w-v|^2
	const float t = dot( p-v, w-v) / l2;
	if( t<0.0 )		return chebychev(p, v);       // Beyond the 'v' end of the segment
	else if( t>1.0 )	return chebychev(p, w);  // Beyond the 'w' end of the segment
	const vec3 projection = v + t * (w - v);  // Projection falls on the segment
	return chebychev( p, projection );
	
}




float point_to_box( )
{



	return 0.0;
}












// return F1, F2, and F2-F1
vec4 Worley( vec2 p )
{
	int	int_x = int(p.x),
		int_y = int(p.y),
		i,j,
		tempx,tempy;

	float	//frac_x = x - (float)int_x,
			//frac_y = y - (float)int_y,
			//points[3][3][2],
			temp_dis,
			distance1, distance2,
			matid;
	
	vec3 points_[9];// xy: points, z:cell index


	//get the point inside the current square, and the point inside the 8 surrounding squares
	for( i=0; i<3; ++i )
	{
		for( j=0; j<3; ++j )
		{
			int idx = j*3+i;
			tempx	= int_x + i - 1;
			tempy	= int_y + j - 1;

			points_[ idx ].x	= tempx + random( tempx, tempy, 1 ) * sign( tempx );
			points_[ idx ].y	= tempy + random( tempx, tempy, 2 ) * sign( tempy );
			points_[ idx ].z	= random( tempx, tempy, 3 );

			//points[i][j][0] = tempx + random(tempx, tempy, 1) * SIGN( tempx );
			//points[i][j][1] = tempy + random(tempx, tempy, 2) * SIGN( tempy );
		}
	}
	
	//get the distance to the closest point from x, y
	distance1 = distance2 = 99999.9;//FLT_MAX;
	matid = 0.0;

	for( i=0; i<3; ++i )
	{
		for( j=0; j<3; ++j )
		{
			int idx = j*3+i;

			temp_dis = euclidean( p, points_[idx].xy );
			//temp_dis = manhattan( p, points_[idx].xy );
			//temp_dis = chebychev( p, points_[idx].xy );
			//temp_dis = quadratic( p, points_[idx].xy );
			
			if( temp_dis < distance1 )
			{
				if( distance1 < distance2 )
					distance2 = distance1;

				distance1 = temp_dis;
				matid = points_[idx].z;
			}
			else if( temp_dis < distance2 )
				distance2 = temp_dis;
		}
	}
	
	return vec4( distance1, distance2-distance1, distance2, matid );
	//return (1.0f - distance1);//this just inverts it
	//return (distance2);
}


//#define DIM		3
//#define DIM2	DIM*DIM
//#define DIM3	DIM*DIM*DIM


vec4 Worley( vec3 p )
{
	//p.x *=0.5;
	int	int_x	= int(p.x),
		int_y	= int(p.y),
		int_z	= int(p.z);
	int i,j,k, tempx,tempy,tempz;

	//float	frac_x	= x - (float)int_x,
	//		frac_y	= y - (float)int_y,
	//		frac_z	= z - (float)int_z;

	float	//points[3][3][3][3],
			temp_dis,
			distance1,distance2,
			matid;

	vec4 points_[27];


	//get the point inside the current cube, and the point inside the 26 surrounding cubes
	for( i=0; i<3; ++i )
	{
		for( j=0; j<3; ++j )
		{
			for( k=0; k<3; ++k )
			{
				tempx = int_x + i - 1;
				tempy = int_y + j - 1;
				tempz = int_z + k - 1;
				
				int idx = k*9 + j*3+i;
				points_[idx].x = tempx + random( tempx, tempy, tempz, 1 ) * sign( tempx );
				points_[idx].y = tempy + random( tempx, tempy, tempz, 2 ) * sign( tempy );
				points_[idx].z = tempz + random( tempx, tempy, tempz, 3 ) * sign( tempz );
				points_[idx].w = random( tempx, tempy, tempz, 4 );

				//points[i][j][k][0] = tempx + random( tempx, tempy, tempz, 1 ) * SIGN( tempx );
				//points[i][j][k][1] = tempy + random( tempx, tempy, tempz, 2 ) * SIGN( tempy );
				//points[i][j][k][2] = tempz + random( tempx, tempy, tempz, 3 ) * SIGN( tempz );

			}// end of k loop
		}// end of j loop
	}// end of i loop
	
	//get the distance to the closest point from x, y
	distance1 = distance2 = 99999.9;
	matid = 0.0;

	for( i=0; i<3; ++i )
	{
		for( j=0; j<3; ++j )
		{
			for( k=0; k<3; ++k )
			{
				int idx = k*9 + j*3+i;

				temp_dis = euclidean( p, points_[idx].xyz );
				//temp_dis = manhattan( p, points_[idx].xyz );
				//temp_dis = chebychev( p, points_[idx].xyz );
				//temp_dis = quadratic( p, points_[idx].xyz );
			
				if( temp_dis < distance1 )
				{
					if( distance1 < distance2 )
						distance2 = distance1;

					distance1 = temp_dis;
					matid = points_[idx].w;
				}
				else if( temp_dis < distance2 )
				{
					distance2 = temp_dis;
				}

			}// end of k loop
		}// end of j loop
	}// end of i loop
	
	return vec4( distance1, distance2-distance1, distance2, matid );
	//return distance1;
	//return 1.0 - distance1;//this just inverts it
	//return (1.0 - distance2);
}



// fractal Brownian motion
// lacunarity: 周波数の増減率
// gain: 振幅の増減率
vec2 Worley_fBm( vec2 p, int octaves, float lacunarity, float gain, int mode=0 )
{
	float amplitude = 1.0;
	float frequency	= 1.0;
	float amp_total	= 0.0;
	float total		= 0.0;

float matid = 0.0;

	for( int i=0; i<octaves; ++i )
	{
		vec4 noise	=  Worley( p*frequency );
		total		+= amplitude * noise[mode];
matid = noise.w;
		amp_total	+= amplitude;

		amplitude	*= gain;
		frequency	*= lacunarity;
	}

	return vec2( total / amp_total, matid );
}



vec2 Worley_fBm( vec2 p, float octaves, float lacunarity, float gain, int mode=0 )
{
	float amplitude = 1.0;
	float frequency	= 1.0;
	float amp_total	= 0.0;
	float total		= 0.0f;
float matid = 0.0;

	float floor_oct	= floor(octaves);

	for( int i=0; i<ceil(octaves); ++i )
	{
		float w = mix( 0.0, 1.0, octaves - float(i) );
		vec4 noise	=  Worley( p*frequency );
		amplitude	*= w;
		total		+= amplitude * noise[mode];
matid = noise.w;
		amp_total	+= amplitude;

		amplitude	*= gain/w;
		frequency	*= lacunarity;
	}

	return vec2( total / amp_total, matid );
}



vec2 Worley_fBm( vec3 p, int octaves, float lacunarity, float gain, int mode=0 )
{
	float amplitude = 1.0;
	float frequency	= 1.0;
	float amp_total	= 0.0;
	float total		= 0.0;
float matid = 0.0;

	for( int i=0; i<octaves; ++i )
	{
		vec4 noise	=  Worley( p*frequency );
		total		+= amplitude * noise[mode];
matid = noise.w;
		amp_total	+= amplitude;

		amplitude	*= gain;
		frequency	*= lacunarity;
	}

	return vec2( total / amp_total, matid );
}


vec2 Worley_fBm( vec3 p, float octaves, float lacunarity, float gain, int mode=0 )
{
	float amplitude = 1.0;
	float frequency	= 1.0;
	float amp_total	= 0.0;
	float total		= 0.0f;
float matid = 0.0;
	float floor_oct	= floor(octaves);

	for( int i=0; i<ceil(octaves); ++i )
	{
		float w = mix( 0.0, 1.0, octaves - float(i) );
		amplitude	*= w;
		vec4 noise	=  Worley( p*frequency );
		total		+= amplitude * Worley( p*frequency )[mode];
matid = noise.w;
		amp_total	+= amplitude;

		amplitude	*= gain/w;
		frequency	*= lacunarity;
	}

	return vec2( total / amp_total, matid );
}