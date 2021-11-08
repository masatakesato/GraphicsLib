﻿#ifndef TEXTURE_H
#define	TEXTURE_H

#include	"GraphicsDevice.h"



namespace vulkan
{

	class Texture
	{
	public:

		Texture();
		Texture( GraphicsDevice& device, uint32_t width, uint32_t height, VkFormat format, bool mipmap );
		Texture( const Texture& )=delete;
		~Texture();

		void Init( GraphicsDevice& device, uint32_t width, uint32_t height, VkFormat format, bool mipmap );
		void Release();

		void UploadData( const void* pData, VkDeviceSize size );

		uint32_t	MipLevels() const	{ return m_MipLevels; }

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


}// end of namespace vulkan


#endif // !TEXTURE_H