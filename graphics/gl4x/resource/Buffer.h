#ifndef	BUFFER_H
#define	BUFFER_H

#include	<memory.h>
#include	<oreore/common/Utility.h>


// buffer
struct Buffer
{
//	uint8	BufferType;

	size_t	elmSize;
	int		numElms;
	uint8*	pData;
};




//#################################################################################//
//	Helper functions
//#################################################################################//

// Allocate Buffer
static void AllocateBuffer( Buffer *buffer, int numelms, size_t size/*, uint8 type*/ )
{
//	buffer->BufferType	= type;
	buffer->elmSize		= size;
	buffer->numElms		= numelms;
	if( numelms*size>0 )	buffer->pData	= new uint8[ numelms * size ];
}

// Copy Buffer
static void CopyBuffer( Buffer *dest, const Buffer *src )
{
	if( !dest|| !src )	return;
	if( !src->pData )	return;
	
	AllocateBuffer( dest, src->numElms, src->elmSize/*, src->BufferType*/ );
	memcpy( dest->pData, src->pData, src->elmSize*src->numElms );
}

// Release Buffer
static void ReleaseBuffer( Buffer *buffer )
{
//	buffer->BufferType = -1;
	buffer->elmSize	= 0;
	buffer->numElms	= 0;
	SafeDeleteArray( buffer->pData );
}







#endif	// BUFFER_H //