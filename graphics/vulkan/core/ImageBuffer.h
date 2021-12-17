#ifndef IMAGE_H
#define	IMAGE_H

#include	<vulkan/vulkan.h>



namespace vk
{

	class ImageBuffer
	{
	public:

		ImageBuffer(){}
		virtual ~ImageBuffer(){}

		virtual VkFormat Format() const	= 0;//		{ return VK_FORMAT_UNDEFINED; }
		virtual const VkExtent2D& Extent() const = 0;
		virtual const VkImageView& View( uint32 i=0 ) const = 0;//	{ return VK_NULL_HANDLE; }
		virtual VkSampleCountFlagBits MultiSampleCount() const = 0;// { return VK_SAMPLE_COUNT_1_BIT; }
		virtual uint32_t MipLevels() const = 0;//	{ return 0; }
		virtual uint32_t NumBuffers() const = 0;// { return 1; }

	};


}// end of namespace vk


#endif // !IMAGE_H
