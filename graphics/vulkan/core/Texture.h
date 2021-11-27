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
		Texture( const Texture& )=delete;
		~Texture();

		void Init( GraphicsDevice& device, uint32_t width, uint32_t height, VkFormat format, bool mipmap );
		void Release();

		void UploadData( const void* pData, VkDeviceSize size );


		// Virtual function override
		uint32_t MipLevels() const	{ return m_MipLevels; }

		const VkImageView& View() const	{ return m_ImageView; }



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
