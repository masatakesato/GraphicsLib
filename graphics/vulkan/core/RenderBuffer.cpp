#include	"RenderBuffer.h"

#include	"../utils/HelperFunctions.h"
#include	"ImageBuffer.h"



namespace vk
{

	RenderBuffer::RenderBuffer()
		: m_Dim{ 0, 0 }
		, m_Format( VK_FORMAT_UNDEFINED )
		, m_MsaaSamples( VK_SAMPLE_COUNT_1_BIT )
		, m_Image( VK_NULL_HANDLE )
		, m_DeviceMemory( VK_NULL_HANDLE )
		, m_ImageView( VK_NULL_HANDLE )
	{

	}
		


	RenderBuffer::RenderBuffer( GraphicsDevice& device, uint32_t width, uint32_t height, VkFormat format, VkSampleCountFlagBits msaasamples, bool transient, bool enableinput )
	{
		Init( device, width, height, format, msaasamples, transient, enableinput );
	}



	RenderBuffer::RenderBuffer( RenderBuffer&& obj )
		: m_refDevice( obj.m_refDevice )
		, m_Dim( obj.m_Dim )
		, m_Format( obj.m_Format )
		, m_MsaaSamples( obj.m_MsaaSamples )
		, m_Image( obj.m_Image )
		, m_DeviceMemory( obj.m_DeviceMemory )
		, m_ImageView( obj.m_ImageView )
	{
		obj.m_refDevice.Reset();
		//obj.m_Dim
		obj.m_Format = VK_FORMAT_UNDEFINED;
		obj.m_MsaaSamples	= VK_SAMPLE_COUNT_1_BIT;
		obj.m_Image			= VK_NULL_HANDLE;
		obj.m_DeviceMemory	= VK_NULL_HANDLE;
		obj.m_ImageView		= VK_NULL_HANDLE;
	}



	RenderBuffer::~RenderBuffer()
	{
		Release();
	}



	void RenderBuffer::Init( GraphicsDevice& device, uint32_t width, uint32_t height, VkFormat format, VkSampleCountFlagBits msaasamples, VkImageUsageFlags usages )
	{
			m_refDevice		= device;
			m_Dim			= { width, height };
			m_Format		= format;
			m_MsaaSamples	= msaasamples;
		
			bool isdepth = HasDepthComponent( m_Format );
			bool hasstencil = HasStencilComponent( m_Format );
			VkImageAspectFlags aspect = ( isdepth ? ( VK_IMAGE_ASPECT_DEPTH_BIT | (hasstencil ? VK_IMAGE_ASPECT_STENCIL_BIT : 0) ) : VK_IMAGE_ASPECT_COLOR_BIT );


			CreateImage(	m_refDevice->PhysicalDevice(), m_refDevice->Device(),
							width, height, 1, m_MsaaSamples, m_Format,
							VK_IMAGE_TILING_OPTIMAL,
							usages,
							VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,//VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT,//LAZIY***はモバイルプラットフォーム向け.
							m_Image,
							m_DeviceMemory );

			CreateImageView( m_refDevice->Device(), m_ImageView, m_Image, m_Format, aspect, 1 );
	}



	void RenderBuffer::Init( GraphicsDevice& device, uint32_t width, uint32_t height, VkFormat format, VkSampleCountFlagBits msaasamples, bool transient, bool enableinput )
	{
		m_refDevice		= device;
		m_Dim			= { width, height };
		m_Format		= format;
		m_MsaaSamples	= msaasamples;
		
		bool isdepth = HasDepthComponent( format );
		bool hasstencil = HasStencilComponent( format );

		VkImageUsageFlags usage =	( transient ? VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT : 0 ) |
									( isdepth ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT ) |
									( enableinput ? VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT : 0 );

		VkImageAspectFlags aspect = ( isdepth ? ( VK_IMAGE_ASPECT_DEPTH_BIT | (hasstencil ? VK_IMAGE_ASPECT_STENCIL_BIT : 0) ) : VK_IMAGE_ASPECT_COLOR_BIT );


		// https://github.com/Overv/VulkanTutorial/issues/118
		// https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/vk__mem__alloc_8h.html
		CreateImage(	m_refDevice->PhysicalDevice(), m_refDevice->Device(),
						width, height, 1, m_MsaaSamples, m_Format,
						VK_IMAGE_TILING_OPTIMAL,
						usage,
						VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,//VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT,//LAZIY***はモバイルプラットフォーム向け.
						m_Image,
						m_DeviceMemory );

		CreateImageView( m_refDevice->Device(), m_ImageView, m_Image, m_Format, aspect, 1 );
	}



	void RenderBuffer::Release()
	{
		if( !m_refDevice.IsNull() && m_refDevice->Device() != VK_NULL_HANDLE )
		{
			SafeDeleteImageView( m_refDevice->Device(), m_ImageView );
			SafeDeleteImage( m_refDevice->Device(), m_Image );
			SafeDeleteDeviceMemory( m_refDevice->Device(), m_DeviceMemory );

			m_Format		= VK_FORMAT_UNDEFINED;
			m_Dim			= { 0, 0 };
			m_MsaaSamples	= VK_SAMPLE_COUNT_1_BIT;

			//m_refDevice.Reset();
		}
	}


}// end of namespace vk