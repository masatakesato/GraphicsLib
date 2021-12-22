#include	"TextureLoader.h"

#include	"../utils/HelperFunctions.h"



namespace vk
{

	TextureLoader::TextureLoader()
		: FreeImageLoader()
	{

	}



	TextureLoader::~TextureLoader()
	{

	}



	bool TextureLoader::Load( GraphicsDevice& device, Texture& tex, const tstring& filepath, bool bFloat, bool bFlip, bool bSrgb )
	{
		auto dib = FreeImageLoader::Load( filepath, bFloat, bFlip );

		uint8 numchannels = 0;
		VkFormat format = VK_FORMAT_UNDEFINED;
		VK_CHECK_RESULT( GetVkFormat( OreOreLib::GetDataFormat( dib, bSrgb ), format, numchannels ) );

		auto texWidth = FreeImage_GetWidth( dib );
		auto texHeight = FreeImage_GetHeight( dib );
		auto pixelSize = FreeImage_GetBPP( dib ) / 8;
		
		tex.Init( device, texWidth, texHeight, format, true );
		tex.UploadData( FreeImage_GetBits( dib ), static_cast<VkDeviceSize>( texWidth * texHeight * pixelSize ) );

		
		FreeImage_Unload( dib );

		return true;
	}



	bool TextureLoader::Save( const Texture& tex, const tstring& filepath, bool bFlip )
	{

		return false;
	}


}// end of namespace vk