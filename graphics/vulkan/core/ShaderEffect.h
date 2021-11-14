#ifndef SHADER_EFFECT_H
#define	SHADER_EFFECT_H

#include	"ShaderPass.h"



namespace vk
{
	using SubpassDependencies = OreOreLib::Array<VkSubpassDependency>;


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
		AttachmentDescArray				m_AttacmentDescs;

		OreOreLib::Array<ShaderPass>	m_ShaderPasses;
		SubpassDependencies				m_Dependencies;

	};


}// end of namespace vk 

#endif // !SHADER_EFFECT_H



