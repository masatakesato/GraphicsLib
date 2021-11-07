#include	"StagingBuffer.h"

#include	"../utils/HelperFunctions.h"




namespace vulkan
{

	StagingBuffer::StagingBuffer()
		: m_Buffer( VK_NULL_HANDLE )
		, m_DeviceMemory( VK_NULL_HANDLE )
	{

	}



	StagingBuffer::StagingBuffer( GraphicsDevice& device, VkDeviceSize bufferSize )
		: m_Buffer( VK_NULL_HANDLE )
		, m_DeviceMemory( VK_NULL_HANDLE )
	{
		Init( device, bufferSize );
	}



	StagingBuffer::~StagingBuffer()
	{
		Release();
	}



	void StagingBuffer::Init( GraphicsDevice& device, VkDeviceSize bufferSize )
	{
		m_refDevice	= device;

		ASSERT( m_refDevice->Device() != VK_NULL_HANDLE );

		CreateBuffer(	m_refDevice->PhysicalDevice(),
						m_refDevice->Device(),
						bufferSize,
						VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						m_Buffer,
						m_DeviceMemory );
	}



	void StagingBuffer::Release()
	{
		if( m_refDevice->Device() != VK_NULL_HANDLE )
		{
			vkDestroyBuffer( m_refDevice->Device(), m_Buffer, nullptr );
			m_Buffer = VK_NULL_HANDLE;

			vkFreeMemory( m_refDevice->Device(), m_DeviceMemory, nullptr );
			m_DeviceMemory = VK_NULL_HANDLE;

			m_refDevice.Reset();
		}
	}



	// Transfer data to VkDeviceMemory
	void StagingBuffer::Update( const void* pData, VkDeviceSize size )
	{
		ASSERT( (m_refDevice->Device() != VK_NULL_HANDLE) && pData );

		void* data;
		vkMapMemory( m_refDevice->Device(), m_DeviceMemory, 0, size, 0, &data );
			memcpy( data, pData, size );
		vkUnmapMemory( m_refDevice->Device(), m_DeviceMemory );
	}


}