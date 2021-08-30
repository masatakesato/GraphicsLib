// Include Files


// Uniform Locations
uniform uint	threads;
uniform uint	stride;
uniform uint	size;


// Random Accessible Buffers
//layout( binding = 0 )	uniform writeonly image2D	g_TexDest;
layout( std430, binding = 0 ) buffer ssbFloatArray
{
	float fArray[];
};


// TODO: need read/write 


// Constant Variables



// Helper Functions
void Comparator( inout float keyA, inout float keyB, uint dir )
{
	if( uint(keyA > keyB) == dir )
	{
		float temp = keyA;
		keyA = keyB;
		keyB = temp;
	}
}


// Workgroup Layout
layout( local_size_x=THREADS_PER_BLOCK, local_size_y=1 ) in;



// Main Kernel
//void main()
//{
//	uint x	= gl_GlobalInvocationID.x;
//	if( x >= threads )
//		return;
//
//	uint	i = x +( x & ~( stride-1 ) ),
//			j = x + (x & ~(stride-1)) + stride,
//			dir = x & size;
//
//	float val_i	= fArray[i];
//	float val_j	= fArray[j];
//
//	Comparator( val_i, val_j, dir );
//	
//	fArray[i]	= val_i;
//	fArray[j]	= val_j;
//
//
//	// for Debug Output
//	//fArray[x] += 0.1;
//}


void main()
{
	//if( gl_GlobalInvocationID.x >= threads )
	//	return;


	const uint j = stride;
	const uint k = size;

	uint i, ixj; /* Sorting partners: i and ixj */
	//i = gl_GlobalInvocationID.x;//threadIdx.x + blockDim.x * blockIdx.x;
	//ixj = i^j;// 

	i	= gl_GlobalInvocationID.x +( gl_GlobalInvocationID.x & ~( stride-1 ) );
	ixj	= i + stride;


	/* The threads with the lowest ids sort the array. */
	//if( ( ixj )>i )
	{
		if( ( i&k )==0 )
		{
			/* Sort ascending */
			if( fArray[i]>fArray[ixj] )
			{
				/* exchange(i,ixj); */
				float temp = fArray[i];
				fArray[i] = fArray[ixj];
				fArray[ixj] = temp;
			}
		}
		if( ( i&k )!=0 )
		{
			/* Sort descending */
			if( fArray[i]<fArray[ixj] )
			{
				/* exchange(i,ixj); */
				float temp = fArray[i];
				fArray[i] = fArray[ixj];
				fArray[ixj] = temp;
			}
		}
	}

}