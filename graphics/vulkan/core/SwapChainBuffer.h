﻿#ifndef SWAP_CHAIN_BUFFER_H
#define	SWAP_CHAIN_BUFFER_H

#include	"GraphicsDevice.h"
#include	"ImageBuffer.h"



namespace vk
{

	class SwapChainBuffer : ImageBuffer
	{
	public:

		SwapChainBuffer();
		SwapChainBuffer( GraphicsDevice& device, VkSwapchainKHR swapchain, VkFormat format );
		SwapChainBuffer( const SwapChainBuffer& )=delete;
		~SwapChainBuffer();
		
		void Init( GraphicsDevice& device, VkSwapchainKHR swapchain, VkFormat format );
		void Release();

		// Virtual function override
		virtual VkFormat Format() const							{ return m_Format; }
		virtual const VkExtent2D& Extent() const				{ return /*m_Dim*/{0, 0}; }
		virtual const VkImageView& View(int i=0 ) const			{ return m_ImageViews[i]; }
		virtual VkSampleCountFlagBits MultiSampleCount() const	{ return /*m_MsaaSamples*/VK_SAMPLE_COUNT_1_BIT; }
		virtual uint32_t MipLevels() const						{ return 1; }
		virtual uint32_t NumImages() const						{ return static_cast<uint32_t>( m_Images.Length() ); }


	private:
			
		GraphicsDeviceRef				m_refDevice;

		VkFormat						m_Format;
		OreOreLib::Array<VkImage>		m_Images;
		OreOreLib::Array<VkImageView>	m_ImageViews;

	};


}// end of namespace vk


#endif // !SWAP_CHAIN_BUFFER_H
