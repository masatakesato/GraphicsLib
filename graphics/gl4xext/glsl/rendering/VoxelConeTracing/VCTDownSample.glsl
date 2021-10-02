// Include Files



// Uniform Locations
uniform sampler3D		g_TexSource3D;


// Random Accessible Buffers
layout( binding = 0, rgba32f )	uniform readonly image3D	g_TexSource;
layout( binding = 1, rgba32f )	uniform writeonly image3D	g_TexDest;


// Constant Variables
const ivec3	g_SourceResolution	= imageSize( g_TexSource );
const vec3	g_SourceVoxelSize	= 1.0 / vec3( g_SourceResolution );
const ivec3	g_DestResolution		= g_SourceResolution / 2;// half size of Source Texture

const ivec3 Ns[] =
{
	{0,0,0},
	{0,0,1},
	{0,1,0},
	{0,1,1},
	{1,0,0},
	{1,0,0},
	{1,1,0},
	{1,1,1},
};


// Workgroup Layout
layout( local_size_x=8, local_size_y=8, local_size_z=8 ) in;// 2D layout. x: numSamplesInSlice dimension, y: numEpipolarSlices dimension



// Main Kernel
void main()
{	
	ivec3 storePos = ivec3( gl_GlobalInvocationID.xyz );// データ格納先のアドレス
	if( storePos.x >= g_DestResolution.x || storePos.y >= g_DestResolution.y || storePos.z >= g_DestResolution.z )	return;
	
	ivec3 centerPos	= storePos * 2;
	vec3 texCoord	= ( centerPos + vec3(0.5) ) * g_SourceVoxelSize;


	// 8近傍のテクセル値を取得し、平均値を計算する
	//vec4 avgVal	= 
	//	(
	//	imageLoad( g_TexSource, centerPos + ivec3(0,0,0) ).xyzw +
	//	imageLoad( g_TexSource, centerPos + ivec3(0,0,1) ).xyzw +
	//	imageLoad( g_TexSource, centerPos + ivec3(0,1,0) ).xyzw +
	//	imageLoad( g_TexSource, centerPos + ivec3(0,1,1) ).xyzw +
	//	imageLoad( g_TexSource, centerPos + ivec3(1,0,0) ).xyzw +
	//	imageLoad( g_TexSource, centerPos + ivec3(1,0,1) ).xyzw +
	//	imageLoad( g_TexSource, centerPos + ivec3(1,1,0) ).xyzw +
	//	imageLoad( g_TexSource, centerPos + ivec3(1,1,1) ).xyzw ) / 8.0;
		

	
	//vec4 avgVal	= texture( g_TexSource3D, texCoord ).xyzw;

	//vec4 avgVal		= vec4(0);
	//float numSamples	= 0;

	//for( int i=0; i<8; ++i )
	//{
	//	vec4 sampleColor	= imageLoad( g_TexSource, centerPos + Ns[i] ).xyzw;
	//	avgVal				+= sampleColor;

	//	numSamples			+= float( sampleColor.w>0.0 );
	//}
	//
	//avgVal	= numSamples>0.0 ? avgVal/numSamples : vec4(0);

	//imageStore( g_TexDest, storePos, avgVal );//vec4( vec3(storePos) / vec3(g_DestResolution), 1.0) );//




	vec4 srcColor0	= imageLoad( g_TexSource, centerPos + ivec3(0,0,0) ).xyzw;
	vec4 srcColor1	= imageLoad( g_TexSource, centerPos + ivec3(0,0,1) ).xyzw;
	vec4 srcColor2	= imageLoad( g_TexSource, centerPos + ivec3(0,1,0) ).xyzw;
	vec4 srcColor3	= imageLoad( g_TexSource, centerPos + ivec3(0,1,1) ).xyzw;
	vec4 srcColor4	= imageLoad( g_TexSource, centerPos + ivec3(1,0,0) ).xyzw;
	vec4 srcColor5	= imageLoad( g_TexSource, centerPos + ivec3(1,0,1) ).xyzw;
	vec4 srcColor6	= imageLoad( g_TexSource, centerPos + ivec3(1,1,0) ).xyzw;
	vec4 srcColor7	= imageLoad( g_TexSource, centerPos + ivec3(1,1,1) ).xyzw;

	float totalOpacity	= srcColor0.w + srcColor1.w + srcColor2.w + srcColor3.w + srcColor4.w + srcColor5.w + srcColor6.w + srcColor7.w;

	vec3 color		=	( srcColor0.xyz * srcColor0.w +
						srcColor1.xyz * srcColor1.w +
						srcColor2.xyz * srcColor2.w +
						srcColor3.xyz * srcColor3.w +
						srcColor4.xyz * srcColor4.w +
						srcColor5.xyz * srcColor5.w +
						srcColor6.xyz * srcColor6.w +
						srcColor7.xyz * srcColor7.w ) / ( totalOpacity + 0.00001 );
	float alpha		= totalOpacity / 8.0;

	imageStore( g_TexDest, storePos, vec4( color, alpha ) );
}
