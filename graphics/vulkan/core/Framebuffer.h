#ifndef FRAMEBUFFER_H
#define	FRAMEBUFFER_H

#include	"SwapChain.h"



namespace vk
{

	class Framebuffer
	{
	public:

		Framebuffer();
		Framebuffer( GraphicsDevice& device, VkRenderPass renderPass, int numswaps, uint32_t width, uint32_t height, const OreOreLib::Memory<VkImageView>& attachmentViews );
		Framebuffer( const Framebuffer& ) = delete;
		~Framebuffer();

		void Init( GraphicsDevice& device, VkRenderPass renderPass, int numswaps, uint32_t width, uint32_t height, const OreOreLib::Memory<VkImageView>& attachmentViews );
		void Release();


	private:

		GraphicsDeviceRef	m_refDevice;

		OreOreLib::Array<VkFramebuffer> swapChainFramebuffers;

	};


}// end of namespace vk


#endif // !FRAMEBUFFER_H
