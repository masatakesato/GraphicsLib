#include	"SwapChain.h"

#include	"../utils/HelperFunctions.h"



namespace vulkan
{

	QueueFamilyIndices FindQueueFamilies( VkPhysicalDevice device, VkSurfaceKHR surface )
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, nullptr );

		OreOreLib::Array<VkQueueFamilyProperties> queueFamilies( queueFamilyCount );
		vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, queueFamilies.begin() );

		int i=0;
		for( const auto& queueFamily : queueFamilies )
		{
			if( queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT )
			{
				indices.graphicsFamily = i;
				indices.graphicsFamilyHasValue = true;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR( device, i, surface, &presentSupport );

			if( queueFamily.queueCount > 0 && presentSupport )
			{
				indices.presentFamily = i;
				indices.presentFamilyHasValue = true;
			}

			if( indices.isComplete() )
				break;

			i++;
		}


		return indices;
	}






	SwapChain::SwapChain()
		: m_refDevice( VK_NULL_HANDLE )
		, m_SwapChain( VK_NULL_HANDLE )
		, m_ImageFormat( VK_FORMAT_UNDEFINED )
	{

	}



	SwapChain::SwapChain( VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface )
		: m_refDevice( VK_NULL_HANDLE )
		, m_SwapChain( VK_NULL_HANDLE )
		, m_ImageFormat( VK_FORMAT_UNDEFINED )
	{
		Init( device, physicalDevice, surface );

	}



	SwapChain::~SwapChain()
	{
		Release();
	}



	void SwapChain::Init( VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface )
	{
		m_refDevice	= device;

		InitSwapChain( physicalDevice, surface );
		InitImageViews();
	}



	void SwapChain::Release()
	{
		if( m_refDevice != VK_NULL_HANDLE )
		{
			for( auto& view : m_ImageViews )
				vkDestroyImageView( m_refDevice, view, nullptr );
			m_ImageViews.Release();

			if( m_SwapChain != VK_NULL_HANDLE )
			{
				vkDestroySwapchainKHR( m_refDevice, m_SwapChain, nullptr );
				m_SwapChain = VK_NULL_HANDLE;
			}
		}

	}




	void SwapChain::InitSwapChain( VkPhysicalDevice physicalDevice, VkSurfaceKHR surface )
	{
		ASSERT( m_refDevice != VK_NULL_HANDLE );

		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport( physicalDevice, surface );

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat( VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, swapChainSupport.formats );
		VkPresentModeKHR presentMode = ChooseSwapPresentMode( VK_PRESENT_MODE_MAILBOX_KHR, swapChainSupport.presentModes );
		VkExtent2D extent = ChooseSwapExtent( swapChainSupport.capabilities );

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if( swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount )
			imageCount = swapChainSupport.capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType			= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface			= surface;

		createInfo.minImageCount	= imageCount;
		createInfo.imageFormat		= surfaceFormat.format;
		createInfo.imageColorSpace	= surfaceFormat.colorSpace;
		createInfo.imageExtent		= extent;
		createInfo.imageArrayLayers	= 1;
		createInfo.imageUsage		= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = FindQueueFamilies( physicalDevice, surface );
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

		VK_CHECK_RESULT( vkCreateSwapchainKHR( m_refDevice, &createInfo, nullptr, &m_SwapChain ) );

		vkGetSwapchainImagesKHR( m_refDevice, m_SwapChain, &imageCount, nullptr );
		m_Images.Resize( imageCount );
		vkGetSwapchainImagesKHR( m_refDevice, m_SwapChain, &imageCount, m_Images.begin() );

		m_ImageFormat	= surfaceFormat.format;
		m_Extent		= extent;

	}



	void SwapChain::InitImageViews()
	{
		ASSERT( m_refDevice != VK_NULL_HANDLE );

		m_ImageViews.Resize( m_Images.Length() );

		for( size_t i=0; i<m_Images.Length(); i++ )
			CreateImageView( m_refDevice, m_ImageViews[i], m_Images[i], m_ImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1 );
	}





	SwapChainSupportDetails SwapChain::QuerySwapChainSupport( VkPhysicalDevice device, VkSurfaceKHR surface )
	{
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device, surface, &details.capabilities );

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR( device, surface, &formatCount, nullptr );
		if( formatCount != 0 )
		{
			details.formats.Resize( formatCount );
			vkGetPhysicalDeviceSurfaceFormatsKHR( device, surface, &formatCount, details.formats.begin() );
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR( device, surface, &presentModeCount, nullptr );
		if( presentModeCount != 0 )
		{
			details.presentModes.Resize( presentModeCount );
			vkGetPhysicalDeviceSurfacePresentModesKHR( device, surface, &presentModeCount, details.presentModes.begin() );
		}

		return details;
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



}// end of namespace vulkan