#ifndef VK_HELPER_FUNCTIONS_H
#define	VK_HELPER_FUNCTIONS_H

#include	<vulkan/vulkan.h>

#include	<oreore/common/Utility.h>
#include	<oreore/common/TString.h>



namespace vulkan
{

#ifdef _DEBUG

	#define	VK_CHECK_RESULT( f )																				\
	{																											\
		VkResult res = (f);																						\
		if( res != VK_SUCCESS )																					\
		{																										\
			tcout << "Fatal : VkResult is \"" << res << " in " << __FILE__ << " at line " << __LINE__ << tendl;	\
			ASSERT( res == VK_SUCCESS );																		\
		}																										\
	}


#else

	#define	VK_CHECK_RESULT( f ) ( (void)f )

#endif





	void CreateBuffer( VkPhysicalDevice, VkDevice, VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags, VkBuffer&, VkDeviceMemory& );

	uint32_t FindMemoryType( VkPhysicalDevice, uint32_t, VkMemoryPropertyFlags );





	void CreateBuffer(	VkPhysicalDevice physicalDevice, 
						VkDevice device,
						VkDeviceSize size,
						VkBufferUsageFlags usage,
						VkMemoryPropertyFlags properties,
						VkBuffer& buffer,
						VkDeviceMemory& bufferMemory )
	{
		// Create VkBuffer
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType		= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size			= size;
		bufferInfo.usage		= usage;
		bufferInfo.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;

		VK_CHECK_RESULT( vkCreateBuffer( device, &bufferInfo, nullptr, &buffer ) );


		// Check memory requirements and Allocate VkDeviceMemory
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements( device, buffer, &memRequirements );

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize	= memRequirements.size;
		allocInfo.memoryTypeIndex	= FindMemoryType( physicalDevice, memRequirements.memoryTypeBits, properties );

		VK_CHECK_RESULT( vkAllocateMemory( device, &allocInfo, nullptr, &bufferMemory ) );

		// Bind allocated device memory(VkDeviceMemory) to vertexBuffer(VkBuffer)
		VK_CHECK_RESULT( vkBindBufferMemory( device, buffer, bufferMemory, 0 ) );

	}



	uint32_t FindMemoryType( VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties )
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties( physicalDevice, &memProperties );

		for( uint32_t i=0; i<memProperties.memoryTypeCount; ++i )
		{
			if( (typeFilter & (1<<i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties )
				return i;
		}

		ASSERT( false && "failed to find suitable memory type" );
		//throw std::runtime_error( "failed to find suitable memory type" );

		return 0;
	}



}



#endif // !VK_HELPER_FUNCTIONS_H
