#ifndef	COLOR_BLEND_MACROS
#define	COLOR_BLEND_MACROS



////#define ChannelBlend_Normal(A,B)     ( float(A))
////#define ChannelBlend_Lighten(A,B)    ( float((B > A) ? B:A))
////#define ChannelBlend_Darken(A,B)     ( float((B > A) ? A:B))
////#define ChannelBlend_Multiply(A,B)   ( float((A * B) / 1.0))
////#define ChannelBlend_Average(A,B)    ( float((A + B) / 2))
////#define ChannelBlend_Add(A,B)        ( float(min(1.0, (A + B))))
////#define ChannelBlend_Subtract(A,B)   ( float((A + B < 1.0) ? 0:(A + B - 1.0)))





//#define ColorBlend_Buffer(T,A,B,M)      (T)[0] = ChannelBlend_##M((A)[0], (B)[0]),	\
//                                        (T)[1] = ChannelBlend_##M((A)[1], (B)[1]),	\
//                                        (T)[2] = ChannelBlend_##M((A)[2], (B)[2])


vec4 ColorBlend_Normal( vec4 A, vec4 B )
{
	return A;
}


vec4 ColorBlend_Lighten( vec4 A, vec4 B )
{
	return vec4( max(A.x, B.x), max(A.y, B.y), max(A.z, B.z), max(A.w, B.w) );
}


vec4 ColorBlend_Darken( vec4 A, vec4 B )
{
	return vec4( min(A.x, B.x), min(A.y, B.y), min(A.z, B.z), min(A.w, B.w) );
}


vec4 ColorBlend_Multiply( vec4 A, vec4 B )
{
	return A * B;
}


vec4 ColorBlend_Average( vec4 A, vec4 B )
{
	return 0.5 * (A + B);
}


vec4 ColorBlend_Add( vec4 A, vec4 B )
{
	return A + B;
}


vec4 ColorBlend_Subtract( vec4 A, vec4 B )
{
	return vec4( 
				(A.x + B.x < 1.0) ? 0 : (A.x + B.x - 1.0),
				(A.y + B.y < 1.0) ? 0 : (A.y + B.y - 1.0),
				(A.z + B.z < 1.0) ? 0 : (A.z + B.z - 1.0),
				(A.w + B.w < 1.0) ? 0 : (A.w + B.w - 1.0)
				);
}


vec4 ColorBlend_Difference( vec4 A, vec4 B )
{
	 return abs(A - B);
}


vec4 ColorBlend_Negation( vec4 A, vec4 B )
{
	return 1.0 - abs(1.0 - A - B);
}

  
//#define ChannelBlend_Alpha(A,B,O)    (float(O * A + (1 - O) * B))
//#define ChannelBlend_AlphaF(A,B,F,O) (ChannelBlend_Alpha(F(A,B),A,O))


vec4 ColorBlend_Screen( vec4 A, vec4 B )
{
	//( float(1.0 - (((1.0 - A) * (1.0 - B)) >> 8)))

	return vec4( 
				1.0 - (1.0 - A.x) * ( 1.0 - B.x ),
				1.0 - (1.0 - A.y) * ( 1.0 - B.y ),
				1.0 - (1.0 - A.z) * ( 1.0 - B.z ),
				1.0 - (1.0 - A.w) * ( 1.0 - B.w )
				);
}



/*
vec4 ColorBlend_Exclusion( vec4 A, vec4 B )
{
	( float(A + B - 2 * A * B / 1.0))
}
*/

vec4 ColorBlend_Overlay( vec4 A, vec4 B )
{
	//( float((B < 0.5) ? (2 * A * B / 1.0):(1.0 - 2 * (1.0 - A) * (1.0 - B) / 1.0)))
	return vec4(
				(B.x < 0.5) ? (2.0 * A.x * B.x) : (1.0 - 2.0 * (1.0 - A.x) * (1.0 - B.x) ),
				(B.y < 0.5) ? (2.0 * A.y * B.y) : (1.0 - 2.0 * (1.0 - A.y) * (1.0 - B.y) ),
				(B.z < 0.5) ? (2.0 * A.z * B.z) : (1.0 - 2.0 * (1.0 - A.z) * (1.0 - B.z) ),
				(B.w < 0.5) ? (2.0 * A.w * B.w) : (1.0 - 2.0 * (1.0 - A.w) * (1.0 - B.w) ) 
				);

	//return vec4(
	//			( 0.5*(B.x+A.x) < 0.5) ? (2.0 * A.x * B.x) : 2.0 * (A.x+B.x - A.x*B.x) - 1.0,//(1.0 - 2.0 * (1.0 - A.x) * (1.0 - B.x) ),
	//			( 0.5*(B.y+A.y) < 0.5) ? (2.0 * A.y * B.y) : 2.0 * (A.y+B.y - A.y*B.y) - 1.0,//(1.0 - 2.0 * (1.0 - A.y) * (1.0 - B.y) ),
	//			( 0.5*(B.z+A.z) < 0.5) ? (2.0 * A.z * B.z) : 2.0 * (A.z+B.z - A.z*B.z) - 1.0,//(1.0 - 2.0 * (1.0 - A.z) * (1.0 - B.z) ),
	//			( 0.5*(B.w+A.w) < 0.5) ? (2.0 * A.w * B.w) : 2.0 * (A.w+B.w - A.w*B.w) - 1.0//(1.0 - 2.0 * (1.0 - A.w) * (1.0 - B.w) ) 
	//			);
}

/*
vec4 ColorBlend_SoftLight( vec4 A, vec4 B )
{
	return vec4(
				( float((B.x < 0.5)?(2*((A.x>>1)+64))*(float(B.x)/1.0):(1.0-(2*(1.0-((A.x>>1)+64))*float(1.0-B.x)/1.0)))),
				( float((B.y < 0.5)?(2*((A.y>>1)+64))*(float(B.y)/1.0):(1.0-(2*(1.0-((A.y>>1)+64))*float(1.0-B.y)/1.0)))),
				( float((B.z < 0.5)?(2*((A.z>>1)+64))*(float(B.z)/1.0):(1.0-(2*(1.0-((A.z>>1)+64))*float(1.0-B.z)/1.0)))),
				( float((B.w < 0.5)?(2*((A.w>>1)+64))*(float(B.w)/1.0):(1.0-(2*(1.0-((A.w>>1)+64))*float(1.0-B.w)/1.0))))
	 );
}

/*
vec4 ColorBlend_HardLight( vec4 A, vec4 B )
{
	return ColorBlend_Overlay( B, A );
}

/*
vec4 ColorBlend_ColorDodge( vec4 A, vec4 B )
{
	return vec4(
				( float((B == 1.0) ? B:min(1.0, ((A << 8 ) / (1.0 - B)))))
				);
}
*/

vec4 ColorBlend_ColorBurn( vec4 A, vec4 B )
{
	return vec4(
				( float((B.x==0) ? B.x : max(0.0, (1.0 - ((1.0 - A.x)) / B.x)))),
				( float((B.y==0) ? B.y : max(0.0, (1.0 - ((1.0 - A.y)) / B.y)))),
				( float((B.z==0) ? B.z : max(0.0, (1.0 - ((1.0 - A.z)) / B.z)))),
				( float((B.w==0) ? B.w : max(0.0, (1.0 - ((1.0 - A.w)) / B.w))))
				);
}

/*
vec4 ColorBlend_LinearDodge( vec4 A, vec4 B )
{
	(ChannelBlend_Add(A,B))
}


vec4 ColorBlend_LinearBurn( vec4 A, vec4 B )
{
	(ChannelBlend_Subtract(A,B))
}


vec4 ColorBlend_LinearLight( vec4 A, vec4 B )
{
	( float(B < 0.5)?ChannelBlend_LinearBurn(A,(2 * B)):ChannelBlend_LinearDodge(A,(2 * (B - 0.5))))
}


vec4 ColorBlend_VividLight( vec4 A, vec4 B )
{
	( float(B < 0.5)?ChannelBlend_ColorBurn(A,(2 * B)):ChannelBlend_ColorDodge(A,(2 * (B - 0.5))))
}


vec4 ColorBlend_PinLight( vec4 A, vec4 B )
{
	( float(B < 0.5)?ChannelBlend_Darken(A,(2 * B)):ChannelBlend_Lighten(A,(2 * (B - 0.5))))
}


vec4 ColorBlend_HardMix( vec4 A, vec4 B )
{
	( float((ChannelBlend_VividLight(A,B) < 0.5) ? 0:1.0))
}


vec4 ColorBlend_Reflect( vec4 A, vec4 B )
{
	( float((B == 1.0) ? B:min(1.0, (A * A / (1.0 - B)))))
}


vec4 ColorBlend_Glow( vec4 A, vec4 B )
{
	(ChannelBlend_Reflect(B,A))
}


vec4 ColorBlend_Phoenix( vec4 A, vec4 B )
{
	(float(min(A,B) - max(A,B) + 1.0))
}
*/





#endif	// COLOR_BLEND_MACROS //