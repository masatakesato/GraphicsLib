#ifndef RENDER_BUFFER_H
#define	RENDER_BUFFER_H

#include	"GraphicsDevice.h"
#include	"ImageBuffer.h"



namespace vk
{

	namespace RenderBufferUsage
	{
		// color or depth, support input or not, transient or not
		const VkImageUsageFlags ColorWrite					= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		const VkImageUsageFlags ColorWrite_Transient		= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;

		const VkImageUsageFlags ColorReadWrite				= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		const VkImageUsageFlags ColorReadWrite_Transient	= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;

		const VkImageUsageFlags DepthWrite					= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		const VkImageUsageFlags DepthWrite_Transient		= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;

		const VkImageUsageFlags DepthReadWrite				= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		const VkImageUsageFlags DepthReadWrite_Transient	= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
	};


	struct RenderBufferDesc
	{
		VkExtent2D				Dim;
		VkFormat				Format;
		VkSampleCountFlagBits	MultiSampleFlag;
		VkImageUsageFlags		UsageFlags;
	};





	class RenderBuffer : ImageBuffer
	{
	public:

		RenderBuffer();
		RenderBuffer( GraphicsDevice& device, uint32_t width, uint32_t height, VkFormat format, VkSampleCountFlagBits msaasamples, bool transient, bool enableinput );
		RenderBuffer( const RenderBuffer& )=delete;
		~RenderBuffer();
		
void Init( GraphicsDevice& device, const RenderBufferDesc& desc );

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
