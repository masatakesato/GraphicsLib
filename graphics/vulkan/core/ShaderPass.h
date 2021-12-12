﻿#ifndef SHADER_PASS_H
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
		~ShaderPass();
		ShaderPass( const ShaderPass& ) = delete;
		ShaderPass( ShaderPass&& obj );

		void Release();

		void BindDevice( GraphicsDevice& device )	{ m_refDevice = device; }

		void AddShaderStage( VkShaderStageFlagBits stage, const tstring& filepath );
		void BuildCreateInfo();

		void SetInputRenderTargetIDs( std::initializer_list<uint32_t> ilist );
		void SetOutputRenderTargetIDs( std::initializer_list<uint32_t> ilist );


		const OreOreLib::Array<VkPipelineShaderStageCreateInfo>& CreateInfos() const	{ return m_CreateInfos; }


		void InitDescriptorSetLayouts( std::initializer_list< std::initializer_list<VkDescriptorSetLayoutBinding> > bindings )
		{
			m_ShaderParamLayout.Init( m_refDevice->Device(), bindings );
		}


		const ShaderParamLayout& ParamLayout() const	{ return m_ShaderParamLayout; }


		const OreOreLib::Array<uint32_t>& InputRenderTargetIDs() const	{ return m_InputRenderTargetIDs; }
		const OreOreLib::Array<uint32_t>& OutputRenderTargetIDs() const	{ return m_OutputRenderTargetIDs; }



	private:

		GraphicsDeviceRef	m_refDevice;

		OreOreLib::Array<ShaderStage>	m_ShaderStages;
		OreOreLib::Array<VkPipelineShaderStageCreateInfo>	m_CreateInfos;


		ShaderParamLayout	m_ShaderParamLayout;
	
		OreOreLib::Array<uint32_t>	m_InputRenderTargetIDs;
		OreOreLib::Array<uint32_t>	m_OutputRenderTargetIDs;


		void InitShaderStage( ShaderStage& shaderstage, const OreOreLib::Array<char>& shadercode, VkShaderStageFlagBits stage );


	};


}// end of namespace vk


#endif // !SHADER_PASS_H
