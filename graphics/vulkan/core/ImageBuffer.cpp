//#include	"ImageBuffer.h"
//
//#include	"../utils/HelperFunctions.h"
//#include	"StagingBuffer.h"
//
//
//
//namespace vk
//{
//
//	ImageBuffer::ImageBuffer()
//		: m_Format( VK_FORMAT_UNDEFINED )
//		, m_Image( VK_NULL_HANDLE )
//		, m_DeviceMemory( VK_NULL_HANDLE )
//		, m_ImageView( VK_NULL_HANDLE )
//	{
//
//	}
//		
//
//
//	ImageBuffer::ImageBuffer( GraphicsDevice& device, uint32_t width, uint32_t height, VkFormat format, bool mipmap, VkImageUsageFlags usage )
//	{
//		Init( device, width, height, format, mipmap, usage );
//	}
//
//
//
//	ImageBuffer::~ImageBuffer()
//	{
//		Release();
//	}
//
//
//
//	void ImageBuffer::Init( GraphicsDevice& device, uint32_t width, uint32_t height, VkFormat format, bool mipmap, VkImageUsageFlags usage )
//	{
//		m_refDevice	= device;
//		m_Format	= format;
//
//		
//		// Create VkImage
//		CreateImage(	m_refDevice->PhysicalDevice(), m_refDevice->Device(),
//						width, height, m_MipLevels, VK_SAMPLE_COUNT_1_BIT,
//						m_Format,
//						VK_IMAGE_TILING_OPTIMAL,
//						usage,//VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
//						VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
//						m_Image,
//						m_DeviceMemory );
//
//		CreateImageView( m_refDevice->Device(), m_ImageView, m_Image, m_Format, VK_IMAGE_ASPECT_COLOR_BIT, m_MipLevels );
//	}
//
//
//
//	void ImageBuffer::Release()
//	{
//		if( !m_refDevice.IsNull() && m_refDevice->Device() != VK_NULL_HANDLE )
//		{
//			SafeDeleteImageView( m_refDevice->Device(), m_ImageView );
//			SafeDeleteImage( m_refDevice->Device(), m_Image );
//			SafeDeleteDeviceMemory( m_refDevice->Device(), m_DeviceMemory );
//
//			//m_refDevice.Reset();
//		}
//	}
//
//
//
//}// end of namespace vk