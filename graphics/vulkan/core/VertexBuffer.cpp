#include	"VertexBuffer.h"

#include	"../utils/HelperFunctions.h"
#include	"StagingBuffer.h"



namespace vk
{

	VertexBuffer::VertexBuffer()
		: MemoryBuffer()
	{

	}



	VertexBuffer::VertexBuffer( GraphicsDevice& device, VkDeviceSize bufferSize )
	{
		Init( device, bufferSize );
	}



	VertexBuffer::~VertexBuffer()
	{
		// ~MemoryBuffer() will be called automatically ();
	}



	void VertexBuffer::Init( GraphicsDevice& device, VkDeviceSize bufferSize )
	{
		MemoryBuffer::Init(	device,
							bufferSize,
							VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
							VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
	}



	// Transfer data to VkDeviceMemory
	void VertexBuffer::Update( const void* vertices, VkDeviceSize size )
	{
		ASSERT( (m_refDevice->Device() != VK_NULL_HANDLE) && vertices );

		// Create staging buffer (host memory for staging)
		StagingBuffer stagingBuffer( m_refDevice, m_Size );
	
		// Transfer vertices to staging buffer
		stagingBuffer.Update( vertices );

		// Transfer vertices from staging buffer to device local buffer 
		CopyBuffer( m_refDevice->Device(), m_refDevice->CommandPool(), m_refDevice->GraphicsQueue(), stagingBuffer.Buffer(), m_Buffer, m_Size );

		// Delete staging buffer
		stagingBuffer.Release();
	}


}