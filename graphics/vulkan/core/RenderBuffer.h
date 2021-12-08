#ifndef RENDER_BUFFER_H
#define	RENDER_BUFFER_H

#include	"GraphicsDevice.h"
#include	"ImageBuffer.h"
#include	"RenderTargetDesc.h"



namespace vk
{
	class RenderBuffer : public ImageBuffer
	{
	public:

		RenderBuffer();
		RenderBuffer( GraphicsDevice& device, uint32_t width, uint32_t height, VkFormat format, VkSampleCountFlagBits msaasamples, bool transient, bool enableinput );
		~RenderBuffer();
		RenderBuffer( const RenderBuffer& )=delete;
		RenderBuffer( RenderBuffer&& obj );
		
// TODO: 廃止. リゾルブバッファ生成に対応できない
//void Init( GraphicsDevice& device, const RenderBufferDesc& desc );
void Init( GraphicsDevice& device, uint32_t width, uint32_t height, VkFormat format, VkSampleCountFlagBits msaasamples, VkImageUsageFlags usages );

		void Init( GraphicsDevice& device, uint32_t width, uint32_t height, VkFormat format, VkSampleCountFlagBits msaasamples,  bool transient, bool enableinput );
		void Release();

		// Virtual function override
		virtual VkFormat Format() const							{ return m_Format; }
		virtual const VkExtent2D& Extent() const				{ return /*m_Dim*/{0, 0}; }
		virtual const VkImageView& View(int i=0 ) const					{ return m_ImageView; }
		virtual VkSampleCountFlagBits MultiSampleCount() const	{ return m_MsaaSamples; }
		virtual uint32_t MipLevels() const						{ return 1; }






	private:
			
		GraphicsDeviceRef		m_refDevice;

		//VkExtent2D			m_Dim;
		VkFormat				m_Format;
		VkSampleCountFlagBits	m_MsaaSamples;

		VkImage					m_Image;
		VkDeviceMemory			m_DeviceMemory;
		VkImageView				m_ImageView;

	};


}// end of namespace vk


#endif // !RENDER_BUFFER_H
