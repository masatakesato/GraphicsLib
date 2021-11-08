#include	"UniformBuffer.h"

#include	"../utils/HelperFunctions.h"




namespace vulkan
{

	UniformBuffer::UniformBuffer()
		: m_Buffer( VK_NULL_HANDLE )
		, m_DeviceMemory( VK_NULL_HANDLE )
	{

	}



	UniformBuffer::UniformBuffer( GraphicsDevice& device, VkDeviceSize bufferSize )
		: m_Buffer( VK_NULL_HANDLE )
		, m_DeviceMemory( VK_NULL_HANDLE )
	{
		Init( device, bufferSize );
	}



	UniformBuffer::~UniformBuffer()
	{
		Release();
	}



	void UniformBuffer::Init( GraphicsDevice& device, VkDeviceSize bufferSize )
	{
		m_refDevice	= device;
		m_Size		= bufferSize;

		ASSERT( m_refDevice->Device() != VK_NULL_HANDLE );

		CreateBuffer(	m_refDevice->PhysicalDevice(),
						m_refDevice->Device(),
						bufferSize,
						VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						m_Buffer,
						m_DeviceMemory );
	}



	void UniformBuffer::Release()
	{
		if( !m_refDevice.IsNull() && m_refDevice->Device() != VK_NULL_HANDLE )
		{
			vkDestroyBuffer( m_refDevice->Device(), m_Buffer, nullptr );
			vkFreeMemory( m_refDevice->Device(), m_DeviceMemory, nullptr );

			m_refDevice.Reset();
		}
	}



	// Transfer data to VkDeviceMemory
	void UniformBuffer::Update( void* pData, VkDeviceSize size )
	{
		ASSERT( (m_refDevice->Device() != VK_NULL_HANDLE) && pData );

		void* data;
		vkMapMemory( m_refDevice->Device(), m_DeviceMemory, 0, size, 0, &data );
			memcpy( data, pData, size );
		vkUnmapMemory( m_refDevice->Device(), m_DeviceMemory );
	}


}