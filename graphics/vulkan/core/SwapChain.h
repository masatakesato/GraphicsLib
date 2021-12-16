#ifndef SWAP_CHAIN_H
#define	SWAP_CHAIN_H

#include	<oreore/memory/ReferenceWrapper.h>

#include	"GraphicsDevice.h"
#include	"RenderBuffer.h"
#include	"SwapChainBuffer.h"
#include	"FrameSynchronizer.h"



namespace vk
{

	// TODO: Enable/Disable depth
	class SwapChain
	{
	public:

		SwapChain();
		SwapChain( GraphicsDevice& device, VkExtent2D extent, VkSampleCountFlagBits msaasamples=VK_SAMPLE_COUNT_1_BIT, bool srgb=true, VkFormat depthformat=VK_FORMAT_D32_SFLOAT );
		~SwapChain();
		SwapChain( const SwapChain& obj ) = delete;

		void Init( GraphicsDevice& device, VkExtent2D extent, VkSampleCountFlagBits msaasamples=VK_SAMPLE_COUNT_1_BIT, bool srgb=true, VkFormat depthformat=VK_FORMAT_D32_SFLOAT );
		void Release();

		void BindSynchronizer( FrameSynchronizer& synchronizer )	{ m_refSynchronizer = synchronizer; }
		void UnbindSynchronizer()	{ m_refSynchronizer.Reset(); }



		VkSwapchainKHR Handle() const	{ return m_SwapChain; }


		uint32_t NumAttachments() const				{ return m_NumAttachments; }

		const SwapChainBuffer& ColorBuffers() const	{ return m_ColorBuffers; }
		uint32_t NumBuffers() const				{ return m_ColorBuffers.NumBuffers(); }
		VkFormat ImageFormat() const				{ return m_ColorBuffers.Format(); }
		VkImageView ImageView( int i ) const		{ return m_ColorBuffers.View(i); }

		const RenderBuffer& DepthBuffer() const		{ return m_DepthBuffer; }
		VkFormat DepthFormat() const				{ return m_DepthBuffer.Format(); }
		VkImageView DepthView() const				{ return m_DepthBuffer.View(); }

		const RenderBuffer& MultiSampleBuffer() const	{ return m_MultiSampleColorBuffer; }
		VkSampleCountFlagBits MultiSampleCount() const	{ return m_MultiSampleColorBuffer.MultiSampleCount(); }
		VkImageView	MultiSampleView() const				{ return m_MultiSampleColorBuffer.View(); }

		void ExposeRenderTargetDescs( OreOreLib::Memory<RenderTargetDesc>& renderTargetDescs );
		void ExposeImageViews( OreOreLib::Memory<VkImageView>& views,  uint32_t imageindex );

		const VkExtent2D& Extent() const	{ return m_WindowExtent; }
		const VkFence& RenderFinishedFence( int i ) const { return m_refRenderFinishedFences[i]; }

		void BindInFlightFence( int i, VkFence fence ){ m_refRenderFinishedFences[i] = fence; }


		VkResult AquireNextImage( uint32_t& imageIndex );
		
		void WaitForAvailable( int imageIndex )
		{
			if( m_refRenderFinishedFences[ imageIndex ] != VK_NULL_HANDLE )
				vkWaitForFences( m_refDevice->Device(), 1, &m_refRenderFinishedFences[ imageIndex ], VK_TRUE, std::numeric_limits<uint64_t>::max() );
		}



	private:

		GraphicsDeviceRef				m_refDevice;
		VkExtent2D						m_WindowExtent;
		uint32_t						m_NumAttachments;

		VkSwapchainKHR					m_SwapChain;
		VkExtent2D						m_SwapChainExtent;

		// Color		
		SwapChainBuffer					m_ColorBuffers;// swapchain color buffer. msaasample is always 1.

		// Depth
		RenderBuffer					m_DepthBuffer;

		// Multisample color
		RenderBuffer					m_MultiSampleColorBuffer;

		// Frame synchronizatin object
		FrameSynchronizerRef			m_refSynchronizer;
		OreOreLib::Array<VkFence>		m_refRenderFinishedFences;


		void InitSwapChain( bool srgb );
		void InitDepthResources( VkFormat format, VkSampleCountFlagBits msaaSamples );
		void InitMsaaResources( VkFormat format, VkSampleCountFlagBits msaaSamples );
		void InitFences();

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat( VkFormat format, VkColorSpaceKHR colorSpace, const OreOreLib::Array<VkSurfaceFormatKHR>& availableFormats );
		VkPresentModeKHR ChooseSwapPresentMode( VkPresentModeKHR presentmode, const OreOreLib::Array<VkPresentModeKHR>& availablePresentModes );
		VkExtent2D ChooseSwapExtent( const VkSurfaceCapabilitiesKHR& capabilities );

	};


	using SwapChainRef = OreOreLib::ReferenceWrapper<SwapChain>;


}// end of namespace vk


#endif // !SWAP_CHAIN_H
