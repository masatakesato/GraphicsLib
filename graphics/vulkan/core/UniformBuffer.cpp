#include	"UniformBuffer.h"

#include	"../utils/HelperFunctions.h"




namespace vk
{

	UniformBuffer::UniformBuffer()
		: MemoryBuffer()
	{

	}



	UniformBuffer::UniformBuffer( GraphicsDevice& device, VkDeviceSize bufferSize, uint32_t numSwaps )
		: MemoryBuffer()
	{
		Init( device, bufferSize, numSwaps );
	}



	UniformBuffer::~UniformBuffer()
	{

	}



	UniformBuffer::UniformBuffer( UniformBuffer&& obj ) : MemoryBuffer( std::move(obj) )
	{

	}



	void UniformBuffer::Init( GraphicsDevice& device, VkDeviceSize bufferSize, uint32_t numSwaps )
	{
		CreateBuffer(	device,
						bufferSize,
						VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );
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