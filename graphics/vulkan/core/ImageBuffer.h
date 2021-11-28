#ifndef IMAGE_H
#define	IMAGE_H

#include	<vulkan/vulkan.h>



namespace vk
{

	class ImageBuffer
	{
	public:

		virtual VkFormat Format() const	= 0;//		{ return VK_FORMAT_UNDEFINED; }
		virtual const VkExtent2D& Extent() const = 0;
		virtual const VkImageView& View( int i=0 ) const = 0;//	{ return VK_NULL_HANDLE; }
		virtual VkSampleCountFlagBits MultiSampleCount() const = 0;// { return VK_SAMPLE_COUNT_1_BIT; }
		virtual uint32_t MipLevels() const = 0;//	{ return 0; }
		virtual uint32_t NumImages() const { return 1; }


	//protected:
	
		//VkFormat		m_Format;
		//VkImage			m_Image;
		//VkDeviceMemory	m_DeviceMemory;
		//VkImageView		m_ImageView;


		//ImageBuffer()
		//	: m_Format( VK_FORMAT_UNDEFINED )
		//	, m_Image( VK_NULL_HANDLE )
		//	, m_DeviceMemory( VK_NULL_HANDLE )
		//	, m_ImageView( VK_NULL_HANDLE )
		//{

		//}


	};


}// end of namespace vk


#endif // !IMAGE_H
