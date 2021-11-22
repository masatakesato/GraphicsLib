#include	"VertexBuffer.h"

#include	"../utils/HelperFunctions.h"
#include	"StagingBuffer.h"



namespace vk
{

	VertexBuffer::VertexBuffer()
		: m_Buffer( VK_NULL_HANDLE )
		, m_DeviceMemory( VK_NULL_HANDLE )
	{

	}



	VertexBuffer::VertexBuffer( GraphicsDevice& device, VkDeviceSize bufferSize )
		: m_Buffer( VK_NULL_HANDLE )
		, m_DeviceMemory( VK_NULL_HANDLE )
	{
		Init( device, bufferSize );
	}



	VertexBuffer::~VertexBuffer()
	{
		Release();
	}



	void VertexBuffer::Init( GraphicsDevice& device, VkDeviceSize bufferSize )
	{
		m_refDevice	= device;
		m_Size		= bufferSize;

		ASSERT( m_refDevice->Device() != VK_NULL_HANDLE );

		CreateBuffer(	m_refDevice->PhysicalDevice(),
						m_refDevice->Device(),
						bufferSize,
						VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
						VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
						m_Buffer,
						m_DeviceMemory );
	}



	void VertexBuffer::Release()
	{
		if( !m_refDevice.IsNull() && m_refDevice->Device() != VK_NULL_HANDLE )
		{
			vkDestroyBuffer( m_refDevice->Device(), m_Buffer, nullptr );
			vkFreeMemory( m_refDevice->Device(), m_DeviceMemory, nullptr );

			//m_refDevice.Reset();
		}
	}



	// Transfer data to VkDeviceMemory
	void VertexBuffer::Update( void* vertices, VkDeviceSize size )
	{
		ASSERT( (m_refDevice->Device() != VK_NULL_HANDLE) && vertices );

		// Create staging buffer (host memory for staging)
		StagingBuffer stagingBuffer( m_refDevice, m_Size );
	
		// Transfer vertices to staging buffer
		stagingBuffer.Update( vertices, m_Size );

		// Transfer vertices from staging buffer to device local buffer 
		CopyBuffer( m_refDevice->Device(), m_refDevice->CommandPool(), m_refDevice->GraphicsQueue(), stagingBuffer.Buffer(), m_Buffer, m_Size );

		// Delete staging buffer
		stagingBuffer.Release();
	}


}