#ifndef VK_HELPER_FUNCTIONS_H
#define	VK_HELPER_FUNCTIONS_H

#include	<vulkan/vulkan.h>

#include	<oreore/common/Utility.h>
#include	<oreore/common/TString.h>
#include	<oreore/memory/Memory.h>



namespace vk
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
	//										Format check									//
	//																						//
	//######################################################################################//


	inline bool IsSupportedFormat( VkPhysicalDevice physicalDevice, VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features )
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties( physicalDevice, format, &props );

		if( tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features )
			return true;
		else if( tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features )
			return true;
		
		ASSERT( false && "unsupported format" );
		return false;
	}



	inline VkFormat FindSupportedFormat( VkPhysicalDevice physicalDevice, const OreOreLib::Memory<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features )
	{
		for( VkFormat format : candidates )
		{
			if( IsSupportedFormat( physicalDevice, format, tiling, features ) )
				return format;
		}

		ASSERT( false && "failed to find supported format" );
		return VK_FORMAT_UNDEFINED;
	}



	inline VkFormat FindDepthFormat( VkPhysicalDevice physicalDevice )
	{
		return FindSupportedFormat( physicalDevice,
									{
										VK_FORMAT_D32_SFLOAT,
										VK_FORMAT_D32_SFLOAT_S8_UINT,
										VK_FORMAT_D24_UNORM_S8_UINT,
										VK_FORMAT_D16_UNORM,
										VK_FORMAT_D16_UNORM_S8_UINT,
									},
									VK_IMAGE_TILING_OPTIMAL,
									VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT );
	}



	inline bool IsValidDepthFormat( VkPhysicalDevice physicalDevice, VkFormat format )
	{
		return IsSupportedFormat( physicalDevice, format, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT );
	}



	inline bool HasStencilComponent( VkFormat format )
	{
		return format==VK_FORMAT_D32_SFLOAT_S8_UINT || format==VK_FORMAT_D24_UNORM_S8_UINT;
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

		EndSingleTimeCommands( device, commandBuffer, commandPool, queue );
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


	inline void CreateImage(	VkPhysicalDevice physicalDevice,
								VkDevice device,
								uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
								VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
								VkImage& image,
								VkDeviceMemory& imageMemory )
	{
		// Create VkImage
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType		= VK_IMAGE_TYPE_2D;
		imageInfo.extent		= { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 };
		imageInfo.mipLevels		= mipLevels;
		imageInfo.arrayLayers	= 1;
		imageInfo.format		= format;
		imageInfo.tiling		= tiling;
		imageInfo.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage			= usage;
		imageInfo.samples		= numSamples;
		imageInfo.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.flags			= 0;// Optional

		VK_CHECK_RESULT( vkCreateImage( device,&imageInfo, nullptr, &image ) );


		// Create texture device memory
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements( device, image, &memRequirements );

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize	= memRequirements.size;
		allocInfo.memoryTypeIndex	= FindMemoryType( physicalDevice, memRequirements.memoryTypeBits, properties );

		VK_CHECK_RESULT( vkAllocateMemory( device, &allocInfo, nullptr, &imageMemory ) );


		// Bind allocated texture memory(VkDeviceMemory) to image buffer(VkImage)
		VK_CHECK_RESULT( vkBindImageMemory( device, image, imageMemory, 0 ) );
	}



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



	inline void TransitionImageLayout(	VkDevice device,
										VkCommandPool commandPool,
										VkQueue queue,
										VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels )
	{
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands( device, commandPool );

			VkImageMemoryBarrier barrier = {};
			barrier.sType		= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout	= oldLayout;
			barrier.newLayout	= newLayout;

			barrier.srcQueueFamilyIndex	= VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex	= VK_QUEUE_FAMILY_IGNORED;

			barrier.image							= image;
			barrier.subresourceRange.baseMipLevel	= 0;
			barrier.subresourceRange.levelCount		= mipLevels;
			barrier.subresourceRange.baseArrayLayer	= 0;
			barrier.subresourceRange.layerCount		= 1;

			barrier.srcAccessMask	= VK_ACCESS_NONE_KHR;//TODO
			barrier.dstAccessMask	= VK_ACCESS_NONE_KHR;//TODO


			// Set Aspectmask ( depth/stencil or color )
			if( newLayout== VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL )
			{
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
				if( HasStencilComponent( format ) )// if format contains stencil component
					barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
			else
			{
				barrier.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
			}


			// Set pipeline stage and access mask transition according to layout transition
			VkPipelineStageFlags sourceStage;
			VkPipelineStageFlags destinationStage;

			if( oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )// undefined -> transfer
			{
				barrier.srcAccessMask	= VK_ACCESS_NONE_KHR;
				barrier.dstAccessMask	= VK_ACCESS_TRANSFER_WRITE_BIT;

				sourceStage			= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage	= VK_PIPELINE_STAGE_TRANSFER_BIT;
			}
			else if( oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL )// transfer -> shader-read-only
			{
				barrier.srcAccessMask	= VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask	= VK_ACCESS_SHADER_READ_BIT;

				sourceStage			= VK_PIPELINE_STAGE_TRANSFER_BIT;
				destinationStage	= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}
			else if( oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL )// undefined -> depth/stencil attachment
			{
				barrier.srcAccessMask	= VK_ACCESS_NONE_KHR;
				barrier.dstAccessMask	= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

				sourceStage			= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage	= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			}
			else
			{
				throw std::runtime_error( "unsupported layout transition" );
			}

			vkCmdPipelineBarrier(	commandBuffer,
									sourceStage, destinationStage,
									0,
									0, nullptr,
									0, nullptr,
									1, &barrier );

		EndSingleTimeCommands( device, commandBuffer, commandPool, queue );
	}



	inline void CopyBufferToImage(	VkDevice device,
									VkCommandPool commandPool,
									VkQueue queue,
									VkBuffer buffer, VkImage image, uint32_t width, uint32_t height )
	{

		VkCommandBuffer commandBuffer = BeginSingleTimeCommands( device, commandPool );

			VkBufferImageCopy region = {};
			region.bufferOffset			= 0;
			region.bufferRowLength		= 0;
			region.bufferImageHeight	= 0;

			region.imageSubresource.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.mipLevel		= 0;
			region.imageSubresource.baseArrayLayer	= 0;
			region.imageSubresource.layerCount		= 1;

			region.imageOffset	= { 0, 0, 0 };
			region.imageExtent	= { width, height, 1 };

			vkCmdCopyBufferToImage(	commandBuffer,
									buffer,
									image,
									VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
									1,
									&region );

		EndSingleTimeCommands( device, commandBuffer, commandPool, queue );
	}




	//######################################################################################//
	//																						//
	//										SafeDelete										//
	//																						//
	//######################################################################################//


	inline static void SafeDeleteImage( VkDevice device, VkImage& image )
	{
		if( image != VK_NULL_HANDLE )
		{
			vkDestroyImage( device, image, nullptr );
			image = VK_NULL_HANDLE;
		}
	}



	inline static void SafeDeleteImageView( VkDevice device, VkImageView& view )
	{
		if( view != VK_NULL_HANDLE )
		{
			vkDestroyImageView( device, view, nullptr );
			view = VK_NULL_HANDLE;
		}
	}



	inline static void SafeDeleteDeviceMemory( VkDevice device, VkDeviceMemory& mem )
	{
		if( mem != VK_NULL_HANDLE )
		{
			vkFreeMemory( device, mem, nullptr );
			mem = VK_NULL_HANDLE;
		}
	}



}// end of vulkan namespace



#endif // !VK_HELPER_FUNCTIONS_H
