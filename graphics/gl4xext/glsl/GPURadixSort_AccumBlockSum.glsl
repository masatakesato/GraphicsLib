// Include Files


// Uniform Locations


// Shader Storage Buffers
layout( std430, binding = 0 ) buffer inoutdata
{
	uint g_iodata[];
};

layout( std430, binding = 1 ) buffer blocksumdata
{
	uint blocksums[];
};



// Workgroup Layout
layout( local_size_x=THREADS_PER_BLOCK ) in;


// Main Kernel
void main()
{
	uint destIdx	= gl_GlobalInvocationID.x + gl_WorkGroupSize.x;
	g_iodata[destIdx]	+= blocksums[gl_WorkGroupID.x+1];

	// Debug Output
	//g_iodata[destIdx]	= -1;	// Check memory access position
	//g_iodata[gl_GlobalInvocationID.x]	+= blocksums[gl_WorkGroupID.x];
}