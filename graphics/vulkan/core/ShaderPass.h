#ifndef SHADER_PASS_H
#define	SHADER_PASS_H

//#include	<oreore/common/TString.h>
//#include	<oreore/container/Array.h>

//#include	"GraphicsDevice.h"

#include	"ShaderParamLayout.h"
#include	"DescriptorSets.h"



namespace vk
{

	class ShaderPass
	{
	public:

		struct ShaderStage
		{
			VkShaderModule module;
			VkShaderStageFlagBits stage;
		};


		ShaderPass();
		ShaderPass( GraphicsDevice& device );
		ShaderPass( const ShaderPass& )=delete;
		~ShaderPass();

		void Release();

		void AddShaderStage( VkShaderStageFlagBits stage, const tstring& filepath );
		void BuildCreateInfo();



		const OreOreLib::Array<VkPipelineShaderStageCreateInfo>& CreateInfos() const	{ return m_CreateInfos; }


		void InitDescriptorSetLayouts( std::initializer_list< std::initializer_list<VkDescriptorSetLayoutBinding> > bindings )
		{
			m_ShaderParamLayout.Init( m_refDevice->Device(), bindings );
		}


		const ShaderParamLayout& ParamLayout() const	{ return m_ShaderParamLayout; }




	private:

		GraphicsDeviceRef	m_refDevice;

		OreOreLib::Array<ShaderStage>	m_ShaderStages;
		OreOreLib::Array<VkPipelineShaderStageCreateInfo>	m_CreateInfos;


		ShaderParamLayout	m_ShaderParamLayout;
	

		void InitShaderStage( ShaderStage& shaderstage, const OreOreLib::Array<char>& shadercode, VkShaderStageFlagBits stage );


	};


}// end of namespace vk


#endif // !SHADER_PASS_H
