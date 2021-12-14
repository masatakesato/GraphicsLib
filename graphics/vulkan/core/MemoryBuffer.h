#ifndef MEMORY_BUFFER_H
#define	MEMORY_BUFFER_H

#include	"GraphicsDevice.h"



namespace vk
{

	class MemoryBuffer
	{
	public:

		MemoryBuffer();
		//MemoryBuffer( GraphicsDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties );
		virtual ~MemoryBuffer();
		MemoryBuffer( const MemoryBuffer& ) = delete;
		MemoryBuffer( MemoryBuffer&& obj );

		void Release();

		VkBuffer Buffer() const				{ return m_Buffer; }
		VkDeviceMemory DeviceMemory() const	{ return m_DeviceMemory; }


	protected:

		GraphicsDeviceRef	m_refDevice;

		VkDeviceSize	m_Size; 
		VkBuffer		m_Buffer;
		VkDeviceMemory	m_DeviceMemory;


		void CreateBuffer( GraphicsDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties );
		
	};


}// end of namespace vk


#endif // !MEMORY_BUFFER_H
