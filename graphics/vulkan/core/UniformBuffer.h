#ifndef UNIFORM_BUFFER_H
#define	UNIFORM_BUFFER_H

#include	<vulkan/vulkan.h>


#include	<oreore/container/Array.h>


namespace vulkan
{

	class UniformBuffer
	{
	public:

		UniformBuffer();
		UniformBuffer( VkDevice device, VkDeviceSize bufferSize, VkPhysicalDevice physicalDevice );
		UniformBuffer( const UniformBuffer& ) = delete;
		~UniformBuffer();

		void Init( VkDevice device, VkDeviceSize bufferSize, VkPhysicalDevice physicalDevice );
		void Release();
		void Update( void* pData, VkDeviceSize size );

		VkBuffer Buffer() const				{ return m_Buffer; }
		VkDeviceMemory DeviceMemory() const	{ return m_DeviceMemory; }
		VkDeviceSize Size() const			{ return m_Size; }


	private:

		VkDeviceSize	m_Size; 
		VkBuffer		m_Buffer;//m_UniformBuffer;
		VkDeviceMemory	m_DeviceMemory;//m_UniformBufferMemory;

		VkDevice		m_refDevice;

	};



}// end of namespace



#endif // !UNIFORM_BUFFER_H
