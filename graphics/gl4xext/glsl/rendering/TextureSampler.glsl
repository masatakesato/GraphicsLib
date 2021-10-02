
// 13x13 Gaussain Kernel
const int g_KernelSize13	=13;
/*static*/const float g_TexOffset13[ g_KernelSize13 ] =
{
	-6.0,
	-5.0,
	-4.0,
	-3.0,
	-2.0,
	-1.0,
	0.0,
	1.0,
	2.0,
	3.0,
	4.0,
	5.0,
	6.0,
};



/*static*/const float g_GaussianWeight13[13] =// ガウスフィルタカーネル
{
	0.002216,
	0.008764,
	0.026995,
	0.064759,
	0.120985,
	0.176033,
	0.199471,
	0.176033,
	0.120985,
	0.064759,
	0.026995,
	0.008764,
	0.002216,
};






// 4x4のピクセル領域をサンプリングする（バイリニア）
vec4 Sample4x4( in sampler2D tex, in vec2 texcoord, in vec2 halfpixeloffset )
{
	const vec4 bl	= texture( tex, texcoord + vec2( -halfpixeloffset.x, -halfpixeloffset.y ) ).xyzw;// Bottom Left
	const vec4 br	= texture( tex, texcoord + vec2( +halfpixeloffset.x, -halfpixeloffset.y ) ).xyzw;// BottomRight
	const vec4 tr	= texture( tex, texcoord + vec2( +halfpixeloffset.x, +halfpixeloffset.y ) ).xyzw;// Top Right
	const vec4 tl	= texture( tex, texcoord + vec2( -halfpixeloffset.x, +halfpixeloffset.y ) ).xyzw;// Top Left
		
	return 0.25 * ( bl + br + tr + tl );
}



// 13x13 Gaussain Filter
vec4 GaussianBlur13( in sampler2D tex, in vec2 texcoord, in vec2 direction )
{ 
	vec4 result	= vec4( 0.0);

	for( int i=0; i<g_KernelSize13; ++i )
	{ 
		result += texture( tex, texcoord + direction * g_TexOffset13[i] ).xyzw * g_GaussianWeight13[i];
	}

	return result;
}