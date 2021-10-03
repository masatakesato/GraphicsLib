#version 430

uniform float roll;

layout (binding = 0) uniform writeonly image2D destTex;
layout (local_size_x = 16, local_size_y = 16) in;


void main()
{
	ivec2 storePos = ivec2( gl_GlobalInvocationID.xy );//ivec2( 15, 15 );//

	float localCoef = length(vec2(ivec2(gl_LocalInvocationID.xy)-8)/8.0);
	float globalCoef = sin(float(gl_WorkGroupID.x+gl_WorkGroupID.y)*0.1 + roll)*0.5;
	imageStore(destTex, storePos, vec4(1.0-globalCoef*localCoef, 0.0, 0.0, 0.0));

}




//uniform float roll;
//
//layout(binding=0) writeonly uniform image2D destTex;// 注意!: GL_TEXTURE_1Dでもimage2Dで対応できる. image1Dは何やっても動かない
//layout (local_size_x = 16, local_size_y = 1) in;
//
//
//void main()
//{
//	ivec2 storePos = ivec2( gl_GlobalInvocationID );
//	imageStore(destTex, storePos, vec4(1.0, 0.0, 1.0, 1.0));
//}
