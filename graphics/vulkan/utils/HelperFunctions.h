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




	//######################################################################################//
	//																						//
	//									Properties retrieval								//
	//																						//
	//######################################################################################//

	inline uint32_t FindMemoryType( VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties )
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



	//######################################################################################//
	//																						//
	//									Begin/End command									//
	//																						//
	//######################################################################################/

	inline VkCommandBuffer BeginSingleTimeCommands( VkDevice device, VkCommandPool commandPool )
	{
		// Create data transfer specific commandbuffer
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level					= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool			= commandPool;
		allocInfo.commandBufferCount	= 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers( device, &allocInfo, &commandBuffer );


		// Record commandbuffer
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType	= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags	= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer( commandBuffer, &beginInfo );

		return commandBuffer;
	}



	inline void EndSingleTimeCommands( VkDevice device, VkCommandBuffer commandBuffer, VkCommandPool commandPool, VkQueue queue )
	{
		vkEndCommandBuffer(commandBuffer );

		// Execute recorded commandbuffer immediately
		VkSubmitInfo submitInfo = {};
		submitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount	= 1;
		submitInfo.pCommandBuffers		= &commandBuffer;

		vkQueueSubmit( queue, 1, &submitInfo, VK_NULL_HANDLE );
		vkQueueWaitIdle( queue );

		// Free commandbuffer
		vkFreeCommandBuffers( device, commandPool, 1, &commandBuffer );
	}




	//######################################################################################//
	//																						//
	//										Buffer											//
	//																						//
	//######################################################################################//

	inline void CreateBuffer(	VkPhysicalDevice physicalDevice, 
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



	inline void CopyBuffer( VkDevice device, VkCommandPool commandPool, VkQueue queue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size )
	{		
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands( device, commandPool );

			VkBufferCopy copyRegion = {};
			copyRegion.srcOffset	= 0;
			copyRegion.dstOffset	= 0;
			copyRegion.size			= size;
		
			vkCmdCopyBuffer( commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion );

		vulkan::EndSingleTimeCommands( device, commandBuffer, commandPool, queue );
	}



	//######################################################################################//
	//																						//
	//										Descriptor***									//
	//																						//
	//######################################################################################//
	

	inline void CreateDescriptorSetLayout( VkDevice device, VkDescriptorSetLayout& descSetLayout, const OreOreLib::Memory<VkDescriptorSetLayoutBinding>& bindings )
	{
		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType		= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount	= static_cast<uint32_t>( bindings.Length() );
		layoutInfo.pBindings	= bindings.begin();

		VK_CHECK_RESULT( vkCreateDescriptorSetLayout( device, &layoutInfo, nullptr, &descSetLayout ) );
	}




	//######################################################################################//
	//																						//
	//										Image/ImageView									//
	//																						//
	//######################################################################################//

	inline void CreateImageView( VkDevice device, VkImageView& imageView, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels )
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType		= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image		= image;
		viewInfo.viewType	= VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format		= format;

		viewInfo.subresourceRange.aspectMask		= aspectFlags;
		viewInfo.subresourceRange.baseMipLevel		= 0;
		viewInfo.subresourceRange.levelCount		= mipLevels;
		viewInfo.subresourceRange.baseArrayLayer	= 0;
		viewInfo.subresourceRange.layerCount		= 1;

		VK_CHECK_RESULT( vkCreateImageView( device, &viewInfo, nullptr, &imageView ) );	
	}








}// end of vulkan namespace



#endif // !VK_HELPER_FUNCTIONS_H
