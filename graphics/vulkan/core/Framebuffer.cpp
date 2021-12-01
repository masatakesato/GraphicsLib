#include	"Framebuffer.h"

#include	"../utils/HelperFunctions.h"



namespace vk
{

	Framebuffer::Framebuffer()
	{

	}
	

	
	Framebuffer::Framebuffer( GraphicsDevice& device, VkRenderPass renderPass, int numswaps, uint32_t width, uint32_t height, const OreOreLib::NDArray<VkImageView, 2>& attachmentViews )
	{
		Init( device, renderPass, numswaps, width, height, attachmentViews );
	}
	

	
	Framebuffer::~Framebuffer()
	{


	}



	void Framebuffer::Init( GraphicsDevice& device, VkRenderPass renderPass, int numswaps, uint32_t width, uint32_t height, const OreOreLib::NDArray<VkImageView, 2>& attachmentViews )
	{
		m_refDevice	= device;
		swapChainFramebuffers.Resize( numswaps );

		for( uint32 i=0; i<swapChainFramebuffers.Length(); ++i )
		{
			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType			= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass		= renderPass;
			framebufferInfo.attachmentCount	= static_cast<uint32_t>( attachmentViews.Dim(1) );
			framebufferInfo.pAttachments	= &attachmentViews( i, (uint32)0 );
			framebufferInfo.width			= width;
			framebufferInfo.height			= height;
			framebufferInfo.layers			= 1;

			VK_CHECK_RESULT( vkCreateFramebuffer( m_refDevice->Device(), &framebufferInfo, nullptr, &swapChainFramebuffers[i] ) );
		}
	}



	void Framebuffer::Release()
	{
		if( !m_refDevice.IsNull() && m_refDevice->Device() != VK_NULL_HANDLE )
		{
			for( auto& framebuffer : swapChainFramebuffers )
			{
				if( framebuffer != VK_NULL_HANDLE )
				{
					vkDestroyFramebuffer( m_refDevice->Device(), framebuffer, nullptr );
					framebuffer = VK_NULL_HANDLE;
				}
			}
			swapChainFramebuffers.Release();


			m_refDevice.Reset();
		}
	}




}// end of namespace vk