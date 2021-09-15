#ifndef PERLIN_NOISE_CPU
#define	PERLIN_NOISE_CPU


#include	"MathLib.h"



// 3D Perlin noise class
class PerlinNoiseCPU
{
private:

	static OreOreLib::Vec3i	grad3[];
	static int				p[];

	// To remove the need for index wrapping, double the permutation table length
	int				perm[512];


	// This method is a *lot* faster than using (int)Math.floor(x)
	int fastfloor(double x)
	{
		return x>0 ? (int)x : (int)x-1;
	}

	double dot(int g[], double x, double y, double z)
	{
		return (double)g[0]*x + (double)g[1]*y + (double)g[2]*z;
	}

	double mix(double a, double b, double t)
	{
		return (1.0-t)*a + t*b;
	}

	double fade(double t)
	{
		return t*t*t*(t*(t*6.0-15.0)+10.0);
	}


public:

	PerlinNoiseCPU();
	~PerlinNoiseCPU();

	// Classic Perlin noise, 3D version
	double noise( double x, double y, double z )
	{
		// Find unit grid cell containing point
		int X = fastfloor(x);
		int Y = fastfloor(y);
		int Z = fastfloor(z);

		// Get relative xyz coordinates of point within that cell
		x = x - X;
		y = y - Y;
		z = z - Z;

		// Wrap the integer cells at 255 (smaller integer period can be introduced here)
		X = X & 255;
		Y = Y & 255;
		Z = Z & 255;

		// Calculate a set of eight hashed gradient indices
		int gi000 = perm[X+perm[Y+perm[Z]]] % 12;
		int gi001 = perm[X+perm[Y+perm[Z+1]]] % 12;
		int gi010 = perm[X+perm[Y+1+perm[Z]]] % 12;
		int gi011 = perm[X+perm[Y+1+perm[Z+1]]] % 12;
		int gi100 = perm[X+1+perm[Y+perm[Z]]] % 12;
		int gi101 = perm[X+1+perm[Y+perm[Z+1]]] % 12;
		int gi110 = perm[X+1+perm[Y+1+perm[Z]]] % 12;
		int gi111 = perm[X+1+perm[Y+1+perm[Z+1]]] % 12;

		// The gradients of each corner are now:
		// g000 = grad3[gi000];
		// g001 = grad3[gi001];
		// g010 = grad3[gi010];
		// g011 = grad3[gi011];
		// g100 = grad3[gi100];
		// g101 = grad3[gi101];
		// g110 = grad3[gi110];
		// g111 = grad3[gi111];

		// Calculate noise contributions from each of the eight corners
		double n000= dot( grad3[gi000].xyz, x, y, z );
		double n100= dot( grad3[gi100].xyz, x-1, y, z );
		double n010= dot( grad3[gi010].xyz, x, y-1, z );
		double n110= dot( grad3[gi110].xyz, x-1, y-1, z );
		double n001= dot( grad3[gi001].xyz, x, y, z-1 );
		double n101= dot( grad3[gi101].xyz, x-1, y, z-1 );
		double n011= dot( grad3[gi011].xyz, x, y-1, z-1 );
		double n111= dot( grad3[gi111].xyz, x-1, y-1, z-1 );

		// Compute the fade curve value for each of x, y, z
		double u = fade(x);
		double v = fade(y);
		double w = fade(z);

		// Interpolate along x the contributions from each of the corners
		double nx00 = mix(n000, n100, u);
		double nx01 = mix(n001, n101, u);
		double nx10 = mix(n010, n110, u);
		double nx11 = mix(n011, n111, u);

		// Interpolate the four results along y
		double nxy0 = mix(nx00, nx10, v);
		double nxy1 = mix(nx01, nx11, v);

		// Interpolate the two last results along z
		double nxyz = mix(nxy0, nxy1, w);
		return nxyz;
	}

};


#endif	// PERLIN_NOISE_CPU //