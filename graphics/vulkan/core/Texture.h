#ifndef TEXTURE_H
#define	TEXTURE_H

#include	"GraphicsDevice.h"
#include	"ImageBuffer.h"



namespace vk
{

	class Texture : ImageBuffer
	{
	public:

		Texture();
		Texture( GraphicsDevice& device, uint32_t width, uint32_t height, VkFormat format, bool mipmap );
		~Texture();
		Texture( const Texture& ) = delete;

		void Init( GraphicsDevice& device, uint32_t width, uint32_t height, VkFormat format, bool mipmap );
		void Release();

		void UploadData( const void* pData, VkDeviceSize size );


		// Virtual function override
		virtual VkFormat Format() const							{ return m_Format; }
		virtual const VkExtent2D& Extent() const				{ return m_Dim; }
		virtual const VkImageView& View( int i=0 ) const		{ return m_ImageView; }
		virtual VkSampleCountFlagBits MultiSampleCount() const	{ return VK_SAMPLE_COUNT_1_BIT; }
		virtual uint32_t MipLevels() const						{ return m_MipLevels; }


	private:
			
		GraphicsDeviceRef	m_refDevice;

		VkExtent2D		m_Dim;
		VkFormat		m_Format;
		uint32_t		m_MipLevels;

		VkImage			m_Image;
		VkDeviceMemory	m_DeviceMemory;
		VkImageView		m_ImageView;


		void GenerateMipmaps();
	};


}// end of namespace vk


#endif // !TEXTURE_H
