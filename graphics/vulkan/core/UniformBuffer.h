#ifndef UNIFORM_BUFFER_H
#define	UNIFORM_BUFFER_H

#include	<oreore/container/Array.h>

#include	"MemoryBuffer.h"



namespace vk
{

	class UniformBuffer : public MemoryBuffer
	{
	public:

		UniformBuffer();
		UniformBuffer( GraphicsDevice& device, VkDeviceSize bufferSize, uint32_t numSwaps );
		~UniformBuffer();
		UniformBuffer( const UniformBuffer& ) = delete;
		UniformBuffer( UniformBuffer&& obj );

		void Init( GraphicsDevice& device, VkDeviceSize bufferSize, uint32_t numSwaps );
		void Update( void* pData, VkDeviceSize size );

		//VkBuffer Buffer() const				{ return m_Buffer; }
		//VkDeviceMemory DeviceMemory() const	{ return m_DeviceMemory; }
		//VkDeviceSize Size() const			{ return m_Size; }


//	private:

//		GraphicsDeviceRef	m_refDevice;

//		VkDeviceSize	m_Size;
//		VkBuffer		m_Buffer;
//		VkDeviceMemory	m_DeviceMemory;

	};



}// end of namespace



#endif // !UNIFORM_BUFFER_H
