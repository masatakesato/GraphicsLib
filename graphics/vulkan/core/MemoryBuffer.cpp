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



	void MemoryBuffer::Init( GraphicsDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties )
	{
		ASSERT( device.Device() != VK_NULL_HANDLE );

		m_refDevice	= device;
		m_Size		= size;

		CreateBuffer(	m_refDevice->PhysicalDevice(),
						m_refDevice->Device(),
						m_Size,
						usage,
						properties,
						m_Buffer,
						m_DeviceMemory );
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