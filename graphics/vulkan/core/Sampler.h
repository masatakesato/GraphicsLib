#ifndef SAMPLER_H
#define	SAMPLER_H

#include	"GraphicsDevice.h"



namespace vk
{

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
