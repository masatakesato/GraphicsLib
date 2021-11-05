#ifndef DEVICE_H
#define	DEVICE_H

#include<vulkan/vulkan.h>

namespace vulkan
{
	class Device
	{
	public:

		Device();
		Device( const Device& )=delete;
		~Device();



	private:

		VkPhysicalDevice	m_PhysicalDevice;
		VkDevice			m_Device;

		VkCommandPool		m_CommandPool;
		VkQueue				m_GraphicsQueue;
		VkQueue				m_PresentQueue;
	};
}


#endif // !DEVICE_H
