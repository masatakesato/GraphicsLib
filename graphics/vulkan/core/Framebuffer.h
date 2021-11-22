#ifndef FRAMEBUFFER_H
#define	FRAMEBUFFER_H

#include	<oreore/container/NDArray.h>

#include	"GraphicsDevice.h"



namespace vk
{

	class Framebuffer
	{
	public:

		Framebuffer();
		Framebuffer( GraphicsDevice& device, VkRenderPass renderPass, int numswaps, uint32_t width, uint32_t height, const OreOreLib::NDArray<VkImageView, 2>& attachmentViews );
		Framebuffer( const Framebuffer& ) = delete;
		~Framebuffer();

		void Init( GraphicsDevice& device, VkRenderPass renderPass, int numswaps, uint32_t width, uint32_t height, const OreOreLib::NDArray<VkImageView, 2>& attachmentViews );
		void Release();

		uint32_t NumBuffers() const	{ return static_cast<uint32_t>( swapChainFramebuffers.Length() ); }
		const VkFramebuffer Buffer( int i ) const	{ return swapChainFramebuffers[i]; }


	private:

		GraphicsDeviceRef	m_refDevice;

		OreOreLib::Array<VkFramebuffer> swapChainFramebuffers;

	};


}// end of namespace vk


#endif // !FRAMEBUFFER_H
