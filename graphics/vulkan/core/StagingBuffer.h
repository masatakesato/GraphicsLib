#ifndef STAGING_BUFFER_H
#define	STAGING_BUFFER_H

#include	<vulkan/vulkan.h>


#include	<oreore/container/Array.h>


namespace vulkan
{

	class StagingBuffer
	{
	public:

		StagingBuffer();
		StagingBuffer( VkDevice device, VkDeviceSize bufferSize, VkPhysicalDevice physicalDevice );
		StagingBuffer( const StagingBuffer& ) = delete;
		~StagingBuffer();

		void Init( VkDevice device, VkDeviceSize bufferSize, VkPhysicalDevice physicalDevice );
		void Release();
		void Update( const void* pData, VkDeviceSize size );

		VkBuffer Buffer() const				{ return m_Buffer; }
		VkDeviceMemory DeviceMemory() const	{ return m_DeviceMemory; }


	private:

		VkBuffer		m_Buffer;
		VkDeviceMemory	m_DeviceMemory;

		VkDevice		m_refDevice;

	};



}// end of namespace



#endif // !STAGING_BUFFER_H
