#include	"UniformBuffer.h"

#include	"../utils/HelperFunctions.h"




namespace vk
{

	//##############################################################################################################//
	//																												//
	//									UniformBuffer class implementation											//
	//																												//
	//##############################################################################################################//

	UniformBuffer::UniformBuffer()
		: MemoryBuffer()
	{

	}



	UniformBuffer::UniformBuffer( GraphicsDevice& device, VkDeviceSize bufferSize )
		: MemoryBuffer()
	{
		Init( device, bufferSize );
	}



	UniformBuffer::~UniformBuffer()
	{
		// ~MemoryBuffer() will be called automatically 
	}



	UniformBuffer::UniformBuffer( UniformBuffer&& obj ) : MemoryBuffer( (MemoryBuffer&&)obj )
	{

	}



	void UniformBuffer::Init( GraphicsDevice& device, VkDeviceSize bufferSize )
	{
		MemoryBuffer::Init(	device,
							bufferSize,
							VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
							VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );
	}



	// Transfer data to VkDeviceMemory
	void UniformBuffer::Update( void* pData )
	{
		ASSERT( (m_refDevice->Device() != VK_NULL_HANDLE) && pData );

		void* data;
		vkMapMemory( m_refDevice->Device(), m_DeviceMemory, 0, m_Size, 0, &data );
			memcpy( data, pData, m_Size );
		vkUnmapMemory( m_refDevice->Device(), m_DeviceMemory );
	}



	void UniformBuffer::Update( void* pData, VkDeviceSize size )
	{
		ASSERT( (m_refDevice->Device() != VK_NULL_HANDLE) && pData );

		size = Min( size, m_Size );
		void* data;
		vkMapMemory( m_refDevice->Device(), m_DeviceMemory, 0, size, 0, &data );
			memcpy( data, pData, size );
		vkUnmapMemory( m_refDevice->Device(), m_DeviceMemory );
	}




	//##############################################################################################################//
	//																												//
	//									UniformBuffers class implementation											//
	//																												//
	//##############################################################################################################//

	UniformBuffers::UniformBuffers()
	{

	}



	UniformBuffers::UniformBuffers( GraphicsDevice& device, VkDeviceSize bufferSize, uint32 numSwaps )
	{
		Init( device, bufferSize, numSwaps );
	}



	UniformBuffers::~UniformBuffers()
	{
		Release();
	}



	UniformBuffers::UniformBuffers( UniformBuffers&& obj )
		: m_UniformBuffers( (OreOreLib::Memory<UniformBuffers>&&)obj.m_UniformBuffers )
	{

	}



	void UniformBuffers::Init( GraphicsDevice& device, VkDeviceSize bufferSize, uint32 numSwaps )
	{
		m_UniformBuffers.Init( numSwaps );

		for( auto& buffer : m_UniformBuffers )
			buffer.Init( device, bufferSize );
	}



	void UniformBuffers::Release()
	{
		m_UniformBuffers.Release();
	}



	// Transfer data to VkDeviceMemory
	void UniformBuffers::Update( void* pData, uint32 swapIndex )
	{
		ASSERT( swapIndex < m_UniformBuffers.Length<uint32>() );
		m_UniformBuffers[ swapIndex ].Update( pData );
	}



	void UniformBuffers::Update( void* pData, VkDeviceSize size, uint32 swapIndex )
	{
		ASSERT( swapIndex < m_UniformBuffers.Length<uint32>() );
		m_UniformBuffers[ swapIndex ].Update( pData, size );
	}


}// end of namespace vk