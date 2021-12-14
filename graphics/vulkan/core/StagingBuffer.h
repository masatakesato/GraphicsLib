#ifndef STAGING_BUFFER_H
#define	STAGING_BUFFER_H

#include	<oreore/container/Array.h>

#include	"MemoryBuffer.h"



namespace vk
{

	class StagingBuffer : public MemoryBuffer
	{
	public:

		StagingBuffer();
		StagingBuffer( GraphicsDevice& device, VkDeviceSize bufferSize );
		~StagingBuffer();
		StagingBuffer( const StagingBuffer& ) = delete;

		void Init( GraphicsDevice& device, VkDeviceSize bufferSize );
		//void Release();
		void Update( const void* pData );
		void Update( const void* pData, VkDeviceSize size );

		//VkBuffer Buffer() const				{ return m_Buffer; }
		//VkDeviceMemory DeviceMemory() const	{ return m_DeviceMemory; }


	//private:

		//GraphicsDeviceRef	m_refDevice;

		//VkDeviceSize	m_Size;
		//VkBuffer		m_Buffer;
		//VkDeviceMemory	m_DeviceMemory;

	};



}// end of namespace



#endif // !STAGING_BUFFER_H
