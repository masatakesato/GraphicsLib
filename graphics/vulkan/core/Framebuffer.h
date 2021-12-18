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
		Framebuffers( GraphicsDevice& device, VkRenderPass renderPass, uint32 numbackbuffers );
		~Framebuffers();
		Framebuffers( const Framebuffers& ) = delete;


		void Init( GraphicsDevice& device, VkRenderPass renderPass, uint32 numbackbuffers );
		void Release();

		void InitFramebuffer( uint32 bufferindex, uint32_t width, uint32_t height, const OreOreLib::Memory<VkImageView>& imageViews );

		template < typename T=uint32 > T NumBuffers() const	{ return m_Framebuffers.Length<T>(); }
		const VkFramebuffer Buffer( int i ) const	{ return m_Framebuffers[i]; }


	private:

		GraphicsDeviceRef	m_refDevice;
		VkRenderPass		m_refRenderPass;

		OreOreLib::Array<VkFramebuffer> m_Framebuffers;

	};


}// end of namespace vk


#endif // !FRAMEBUFFER_H
