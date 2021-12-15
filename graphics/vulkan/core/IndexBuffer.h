#ifndef INDEX_BUFFER_H
#define	INDEX_BUFFER_H

#include	<oreore/container/Array.h>

#include	"MemoryBuffer.h"



namespace vk
{

	class IndexBuffer : public MemoryBuffer
	{
	public:

		IndexBuffer();
		IndexBuffer( GraphicsDevice& device, VkDeviceSize bufferSize );
		~IndexBuffer();
		IndexBuffer( const IndexBuffer& ) = delete;

		void Init( GraphicsDevice& device, VkDeviceSize bufferSize );
		using MemoryBuffer::Release;
		void Update( void* pData, VkDeviceSize size );

		//VkBuffer Buffer() const				{ return m_Buffer; }
		//VkDeviceMemory DeviceMemory() const	{ return m_DeviceMemory; }
		//VkDeviceSize Size() const			{ return m_Size; }


	//private:

	//	GraphicsDeviceRef	m_refDevice;

	//	VkDeviceSize	m_Size; 
	//	VkBuffer		m_Buffer;
	//	VkDeviceMemory	m_DeviceMemory;

	};



}// end of namespace



#endif // !INDEX_BUFFER_H
