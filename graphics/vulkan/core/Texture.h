#ifndef TEXTURE_H
#define	TEXTURE_H

#include	<vulkan/vulkan.h>



namespace vulkan
{

	class Texture
	{
	public:

		Texture();
		Texture( VkPhysicalDevice physicalDevice, VkDevice device, uint32_t width, uint32_t height, VkFormat format, bool mipmap );
		Texture( const Texture& )=delete;
		~Texture();

		void Init( VkPhysicalDevice physicalDevice, VkDevice device, uint32_t width, uint32_t height, VkFormat format, bool mipmap );
		void Release();

		void UploadData( VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue queue, const void* pData, VkDeviceSize size );

		uint32_t	MipLevels() const	{ return m_MipLevels; }

		const VkImageView& View() const	{ return m_ImageView; }



	private:
			
		VkDevice		m_refDevice;

		VkExtent2D		m_Dim;
		VkFormat		m_Format;
		uint32_t		m_MipLevels;

		VkImage			m_Image;
		VkDeviceMemory	m_DeviceMemory;
		VkImageView		m_ImageView;


		void GenerateMipmaps( VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue queue );
	};


}// end of namespace vulkan


#endif // !TEXTURE_H
