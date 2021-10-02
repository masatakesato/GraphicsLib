#ifndef	VECTOR_FORMAT_CONVERSION_GLSL
#define	VECTOR_FORMAT_CONVERSION_GLSL



vec4 convRGBA8ToVec4( uint val )
{
	return vec4(
		float( ( val & 0x000000FF ) ),
		float( ( val & 0x0000FF00 )	>> 8U ),
		float( ( val & 0x00FF0000 )	>> 16U ),
		float( ( val & 0xFF000000 )	>> 24U ) );
}



uint convVec4ToRGBA8( vec4 val )
{
	return	( uint(val.w) & 0x000000FF ) << 24U	|
			( uint(val.z) & 0x000000FF ) << 16U	|
			( uint(val.y) & 0x000000FF ) << 8U	|
			( uint(val.x) & 0x000000FF );
}



uvec3 convUINTXYZ10ToVec3( uint val )
{
    return	uvec3(
		uint( ( val & 0x000003FF ) ),
		uint( ( val & 0x000FFC00 ) >> 10U ),
		uint( ( val & 0x3FF00000 ) >> 20U ) );
}



uint convVec3ToUINTXYZ10( uvec3 val )
{
    return	( uint(val.z) & 0x000003FF ) << 20U	|
			( uint(val.y) & 0x000003FF ) << 10U	|
			( uint(val.x) & 0x000003FF );
}




#endif	// VECTOR_FORMAT_CONVERSION_GLSL //
