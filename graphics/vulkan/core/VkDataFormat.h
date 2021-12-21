#ifndef VK_DATA_FORMAT_H
#define	VK_DATA_FORMAT_H

#include	<vulkan/vulkan.h>

#include	<oreore/common/Utility.h>
#include	<oreore/images/DataFormat.h>



namespace vk
{

	static void GetVkFormat( OreOreLib::DATA_FORMAT dataFormat, int &internalFormat, VkFormat &format, uint8 &channels )
	{

		//============== identify opengl texture format ===========//
		switch( dataFormat )
		{
		// R32G32B32A32
		case OreOreLib::FORMAT_R32G32B32A32_FLOAT:
			format			= VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
			channels		= 4;
			return;
		case OreOreLib::FORMAT_R32G32B32A32_UINT:
			format			= VkFormat::VK_FORMAT_R32G32B32A32_UINT;
			channels		= 4;
			return;
		case OreOreLib::FORMAT_R32G32B32A32_SINT:
			format			= VkFormat::VK_FORMAT_R32G32B32A32_SINT;
			channels		= 4;
			return;


		// R16G16B16A16
		case OreOreLib::FORMAT_R16G16B16A16_FLOAT:
			format			= VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT;
			channels		= 4;
			return;
		case OreOreLib::FORMAT_R16G16B16A16_UNORM:
			format			= VkFormat::VK_FORMAT_R16G16B16A16_UNORM;
			channels		= 4;
			return;
		case OreOreLib::FORMAT_R16G16B16A16_UINT:
			format			= VkFormat::VK_FORMAT_R16G16B16A16_UINT;
			channels		= 4;
			return;
		case OreOreLib::FORMAT_R16G16B16A16_SNORM:
			format			= VkFormat::VK_FORMAT_R16G16B16A16_SNORM;
			channels		= 4;
			return;
		case OreOreLib::FORMAT_R16G16B16A16_SINT:
			format			= VkFormat::VK_FORMAT_R16G16B16A16_SINT;
			channels		= 4;
			return;


		// R8G8B8A8
		case OreOreLib::FORMAT_R8G8B8A8_UNORM:
			format			= VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
			channels		= 4;
			return;
		case OreOreLib::FORMAT_R8G8B8A8_UINT:
			format			= VkFormat::VK_FORMAT_R8G8B8A8_UINT;
			channels		= 4;
			return;
		case OreOreLib::FORMAT_R8G8B8A8_SNORM:
			format			= VkFormat::VK_FORMAT_R8G8B8A8_SNORM;
			channels		= 4;
			return;
		case OreOreLib::FORMAT_R8G8B8A8_SINT:
			format			= VkFormat::VK_FORMAT_R8G8B8A8_SINT;
			channels		= 4;
			return;
		case OreOreLib::FORMAT_R8G8B8A8_UNORM_SRGB:
			format			= VkFormat::VK_FORMAT_R8G8B8A8_SRGB;
			channels		= 4;
			return;


		// B8G8R8A8
		case OreOreLib::FORMAT_B8G8R8A8_UNORM:
			format			= VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
			channels		= 4;
			return;
		case OreOreLib::FORMAT_B8G8R8A8_UINT:
			format			= VkFormat::VK_FORMAT_B8G8R8A8_UINT;
			channels		= 4;
			return;
		case OreOreLib::FORMAT_B8G8R8A8_SNORM:
			format			= VkFormat::VK_FORMAT_B8G8R8A8_SNORM;
			channels		= 4;
			return;
		case OreOreLib::FORMAT_B8G8R8A8_SINT:
			format			= VkFormat::VK_FORMAT_B8G8R8A8_SINT;
			channels		= 4;
			return;
		case OreOreLib::FORMAT_B8G8R8A8_UNORM_SRGB:
			format			= VkFormat::VK_FORMAT_B8G8R8A8_SRGB;
			channels		= 4;
			return;


		// R32G32B32
		case OreOreLib::FORMAT_R32G32B32_FLOAT:
			format			= VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
			channels		= 3;
			return;
		case OreOreLib::FORMAT_R32G32B32_UINT:
			format			= VkFormat::VK_FORMAT_R32G32B32_UINT;
			channels		= 3;
			return;
		case OreOreLib::FORMAT_R32G32B32_SINT:
			format			= VkFormat::VK_FORMAT_R32G32B32_SINT;
			channels		= 3;
			return;


		// R16G16B16
		case OreOreLib::FORMAT_R16G16B16_FLOAT:
			format			= VkFormat::VK_FORMAT_R16G16B16_SFLOAT;
			channels		= 3;
			return;
		case OreOreLib::FORMAT_R16G16B16_UNORM:
			format			= VkFormat::VK_FORMAT_R16G16B16_UNORM;
			channels		= 3;
			return;
		case OreOreLib::FORMAT_R16G16B16_UINT:
			format			= VkFormat::VK_FORMAT_R16G16B16_UINT;
			channels		= 3;
			return;
		case OreOreLib::FORMAT_R16G16B16_SNORM:
			format			= VkFormat::VK_FORMAT_R16G16B16_SNORM;
			channels		= 3;
			return;
		case OreOreLib::FORMAT_R16G16B16_SINT:
			format			= VkFormat::VK_FORMAT_R16G16B16_SINT;
			channels		= 3;
			return;


		// R8G8B8
		case OreOreLib::FORMAT_R8G8B8_UNORM:
			format			= VkFormat::VK_FORMAT_R8G8B8_UNORM;
			channels		= 3;
			return;
		case OreOreLib::FORMAT_R8G8B8_UINT:
			format			= VkFormat::VK_FORMAT_R8G8B8_UINT;
			channels		= 3;
			return;
		case OreOreLib::FORMAT_R8G8B8_SNORM:
			format			= VkFormat::VK_FORMAT_R8G8B8_SNORM;
			channels		= 3;
			return;
		case OreOreLib::FORMAT_R8G8B8_SINT:
			format			= VkFormat::VK_FORMAT_R8G8B8_SINT;
			channels		= 3;
			return;
		case OreOreLib::FORMAT_R8G8B8_SRGB:
			format			= VkFormat::VK_FORMAT_R8G8B8_SRGB;
			channels		= 3;
			return;

		// B8G8R8
		case OreOreLib::FORMAT_B8G8R8_UNORM:
			format			= VkFormat::VK_FORMAT_B8G8R8_UNORM;
			channels		= 3;
			return;
		case OreOreLib::FORMAT_B8G8R8_UINT:
			format			= VkFormat::VK_FORMAT_B8G8R8_UINT;
			channels		= 3;
			return;
		case OreOreLib::FORMAT_B8G8R8_SNORM:
			format			= VkFormat::VK_FORMAT_B8G8R8_SNORM;
			channels		= 3;
			return;
		case OreOreLib::FORMAT_B8G8R8_SINT:
			format			= VkFormat::VK_FORMAT_B8G8R8_SINT;
			channels		= 3;
			return;
		case OreOreLib::FORMAT_B8G8R8_SRGB:
			format			= VkFormat::VK_FORMAT_B8G8R8_SRGB;
			channels		= 3;
			return;

		//R32G32
		case OreOreLib::FORMAT_R32G32_FLOAT:
			format			= VkFormat::VK_FORMAT_R32G32_SFLOAT;
			channels		= 2;
			return;
			//case OreOreLib::FORMAT_R32G32_UNORM:
			//	return;
		case OreOreLib::FORMAT_R32G32_UINT:
			format			= VkFormat::VK_FORMAT_R32G32_UINT;
			channels		= 2;
			return;
			//case OreOreLib::FORMAT_R32G32_SNORM:
			//	return;
		case OreOreLib::FORMAT_R32G32_SINT:
			format			= VkFormat::VK_FORMAT_R32G32_SINT;
			channels		= 2;
			return;


		// R16G16
		case OreOreLib::FORMAT_R16G16_FLOAT:
			format			= VkFormat::VK_FORMAT_R16G16_SFLOAT;
			channels		= 2;
			return;
		case OreOreLib::FORMAT_R16G16_UNORM:
			format			= VkFormat::VK_FORMAT_R16G16_UNORM;
			channels		= 2;
			return;
		case OreOreLib::FORMAT_R16G16_UINT:
			format			= VkFormat::VK_FORMAT_R16G16_UINT;
			channels		= 2;
			return;
		case OreOreLib::FORMAT_R16G16_SNORM:
			format			= VkFormat::VK_FORMAT_R16G16_SNORM;
			channels		= 2;
			return;
		case OreOreLib::FORMAT_R16G16_SINT:
			format			= VkFormat::VK_FORMAT_R16G16_SINT;
			channels		= 2;
			return;


		// R8G8
			//case OreOreLib::FORMAT_R8G8_FLOAT:
			//	return;
		case OreOreLib::FORMAT_R8G8_UNORM:
			format			= VkFormat::VK_FORMAT_R8G8_UNORM;
			channels		= 2;
			return;
		case OreOreLib::FORMAT_R8G8_UINT:
			format			= VkFormat::VK_FORMAT_R8G8_UINT;
			channels		= 2;
			return;
		case OreOreLib::FORMAT_R8G8_SNORM:
			format			= VkFormat::VK_FORMAT_R8G8_SNORM;
			channels		= 2;
			return;
		case OreOreLib::FORMAT_R8G8_SINT:
			format			= VkFormat::VK_FORMAT_R8G8_SINT;
			channels		= 2;
			return;


		// R32
		case OreOreLib::FORMAT_R32_FLOAT:
			format			= VkFormat::VK_FORMAT_R32_SFLOAT;
			channels		= 1;
			return;
		//case OreOreLib::FORMAT_R32_UNORM:
			//	return;
		case OreOreLib::FORMAT_R32_UINT:
			format			= VkFormat::VK_FORMAT_R32_UINT;
			channels		= 1;
			return;
		//case OreOreLib::FORMAT_R32_SNORM:
			//	return;
		case OreOreLib::FORMAT_R32_SINT:
			format			= VkFormat::VK_FORMAT_R32_SINT;
			channels		= 1;
			return;


		// R16
		case OreOreLib::FORMAT_R16_FLOAT:
			format			= VkFormat::VK_FORMAT_R16_SFLOAT;
			channels		= 1;
			return;
		case OreOreLib::FORMAT_R16_UNORM:
			format			= VkFormat::VK_FORMAT_R16_UNORM;
			channels		= 1;
			return;
		case OreOreLib::FORMAT_R16_UINT:
			format			= VkFormat::VK_FORMAT_R16_UINT;
			channels		= 1;
			return;
		case OreOreLib::FORMAT_R16_SNORM:
			format			= VkFormat::VK_FORMAT_R16_SNORM;
			channels		= 1;
			return;
		case OreOreLib::FORMAT_R16_SINT:
			format			= VkFormat::VK_FORMAT_R16_SINT;
			channels		= 1;
			return;


		// R8
		//case OreOreLib::FORMAT_R8_FLOAT:
			//	return;
		case OreOreLib::FORMAT_R8_UNORM:
			format			= VkFormat::VK_FORMAT_R8_UNORM;
			channels		= 1;
			return;
		case OreOreLib::FORMAT_R8_UINT:
			format			= VkFormat::VK_FORMAT_R8_UINT;
			channels		= 1;
			return;
		case OreOreLib::FORMAT_R8_SNORM:
			format			= VkFormat::VK_FORMAT_R8_SNORM;
			channels		= 1;
			return;
		case OreOreLib::FORMAT_R8_SINT:
			format			= VkFormat::VK_FORMAT_R8_SINT;
			channels		= 1;
			return;


		// D32
		case OreOreLib::FORMAT_D32_FLOAT:
			format			= VkFormat::VK_FORMAT_D32_SFLOAT;
			channels		= 1;
			return;
		case OreOreLib::FORMAT_D32_FLOAT_S8X24_UINT:
			format			= VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT;
			channels		= 1;
			return;


		// D24
		case OreOreLib::FORMAT_D24_UNORM_S8_UINT:
			format			= VkFormat::VK_FORMAT_D24_UNORM_S8_UINT;
			channels		= 2;
			return;


		// D16
		case OreOreLib::FORMAT_D16_UNORM:
			format			= VkFormat::VK_FORMAT_D16_UNORM;
			channels		= 1;
			return;
		case OreOreLib::FORMAT_D16_UNORM_S8_UINT:
			format			= VkFormat::VK_FORMAT_D16_UNORM_S8_UINT;
			channels		= 1;
			return;

		// S8
		case OreOreLib::FORMAT_S8_UINT:
			format			= VkFormat::VK_FORMAT_S8_UINT;
			channels		= 1;
			return;


		//default format
		default:
			format			= VkFormat::VK_FORMAT_UNDEFINED;
			channels		= 4;
			return;
		}

	}






	// Texture Address Mode
	enum TEXTURE_ADDRESS_MODE
	{
		TEX_ADDRESS_WRAP,
		TEX_ADDRESS_MIRROR,
		TEX_ADDRESS_CLAMP,
		TEX_ADDRESS_BORDER,
		TEX_ADDRESS_MIRRORCLAMP,
	};



	// Filter Mode
	enum FILTER_MODE
	{
		FILTER_MAG_NEAREST_MIN_NEAREST_MIPMAP_NEAREST,
		FILTER_MAG_NEAREST_MIN_LINEAR_MIPMAP_NEAREST,
		FILTER_MAG_NEAREST_MIN_NEAREST_MIPMAP_LINEAR,
		FILTER_MAG_NEAREST_MIN_LINEAR_MIPMAP_LINEAR,
		FILTER_MAG_MIN_NEAREST,
		FILTER_MAG_NEAREST_MIN_LINEAR,
		FILTER_MAG_LINEAR_MIN_NEAREST_MIPMAP_NEAREST,
		FILTER_MAG_LINEAR_MIN_LINEAR_MIPMAP_NEAREST,
		FILTER_MAG_LINEAR_MIN_NEAREST_MIPMAP_LINEAR,
		FILTER_MAG_LINEAR_MIN_LINEAR_MIPMAP_LINEAR,
		FILTER_MAG_LINEAR_MIN_NEAREST,
		FILTER_MAG_MIN_LINEAR,
	};


	static VkSamplerAddressMode GetAddressMode( TEXTURE_ADDRESS_MODE wrap )
	{
		switch( wrap )
		{
		case TEX_ADDRESS_WRAP:
			return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case TEX_ADDRESS_MIRROR:
			return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		case TEX_ADDRESS_CLAMP:
			return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		case TEX_ADDRESS_BORDER:
			return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		case TEX_ADDRESS_MIRRORCLAMP:
			return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
		default:
			return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		}
	}



	static void GetVkFilterMode( FILTER_MODE mode, VkFilter& mag_filter, VkFilter& min_filter, VkSamplerMipmapMode& mipmapMode )
	{
		switch( mode )
		{
		case FILTER_MAG_NEAREST_MIN_NEAREST_MIPMAP_NEAREST:
			mag_filter	= VkFilter::VK_FILTER_NEAREST;
			min_filter	= VkFilter::VK_FILTER_NEAREST;//GL_NEAREST_MIPMAP_NEAREST;
			mipmapMode	= VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST;
			return;
		case FILTER_MAG_NEAREST_MIN_LINEAR_MIPMAP_NEAREST:
			mag_filter = VkFilter::VK_FILTER_NEAREST;
			min_filter = VkFilter::VK_FILTER_LINEAR;//GL_LINEAR_MIPMAP_NEAREST;
			mipmapMode	= VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST;
			return;
		case FILTER_MAG_NEAREST_MIN_NEAREST_MIPMAP_LINEAR:
			mag_filter = VkFilter::VK_FILTER_NEAREST;
			min_filter = VkFilter::VK_FILTER_NEAREST;//GL_NEAREST_MIPMAP_LINEAR;
			mipmapMode	= VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR;
			return;
		case FILTER_MAG_NEAREST_MIN_LINEAR_MIPMAP_LINEAR:
			mag_filter = VkFilter::VK_FILTER_NEAREST;
			min_filter = VkFilter::VK_FILTER_LINEAR;//GL_LINEAR_MIPMAP_LINEAR;
			mipmapMode	= VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR;
			return;
		case FILTER_MAG_MIN_NEAREST:
			mag_filter = VkFilter::VK_FILTER_NEAREST;
			min_filter = VkFilter::VK_FILTER_NEAREST;
			return;
		case FILTER_MAG_NEAREST_MIN_LINEAR:
			mag_filter = VkFilter::VK_FILTER_NEAREST;
			min_filter = VkFilter::VK_FILTER_LINEAR;
			return;
		case FILTER_MAG_LINEAR_MIN_NEAREST_MIPMAP_NEAREST:
			mag_filter = VkFilter::VK_FILTER_LINEAR;
			min_filter = VkFilter::VK_FILTER_NEAREST;//GL_NEAREST_MIPMAP_NEAREST;
			mipmapMode	= VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST;
			return;
		case FILTER_MAG_LINEAR_MIN_LINEAR_MIPMAP_NEAREST:
			mag_filter = VkFilter::VK_FILTER_LINEAR;
			min_filter = VkFilter::VK_FILTER_LINEAR;// GL_LINEAR_MIPMAP_NEAREST;
			mipmapMode	= VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST;
			return;
		case FILTER_MAG_LINEAR_MIN_NEAREST_MIPMAP_LINEAR:
			mag_filter = VkFilter::VK_FILTER_LINEAR;
			min_filter = VkFilter::VK_FILTER_NEAREST;//GL_NEAREST_MIPMAP_LINEAR;
			mipmapMode	= VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR;
			return;
		case FILTER_MAG_LINEAR_MIN_LINEAR_MIPMAP_LINEAR:
			mag_filter = VkFilter::VK_FILTER_LINEAR;
			min_filter = VkFilter::VK_FILTER_LINEAR;//GL_LINEAR_MIPMAP_LINEAR;
			mipmapMode	= VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR;
			return;
		case FILTER_MAG_LINEAR_MIN_NEAREST:
			mag_filter = VkFilter::VK_FILTER_LINEAR;
			min_filter = VkFilter::VK_FILTER_NEAREST;
			return;
		case FILTER_MAG_MIN_LINEAR:
			mag_filter = VkFilter::VK_FILTER_LINEAR;
			min_filter = VkFilter::VK_FILTER_LINEAR;
			return;
		default:
			mag_filter = VkFilter::VK_FILTER_NEAREST;
			min_filter = VkFilter::VK_FILTER_NEAREST;
			return;
		}


	}


}// end of namespace vk


#endif // !VK_DATA_FORMAT_H
