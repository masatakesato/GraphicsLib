#include	"UniformBuffer.h"

#include	"../utils/HelperFunctions.h"




namespace vulkan
{


	UniformBuffer::UniformBuffer( VkDevice device )
		: device( device )
		, m_Buffer( VK_NULL_HANDLE )
		, m_DeviceMemory( VK_NULL_HANDLE )
	{

	}


	UniformBuffer::~UniformBuffer()
	{
		Release();
	}





	void UniformBuffer::Init( VkPhysicalDevice physicalDevice, /*VkDevice device,*/ VkDeviceSize bufferSize )
	{
		//VkDeviceSize bufferSize = static_cast<VkDeviceSize>( ubsize );

		// Create UniformBuffer for each swap chain
		//uniformBuffers.Resize( (int)swapChainImages.size() );
		//uniformBuffersMemory.Resize( (int)swapChainImages.size() );

		//for( size_t i=0; i<swapChainImages.size(); ++i )
		//{
			CreateBuffer(	physicalDevice,
							device,
							bufferSize,
							VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
							VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
							m_Buffer,
							m_DeviceMemory );
		//}

	}



	void UniformBuffer::Release( /*VkDevice device*/ )
	{
		vkDestroyBuffer( device, m_Buffer, nullptr );
		vkFreeMemory( device, m_DeviceMemory, nullptr );
	}





}