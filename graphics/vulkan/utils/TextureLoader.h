#ifndef TEXTURE_LOADER_H
#define	TEXTURE_LOADER_H

#include	<oreore/images/FreeImageLoader.h>

#include	"../core/Texture.h"
#include	"../core/VkDataFormat.h"



namespace vk
{

	class TextureLoader : public OreOreLib::FreeImageLoader
	{
	public:

		TextureLoader();
		~TextureLoader();

		bool Load( GraphicsDevice& device, Texture& tex, const tstring& filepath, bool bFloat, bool bFlip, bool bSrgb );
		bool Save( const Texture& tex, const tstring& filepath, bool bFlip );

	};


}// end of namespace vk


#endif // !TEXTURE_LOADER_H
