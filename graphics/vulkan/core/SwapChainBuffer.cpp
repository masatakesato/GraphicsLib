#include	"SwapChainBuffer.h"

#include	"../utils/HelperFunctions.h"
#include	"ImageBuffer.h"



namespace vk
{

	SwapChainBuffer::SwapChainBuffer()
		: m_Format( VK_FORMAT_UNDEFINED )
	{

	}
		


	SwapChainBuffer::SwapChainBuffer( GraphicsDevice& device, VkSwapchainKHR swapchain, VkFormat format )
	{
		Init( device, swapchain, format );
	}



	SwapChainBuffer::~SwapChainBuffer()
	{
		Release();
	}



	void SwapChainBuffer::Init( GraphicsDevice& device, VkSwapchainKHR swapchain, VkFormat format )
	{
		m_refDevice		= device;
		//m_Dim			= { width, height };
		m_Format		= format;
		//m_MsaaSamples	= msaasamples;
		
		uint32_t numimages = 0;

		vkGetSwapchainImagesKHR( m_refDevice->Device(), swapchain, &numimages, nullptr );
		m_Images.Resize( static_cast<int>(numimages) );
		vkGetSwapchainImagesKHR( m_refDevice->Device(), swapchain, &numimages, m_Images.begin() );

		m_ImageViews.Resize( static_cast<int>(numimages) );
		for( uint32 i=0; i<m_Images.Length<uint32>(); ++i )
			CreateImageView( m_refDevice->Device(), m_ImageViews[i], m_Images[i], m_Format, VK_IMAGE_ASPECT_COLOR_BIT, 1 );
	}



	void SwapChainBuffer::Release()
	{
		if( !m_refDevice.IsNull() && m_refDevice->Device() != VK_NULL_HANDLE )
		{
			for( auto& view : m_ImageViews )
				SafeDeleteImageView( m_refDevice->Device(), view );

			m_ImageViews.Release();
			m_Images.Release();
			m_Format = VK_FORMAT_UNDEFINED;

			//m_refDevice.Reset();
		}
	}


}// end of namespace vk