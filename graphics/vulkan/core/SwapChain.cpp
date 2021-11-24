#include	"SwapChain.h"

#include	"../utils/HelperFunctions.h"



namespace vk
{

	//MultiSampleBuffer::MultiSampleBuffer()
	//	: m_refDevice()
	//	, msaaSamples( VK_SAMPLE_COUNT_1_BIT )
	//	, colorImage( VK_NULL_HANDLE )
	//	, colorImageMemory( VK_NULL_HANDLE )
	//	, colorImageView( VK_NULL_HANDLE )
	//{

	//}


	//MultiSampleBuffer::MultiSampleBuffer( GraphicsDevice& device, VkFormat format, VkExtent2D extent, VkSampleCountFlagBits msaasamples )
	//{
	//	Init( device, format, extent, msaasamples );
	//}


	//MultiSampleBuffer::~MultiSampleBuffer()
	//{

	//}



	//void MultiSampleBuffer::Init( GraphicsDevice& device, VkFormat format, VkExtent2D extent, VkSampleCountFlagBits msaasamples )
	//{
	//	m_refDevice	= device;
	//	msaaSamples	= msaasamples;

	//	// https://github.com/Overv/VulkanTutorial/issues/118
	//	// https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/vk__mem__alloc_8h.html
	//	CreateImage(	m_refDevice->PhysicalDevice(), m_refDevice->Device(),
	//					extent.width, extent.height, 1, msaaSamples, format,
	//					VK_IMAGE_TILING_OPTIMAL,
	//					VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
	//					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,//VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT,//LAZIY***はモバイルプラットフォーム向け.
	//					colorImage, colorImageMemory );

	//	CreateImageView(	m_refDevice->Device(),
	//						colorImageView,
	//						colorImage,
	//						format,
	//						VK_IMAGE_ASPECT_COLOR_BIT, 1 );
	//}



	//void MultiSampleBuffer::Release()
	//{

	//	if( !m_refDevice.IsNull() && m_refDevice->Device() != VK_NULL_HANDLE )
	//	{
	//		// Delete Multisampling relevant variables
	//		if( colorImageView != VK_NULL_HANDLE )
	//		{
	//			vkDestroyImageView( m_refDevice->Device(), colorImageView, nullptr );
	//			colorImageView = VK_NULL_HANDLE;
	//		}

	//		if( colorImage != VK_NULL_HANDLE )
	//		{
	//			vkDestroyImage( m_refDevice->Device(), colorImage, nullptr );
	//			colorImage = VK_NULL_HANDLE;
	//		}

	//		if( colorImageMemory != VK_NULL_HANDLE )
	//		{
	//			vkFreeMemory( m_refDevice->Device(), colorImageMemory, nullptr );
	//			colorImageMemory = VK_NULL_HANDLE;
	//		}

	//		// Reset GraphicsDevice
	//		m_refDevice.Reset();
	//	}
	//}








	SwapChain::SwapChain()
		: m_refDevice()
		, m_WindowExtent{ 0, 0 }
		, m_SwapChain( VK_NULL_HANDLE )
		, m_SwapChainExtent{ 0, 0 }
		, m_NumImages( 0 )
		, m_ImageFormat( VK_FORMAT_UNDEFINED )

		, m_DepthFormat( VK_FORMAT_UNDEFINED )
		, m_DepthImage( VK_NULL_HANDLE )
		, m_DepthImageMemory( VK_NULL_HANDLE )
		, m_DepthImageView( VK_NULL_HANDLE )

		, m_bEnableMultisample( false )
		, msaaSamples( VK_SAMPLE_COUNT_1_BIT )
		, colorImage( VK_NULL_HANDLE )
		, colorImageMemory( VK_NULL_HANDLE )
		, colorImageView( VK_NULL_HANDLE )
	{

	}


	SwapChain::SwapChain( GraphicsDevice& device, VkExtent2D extent, VkSampleCountFlagBits msaasamples )
		: m_SwapChain( VK_NULL_HANDLE )
		, m_SwapChainExtent{ 0, 0 }
		, m_ImageFormat( VK_FORMAT_UNDEFINED )
		, m_DepthFormat( VK_FORMAT_UNDEFINED )
	{
		Init( device, extent );
	}



	SwapChain::~SwapChain()
	{
		Release();
	}



	void SwapChain::Init( GraphicsDevice& device, VkExtent2D extent, VkSampleCountFlagBits msaasamples )
	{
		m_refDevice				= device;
		m_WindowExtent			= extent;
		m_bEnableMultisample	= msaasamples != VK_SAMPLE_COUNT_1_BIT;
		msaaSamples				= msaasamples;

		InitSwapChain();
		InitImageViews();
		InitDepthResources();
		InitMsaaResources();
		InitFramebufferAttachments();
		InitFences();
	}



	void SwapChain::Release()
	{

		if( !m_refDevice.IsNull() && m_refDevice->Device() != VK_NULL_HANDLE )
		{
			m_FramebufferAttachments.Release();

			// Delete Multisampling buffers
			if( colorImageView != VK_NULL_HANDLE )
			{
				vkDestroyImageView( m_refDevice->Device(), colorImageView, nullptr );
				colorImageView = VK_NULL_HANDLE;
			}

			if( colorImage != VK_NULL_HANDLE )
			{
				vkDestroyImage( m_refDevice->Device(), colorImage, nullptr );
				colorImage = VK_NULL_HANDLE;
			}

			if( colorImageMemory != VK_NULL_HANDLE )
			{
				vkFreeMemory( m_refDevice->Device(), colorImageMemory, nullptr );
				colorImageMemory = VK_NULL_HANDLE;
			}


			// Delete Depth buffer
			if( m_DepthImageView != VK_NULL_HANDLE )
			{
				vkDestroyImageView( m_refDevice->Device(), m_DepthImageView, nullptr );
				m_DepthImageView = VK_NULL_HANDLE;
			}

			if( m_DepthImage != VK_NULL_HANDLE )
			{
				vkDestroyImage( m_refDevice->Device(), m_DepthImage, nullptr );
				m_DepthImage = VK_NULL_HANDLE;
			}

			if( m_DepthImageMemory != VK_NULL_HANDLE )
			{
				vkFreeMemory( m_refDevice->Device(), m_DepthImageMemory, nullptr );
				m_DepthImageMemory = VK_NULL_HANDLE;
			}


			// Delete m_ColorImageViews
			for( auto& view : m_ColorImageViews )
				vkDestroyImageView( m_refDevice->Device(), view, nullptr );
			m_ColorImageViews.Release();


			// Delete m_SwapChain
			if( m_SwapChain != VK_NULL_HANDLE )
			{
				vkDestroySwapchainKHR( m_refDevice->Device(), m_SwapChain, nullptr );
				m_SwapChain = VK_NULL_HANDLE;
			}

			m_NumImages = 0;
			// Reset GraphicsDevice
			//m_refDevice.Reset();
		}
	}




	void SwapChain::InitSwapChain()
	{
	
		SwapChainSupportDetails swapChainSupport = m_refDevice->GetSwapChainSupport();

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat( VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, swapChainSupport.formats );
		VkPresentModeKHR presentMode = ChooseSwapPresentMode( VK_PRESENT_MODE_MAILBOX_KHR, swapChainSupport.presentModes );
		VkExtent2D extent = ChooseSwapExtent( swapChainSupport.capabilities );

		m_NumImages = swapChainSupport.capabilities.minImageCount + 1;
		if( swapChainSupport.capabilities.maxImageCount > 0 && m_NumImages > swapChainSupport.capabilities.maxImageCount )
			m_NumImages = swapChainSupport.capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType			= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface			= m_refDevice->Surface();

		createInfo.minImageCount	= m_NumImages;
		createInfo.imageFormat		= surfaceFormat.format;
		createInfo.imageColorSpace	= surfaceFormat.colorSpace;
		createInfo.imageExtent		= extent;
		createInfo.imageArrayLayers	= 1;
		createInfo.imageUsage		= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		
		QueueFamilyIndices indices = m_refDevice->GetQueueFamilies();
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

		if( indices.graphicsFamily != indices.presentFamily )
		{
			createInfo.imageSharingMode			= VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount	= 2;
			createInfo.pQueueFamilyIndices		= queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode			= VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount	= 0;		// Optional
			createInfo.pQueueFamilyIndices		= nullptr;	// Optional
		}

		createInfo.preTransform		= swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha	= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode		= presentMode;
		createInfo.clipped			= VK_TRUE;

		createInfo.oldSwapchain		= VK_NULL_HANDLE;

		VK_CHECK_RESULT( vkCreateSwapchainKHR( m_refDevice->Device(), &createInfo, nullptr, &m_SwapChain ) );

		vkGetSwapchainImagesKHR( m_refDevice->Device(), m_SwapChain, &m_NumImages, nullptr );
		m_ColorImages.Resize( m_NumImages );
		vkGetSwapchainImagesKHR( m_refDevice->Device(), m_SwapChain, &m_NumImages, m_ColorImages.begin() );

		m_ImageFormat		= surfaceFormat.format;
		m_SwapChainExtent	= extent;

	}



	void SwapChain::InitImageViews()
	{
		m_ColorImageViews.Resize( m_NumImages );

		for( size_t i=0; i<m_ColorImages.Length(); i++ )
			CreateImageView( m_refDevice->Device(), m_ColorImageViews[i], m_ColorImages[i], m_ImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1 );
	}



	void SwapChain::InitDepthResources()
	{
		m_DepthFormat = FindDepthFormat( m_refDevice->PhysicalDevice() );

		// Create VkImage and allocate VkDeviceMemory
		CreateImage(	m_refDevice->PhysicalDevice(),  m_refDevice->Device(),
						m_WindowExtent.width, m_WindowExtent.height, 1, msaaSamples, m_DepthFormat,
						VK_IMAGE_TILING_OPTIMAL,
						VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
						VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
						m_DepthImage, m_DepthImageMemory );

		// Create VkImageView
		CreateImageView(	m_refDevice->Device(),
							m_DepthImageView,
							m_DepthImage,
							m_DepthFormat,
							VK_IMAGE_ASPECT_DEPTH_BIT,
							1 );

		// renderpass implicitly deals with transition.
		TransitionImageLayout(	m_refDevice->Device(), m_refDevice->CommandPool(), m_refDevice->GraphicsQueue(),
								m_DepthImage,
								m_DepthFormat,
								VK_IMAGE_LAYOUT_UNDEFINED,
								VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
								1 );

		//TODO: テクスチャアセットではなく、フレームバッファとして確保したい  VK_IMAGE_USAGE_***_ATTACHMENT_BIT
		//m_DepthBuffer.Init( m_Device, m_SwapChain.Extent().width, m_SwapChain.Extent().height, depthFormat, false );

	}



	void SwapChain::InitMsaaResources()
	{
		if( msaaSamples == VK_SAMPLE_COUNT_1_BIT )
			return;

		// https://github.com/Overv/VulkanTutorial/issues/118
		// https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/vk__mem__alloc_8h.html
		CreateImage(	m_refDevice->PhysicalDevice(), m_refDevice->Device(),
						m_WindowExtent.width, m_WindowExtent.height, 1, msaaSamples, m_ImageFormat,
						VK_IMAGE_TILING_OPTIMAL,
						VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
						VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,//VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT,//LAZIY***はモバイルプラットフォーム向け.
						colorImage, colorImageMemory );

		CreateImageView(	m_refDevice->Device(),
							colorImageView,
							colorImage,
							m_ImageFormat,
							VK_IMAGE_ASPECT_COLOR_BIT, 1 );

	}



	void SwapChain::InitFramebufferAttachments()
	{
		if( m_bEnableMultisample )
		{
			m_FramebufferAttachments.Init( (int)m_NumImages, 3 );

			for( int i=0; i<m_FramebufferAttachments.Dim(0); ++i )
			{
				m_FramebufferAttachments(i, 0) = colorImageView;
				m_FramebufferAttachments(i, 1) = m_DepthImageView;
				m_FramebufferAttachments(i, 2) = m_ColorImageViews[i];
			}
		}
		else
		{
			m_FramebufferAttachments.Init( (int)m_NumImages, 2 );

			for( int i=0; i<m_FramebufferAttachments.Dim(0); ++i )
			{
				m_FramebufferAttachments(i, 0) = m_ColorImageViews[i];
				m_FramebufferAttachments(i, 1) = m_DepthImageView;
			}
		}


	}



	void SwapChain::InitFences()
	{
		m_refRenderFinishedFences.Resize( m_NumImages, VK_NULL_HANDLE );
	}





	// format = VK_FORMAT_B8G8R8A8_SRGB / VK_FORMAT_B8G8R8A8_UNORM
	// colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
	VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat( VkFormat format, VkColorSpaceKHR colorSpace, const OreOreLib::Array<VkSurfaceFormatKHR>& availableFormats )
	{
		for( const auto& availableFormat : availableFormats )
		{
			if( availableFormat.format == format && availableFormat.colorSpace == colorSpace )
				return availableFormat;
		}

		return availableFormats[0];
	}



	// presentmode = VK_PRESENT_MODE_MAILBOX_KHR
	VkPresentModeKHR SwapChain::ChooseSwapPresentMode( VkPresentModeKHR presentmode, const OreOreLib::Array<VkPresentModeKHR>& availablePresentModes )
	{
		for( const auto& availablePresentMode : availablePresentModes )
		{
			if( availablePresentMode == presentmode )
				return availablePresentMode;
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}



	VkExtent2D SwapChain::ChooseSwapExtent( const VkSurfaceCapabilitiesKHR& capabilities )
	{
		if( capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() )
		{
			return capabilities.currentExtent;
		}
		else
		{
			VkExtent2D actualExtent = m_WindowExtent;

			actualExtent.width = std::max( capabilities.minImageExtent.width, std::min( capabilities.maxImageExtent.width, actualExtent.width ) );
			actualExtent.height = std::max( capabilities.minImageExtent.height, std::min( capabilities.maxImageExtent.height, actualExtent.height ) );

			return actualExtent;
		}

	}



}// end of namespace vk