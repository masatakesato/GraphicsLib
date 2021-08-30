#ifndef SIMPLEX_NOISE_HELPER_FUNC_H
#define SIMPLEX_NOISE_HELPER_FUNC_H



float permute(float x)
{
	return floor(mod(((x*34.0)+1.0)*x, 289.0));
}


vec3 perm3d(vec3 x)
{
	return mod(((x*34.0)+1.0)*x, 289.0);
}

vec4 perm4d(vec4 x)
{
	return mod(((x*34.0)+1.0)*x, 289.0);
}


float taylorInvSqrt(float r)
{
	return 1.79284291400159 - 0.85373472095314 * r;
}

vec4 taylorInvSqrt(vec4 r)
{
	return 1.79284291400159 - 0.85373472095314 * r;
}







#endif