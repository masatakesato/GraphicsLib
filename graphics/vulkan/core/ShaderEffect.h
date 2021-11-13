#ifndef SHADER_EFFECT_H
#define	SHADER_EFFECT_H

#include	"ShaderPass.h"



namespace vk
{

	class ShaderEffect
	{
	public:

		ShaderEffect();
		ShaderEffect( GraphicsDevice& device );
		ShaderEffect( const ShaderEffect& )=delete;
		~ShaderEffect();


	private:

		GraphicsDeviceRef				m_refDevice;

		OreOreLib::Array<ShaderPass>	m_ShaderPasses;
		VkRenderPass					m_RenderPass;

	};


}// end of namespace vk 

#endif // !SHADER_EFFECT_H



