#include	"MemoryBuffer.h"

#include	"../utils/HelperFunctions.h"



namespace vk
{

	MemoryBuffer::MemoryBuffer()
		: m_Size( 0 )
		, m_Buffer( VK_NULL_HANDLE )
		, m_DeviceMemory( VK_NULL_HANDLE )
	{

	}
		


	//MemoryBuffer::MemoryBuffer( GraphicsDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties )
	//{
	//	Init( device, size, usage, properties );
	//}
	


	MemoryBuffer::~MemoryBuffer()
	{
		Release();
	}



	MemoryBuffer::MemoryBuffer( MemoryBuffer&& obj )
		: m_refDevice( obj.m_refDevice )
		, m_Size( obj.m_Size )
		, m_Buffer( obj.m_Buffer )
		, m_DeviceMemory( obj.m_DeviceMemory )
	{
		obj.m_refDevice.Reset();
		obj.m_Size			= 0;
		obj.m_Buffer		= VK_NULL_HANDLE;
		obj.m_DeviceMemory	= VK_NULL_HANDLE;
	}



	void MemoryBuffer::CreateBuffer( GraphicsDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties )
	{
		ASSERT( device.Device() != VK_NULL_HANDLE );

		m_refDevice	= device;
		m_Size		= size;

		// Create VkBuffer
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType		= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size			= size;
		bufferInfo.usage		= usage;
		bufferInfo.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;

		VK_CHECK_RESULT( vkCreateBuffer( m_refDevice->Device(), &bufferInfo, nullptr, &m_Buffer ) );


		// Check memory requirements and Allocate VkDeviceMemory
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements( m_refDevice->Device(), m_Buffer, &memRequirements );

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize	= memRequirements.size;
		allocInfo.memoryTypeIndex	= FindMemoryType( m_refDevice->PhysicalDevice(), memRequirements.memoryTypeBits, properties );

		VK_CHECK_RESULT( vkAllocateMemory( m_refDevice->Device(), &allocInfo, nullptr, &m_DeviceMemory ) );

		// Bind allocated device memory(VkDeviceMemory) to vertexBuffer(VkBuffer)
		VK_CHECK_RESULT( vkBindBufferMemory( m_refDevice->Device(), m_Buffer, m_DeviceMemory, 0 ) );
	}	



	void MemoryBuffer::Release()
	{
		if( !m_refDevice.IsNull() && m_refDevice->Device() != VK_NULL_HANDLE )
		{
			SafeDeleteBuffer( m_refDevice->Device(), m_Buffer );
			SafeDeleteDeviceMemory( m_refDevice->Device(), m_DeviceMemory );

			m_refDevice.Reset();
		}
	}



}// end of namespace vk