#ifndef VK_DATA_FORMAT_H
#define	VK_DATA_FORMAT_H

#include	<vulkan/vulkan.h>

#include	<oreore/common/Utility.h>
#include	<oreore/images/DataFormat.h>



namespace vk
{

	static VkResult GetVkFormat( OreOreLib::DATA_FORMAT dataFormat, VkFormat &format, uint8 &channels )
	{

		//============== identify opengl texture format ===========//
		switch( dataFormat )
		{
		// R32G32B32A32
		case OreOreLib::FORMAT_R32G32B32A32_FLOAT:
			format			= VK_FORMAT_R32G32B32A32_SFLOAT;
			channels		= 4;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R32G32B32A32_UINT:
			format			= VK_FORMAT_R32G32B32A32_UINT;
			channels		= 4;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R32G32B32A32_SINT:
			format			= VK_FORMAT_R32G32B32A32_SINT;
			channels		= 4;
			return VK_SUCCESS;


		// R16G16B16A16
		case OreOreLib::FORMAT_R16G16B16A16_FLOAT:
			format			= VK_FORMAT_R16G16B16A16_SFLOAT;
			channels		= 4;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R16G16B16A16_UNORM:
			format			= VK_FORMAT_R16G16B16A16_UNORM;
			channels		= 4;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R16G16B16A16_UINT:
			format			= VK_FORMAT_R16G16B16A16_UINT;
			channels		= 4;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R16G16B16A16_SNORM:
			format			= VK_FORMAT_R16G16B16A16_SNORM;
			channels		= 4;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R16G16B16A16_SINT:
			format			= VK_FORMAT_R16G16B16A16_SINT;
			channels		= 4;
			return VK_SUCCESS;


		// R8G8B8A8
		case OreOreLib::FORMAT_R8G8B8A8_UNORM:
			format			= VK_FORMAT_R8G8B8A8_UNORM;
			channels		= 4;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R8G8B8A8_UINT:
			format			= VK_FORMAT_R8G8B8A8_UINT;
			channels		= 4;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R8G8B8A8_SNORM:
			format			= VK_FORMAT_R8G8B8A8_SNORM;
			channels		= 4;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R8G8B8A8_SINT:
			format			= VK_FORMAT_R8G8B8A8_SINT;
			channels		= 4;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R8G8B8A8_UNORM_SRGB:
			format			= VK_FORMAT_R8G8B8A8_SRGB;
			channels		= 4;
			return VK_SUCCESS;


		// B8G8R8A8
		case OreOreLib::FORMAT_B8G8R8A8_UNORM:
			format			= VK_FORMAT_B8G8R8A8_UNORM;
			channels		= 4;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_B8G8R8A8_UINT:
			format			= VK_FORMAT_B8G8R8A8_UINT;
			channels		= 4;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_B8G8R8A8_SNORM:
			format			= VK_FORMAT_B8G8R8A8_SNORM;
			channels		= 4;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_B8G8R8A8_SINT:
			format			= VK_FORMAT_B8G8R8A8_SINT;
			channels		= 4;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_B8G8R8A8_UNORM_SRGB:
			format			= VK_FORMAT_B8G8R8A8_SRGB;
			channels		= 4;
			return VK_SUCCESS;


		// R32G32B32
		case OreOreLib::FORMAT_R32G32B32_FLOAT:
			format			= VK_FORMAT_R32G32B32_SFLOAT;
			channels		= 3;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R32G32B32_UINT:
			format			= VK_FORMAT_R32G32B32_UINT;
			channels		= 3;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R32G32B32_SINT:
			format			= VK_FORMAT_R32G32B32_SINT;
			channels		= 3;
			return VK_SUCCESS;


		// R16G16B16
		case OreOreLib::FORMAT_R16G16B16_FLOAT:
			format			= VK_FORMAT_R16G16B16_SFLOAT;
			channels		= 3;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R16G16B16_UNORM:
			format			= VK_FORMAT_R16G16B16_UNORM;
			channels		= 3;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R16G16B16_UINT:
			format			= VK_FORMAT_R16G16B16_UINT;
			channels		= 3;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R16G16B16_SNORM:
			format			= VK_FORMAT_R16G16B16_SNORM;
			channels		= 3;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R16G16B16_SINT:
			format			= VK_FORMAT_R16G16B16_SINT;
			channels		= 3;
			return VK_SUCCESS;


		// R8G8B8
		case OreOreLib::FORMAT_R8G8B8_UNORM:
			format			= VK_FORMAT_R8G8B8_UNORM;
			channels		= 3;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R8G8B8_UINT:
			format			= VK_FORMAT_R8G8B8_UINT;
			channels		= 3;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R8G8B8_SNORM:
			format			= VK_FORMAT_R8G8B8_SNORM;
			channels		= 3;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R8G8B8_SINT:
			format			= VK_FORMAT_R8G8B8_SINT;
			channels		= 3;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R8G8B8_SRGB:
			format			= VK_FORMAT_R8G8B8_SRGB;
			channels		= 3;
			return VK_SUCCESS;

		// B8G8R8
		case OreOreLib::FORMAT_B8G8R8_UNORM:
			format			= VK_FORMAT_B8G8R8_UNORM;
			channels		= 3;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_B8G8R8_UINT:
			format			= VK_FORMAT_B8G8R8_UINT;
			channels		= 3;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_B8G8R8_SNORM:
			format			= VK_FORMAT_B8G8R8_SNORM;
			channels		= 3;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_B8G8R8_SINT:
			format			= VK_FORMAT_B8G8R8_SINT;
			channels		= 3;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_B8G8R8_SRGB:
			format			= VK_FORMAT_B8G8R8_SRGB;
			channels		= 3;
			return VK_SUCCESS;

		//R32G32
		case OreOreLib::FORMAT_R32G32_FLOAT:
			format			= VK_FORMAT_R32G32_SFLOAT;
			channels		= 2;
			return VK_SUCCESS;
			//case OreOreLib::FORMAT_R32G32_UNORM:
			//	return VK_SUCCESS;
		case OreOreLib::FORMAT_R32G32_UINT:
			format			= VK_FORMAT_R32G32_UINT;
			channels		= 2;
			return VK_SUCCESS;
			//case OreOreLib::FORMAT_R32G32_SNORM:
			//	return VK_SUCCESS;
		case OreOreLib::FORMAT_R32G32_SINT:
			format			= VK_FORMAT_R32G32_SINT;
			channels		= 2;
			return VK_SUCCESS;


		// R16G16
		case OreOreLib::FORMAT_R16G16_FLOAT:
			format			= VK_FORMAT_R16G16_SFLOAT;
			channels		= 2;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R16G16_UNORM:
			format			= VK_FORMAT_R16G16_UNORM;
			channels		= 2;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R16G16_UINT:
			format			= VK_FORMAT_R16G16_UINT;
			channels		= 2;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R16G16_SNORM:
			format			= VK_FORMAT_R16G16_SNORM;
			channels		= 2;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R16G16_SINT:
			format			= VK_FORMAT_R16G16_SINT;
			channels		= 2;
			return VK_SUCCESS;


		// R8G8
			//case OreOreLib::FORMAT_R8G8_FLOAT:
			//	return VK_SUCCESS;
		case OreOreLib::FORMAT_R8G8_UNORM:
			format			= VK_FORMAT_R8G8_UNORM;
			channels		= 2;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R8G8_UINT:
			format			= VK_FORMAT_R8G8_UINT;
			channels		= 2;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R8G8_SNORM:
			format			= VK_FORMAT_R8G8_SNORM;
			channels		= 2;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R8G8_SINT:
			format			= VK_FORMAT_R8G8_SINT;
			channels		= 2;
			return VK_SUCCESS;


		// R32
		case OreOreLib::FORMAT_R32_FLOAT:
			format			= VK_FORMAT_R32_SFLOAT;
			channels		= 1;
			return VK_SUCCESS;
		//case OreOreLib::FORMAT_R32_UNORM:
			//	return VK_SUCCESS;
		case OreOreLib::FORMAT_R32_UINT:
			format			= VK_FORMAT_R32_UINT;
			channels		= 1;
			return VK_SUCCESS;
		//case OreOreLib::FORMAT_R32_SNORM:
			//	return VK_SUCCESS;
		case OreOreLib::FORMAT_R32_SINT:
			format			= VK_FORMAT_R32_SINT;
			channels		= 1;
			return VK_SUCCESS;


		// R16
		case OreOreLib::FORMAT_R16_FLOAT:
			format			= VK_FORMAT_R16_SFLOAT;
			channels		= 1;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R16_UNORM:
			format			= VK_FORMAT_R16_UNORM;
			channels		= 1;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R16_UINT:
			format			= VK_FORMAT_R16_UINT;
			channels		= 1;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R16_SNORM:
			format			= VK_FORMAT_R16_SNORM;
			channels		= 1;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R16_SINT:
			format			= VK_FORMAT_R16_SINT;
			channels		= 1;
			return VK_SUCCESS;


		// R8
		//case OreOreLib::FORMAT_R8_FLOAT:
			//	return VK_SUCCESS;
		case OreOreLib::FORMAT_R8_UNORM:
			format			= VK_FORMAT_R8_UNORM;
			channels		= 1;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R8_UINT:
			format			= VK_FORMAT_R8_UINT;
			channels		= 1;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R8_SNORM:
			format			= VK_FORMAT_R8_SNORM;
			channels		= 1;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_R8_SINT:
			format			= VK_FORMAT_R8_SINT;
			channels		= 1;
			return VK_SUCCESS;


		// D32
		case OreOreLib::FORMAT_D32_FLOAT:
			format			= VK_FORMAT_D32_SFLOAT;
			channels		= 1;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_D32_FLOAT_S8X24_UINT:
			format			= VK_FORMAT_D32_SFLOAT_S8_UINT;
			channels		= 1;
			return VK_SUCCESS;


		// D24
		case OreOreLib::FORMAT_D24_UNORM_S8_UINT:
			format			= VK_FORMAT_D24_UNORM_S8_UINT;
			channels		= 2;
			return VK_SUCCESS;


		// D16
		case OreOreLib::FORMAT_D16_UNORM:
			format			= VK_FORMAT_D16_UNORM;
			channels		= 1;
			return VK_SUCCESS;
		case OreOreLib::FORMAT_D16_UNORM_S8_UINT:
			format			= VK_FORMAT_D16_UNORM_S8_UINT;
			channels		= 1;
			return VK_SUCCESS;

		// S8
		case OreOreLib::FORMAT_S8_UINT:
			format			= VK_FORMAT_S8_UINT;
			channels		= 1;
			return VK_SUCCESS;


		//default format
		default:
			format			= VK_FORMAT_UNDEFINED;
			channels		= 4;
			return VK_ERROR_FORMAT_NOT_SUPPORTED;
		}

	}


}// end of namespace vk


#endif // !VK_DATA_FORMAT_H
