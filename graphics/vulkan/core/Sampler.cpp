#include	"Sampler.h"

#include	"../utils/HelperFunctions.h"



namespace vulkan
{

	static VkSamplerCreateInfo DefaultSamplerInfo()
	{
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType		= VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter	= VK_FILTER_LINEAR;
		samplerInfo.minFilter	= VK_FILTER_LINEAR;

		samplerInfo.addressModeU	= VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV	= VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW	= VK_SAMPLER_ADDRESS_MODE_REPEAT;

		samplerInfo.anisotropyEnable	= VK_FALSE;
		samplerInfo.maxAnisotropy		= 1.0f;// set 1.0 if anisotropyEnable is VK_FALSE

		samplerInfo.borderColor	= VK_BORDER_COLOR_INT_OPAQUE_BLACK;

		samplerInfo.unnormalizedCoordinates	= VK_FALSE;// VK_TRUE: (0, width/height], VK_FALSE: (0, 1]

		samplerInfo.mipmapMode	= VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.minLod		= 0.0f;
		samplerInfo.maxLod		= 1.0f;//static_cast<float>( m_Texture.MipLevels() );
		samplerInfo.mipLodBias	= 0.0f;

		return samplerInfo;
	}




	Sampler::Sampler()
		: m_Sampler( VK_NULL_HANDLE )
		, m_SamplerInfo( DefaultSamplerInfo() )
	{

	}



	Sampler::Sampler( GraphicsDevice& device, float maxAnisotropy, float maxlod )
		: m_Sampler( VK_NULL_HANDLE )
		, m_SamplerInfo( DefaultSamplerInfo() )
	{
		Init( device, maxAnisotropy, maxlod );
	}



	Sampler::~Sampler()
	{

	}



	void Sampler::Init( GraphicsDevice& device, float maxAnisotropy, float maxlod )
	{
		m_refDevice	= device;

		//VkSamplerCreateInfo samplerInfo = {};
		//samplerInfo.sType		= VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		//samplerInfo.magFilter	= VK_FILTER_LINEAR;
		//samplerInfo.minFilter	= VK_FILTER_LINEAR;

		//samplerInfo.addressModeU	= VK_SAMPLER_ADDRESS_MODE_REPEAT;
		//samplerInfo.addressModeV	= VK_SAMPLER_ADDRESS_MODE_REPEAT;
		//samplerInfo.addressModeW	= VK_SAMPLER_ADDRESS_MODE_REPEAT;

		VkPhysicalDeviceProperties properties = {};
		vkGetPhysicalDeviceProperties( m_refDevice->PhysicalDevice(), &properties );

		m_SamplerInfo.anisotropyEnable	= maxAnisotropy==1.0f ? VK_FALSE : VK_TRUE;
		m_SamplerInfo.maxAnisotropy		= Min( maxAnisotropy, properties.limits.maxSamplerAnisotropy );// set 1.0 if anisotropyEnable is VK_FALSE

		//samplerInfo.borderColor	= VK_BORDER_COLOR_INT_OPAQUE_BLACK;

		//samplerInfo.unnormalizedCoordinates	= VK_FALSE;// VK_TRUE: (0, width/height], VK_FALSE: (0, 1]

		//samplerInfo.mipmapMode	= VK_SAMPLER_MIPMAP_MODE_LINEAR;
		//samplerInfo.minLod		= 0.0f;
		m_SamplerInfo.maxLod		= maxlod;//static_cast<float>( m_Texture.MipLevels() );
		//samplerInfo.mipLodBias	= 0.0f;


		VK_CHECK_RESULT( vkCreateSampler( m_refDevice->Device(), &m_SamplerInfo, nullptr, &m_Sampler ) );
	}



	void Sampler::Release()
	{
		if( !m_refDevice.IsNull() && m_refDevice->Device() != VK_NULL_HANDLE )
		{
			if( m_Sampler != VK_NULL_HANDLE )
			{
				vkDestroySampler( m_refDevice->Device(), m_Sampler, nullptr );
				m_Sampler = VK_NULL_HANDLE;
			}

			m_refDevice.Reset();
		}

		m_SamplerInfo = DefaultSamplerInfo();
	}



	void Sampler::SetFilter( VkFilter magFilter, VkFilter minfilter )
	{
		m_SamplerInfo.magFilter	= magFilter;
		m_SamplerInfo.minFilter	= minfilter;
	}



	void Sampler::SetAddressMode( VkSamplerAddressMode u, VkSamplerAddressMode v, VkSamplerAddressMode w )
	{
		m_SamplerInfo.addressModeU	= u;
		m_SamplerInfo.addressModeV	= v;
		m_SamplerInfo.addressModeW	= w;
	}
	


	void Sampler::SetMaxAnisotropy( float maxAnisotropy )
	{
		m_SamplerInfo.anisotropyEnable	= maxAnisotropy==1.0f ? VK_FALSE : VK_TRUE;
		m_SamplerInfo.maxAnisotropy		= maxAnisotropy;//Min( maxAnisotropy, properties.limits.maxSamplerAnisotropy );// set 1.0 if anisotropyEnable is VK_FALSE
	}
	
	

	void Sampler::SetMipmapLod( VkSamplerMipmapMode mode, float minlod, float maxlod, float bias )
	{
		m_SamplerInfo.mipmapMode	= mode;
		m_SamplerInfo.minLod		= minlod;
		m_SamplerInfo.maxLod		= maxlod;
		m_SamplerInfo.mipLodBias	= bias;
	}



}// end of namespace vulkan