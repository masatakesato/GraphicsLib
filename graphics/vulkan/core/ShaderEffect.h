#ifndef SHADER_EFFECT_H
#define	SHADER_EFFECT_H

#include	"Attachment.h"
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

		VkRenderPass					m_RenderPass;


		OreOreLib::Array<ShaderPass>	m_ShaderPasses;// shader modules
//		OreOreLib::Array<VkSubpassDependency>	m_SubpassDependencies;


	};


}// end of namespace vk 

#endif // !SHADER_EFFECT_H



