// Include Files


// Uniform Locations
//uniform uint	g_ArrayLength;
uniform bool	g_OutputBlockSum;


// Shader Storage Buffers
layout( std430, binding = 0 ) buffer inputdata
{
	uint g_idata[];
};

layout( std430, binding = 1 ) buffer outputdata
{
	uint g_odata[];
};

layout( std430, binding = 2 ) buffer blockSum
{
	uint g_BlockSums[];
};



// Workgroup Layout
layout( local_size_x=THREADS_PER_BLOCK ) in;


// Shared Variables
const int g_PartialArrayLength = int( 2 * gl_WorkGroupSize.x );
shared uint	temp[g_PartialArrayLength];  // allocated on invocation  


// Main Kernel
void main()
{
	int thid	= int( gl_LocalInvocationID.x );//threadIdx.x;
	int offset	= 1;

	// load input into shared memory
	temp[2 * thid]		= g_idata[2 * gl_GlobalInvocationID.x];
	temp[2 * thid + 1]	= g_idata[2 * gl_GlobalInvocationID.x + 1];

	//===================	build sum in place up the tree	===================//
	for( int d=g_PartialArrayLength>>1; d>0; d>>=1 )
	{
		barrier();// __syncthreads();

		if( thid < d )
		{
			int ai = offset*( 2*thid+1 )-1;
			int bi = offset*( 2*thid+2 )-1;

			temp[bi] += temp[ai];
		}
		offset *= 2;
	}

	if( thid == 0 )
	{
		if( g_OutputBlockSum )
		{
			g_BlockSums[gl_WorkGroupID.x]	=  temp[g_PartialArrayLength - 1];//float( gl_WorkGroupID.x );// Store Per Block Sums for Large array
		}

		temp[g_PartialArrayLength - 1] = 0;// clear the last element 
	}

	//===================	traverse down tree & build scan	===================//
	for( int d = 1; d < g_PartialArrayLength; d *= 2 )
	{
		offset >>= 1;
		barrier();// __syncthreads();

		if( thid < d )
		{
			int ai = offset*( 2*thid+1 )-1;
			int bi = offset*( 2*thid+2 )-1;

			uint t		= temp[ai];
			temp[ai]	= temp[bi];
			temp[bi]	+= t;
		}
	}

	barrier();// __syncthreads();
	

	g_odata[2 * gl_GlobalInvocationID.x]		= temp[2 * thid]; // write results to device memory  
	g_odata[2 * gl_GlobalInvocationID.x + 1]	= temp[2 * thid + 1];

}