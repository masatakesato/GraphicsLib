#ifndef TEXTURE_H
#define	TEXTURE_H

#include	<vulkan/vulkan.h>



namespace vulkan
{

	class Texture
	{
	public:


	private:
			
		uint32_t		m_MipLevels;
		VkImage			m_Image;
		VkDeviceMemory	m_DeviceMemory;
		VkImageView		m_ImageView;

	};


}// end of namespace vulkan


#endif // !TEXTURE_H
