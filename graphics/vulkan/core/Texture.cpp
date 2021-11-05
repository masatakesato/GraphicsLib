#include	"Texture.h"

#include	"../utils/HelperFunctions.h"
#include	"StagingBuffer.h"



namespace vulkan
{

	Texture::Texture()
		: m_refDevice( VK_NULL_HANDLE )
		, m_Dim{ 0, 0 }
		, m_Format( VK_FORMAT_UNDEFINED )
		, m_MipLevels( 0 )
		, m_Image( VK_NULL_HANDLE )
		, m_DeviceMemory( VK_NULL_HANDLE )
		, m_ImageView( VK_NULL_HANDLE )
	{

	}
		


	Texture::Texture( VkPhysicalDevice physicalDevice, VkDevice device, uint32_t width, uint32_t height, VkFormat format, bool mipmap )
	{
		Init( physicalDevice, device, width, height, format, mipmap );
	}



	Texture::~Texture()
	{
		Release();
	}



	void Texture::Release()
	{
		if( m_refDevice != VK_NULL_HANDLE )
		{
			vkDestroyImageView( m_refDevice, m_ImageView, nullptr );
			vkDestroyImage( m_refDevice, m_Image, nullptr );
			vkFreeMemory( m_refDevice, m_DeviceMemory, nullptr );	
		}

		m_refDevice	= VK_NULL_HANDLE;
	}



	void Texture::Init( VkPhysicalDevice physicalDevice, VkDevice device, uint32_t width, uint32_t height, VkFormat format, bool mipmap )
	{
		m_refDevice	= device;
		m_Dim		= { width, height };
		m_Format	= format;
		m_MipLevels	= mipmap ? static_cast<uint32_t>( floor( log2( Max(width, height) ) ) ) + 1 : 1;
		
		// Create VkImage
		CreateImage(	physicalDevice, device,
						width, height, m_MipLevels, VK_SAMPLE_COUNT_1_BIT,
						m_Format,
						VK_IMAGE_TILING_OPTIMAL,
						VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
						VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
						m_Image,
						m_DeviceMemory );

		CreateImageView( m_refDevice, m_ImageView, m_Image, m_Format, VK_IMAGE_ASPECT_COLOR_BIT, m_MipLevels );
	}



	void Texture::UploadData( VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue queue, const void* pData, VkDeviceSize imageSize )
	{
		ASSERT( m_refDevice != VK_NULL_HANDLE );

		if( m_Dim.width * m_Dim.height == 0 )
			return;

		// Create staging buffer
		StagingBuffer stagingBuffer( m_refDevice, imageSize, physicalDevice );

		// Transfer iamge data to staging buffer
		stagingBuffer.Update( pData, static_cast<VkDeviceSize>(imageSize) );

		// Set VkImage layout from "undefined" to "data transfer from host memory"
		TransitionImageLayout( m_refDevice, commandPool, queue, m_Image, m_Format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_MipLevels );
		CopyBufferToImage( m_refDevice, commandPool, queue, stagingBuffer.Buffer(), m_Image, m_Dim.width, m_Dim.height );

		// Set VkImage layout from "data transfer from host memory" to "Shader Read-only"
		if( m_MipLevels == 1 )
			TransitionImageLayout( m_refDevice, commandPool, queue, m_Image, m_Format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_MipLevels );
		else
			GenerateMipmaps( physicalDevice, commandPool, queue );

		// cleanup staging memory
		stagingBuffer.Release();
	}



	void Texture::GenerateMipmaps( VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue queue )
	{
		ASSERT( m_refDevice != VK_NULL_HANDLE );

		m_MipLevels	= static_cast<uint32_t>( floor( log2( Max(m_Dim.width, m_Dim.height) ) ) ) + 1;

		TransitionImageLayout( m_refDevice, commandPool, queue, m_Image, m_Format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_MipLevels );


		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties( physicalDevice, m_Format, &formatProperties );

		if( !( formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT) )
			throw std::runtime_error( "texture image format does not support linear blitting" );


		VkCommandBuffer commandBuffer = BeginSingleTimeCommands( m_refDevice, commandPool );

			VkImageMemoryBarrier barrier = {};
			barrier.sType							= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.image							= m_Image;
			barrier.srcQueueFamilyIndex				= VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex				= VK_QUEUE_FAMILY_IGNORED;
			barrier.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseArrayLayer	= 0;
			barrier.subresourceRange.layerCount		= 1;
			barrier.subresourceRange.levelCount		= 1;


			int32_t mipWidth = m_Dim.width;
			int32_t mipHeight = m_Dim.height;

			for( uint32_t i=1; i<m_MipLevels; ++i )
			{
				barrier.subresourceRange.baseMipLevel	= i - 1;

				barrier.oldLayout		= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier.newLayout		= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.srcAccessMask	= VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask	= VK_ACCESS_TRANSFER_READ_BIT;

				vkCmdPipelineBarrier(	commandBuffer,
										VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
										0, nullptr,
										0, nullptr,
										1, &barrier	);


				VkImageBlit blit = {};
				blit.srcOffsets[0]					= { 0, 0, 0 };
				blit.srcOffsets[1]					= { mipWidth, mipHeight, 1 };
				blit.srcSubresource.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
				blit.srcSubresource.mipLevel		= i - 1;
				blit.srcSubresource.baseArrayLayer	= 0;
				blit.srcSubresource.layerCount		= 1;
				blit.dstOffsets[0]					= { 0, 0, 0 };
				blit.dstOffsets[1]					= { mipWidth > 1 ? mipWidth/2 : 1, mipHeight > 1 ? mipHeight/2 : 1, 1 };
				blit.dstSubresource.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
				blit.dstSubresource.mipLevel		= i;
				blit.dstSubresource.baseArrayLayer	= 0;
				blit.dstSubresource.layerCount		= 1;

				vkCmdBlitImage(	commandBuffer,
								m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
								m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
								1, &blit,
								VK_FILTER_LINEAR );


				barrier.oldLayout		= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.newLayout		= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				barrier.srcAccessMask	= VK_ACCESS_TRANSFER_READ_BIT;
				barrier.dstAccessMask	= VK_ACCESS_SHADER_READ_BIT;

				vkCmdPipelineBarrier(	commandBuffer,
										VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
										0, nullptr,
										0, nullptr,
										1, &barrier	);


				if( mipWidth > 1 )	mipWidth /= 2;
				if( mipHeight > 1 )	mipHeight /= 2;
			}


			barrier.subresourceRange.baseMipLevel	= m_MipLevels - 1;
			barrier.oldLayout						= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout						= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask					= VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask					= VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(	commandBuffer,
									VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
									0, nullptr,
									0, nullptr,
									1, &barrier	);

		EndSingleTimeCommands( m_refDevice, commandBuffer, commandPool, queue );
	}







}// end of namespace vulkan