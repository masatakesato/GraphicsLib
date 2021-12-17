#include	"SwapChain.h"

#include	"../utils/HelperFunctions.h"



namespace vk
{

	SwapChain::SwapChain()
		: m_refDevice()
		, m_WindowExtent{ 0, 0 }
		, m_SwapChain( VK_NULL_HANDLE )
		, m_SwapChainExtent{ 0, 0 }
		, m_NumAttachments( 0 )
	{

	}


	SwapChain::SwapChain( GraphicsDevice& device, VkExtent2D extent, VkSampleCountFlagBits msaasamples,  bool srgb, VkFormat depthformat )
		: m_SwapChain( VK_NULL_HANDLE )
		, m_SwapChainExtent{ 0, 0 }
	{
		Init( device, extent, msaasamples, srgb, depthformat );
	}



	SwapChain::~SwapChain()
	{
		Release();
	}



	void SwapChain::Init( GraphicsDevice& device, VkExtent2D extent, VkSampleCountFlagBits msaasamples, bool srgb, VkFormat depthformat )
	{
		m_refDevice				= device;
		m_WindowExtent			= extent;
		m_NumAttachments		= uint32( msaasamples != VK_SAMPLE_COUNT_1_BIT ) + uint32( depthformat != VK_FORMAT_UNDEFINED ) + 1;
		//m_bEnableMultisample	= msaasamples != VK_SAMPLE_COUNT_1_BIT;
		//msaaSamples				= msaasamples;

		InitSwapChain( srgb );
		InitDepthResources( depthformat, msaasamples );
		InitMsaaResources( m_ColorBuffers.Format(), msaasamples );
		InitFences();
	}



	void SwapChain::Release()
	{

		if( !m_refDevice.IsNull() && m_refDevice->Device() != VK_NULL_HANDLE )
		{
			// Delete Multisampling buffers
			m_MultiSampleColorBuffer.Release();

			// Delete Depth buffer
			m_DepthBuffer.Release();

			// Delete Color buffer
			m_ColorBuffers.Release();

			// Delete m_SwapChain
			SafeDeleteSwapChain(  m_refDevice->Device(), m_SwapChain );

			// Reset Synchronizer
			m_refSynchronizer.Reset();

			// Reset GraphicsDevice
			//m_refDevice.Reset();

			m_NumAttachments = 0;
		}
	}


	
	void SwapChain::ExposeRenderTargetDescs( OreOreLib::Memory<RenderTargetDesc>& renderTargetDescs )
	{

		if( m_MultiSampleColorBuffer.MultiSampleCount() != VK_SAMPLE_COUNT_1_BIT )//m_bEnableMultisample )
		{
			renderTargetDescs[0] =
				// msaa color buffer( with resolving enabled )
				{	m_WindowExtent, m_MultiSampleColorBuffer.Format(), m_MultiSampleColorBuffer.MultiSampleCount(), true, vk::RenderBufferUsage::ColorWrite_Transient,
					vk::AttachmentOp::Clear_DontCare, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR };

			renderTargetDescs[1] =
				// depth buffer
				{	m_WindowExtent, m_DepthBuffer.Format(), m_DepthBuffer.MultiSampleCount(), false, vk::RenderBufferUsage::DepthWrite,
					vk::AttachmentOp::Clear_DontCare, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
		}
		else
		{
			renderTargetDescs[0] =
				// color buffer
				{	m_WindowExtent, m_ColorBuffers.Format(), m_ColorBuffers.MultiSampleCount(), false, vk::RenderBufferUsage::ColorWrite_Transient,
					vk::AttachmentOp::Clear_DontCare, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR };

			renderTargetDescs[1] =
				// depth buffer
				{	m_WindowExtent, m_DepthBuffer.Format(), m_DepthBuffer.MultiSampleCount(), false, vk::RenderBufferUsage::DepthWrite,
					vk::AttachmentOp::Clear_DontCare, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
		}

	}



	void SwapChain::ExposeImageViews( OreOreLib::Memory<VkImageView>& views, uint32_t imageindex )
	{
//TODO: VkImageView配列要素の並び順はどうやって決める？
//		VkRenderPassに与えるVkAttachmentDescription配列の並び順でそう決めた
//		[0]: カラー描画結果を格納するアタッチメント, [1]: デプス描画結果を格納するアタッチメント、、、、、、、、、、、、、、、、、、、、、、、、、、、これが基本
//		[0]: カラー描画結果(MSAA)アタッチメント,     [1]: デプス描画結果(MSAA)アタッチメント     [2]: [0]を縮小した結果を格納するアタッチメント、、、、MSAAの時はこっち

//TODO: 上記アタッチメントに対応するイメージビューは？
//		[0]: スワップチェーンイメージビュー,         [1]: デプスイメージビュー、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、これが基本
//		[0]: マルチサンプルカラーイメージビュー,     [1]: デプスイメージビュー,                  [2]: スワップチェーンイメージビュー、、、、、、、、、MSAAの時はこっち

		if( m_MultiSampleColorBuffer.MultiSampleCount() != VK_SAMPLE_COUNT_1_BIT )
		{
			views[0] = m_MultiSampleColorBuffer.View();		// 0: Multi-Sampled color view
			views[1] = m_ColorBuffers.View( imageindex );	// 1: NO Multisampled image view
			views[2] = m_DepthBuffer.View();				// 2: Depth buffer view
		}
		else
		{
			views[0] = m_ColorBuffers.View( imageindex );
			views[1] = m_DepthBuffer.View();
		}
	}



	//VkResult SwapChain::AquireNextImage( uint32_t& imageIndex )
	//{
	//	// Wait for 
	//	m_refSynchronizer->WaitForCurrentFrame();

	//	return vkAcquireNextImageKHR( m_refDevice->Device(), m_SwapChain, std::numeric_limits<uint64_t>::max(), m_refSynchronizer->CurrentPresentFinishedSemaphore(), VK_NULL_HANDLE, &imageIndex );		

	//	// Moved to QueuePresent()
	//	//// Wait for previous frame's RenderFinished fence
	//	//if( m_refRenderFinishedFences[ imageIndex ] != VK_NULL_HANDLE )
	//	//	VK_CHECK_RESULT( vkWaitForFences( m_refDevice->Device(), 1, &m_refRenderFinishedFences[ imageIndex ], VK_TRUE, std::numeric_limits<uint64_t>::max() ) );

	//	//// Assign new RenderFinished fence for current frame
	//	//m_refRenderFinishedFences[ imageIndex ] = m_refSynchronizer->CurrentInFlightFence();

	//	//return result;
	//}



	//VkResult SwapChain::QueuePresent( const uint32_t& imageIndex )
	//{
	//	// imageIndex番目のスワップチェーン画像がまだ処理中だったら終わるまで待つ
	//	if( m_refRenderFinishedFences[ imageIndex ] != VK_NULL_HANDLE )
	//			vkWaitForFences( m_refDevice->Device(), 1, &m_refRenderFinishedFences[ imageIndex ], VK_TRUE, std::numeric_limits<uint64_t>::max() );

	//	// コンカレントフレーム用のフェンスをimageIndexに割り当てる
	//	m_refRenderFinishedFences[ imageIndex ] = m_refSynchronizer->CurrentInFlightFence();


	//	//================= Present処理 ===================//
	//	VkPresentInfoKHR presentInfo = {};
	//	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	//	presentInfo.waitSemaphoreCount = 1;
	//	presentInfo.pWaitSemaphores = &m_refSynchronizer->CurrentRenderFinishedSemaphore();// 描画完了を待ってからPresent開始するためのセマフォを指定する

	//	presentInfo.swapchainCount	= 1;
	//	presentInfo.pSwapchains		= &m_SwapChain;
	//	presentInfo.pImageIndices	= &imageIndex;
	//	presentInfo.pResults		= nullptr; // Optional
	//	
	//	return vkQueuePresentKHR( m_refDevice->PresentQueue(), &presentInfo );
	//}




	void SwapChain::InitSwapChain( bool srgb )
	{
	
		SwapChainSupportDetails swapChainSupport = m_refDevice->GetSwapChainSupport();

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat( srgb ? VK_FORMAT_B8G8R8A8_SRGB : VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, swapChainSupport.formats );
		VkPresentModeKHR presentMode = ChooseSwapPresentMode( VK_PRESENT_MODE_MAILBOX_KHR, swapChainSupport.presentModes );
		VkExtent2D extent = ChooseSwapExtent( swapChainSupport.capabilities );

		auto numSwaps = swapChainSupport.capabilities.minImageCount + 1;
		if( swapChainSupport.capabilities.maxImageCount > 0 && numSwaps > swapChainSupport.capabilities.maxImageCount )
			numSwaps = swapChainSupport.capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType			= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface			= m_refDevice->Surface();

		createInfo.minImageCount	= numSwaps;
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

		m_SwapChainExtent	= extent;

		m_ColorBuffers.Init( m_refDevice, m_SwapChain, surfaceFormat.format );
	}



	void SwapChain::InitDepthResources( VkFormat format, VkSampleCountFlagBits msaaSamples )
	{
		// Set depth format
		if( !IsValidDepthFormat( m_refDevice->PhysicalDevice(), format ) )
			format = FindDepthFormat( m_refDevice->PhysicalDevice() );

		m_DepthBuffer.Init( m_refDevice, m_WindowExtent.width, m_WindowExtent.height, format, msaaSamples, true, false );
	}



	void SwapChain::InitMsaaResources( VkFormat format, VkSampleCountFlagBits msaaSamples )
	{
		if( msaaSamples == VK_SAMPLE_COUNT_1_BIT )
			return;

		m_MultiSampleColorBuffer.Init( m_refDevice, m_WindowExtent.width, m_WindowExtent.height, format, msaaSamples, true, false );
	}



	void SwapChain::InitFences()
	{
		m_refRenderFinishedFences.Resize( m_ColorBuffers.NumBuffers(), VK_NULL_HANDLE );
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