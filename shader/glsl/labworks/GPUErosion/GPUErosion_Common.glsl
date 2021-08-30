#ifndef	GPU_EROSION_COMMON_GLSL
#define	GPU_EROSION_COMMON_GLSL



struct ErosionAttribs
{
	vec4	TexSize;

};


uniform ubErosionAttribs
{
	ErosionAttribs	g_ErosionAttribs;
};



#endif
