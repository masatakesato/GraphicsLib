#ifndef VERTEX_BUFFER_H
#define	VERTEX_BUFFER_H

#include	<oreore/container/Array.h>

#include	"GraphicsDevice.h"



namespace vk
{

	class VertexBuffer
	{
	public:

		VertexBuffer();
		VertexBuffer( GraphicsDevice& device, VkDeviceSize bufferSize );
		VertexBuffer( const VertexBuffer& ) = delete;
		~VertexBuffer();

		void Init( GraphicsDevice& device, VkDeviceSize bufferSize );
		void Release();
		void Update( void* vertices, VkDeviceSize size );

		VkBuffer Buffer() const				{ return m_Buffer; }
		VkDeviceMemory DeviceMemory() const	{ return m_DeviceMemory; }
		VkDeviceSize Size() const			{ return m_Size; }


	private:

		GraphicsDeviceRef	m_refDevice;

		VkDeviceSize	m_Size; 
		VkBuffer		m_Buffer;//m_UniformBuffer;
		VkDeviceMemory	m_DeviceMemory;//m_UniformBufferMemory;

	};



}// end of namespace



#endif // !VERTEX_BUFFER_H
