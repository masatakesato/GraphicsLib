#ifndef VERTEX_BUFFER_H
#define	VERTEX_BUFFER_H

#include	<oreore/container/Array.h>

#include	"MemoryBuffer.h"



namespace vk
{

	class VertexBuffer : public MemoryBuffer
	{
	public:

		VertexBuffer();
		VertexBuffer( GraphicsDevice& device, VkDeviceSize bufferSize );
		~VertexBuffer();
		VertexBuffer( const VertexBuffer& ) = delete;

		void Init( GraphicsDevice& device, VkDeviceSize bufferSize );
		//void Release();
		void Update( void* vertices, VkDeviceSize size );

	//	VkBuffer Buffer() const				{ return m_Buffer; }
	//	VkDeviceMemory DeviceMemory() const	{ return m_DeviceMemory; }
	//	VkDeviceSize Size() const			{ return m_Size; }


	//private:

	//	GraphicsDeviceRef	m_refDevice;

	//	VkDeviceSize	m_Size; 
	//	VkBuffer		m_Buffer;
	//	VkDeviceMemory	m_DeviceMemory;

	};



}// end of namespace



#endif // !VERTEX_BUFFER_H
