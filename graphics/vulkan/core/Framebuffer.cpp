#include	"Framebuffer.h"

#include	"../utils/HelperFunctions.h"



namespace vk
{

	Framebuffers::Framebuffers()
		: m_refRenderPass( VK_NULL_HANDLE )
	{

	}
	

	
	Framebuffers::Framebuffers( GraphicsDevice& device, VkRenderPass renderPass, uint32 numbackbuffers )
		: m_refDevice( device )
		, m_refRenderPass( renderPass )
		, m_Framebuffers( numbackbuffers )
	{
		
	}
	

	
	Framebuffers::~Framebuffers()
	{
		Release();
	}



	void Framebuffers::Init( GraphicsDevice& device, VkRenderPass renderPass, uint32 numbackbuffers )
	{
		m_refDevice		= device;
		m_refRenderPass	= renderPass;
		m_Framebuffers.Resize( numbackbuffers );
	}



	void Framebuffers::Release()
	{
		if( !m_refDevice.IsNull() && m_refDevice->Device() != VK_NULL_HANDLE )
		{
			for( auto& framebuffer : m_Framebuffers )
			{
				if( framebuffer != VK_NULL_HANDLE )
				{
					vkDestroyFramebuffer( m_refDevice->Device(), framebuffer, nullptr );
					framebuffer = VK_NULL_HANDLE;
				}
			}
			m_Framebuffers.Release();

			//m_refDevice.Reset();
		}
	}



	void Framebuffers::InitFramebuffer( uint32 bufferindex, uint32_t width, uint32_t height, const OreOreLib::Memory<VkImageView>& imageViews )
	{
		ASSERT( !m_refDevice.IsNull() && m_refDevice->Device() != VK_NULL_HANDLE && bufferindex < m_Framebuffers.Length<uint32>() );

		// Delete existing framebuffer resource 
		if( m_Framebuffers[ bufferindex ] != VK_NULL_HANDLE )
			vkDestroyFramebuffer( m_refDevice->Device(), m_Framebuffers[ bufferindex ], nullptr );

		// Allocate framebuffer
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType			= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass		= m_refRenderPass;
		framebufferInfo.attachmentCount	= imageViews.Length<uint32_t>();
		framebufferInfo.pAttachments	= imageViews.begin();
		framebufferInfo.width			= width;
		framebufferInfo.height			= height;
		framebufferInfo.layers			= 1;

		VK_CHECK_RESULT( vkCreateFramebuffer( m_refDevice->Device(), &framebufferInfo, nullptr, &m_Framebuffers[ bufferindex ] ) );
	}



}// end of namespace vk