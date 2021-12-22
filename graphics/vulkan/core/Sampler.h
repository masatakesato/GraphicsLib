#ifndef SAMPLER_H
#define	SAMPLER_H

#include	"GraphicsDevice.h"



namespace vk
{
	//struct FilterMode
	//{
	//	VkFilter			MagFilter;
	//	VkFilter			MinFilter;
	//	VkSamplerMipmapMode	MipmapMode;
	//};
	//	

	//const FilterMode Mag_Min_Nearest			{ VK_FILTER_NEAREST, VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_NEAREST };
	//const FilterMode Mag_Min_Nearest_Mip_Nearest{ VK_FILTER_NEAREST, VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_NEAREST };
	//const FilterMode Mag_Min_Nearest_Mip_Linear	{ VK_FILTER_NEAREST, VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_LINEAR };

	//const FilterMode Mag_Min_Linear				{ VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST };
	//const FilterMode Mag_Min_Linear_Mip_Nearest	{ VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST };
	//const FilterMode Mag_Min_Linear_Mip_Linear	{ VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR };

	//const FilterMode Mag_Nearest_Min_Linear				{ VK_FILTER_NEAREST, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST };
	//const FilterMode Mag_Nearest_Min_Linear_Mip_Nearest	{ VK_FILTER_NEAREST, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST };
	//const FilterMode Mag_Nearest_Min_Linear_Mip_Linear	{ VK_FILTER_NEAREST, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR };

	//const FilterMode Mag_Linear_Min_Nearest				{ VK_FILTER_LINEAR, VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_NEAREST };
	//const FilterMode Mag_Linear_Min_Nearest_Mip_Nearest	{ VK_FILTER_LINEAR, VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_NEAREST };
	//const FilterMode Mag_Linear_Min_Nearest_Mip_Linear	{ VK_FILTER_LINEAR, VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_LINEAR };





	class Sampler
	{
	public:

		Sampler();
		Sampler( GraphicsDevice& device, float maxAnisotropy, float maxlod );
		~Sampler();
		Sampler( const Sampler& )=delete;

		void Init( GraphicsDevice& device, float maxAnisotropy, float maxlod );
		void Release();

		void SetFilter( VkFilter magFilter, VkFilter minfilter );
		//void SetFilter( FilterMode mode );
		void SetAddressMode( VkSamplerAddressMode u, VkSamplerAddressMode v, VkSamplerAddressMode w );
		void SetMaxAnisotropy( float maxAnisotropy );
		void SetMipmapLod( VkSamplerMipmapMode mode, float minlod, float maxlod, float bias );


		const VkSampler& Handle() const	{ return m_Sampler; }




	private:
		
		GraphicsDeviceRef	m_refDevice;

		VkSampler	m_Sampler;
		VkSamplerCreateInfo m_SamplerInfo{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };

	};


}// end of namespace vk


#endif // !SAMPLER_H
