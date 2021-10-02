#ifndef	NOISE_FUNCTION_H
#define	NOISE_FUNCTION_H



const int permutation[256] = int[256]
(
	151,160,137,91,90,15,
	131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
	190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
	88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
	77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
	102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
	135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
	5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
	223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
	129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
	251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
	49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
);



uniform sampler1D	permTexture;	// 256x1
uniform sampler2D	permTexture2d;	// 256x256
uniform sampler1D	permGradTexture;	// rgba8. 16x1



int perm( int x )// xは正規化座標
{
	return permutation[ int(mod(x,256)) ];//texture1D( permTexture, vec2(x/256.0, 0.5) ).x * 256.0;
}

/*
vec4 GenerateParmTexture2d( vec2 p )
{
	p *= 256.0;
	int A	= perm( p.x ) + p.y;
	int AA	= perm( A );
	int AB	= perm( A + 1 );
	int B	= perm( p.x + 1 ) + p.y;
	int BA	= perm( B );
	int BB	= perm( B + 1 );
	
	return vec4(AA, AB, BA, BB) / 255.0;
}*/

/*
vec3 GeneratePermGradTexture( float p)
{
	return g[ permutation[p*256] % 16 ];
}
*/

vec3 fade( vec3 t )
{
	return t * t * t * (t * (t * 6.0 - vec3(15.0,15.0,15.0) ) + vec3(10.0,10.0,10.0) ); // new curve
	  //return t * t * (3.0 - 2.0 * t); // old curve
}


float perm( float x )// xは正規化座標
{
	return texture( permTexture, x ).x;
}


vec4 perm2d( vec2 p )
{
	return texture( permTexture2d, p );
}

/*
float grad( float x, vec3 p )
{
	return dot( texture(gradTexture, x*16).xyz, p );
}
*/

float gradperm( float x, vec3 p )
{
	return dot( texture(permGradTexture, x).xyz, p );
}





float inoise( in vec3 p )
{
	vec3 P	= mod( floor(p), 256.0 );
	p		-= floor(p);
	vec3 f	= fade(p);
	

	P	= P/256.0;
	const float one	= 1.0 / 256.0;

	// HASH COORDINATES OF THE 8 CORNERS
	vec4 AA	= perm2d(P.xy) + P.z;

	// AND ADD BLENDED RESULTS FROM 8 CORNERS OF CUBE

	return mix(	mix(	mix(	gradperm(AA.x, p),
								gradperm(AA.z, p + vec3(-1.0, 0.0, 0.0)), f.x),
						mix(	gradperm(AA.y, p + vec3(0.0, -1.0, 0.0)),
								gradperm(AA.w, p + vec3(-1.0, -1.0, 0.0)), f.x), f.y),

				mix(	mix(	gradperm(AA.x+one, p + vec3(0.0, 0.0, -1.0)),
								gradperm(AA.z+one, p + vec3(-1.0, 0.0, -1.0)), f.x),
						mix(	gradperm(AA.y+one, p + vec3(0.0, -1.0, -1.0)),
								gradperm(AA.w+one, p + vec3(-1.0, -1.0, -1.0)), f.x), f.y),
			  f.z);
	
	
}



/*
float rand(vec2 co)
{
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
*/



#endif	// NOISE_FUNCTION_H //
