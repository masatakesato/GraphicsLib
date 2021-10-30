#ifndef UNIFORM_BUFFER_H
#define	UNIFORM_BUFFER_H

#include	<vulkan/vulkan.h>


#include	<oreore/container/Array.h>


namespace vulkan
{

	class UniformBuffer
	{
	public:


		UniformBuffer( VkDevice device );
		~UniformBuffer();


		void Init( VkPhysicalDevice physicalDevice, /*VkDevice device,*/ VkDeviceSize bufferSize );
		void Release( /*VkDevice device*/ );



		VkBuffer Buffer() const				{ return m_Buffer; }
		VkDeviceMemory DeviceMemory() const	{ return m_DeviceMemory; }



	private:

		VkBuffer		m_Buffer;//m_UniformBuffer;
		VkDeviceMemory	m_DeviceMemory;//m_UniformBufferMemory;

		const VkDevice& device;

	};



}// end of namespace



#endif // !UNIFORM_BUFFER_H
