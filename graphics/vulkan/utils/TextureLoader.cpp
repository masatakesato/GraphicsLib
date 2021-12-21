#include	"TextureLoader.h"


namespace vk
{

	TextureLoader::TextureLoader()
		: FreeImageLoader()
	{

	}



	TextureLoader::~TextureLoader()
	{

	}



	bool TextureLoader::Load( GraphicsDevice& device, Texture& tex, const tstring& filepath, bool bFloat, bool bFlip )
	{
		auto dib = FreeImageLoader::Load( filepath, bFloat, bFlip );

		auto texWidth = FreeImage_GetWidth( dib );
		auto texHeight = FreeImage_GetHeight( dib );
		auto bpp = FreeImage_GetBPP( dib );
		
		VkDeviceSize imageSize = texWidth * texHeight * 4;// * bpp;

		tex.Init( device, texWidth, texHeight, VK_FORMAT_B8G8R8A8_SRGB, true );
		tex.UploadData( FreeImage_GetBits( dib ), imageSize );

		FreeImage_Unload( dib );

		return true;
	}



	bool TextureLoader::Save( const Texture& tex, const tstring& filepath, bool bFlip )
	{

		return false;
	}


}// end of namespace vk