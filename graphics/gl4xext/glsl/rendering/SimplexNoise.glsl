#ifndef SIMPLEX_NOISE_GLSL
#define	SIMPLEX_NOISE_GLSL


#define  NORMALIZE_GRADIENTS
#undef  USE_CIRCLE
#define COLLAPSE_SORTNET

float permute(float x0,vec3 p) { 
	float x1 = mod(x0 * p.y, p.x);
	return floor(  mod( (x1 + p.z) *x0, p.x ));
}
vec2 permute(vec2 x0,vec3 p) { 
	vec2 x1 = mod(x0 * p.y, p.x);
	return floor(  mod( (x1 + p.z) *x0, p.x ));
}
vec3 permute(vec3 x0,vec3 p) { 
	vec3 x1 = mod(x0 * p.y, p.x);
	return floor(  mod( (x1 + p.z) *x0, p.x ));
}
vec4 permute(vec4 x0,vec3 p) { 
	vec4 x1 = mod(x0 * p.y, p.x);
	return floor(  mod( (x1 + p.z) *x0, p.x ));
}

//uniform vec4 pParam; 
// Example constant with a 289 element permutation
const vec4 pParam = vec4( 63.0*63.0, 126.0, 1.0, 23.0);
//const vec4 pParam = vec4( 17.0*17.0, 34.0, 1.0, 7.0);


float taylorInvSqrt(float r)
{ 
	return ( 0.83666002653408 + 0.7*0.85373472095314 - 0.85373472095314 * r );
}

float simplexNoise2(vec2 v)
{
	const vec2 C = vec2(0.211324865405187134, // (3.0-sqrt(3.0))/6.;
		0.366025403784438597); // 0.5*(sqrt(3.0)-1.);
	const vec3 D = vec3( 0., 0.5, 2.0) * 3.14159265358979312;
	// First corner
	vec2 i  = floor(v + dot(v, C.yy) );
	vec2 x0 = v -   i + dot(i, C.xx);

	// Other corners
	vec2 i1  =  (x0.x > x0.y) ? vec2(1.,0.) : vec2(0.,1.) ;

	//  x0 = x0 - 0. + 0. * C
	vec2 x1 = x0 - i1 + 1. * C.xx ;
	vec2 x2 = x0 - 1. + 2. * C.xx ;

	// Permutations
	i = mod(i, pParam.x);
	vec3 p = permute( permute( 
		i.y + vec3(0., i1.y, 1. ), pParam.xyz)
		+ i.x + vec3(0., i1.x, 1. ), pParam.xyz);

#ifndef USE_CIRCLE
	// ( N points uniformly over a line, mapped onto a diamond.)
	vec3 x = fract(p / pParam.w) ;
	vec3 h = 0.5 - abs(x) ;

	vec3 sx = vec3(lessThan(x,D.xxx)) *2. -1.;
	vec3 sh = vec3(lessThan(h,D.xxx));

	vec3 a0 = x + sx*sh;
	vec2 p0 = vec2(a0.x,h.x);
	vec2 p1 = vec2(a0.y,h.y);
	vec2 p2 = vec2(a0.z,h.z);

#ifdef NORMALISE_GRADIENTS
	p0 *= taylorInvSqrt(dot(p0,p0));
	p1 *= taylorInvSqrt(dot(p1,p1));
	p2 *= taylorInvSqrt(dot(p2,p2));
#endif

	vec3 g = 2.0 * vec3( dot(p0, x0), dot(p1, x1), dot(p2, x2) );
#else 
	// N points around a unit circle.
	vec3 phi = D.z * mod(p,pParam.w) /pParam.w ;
	vec4 a0 = sin(phi.xxyy+D.xyxy);
	vec2 a1 = sin(phi.zz  +D.xy);
	vec3 g = vec3( dot(a0.xy, x0), dot(a0.zw, x1), dot(a1.xy, x2) );
#endif
	// mix
	vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x1,x1), dot(x2,x2)), 0.);
	m = m*m ;
	return 1.66666* 70.*dot(m*m, g);
}

float simplexNoise3(vec3 v)
{ 
	const vec2  C = vec2(1./6. , 1./3. ) ;
	const vec4  D = vec4(0., 0.5, 1.0, 2.0);

	// First corner
	vec3 i  = floor(v + dot(v, C.yyy) );
	vec3 x0 =   v - i + dot(i, C.xxx) ;

	// Other corners
#ifdef COLLAPSE_SORTNET
	vec3 g = vec3( greaterThan(   x0.xyz, x0.yzx) );
	vec3 l = vec3( lessThanEqual( x0.xyz, x0.yzx) );

	vec3 i1 = g.xyz  * l.zxy;
	vec3 i2 = max( g.xyz, l.zxy);
#else
	// Keeping this clean - let the compiler optimize.
	vec3 q1;
	q1.x = max(x0.x, x0.y);
	q1.y = min(x0.x, x0.y);
	q1.z = x0.z;

	vec3 q2;
	q2.x = max(q1.x,q1.z);
	q2.z = min(q1.x,q1.z);
	q2.y = q1.y;

	vec3 q3;
	q3.y = max(q2.y, q2.z);
	q3.z = min(q2.y, q2.z);
	q3.x = q2.x;

	vec3 i1 = vec3(equal(q3.xxx, x0));
	vec3 i2 = i1 + vec3(equal(q3.yyy, x0));
#endif

	//  x0 = x0 - 0. + 0. * C 
	vec3 x1 = x0 - i1 + 1. * C.xxx;
	vec3 x2 = x0 - i2 + 2. * C.xxx;
	vec3 x3 = x0 - 1. + 3. * C.xxx;

	// Permutations
	i = mod(i, pParam.x ); 
	vec4 p = permute( permute( permute( 
		i.z + vec4(0., i1.z, i2.z, 1. ), pParam.xyz)
		+ i.y + vec4(0., i1.y, i2.y, 1. ), pParam.xyz) 
		+ i.x + vec4(0., i1.x, i2.x, 1. ), pParam.xyz);

	// Gradients
	// ( N*N points uniformly over a square, mapped onto a octohedron.)
	float n_ = 1.0/pParam.w ;
	vec3  ns = n_ * D.wyz - D.xzx ;

	vec4 j = p - pParam.w*pParam.w*floor(p * ns.z *ns.z);  //  mod(p,N*N)

	vec4 x_ = floor(j * ns.z)  ;
	vec4 y_ = floor(j - pParam.w * x_ ) ;    // mod(j,N)

	vec4 x = x_ *ns.x + ns.yyyy;
	vec4 y = y_ *ns.x + ns.yyyy;
	vec4 h = 1. - abs(x) - abs(y);

	vec4 b0 = vec4( x.xy, y.xy );
	vec4 b1 = vec4( x.zw, y.zw );

	vec4 s0 = vec4(lessThan(b0,D.xxxx)) *2. -1.;
	vec4 s1 = vec4(lessThan(b1,D.xxxx)) *2. -1.;
	vec4 sh = vec4(lessThan(h, D.xxxx));

	vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
	vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

	vec3 p0 = vec3(a0.xy,h.x);
	vec3 p1 = vec3(a0.zw,h.y);
	vec3 p2 = vec3(a1.xy,h.z);
	vec3 p3 = vec3(a1.zw,h.w);

#ifdef NORMALISE_GRADIENTS
	p0 *= taylorInvSqrt(dot(p0,p0));
	p1 *= taylorInvSqrt(dot(p1,p1));
	p2 *= taylorInvSqrt(dot(p2,p2));
	p3 *= taylorInvSqrt(dot(p3,p3));
#endif

	// Mix
	vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.);
	m = m * m;
	//used to be 64.
	return 48.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
		dot(p2,x2), dot(p3,x3) ) );
}

vec4 grad4_(float j, vec4 ip)
{
	const vec4 ones = vec4(1.,1.,1.,-1.);
	vec4 p,s;

	p.xyz = floor( fract (vec3(j) * ip.xyz) *pParam.w) * ip.z -1.0;
	p.w = 1.5 - dot(abs(p.xyz), ones.xyz);
	s = vec4(lessThan(p,vec4(0.)));
	p.xyz = p.xyz + (s.xyz*2.-1.) * s.www; 

	return p;
}

float simplexNoise4(vec4 v)
{
	const vec2  C = vec2( 0.138196601125010504, 
		0.309016994374947451); 
	// First corner
	vec4 i  = floor(v + dot(v, C.yyyy) );
	vec4 x0 = v -   i + dot(i, C.xxxx);

	// Other corners

	// Force existance of strict total ordering in sort.
	vec4 q0 = floor(x0 * 1024.0) + vec4( 0., 1./4., 2./4. , 3./4.);
	vec4 q1;
	q1.xy = max(q0.xy,q0.zw);   //  x:z  y:w
	q1.zw = min(q0.xy,q0.zw);

	vec4 q2;
	q2.xz = max(q1.xz,q1.yw);   //  x:y  z:w
	q2.yw = min(q1.xz,q1.yw);

	vec4 q3;
	q3.y = max(q2.y,q2.z);      //  y:z
	q3.z = min(q2.y,q2.z);
	q3.xw = q2.xw;

	vec4 i1 = vec4(lessThanEqual(q3.xxxx, q0));
	vec4 i2 = vec4(lessThanEqual(q3.yyyy, q0));
	vec4 i3 = vec4(lessThanEqual(q3.zzzz, q0));

	//  x0 = x0 - 0. + 0. * C 
	vec4 x1 = x0 - i1 + 1. * C.xxxx;
	vec4 x2 = x0 - i2 + 2. * C.xxxx;
	vec4 x3 = x0 - i3 + 3. * C.xxxx;
	vec4 x4 = x0 - 1. + 4. * C.xxxx;

	// Permutations
	i = mod(i, pParam.x ); 
	float j0 = permute( permute( permute( permute (
		i.w, pParam.xyz) + i.z, pParam.xyz) 
		+ i.y, pParam.xyz) + i.x, pParam.xyz);
	vec4 j1 = permute( permute( permute( permute (
		i.w + vec4(i1.w, i2.w, i3.w, 1. ), pParam.xyz)
		+ i.z + vec4(i1.z, i2.z, i3.z, 1. ), pParam.xyz)
		+ i.y + vec4(i1.y, i2.y, i3.y, 1. ), pParam.xyz)
		+ i.x + vec4(i1.x, i2.x, i3.x, 1. ), pParam.xyz);
	// Gradients
	// ( N*N*N points uniformly over a cube, 
	// mapped onto a 4-octohedron.)
	vec4 ip = pParam ;
	ip.xy *= pParam.w ;
	ip.x  *= pParam.w ;
	ip = vec4(1.,1.,1.,2.) / ip ;

	vec4 p0 = grad4_(j0,   ip);
	vec4 p1 = grad4_(j1.x, ip);
	vec4 p2 = grad4_(j1.y, ip);
	vec4 p3 = grad4_(j1.z, ip);
	vec4 p4 = grad4_(j1.w, ip);

#ifdef NORMALISE_GRADIENTS
	p0 *= taylorInvSqrt(dot(p0,p0));
	p1 *= taylorInvSqrt(dot(p1,p1));
	p2 *= taylorInvSqrt(dot(p2,p2));
	p3 *= taylorInvSqrt(dot(p3,p3));
	p4 *= taylorInvSqrt(dot(p4,p4));
#endif

	// Mix
	vec3 m0 = max(0.6 - vec3(dot(x0,x0), dot(x1,x1), dot(x2,x2)), 0.);
	vec2 m1 = max(0.6 - vec2(dot(x3,x3), dot(x4,x4)            ), 0.);
	m0 = m0 * m0;
	m1 = m1 * m1;
	return 32. * (dot(m0*m0, vec3(dot(p0, x0), dot(p1, x1), dot(p2, x2)))
		+ dot(m1*m1, vec2(dot(p3, x3), dot(p4, x4)))) ;

}












const vec3 grad3[] =
{
	{1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},
	{1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},
	{0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1}
};

const vec4 grad4[] =
{
	{0,1,1,1}, {0,1,1,-1}, {0,1,-1,1}, {0,1,-1,-1},
	{0,-1,1,1}, {0,-1,1,-1}, {0,-1,-1,1}, {0,-1,-1,-1},
	{1,0,1,1}, {1,0,1,-1}, {1,0,-1,1}, {1,0,-1,-1},
	{-1,0,1,1}, {-1,0,1,-1}, {-1,0,-1,1}, {-1,0,-1,-1},
	{1,1,0,1}, {1,1,0,-1}, {1,-1,0,1}, {1,-1,0,-1},
	{-1,1,0,1}, {-1,1,0,-1}, {-1,-1,0,1}, {-1,-1,0,-1},
	{1,1,1,0}, {1,1,-1,0}, {1,-1,1,0}, {1,-1,-1,0},
	{-1,1,1,0}, {-1,1,-1,0}, {-1,-1,1,0}, {-1,-1,-1,0}
};

//int p[] = {
//
//	151,160,137,91,90,15,
//	131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
//	190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
//	88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
//	77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
//	102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
//	135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
//	5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
//	223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
//	129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
//	251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
//	49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
//	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
//};

// perm_simplexをテクスチャ化して持ち込んだほうがいいか、、、非常に遅い



const vec4 simplex[] = 
{
	{0,1,2,3},{0,1,3,2},{0,0,0,0},{0,2,3,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,2,3,0},
	{0,2,1,3},{0,0,0,0},{0,3,1,2},{0,3,2,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,3,2,0},
	{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
	{1,2,0,3},{0,0,0,0},{1,3,0,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,3,0,1},{2,3,1,0},
	{1,0,2,3},{1,0,3,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,0,3,1},{0,0,0,0},{2,1,3,0},
	{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
	{2,0,1,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,0,1,2},{3,0,2,1},{0,0,0,0},{3,1,2,0},
	{2,1,0,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,1,0,2},{0,0,0,0},{3,2,0,1},{3,2,1,0}
};



uniform usampler1D	g_SimplexPermTexture;


#define perm_simplex(i) int( texture( g_SimplexPermTexture, float(i)/511.0/*255.0*/ ).r )


// 2D simplex noise
float Simplex( vec2 p )
{
	
	float n0, n1, n2; // Noise contributions from the three corners
	// Skew the input space to determine which simplex cell we're in
	const float F2 = 0.5*(sqrt(3.0)-1.0);
	float s = ( p.x + p.y ) * F2; // Hairy factor for 2D
	int i = int( floor( p.x + s ) );
	int j = int( floor( p.y + s ) );
	
	const float G2 = (3.0-sqrt(3.0))/6.0;
	float t = (i+j)*G2;
	float X0 = i-t; // Unskew the cell origin back to (x,y) space
	float Y0 = j-t;
	
	const vec2 v0 = vec2( p.x-X0, p.y-Y0 );
	//float x0 = xin-X0; // The x,y distances from the cell origin
	//float y0 = yin-Y0;
	
	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
	
	if( v0.x>v0.y )// lower triangle, XY order: (0,0)->(1,0)->(1,1)
	{
		i1=1; j1=0;
	}
	else	// upper triangle, YX order: (0,0)->(0,1)->(1,1)
	{
		i1=0; j1=1;
	} 
	
	// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
	// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
	// c = (3-sqrt(3))/6
	const vec2 v1	= vec2( v0.x-i1+G2, v0.y-j1+G2 );			// Offsets for middle corner in (x,y) unskewed coords
	const vec2 v2	= vec2( v0.x-1.0+2.0*G2, v0.y-1.0+2.0*G2 );	// Offsets for last corner in (x,y) unskewed coords
	
	// Work out the hashed gradient indices of the three simplex corners
	int ii = i & 511;//255;
	int jj = j & 511;//255;
	int gi0 = perm_simplex( ii+perm_simplex(jj) ) % 12;
	int gi1 = perm_simplex( ii+i1+perm_simplex(jj+j1) ) % 12;
	int gi2 = perm_simplex( ii+1+perm_simplex(jj+1) ) % 12;
	// Calculate the contribution from the three corners
	float t0 = 0.5 - v0.x*v0.x - v0.y*v0.y;//x0*x0-y0*y0;
	if(t0<0)
	{
		n0 = 0.0;
	}
	else
	{
		t0 *= t0;
		n0 = t0 * t0 * dot( grad3[gi0].xy, v0 ); // (x,y) of grad3 used for 2D gradient
	}

	float t1 = 0.5 - v1.x*v1.x - v1.y*v1.y;
	if(t1<0)
	{
		n1 = 0.0;
	}
	else
	{
		t1 *= t1;
		n1 = t1 * t1 * dot( grad3[gi1].xy, v1 );
	}
	float t2 = 0.5 - v2.x*v2.x-v2.y*v2.y;
	if(t2<0) n2 = 0.0;
	else
	{
		t2 *= t2;
		n2 = t2 * t2 * dot( grad3[gi2].xy, v2 );
	}
	
	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1].
	return 70.0 * (n0 + n1 + n2);
	
}


// 3D simplex noise
float Simplex( vec3 p )
{
	float n0, n1, n2, n3; // Noise contributions from the four corners
	// Skew the input space to determine which simplex cell we're in
	const float F3 = 1.0/3.0;
	float s = ( p.x+p.y+p.z ) * F3; // Very nice and simple skew factor for 3D
	
	vec3 ijk = vec3( floor(p.x+s), floor(p.y+s), floor(p.z+s) );
	const float G3 = 1.0/6.0; // Very nice and simple unskew factor, too
	float t = (ijk.x + ijk.y + ijk.z) * G3;//(i+j+k)*G3;
	vec3 XYZ0	= ijk - t;// Unskew the cell origin back to (x,y,z) space	
	vec3 xyz0	= p - XYZ0;// The x,y,z distances from the cell origin

	// For the 3D case, the simplex shape is a slightly irregular tetrahedron.
	// Determine which simplex we are in.
	ivec3 ijk1;	// Offsets for second corner of simplex in (i,j,k) coords
	ivec3 ijk2;	// Offsets for third corner of simplex in (i,j,k) coords

	if( xyz0.x >= xyz0.y ) 
	{
		if( xyz0.y>=xyz0.z )		// X Y Z order
		{
			ijk1 = ivec3(1,0,0);//i1=1; j1=0; k1=0;
			ijk2 = ivec3(1,1,0);//i2=1; j2=1; k2=0;
		}
		else if( xyz0.x>=xyz0.z )	// X Z Y order
		{
			ijk1 = ivec3(1,0,0);//i1=1; j1=0; k1=0;
			ijk2 = ivec3(1,0,1);//i2=1; j2=0; k2=1;
		}
		else						// Z X Y order
		{
			ijk1 = ivec3(0,0,1);//i1=0; j1=0; k1=1;
			ijk2 = ivec3(1,0,1);//i2=1; j2=0; k2=1;
		}
	}
	else { // x0<y0
		if( xyz0.y<xyz0.z )			// Z Y X order
		{
			ijk1 = ivec3(0,0,1);//i1=0; j1=0; k1=1;
			ijk2 = ivec3(0,1,1);//i2=0; j2=1; k2=1;
		}
		else if( xyz0.x<xyz0.z )	// Y Z X order
		{
			ijk1 = ivec3(0,1,0);//i1=0; j1=1; k1=0;
			ijk2 = ivec3(0,1,1);//i2=0; j2=1; k2=1;
		}
		else						// Y X Z order
		{
			ijk1 = ivec3(0,1,0);//i1=0; j1=1; k1=0;
			ijk2 = ivec3(1,1,0);//i2=1; j2=1; k2=0;
		} 
	}
	
	// A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
	// a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
	// a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
	// c = 1/6.
	vec3 xyz1 = xyz0 - ijk1 + G3;	// Offsets for second corner in (x,y,z) coords
	vec3 xyz2 = xyz0 - ijk2 + 2.0*G3;// Offsets for third corner in (x,y,z) coords
	vec3 xyz3 = xyz0 - 1.0 + 3.0*G3;// Offsets for last corner in (x,y,z) coords

	// Work out the hashed gradient indices of the four simplex corners
	int ii = /*i*/int(ijk.x) & 511;//255;
	int jj = /*j*/int(ijk.y) & 511;//255;
	int kk = /*k*/int(ijk.z) & 511;//255;

	//############################## TODO: 2013.10.01 めちゃくちゃおそい。なんで？？？ -> ローカル配列がダメ.pertTexture化で高速化2013.10.02 ############################//
	int gi0 = perm_simplex( ii+perm_simplex( jj+perm_simplex(kk) ) ) % 12;
	int gi1 = perm_simplex( ii+ijk1.x+perm_simplex( jj+ijk1.y+perm_simplex( kk+ijk1.z) ) ) % 12;
	int gi2 = perm_simplex( ii+ijk2.x+perm_simplex( jj+ijk2.y+perm_simplex( kk+ijk2.z) ) ) % 12;
	int gi3 = perm_simplex( ii+1+perm_simplex( jj+1+perm_simplex( kk+1 ) ) ) % 12;

	// Calculate the contribution from the four corners
	float t0 = 0.5 - xyz0.x*xyz0.x - xyz0.y*xyz0.y - xyz0.z*xyz0.z;
	if(t0<0)
	{
		n0 = 0.0;
	}
	else
	{
		t0 *= t0;
		n0 = t0 * t0 * dot( grad3[gi0].xyz, xyz0 );
	}

	float t1 = 0.5 - xyz1.x*xyz1.x - xyz1.y*xyz1.y - xyz1.z*xyz1.z;//x1*x1 - y1*y1 - z1*z1;
	if( t1<0 )
	{
		n1 = 0.0;
	}
	else
	{
		t1 *= t1;
		n1 = t1 * t1 * dot( grad3[gi1].xyz, xyz1 );//vec3(x1, y1, z1) );
	}

	float t2 = 0.5 - xyz2.x*xyz2.x - xyz2.y*xyz2.y - xyz2.z*xyz2.z;//x2*x2 - y2*y2 - z2*z2;
	if(t2<0)
	{
		n2 = 0.0;
	}
	else
	{
		t2 *= t2;
		n2 = t2 * t2 * dot( grad3[gi2].xyz, xyz2 );//vec3(x2, y2, z2) );
	}

	float t3 = 0.5 - xyz3.x*xyz3.x - xyz3.y*xyz3.y - xyz3.z*xyz3.z;//x3*x3 - y3*y3 - z3*z3;
	if(t3<0)
	{
		n3 = 0.0;
	}
	else
	{
		t3 *= t3;
		n3 = t3 * t3 * dot( grad3[gi3].xyz, xyz3 );//vec3(x3, y3, z3) );
	}
	
	// Add contributions from each corner to get the final noise value.
	// The result is scaled to stay just inside [-1,1]
	return 32.0*(n0 + n1 + n2 + n3);
}


// 4D simplex noise
float Simplex( vec4 p )
{
	// The skewing and unskewing factors are hairy again for the 4D case
	const float F4 = (sqrt(5.0)-1.0) / 4.0;
	const float G4 = (5.0-sqrt(5.0)) / 20.0;
	float n0, n1, n2, n3, n4; // Noise contributions from the five corners
	// Skew the (x,y,z,w) space to determine which cell of 24 simplices we're in
	float s = ( p.x + p.y + p.z + p.w ) * F4; // Factor for 4D skewing
	int i = int(floor( p.x + s ));
	int j = int(floor( p.y + s ));
	int k = int(floor( p.z + s ));
	int l = int(floor( p.w + s ));
	float t = (i + j + k + l) * G4; // Factor for 4D unskewing
	float X0 = i - t; // Unskew the cell origin back to (x,y,z,w) space
	float Y0 = j - t;
	float Z0 = k - t;
	float W0 = l - t;
	float x0 = p.x - X0; // The x,y,z,w distances from the cell origin
	float y0 = p.y - Y0;
	float z0 = p.z - Z0;
	float w0 = p.w - W0;
	// For the 4D case, the simplex is a 4D shape I won't even try to describe.
	// To find out which of the 24 possible simplices we're in, we need to
	// determine the magnitude ordering of x0, y0, z0 and w0.
	// The method below is a good way of finding the ordering of x,y,z,w and
	// then find the correct traversal order for the simplex we’re in.
	// First, six pair-wise comparisons are performed between each possible pair
	// of the four coordinates, and the results are used to add up binary bits
	// for an integer index.
	int c1 = (x0 > y0) ? 32 : 0;
	int c2 = (x0 > z0) ? 16 : 0;
	int c3 = (y0 > z0) ? 8 : 0;
	int c4 = (x0 > w0) ? 4 : 0;
	int c5 = (y0 > w0) ? 2 : 0;
	int c6 = (z0 > w0) ? 1 : 0;
	int c = c1 + c2 + c3 + c4 + c5 + c6;
	int i1, j1, k1, l1; // The integer offsets for the second simplex corner
	int i2, j2, k2, l2; // The integer offsets for the third simplex corner
	int i3, j3, k3, l3; // The integer offsets for the fourth simplex corner
	// simplex[c] is a 4-vector with the numbers 0, 1, 2 and 3 in some order.
	// Many values of c will never occur, since e.g. x>y>z>w makes x<z, y<w and x<w
	// impossible. Only the 24 indices which have non-zero entries make any sense.
	// We use a thresholding to set the coordinates in turn from the largest magnitude.
	// The number 3 in the "simplex" array is at the position of the largest coordinate.
	i1 = simplex[c].x>=3 ? 1 : 0;
	j1 = simplex[c].y>=3 ? 1 : 0;
	k1 = simplex[c].z>=3 ? 1 : 0;
	l1 = simplex[c].w>=3 ? 1 : 0;
	// The number 2 in the "simplex" array is at the second largest coordinate.
	i2 = simplex[c].x>=2 ? 1 : 0;
	j2 = simplex[c].y>=2 ? 1 : 0;
	k2 = simplex[c].z>=2 ? 1 : 0;
	l2 = simplex[c].w>=2 ? 1 : 0;
	// The number 1 in the "simplex" array is at the second smallest coordinate.
	i3 = simplex[c].x>=1 ? 1 : 0;
	j3 = simplex[c].y>=1 ? 1 : 0;
	k3 = simplex[c].z>=1 ? 1 : 0;
	l3 = simplex[c].w>=1 ? 1 : 0;
	// The fifth corner has all coordinate offsets = 1, so no need to look that up.
	float x1 = x0 - i1 + G4; // Offsets for second corner in (x,y,z,w) coords
	float y1 = y0 - j1 + G4;
	float z1 = z0 - k1 + G4;
	float w1 = w0 - l1 + G4;
	float x2 = x0 - i2 + 2.0*G4; // Offsets for third corner in (x,y,z,w) coords
	float y2 = y0 - j2 + 2.0*G4;
	float z2 = z0 - k2 + 2.0*G4;
	float w2 = w0 - l2 + 2.0*G4;
	float x3 = x0 - i3 + 3.0*G4; // Offsets for fourth corner in (x,y,z,w) coords
	float y3 = y0 - j3 + 3.0*G4;
	float z3 = z0 - k3 + 3.0*G4;
	float w3 = w0 - l3 + 3.0*G4;
	float x4 = x0 - 1.0 + 4.0*G4; // Offsets for last corner in (x,y,z,w) coords
	float y4 = y0 - 1.0 + 4.0*G4;
	float z4 = z0 - 1.0 + 4.0*G4;
	float w4 = w0 - 1.0 + 4.0*G4;
	// Work out the hashed gradient indices of the five simplex corners
	int ii = i & 511;//255;
	int jj = j & 511;//255;
	int kk = k & 511;//255;
	int ll = l & 511;//255;
	int gi0 = perm_simplex( ii + perm_simplex( jj + perm_simplex( kk + perm_simplex(ll) ) ) ) % 32;
	int gi1 = perm_simplex( ii + i1 + perm_simplex( jj + j1 + perm_simplex( kk + k1 + perm_simplex(ll+l1) ) ) ) % 32;
	int gi2 = perm_simplex( ii + i2 + perm_simplex( jj + j2 + perm_simplex( kk + k2 + perm_simplex(ll+l2) ) ) ) % 32;
	int gi3 = perm_simplex( ii + i3 + perm_simplex( jj + j3 + perm_simplex( kk + k3 + perm_simplex(ll+l3) ) ) ) % 32;
	int gi4 = perm_simplex( ii + 1  + perm_simplex( jj + 1  + perm_simplex( kk + 1  + perm_simplex(ll+1) ) ) ) % 32;
	// Calculate the contribution from the five corners
	float t0 = 0.5 - x0*x0 - y0*y0 - z0*z0 - w0*w0;
	if(t0<0)
	{
		n0 = 0.0;
	}
	else
	{
		t0 *= t0;
		n0 = t0 * t0 * dot( grad4[gi0].xyzw, vec4(x0, y0, z0, w0) );
	}

	float t1 = 0.5 - x1*x1 - y1*y1 - z1*z1 - w1*w1;
	if(t1<0)
	{
		n1 = 0.0;
	}
	else
	{
		t1 *= t1;
		n1 = t1 * t1 * dot( grad4[gi1].xyzw, vec4(x1, y1, z1, w1) );
	}

	float t2 = 0.5 - x2*x2 - y2*y2 - z2*z2 - w2*w2;
	if(t2<0)
	{
		n2 = 0.0;
	}
	else
	{
		t2 *= t2;
		n2 = t2 * t2 * dot( grad4[gi2].xyzw, vec4(x2, y2, z2, w2) );
	}

	float t3 = 0.5 - x3*x3 - y3*y3 - z3*z3 - w3*w3;
	if(t3<0)
	{
		n3 = 0.0;
	}
	else
	{
		t3 *= t3;
		n3 = t3 * t3 * dot( grad4[gi3].xyzw, vec4(x3, y3, z3, w3) );
	}

	float t4 = 0.5 - x4*x4 - y4*y4 - z4*z4 - w4*w4;
	if(t4<0)
	{
		n4 = 0.0;
	}
	else
	{
		t4 *= t4;
		n4 = t4 * t4 * dot( grad4[gi4].xyzw, vec4(x4, y4, z4, w4) );
	}
	// Sum up and scale the result to cover the range [-1,1]
	return 27.0 * (n0 + n1 + n2 + n3 + n4);
}


// fractal Brownian motion
// lacunarity: 周波数の増減率
// gain: 振幅の増減率
float Simplex_fBm( vec2 p, int octaves, float lacunarity, float gain )
{
	float amplitude = 1.0;
	float frequency	= 1.0;
	float amp_total	= 0.0;
	float total		= 0.0f;

	for( int i=0; i<octaves; ++i )
	{
		total		+= amplitude * Simplex( p*frequency );
		amp_total	+= amplitude;

		amplitude	*= gain;
		frequency	*= lacunarity;
	}

	return total / amp_total;
}


float Simplex_fBm( vec2 p, float octaves, float lacunarity, float gain )
{
	float amplitude = 1.0;
	float frequency	= 1.0;
	float amp_total	= 0.0;
	float total		= 0.0f;
	float floor_oct	= floor(octaves);

	for( int i=0; i<ceil(octaves); ++i )
	{
		float w = mix( 0.0, 1.0, octaves - float(i) );
		amplitude	*= w;
		total		+= amplitude * Simplex( p*frequency );
		amp_total	+= amplitude;

		amplitude	*= gain/w;
		frequency	*= lacunarity;
	}

	return total / amp_total;
}





float Simplex_fBm( vec3 p, int octaves, float lacunarity, float gain )
{
	float amplitude = 1.0;
	float frequency	= 1.0;
	float amp_total	= 0.0;
	float total		= 0.0f;

	for( int i=0; i<octaves; ++i )
	{
		total		+= amplitude * Simplex( p*frequency );
		amp_total	+= amplitude;

		amplitude	*= gain;
		frequency	*= lacunarity;
	}

	return total / amp_total;
}



float Simplex_fBm( vec3 p, float octaves, float lacunarity, float gain )
{
	//float amplitude = 1.0;
	//float frequency	= 1.0;
	//float amp_total	= 0.0;
	//float total		= 0.0f;
	//float floor_oct	= floor(octaves);

	//for( int i=0; i<floor_oct; ++i )
	//{
	//	total		+= amplitude * Simplex( p*frequency );
	//	amp_total	+= amplitude;

	//	amplitude	*= gain;
	//	frequency	*= lacunarity;
	//}

	//amplitude *=  mix( 0.0, 1.0, octaves - floor_oct );
	//total		+= amplitude * Simplex( p*frequency );
	//amp_total	+= amplitude;

	//return total / amp_total;

	float amplitude = 1.0;
	float frequency	= 1.0;
	float amp_total	= 0.0;
	float total		= 0.0f;
	float floor_oct	= floor(octaves);

	for( int i=0; i<ceil(octaves); ++i )
	{
		float w = mix( 0.0, 1.0, octaves - float(i) );
		amplitude	*= w;
		total		+= amplitude * Simplex( p*frequency );//simplexNoise3( p*frequency );//
		amp_total	+= amplitude;

		amplitude	*= gain/w;
		frequency	*= lacunarity;
	}

	return total / amp_total;
}




float Simplex_fBm( vec4 p, int octaves, float lacunarity, float gain )
{
	float amplitude = 1.0;
	float frequency	= 1.0;
	float amp_total	= 0.0;
	float total		= 0.0f;

	for( int i=0; i<octaves; ++i )
	{
		total		+= amplitude * Simplex( p*frequency );
		amp_total	+= amplitude;
		
		amplitude	*= gain;
		frequency	*= lacunarity;
	}

	return total / amp_total;
}



float Simplex_fBm( vec4 p, float octaves, float lacunarity, float gain )
{
	float amplitude = 1.0;
	float frequency	= 1.0;
	float amp_total	= 0.0;
	float total		= 0.0f;
	float floor_oct	= floor(octaves);

	for( int i=0; i<ceil(octaves); ++i )
	{
		float w = mix( 0.0, 1.0, octaves - float(i) );
		amplitude	*= w;
		total		+= amplitude * Simplex( p*frequency );
		amp_total	+= amplitude;

		amplitude	*= gain/w;
		frequency	*= lacunarity;
	}

	return total / amp_total;
}





//uniform float time;
//varying vec2 v_texCoord2D;
//varying vec3 v_texCoord3D;
//varying vec4 v_color;
//
//void main(void)
//{
//	vec3 v = vec3(2.0 * v_texCoord3D.xyz * (2.0 + sin(0.5*time)));  
//	float n = simplexNoise3(v);
//	gl_FragColor = v_color * vec4(0.5 + 0.5 * vec3(n, n, n), 1.0);
//}

#endif	// SIMPLEX_NOISE_GLSL //
