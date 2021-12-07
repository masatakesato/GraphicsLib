#ifndef FRAMEBUFFER_H
#define	FRAMEBUFFER_H

#include	<oreore/memory/Memory.h>

#include	"GraphicsDevice.h"



namespace vk
{

	class Framebuffers
	{
	public:

		Framebuffers();
		Framebuffers( GraphicsDevice& device, VkRenderPass renderPass, uint32_t numbackbuffers );
		Framebuffers( const Framebuffers& ) = delete;
		~Framebuffers();

		void Init( GraphicsDevice& device, VkRenderPass renderPass, uint32_t numbackbuffers );
		void Release();

		void InitFramebuffer( uint32_t bufferindex, uint32_t width, uint32_t height, const OreOreLib::Memory<VkImageView>& imageViews );

		uint32_t NumBuffers() const	{ return static_cast<uint32_t>( m_Framebuffers.Length() ); }
		const VkFramebuffer Buffer( int i ) const	{ return m_Framebuffers[i]; }


	private:

		GraphicsDeviceRef	m_refDevice;
		VkRenderPass		m_refRenderPass;

		OreOreLib::Array<VkFramebuffer> m_Framebuffers;

	};


}// end of namespace vk


#endif // !FRAMEBUFFER_H
