#ifndef SHADER_PASS_H
#define	SHADER_PASS_H

#include	<oreore/common/TString.h>
#include	<oreore/container/Array.h>

#include	"GraphicsDevice.h"



namespace vk
{



	//class ShaderModule
	//{
	//public:

	//	ShaderModule();
	//	~ShaderModule();

	//	void Init();
	//	void Release();


	//	VkShaderStageFlagBits


	//private:

	//	VkShaderModule	m_Module;
	//	VkShaderStageFlagBits	m_Stage;
	//};




	class ShaderPass
	{
	public:

		struct ShaderStage
		{
			VkShaderModule module;
			VkShaderStageFlagBits stage;
		};


		ShaderPass();
		~ShaderPass();

		void Release( GraphicsDevice& device );

		void AddShaderStage( GraphicsDevice& device, VkShaderStageFlagBits stage, const tstring& filepath );
		void BuildCreateInfo();

	//	const uint32_t 

		const OreOreLib::Array<VkPipelineShaderStageCreateInfo>& CreateInfoArray() const	{ return m_CreateInfos; }



	private:

		OreOreLib::Array<ShaderStage>	m_ShaderStages;
		OreOreLib::Array<VkPipelineShaderStageCreateInfo>	m_CreateInfos;


		void InitShaderStage( GraphicsDevice& device, ShaderStage& shaderstage, const OreOreLib::Array<char>& shadercode, VkShaderStageFlagBits stage );


	};


}// end of namespace vk


#endif // !SHADER_PASS_H
