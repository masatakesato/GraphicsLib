#ifndef SWAP_CHAIN_H
#define	SWAP_CHAIN_H

#include	<oreore/container/NDArray.h>

#include	"GraphicsDevice.h"
#include	"RenderBuffer.h"//#include	"Texture.h"
#include	"SwapChainBuffer.h"



namespace vk
{

	// TODO: Enable/Disable depth
	class SwapChain
	{
	public:

		SwapChain();
		SwapChain( GraphicsDevice& device, VkExtent2D extent, VkSampleCountFlagBits msaasamples=VK_SAMPLE_COUNT_1_BIT, bool srgb=true, VkFormat depthformat=VK_FORMAT_D32_SFLOAT );
		SwapChain( const SwapChain& obj )=delete;
		~SwapChain();

		void Init( GraphicsDevice& device, VkExtent2D extent, VkSampleCountFlagBits msaasamples=VK_SAMPLE_COUNT_1_BIT, bool srgb=true, VkFormat depthformat=VK_FORMAT_D32_SFLOAT );
		void Release();

		VkSwapchainKHR Handle() const	{ return m_SwapChain; }

		const SwapChainBuffer& ColorBuffers() const	{ return m_ColorBuffers; }
		uint32_t NumImages() const					{ return /*m_NumImages*/m_ColorBuffers.NumImages(); }
		VkFormat ImageFormat() const				{ return /*m_ImageFormat*/m_ColorBuffers.Format(); }
		VkImageView ImageView( int i ) const		{ return /*m_ColorImageViews[i]*/m_ColorBuffers.View(i); }

		const RenderBuffer& DepthBuffer() const		{ return m_DepthBuffer; }
		VkFormat DepthFormat() const				{ return /*m_DepthFormat*/m_DepthBuffer.Format(); }
		VkImageView DepthView() const				{ return /*m_DepthImageView*/m_DepthBuffer.View(); }

		const RenderBuffer& ResolveBuffer() const		{ return m_ResolveBuffer; }
		VkSampleCountFlagBits MultiSampleCount() const	{ return /*msaaSamples*/m_ResolveBuffer.MultiSampleCount(); }
		VkImageView	MultiSampleView() const				{ return /*m_ResolveImageView*/m_ResolveBuffer.View(); }

		const OreOreLib::NDArray<VkImageView, 2> FramebufferAttachments() const	{ return m_FramebufferAttachments; }

		const VkExtent2D& Extent() const	{ return m_WindowExtent; }
		const VkFence& RenderFinishedFence( int i ) const { return m_refRenderFinishedFences[i]; }

		void BindInFlightFence( int i, VkFence fence ){ m_refRenderFinishedFences[i] = fence; }


//		VkResult AquireNextImage( VkSemaphore semaphore, VkFence fence )
//		{
//			return vkAcquireNextImageKHR( m_refDevice->Device(), m_SwapChain, std::numeric_limits<uint64_t>::max(), semaphore, fence, &imageIndex );		
//		}

		
		void WaitForAvailable( int imageIndex )
		{
			if( m_refRenderFinishedFences[ imageIndex ] != VK_NULL_HANDLE )
				vkWaitForFences( m_refDevice->Device(), 1, &m_refRenderFinishedFences[ imageIndex ], VK_TRUE, std::numeric_limits<uint64_t>::max() );
		}



	private:

		GraphicsDeviceRef				m_refDevice;
		VkExtent2D						m_WindowExtent;

		VkSwapchainKHR					m_SwapChain;
		VkExtent2D						m_SwapChainExtent;

		// Color
//		uint32_t						m_NumImages;
//		VkFormat						m_ImageFormat;
//		OreOreLib::Array<VkImage>		m_ColorImages;
//		OreOreLib::Array<VkImageView>	m_ColorImageViews;
		
		SwapChainBuffer					m_ColorBuffers;

		// Depth
//		VkFormat						m_DepthFormat;
//		VkImage							m_DepthImage;
//		VkDeviceMemory					m_DepthImageMemory;
//		VkImageView						m_DepthImageView;

		RenderBuffer					m_DepthBuffer;

		// Multisample
		//bool							m_bEnableMultisample;
		//VkSampleCountFlagBits	msaaSamples;
		//VkImage							m_ResolveImage;
		//VkDeviceMemory					m_ResolveImageMemory;
		//VkImageView						m_ResolveImageView;

		RenderBuffer					m_ResolveBuffer;




//TODO: VkFramebufferに登録するアタッチメントはスワップチェーンのビューだけとは限らない. 例えば、遅延レンダリングで位置/法線/アルベドを書き込むVkImageViewも含まれる
//https://github.com/SaschaWillems/Vulkan/blob/master/examples/subpasses/subpasses.cpp　Line 251

		// Attachment views for Framebuffer
		OreOreLib::NDArray<VkImageView, 2>	m_FramebufferAttachments;

		// レンダリング同期オブジェクト(参照)
		OreOreLib::Array<VkFence>		m_refRenderFinishedFences;


		void InitSwapChain( bool srgb );
		void InitImageViews();
		void InitDepthResources( VkFormat format, VkSampleCountFlagBits msaaSamples );
		void InitMsaaResources( VkFormat format, VkSampleCountFlagBits msaaSamples );
		void InitFramebufferAttachments();
		void InitFences();

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat( VkFormat format, VkColorSpaceKHR colorSpace, const OreOreLib::Array<VkSurfaceFormatKHR>& availableFormats );
		VkPresentModeKHR ChooseSwapPresentMode( VkPresentModeKHR presentmode, const OreOreLib::Array<VkPresentModeKHR>& availablePresentModes );
		VkExtent2D ChooseSwapExtent( const VkSurfaceCapabilitiesKHR& capabilities );

	};

}// end of namespace vk


#endif // !SWAP_CHAIN_H
