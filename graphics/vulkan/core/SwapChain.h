#ifndef SWAP_CHAIN_H
#define	SWAP_CHAIN_H

#include	<oreore/container/NDArray.h>

#include	"GraphicsDevice.h"
#include	"Texture.h"



namespace vk
{

	//class MultiSampleBuffer
	//{
	//public:

	//	MultiSampleBuffer();
	//	MultiSampleBuffer( GraphicsDevice& device, VkFormat format, VkExtent2D extent, VkSampleCountFlagBits msaasamples );
	//	~MultiSampleBuffer();

	//	void Init( GraphicsDevice& device, VkFormat format, VkExtent2D extent, VkSampleCountFlagBits msaasamples );
	//	void Release();


	//private:

	//	GraphicsDeviceRef	m_refDevice;

	//	VkSampleCountFlagBits	msaaSamples = VK_SAMPLE_COUNT_1_BIT;
	//	VkImage					colorImage;
	//	VkDeviceMemory			colorImageMemory;
	//	VkImageView				colorImageView;


	//	friend class SwapChain;

	//};




	// TODO: Enable/Disable depth
	class SwapChain
	{
	public:

		SwapChain();
		SwapChain( GraphicsDevice& device, VkExtent2D extent, VkSampleCountFlagBits msaasamples=VK_SAMPLE_COUNT_1_BIT );
		SwapChain( const SwapChain& obj )=delete;
		~SwapChain();

		void Init( GraphicsDevice& device, VkExtent2D extent, VkSampleCountFlagBits msaasamples=VK_SAMPLE_COUNT_1_BIT );
		void Release();

		VkSwapchainKHR Handle() const	{ return m_SwapChain; }

		uint32_t NumImages() const		{ return m_NumImages; }
		VkFormat ImageFormat() const	{ return m_ImageFormat; }
		VkImageView ImageView( int i ) const	{ return m_ColorImageViews[i]; }

		VkFormat DepthFormat() const	{ return m_DepthFormat; }
		VkImageView DepthView() const	{ return m_DepthImageView; }

		VkSampleCountFlagBits MultiSampleCount() const { return msaaSamples; }
		VkImageView	MultiSampleView() const	{ return colorImageView; }

		const OreOreLib::NDArray<VkImageView, 2> FramebufferAttachments() const	{ return m_FramebufferAttachments; }

		const VkExtent2D& Extent() const	{ return m_WindowExtent; }
		const VkFence& ImagesInFlight( int i ) const { return imagesInFlight[i]; }

		void SetImagesInFlight( int i, VkFence fence ){ imagesInFlight[i]=fence; }


		VkResult AquireNextImage( VkSemaphore semaphore, VkFence fence )
		{
			return vkAcquireNextImageKHR( m_refDevice->Device(), m_SwapChain, std::numeric_limits<uint64_t>::max(), semaphore, fence, &imageIndex );		
		}

		
		void WaitForAvailable( int imageIndex )
		{
			if( imagesInFlight[ imageIndex ] != VK_NULL_HANDLE )
				vkWaitForFences( m_refDevice->Device(), 1, &imagesInFlight[ imageIndex ], VK_TRUE, std::numeric_limits<uint64_t>::max() );
		}



	private:

		GraphicsDeviceRef				m_refDevice;
		VkExtent2D						m_WindowExtent;

		VkSwapchainKHR					m_SwapChain;
		VkExtent2D						m_SwapChainExtent;

		// Color
		uint32_t						m_NumImages;
		VkFormat						m_ImageFormat;
		OreOreLib::Array<VkImage>		m_ColorImages;
		OreOreLib::Array<VkImageView>	m_ColorImageViews;
		
		// Depth
		VkFormat						m_DepthFormat;
		VkImage							m_DepthImage;
		VkDeviceMemory					m_DepthImageMemory;
		VkImageView						m_DepthImageView;

		// Multisample
		bool							m_bEnableMultisample;
		VkSampleCountFlagBits	msaaSamples;
		VkImage					colorImage;
		VkDeviceMemory			colorImageMemory;
		VkImageView				colorImageView;

		// Attachment views for Framebuffer
		OreOreLib::NDArray<VkImageView, 2>	m_FramebufferAttachments;

		// レンダリング同期オブジェクト(参照)
		OreOreLib::Array<VkFence>		imagesInFlight;


		void InitSwapChain();
		void InitImageViews();
		void InitDepthResources();
		void InitMsaaResources();
		void InitFramebufferAttachments();
		void InitFences();

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat( VkFormat format, VkColorSpaceKHR colorSpace, const OreOreLib::Array<VkSurfaceFormatKHR>& availableFormats );
		VkPresentModeKHR ChooseSwapPresentMode( VkPresentModeKHR presentmode, const OreOreLib::Array<VkPresentModeKHR>& availablePresentModes );
		VkExtent2D ChooseSwapExtent( const VkSurfaceCapabilitiesKHR& capabilities );

	};

}// end of namespace vk


#endif // !SWAP_CHAIN_H
