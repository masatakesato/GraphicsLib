#ifndef SHADER_PASS_H
#define	SHADER_PASS_H

#include	"ShaderParamLayout.h"
#include	"DescriptorBuffer.h"
#include	"PipelineState.h"



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

		void InitPushConstantRanges( std::initializer_list<VkPushConstantRange> constRanges );
		void InitDescriptorSetLayouts( std::initializer_list< std::initializer_list<VkDescriptorSetLayoutBinding> > bindings );


		const OreOreLib::Array<VkPipelineShaderStageCreateInfo>& CreateInfos() const	{ return m_CreateInfos; }
		const OreOreLib::Array<VkPushConstantRange>& PushConstantRanges() const			{ return m_PushConstantRanges; }
		const ShaderParamLayout& ParamLayout() const									{ return m_ShaderParamLayout; }

		const OreOreLib::Array<uint32_t>& InputRenderTargetIDs() const					{ return m_InputRenderTargetIDs; }
		const OreOreLib::Array<uint32_t>& OutputRenderTargetIDs() const					{ return m_OutputRenderTargetIDs; }

//		void SetPipelineState( const PipelineState& pipelineState )	{ m_State = pipelineState; }
//		const PipelineState& State() const					{ return m_State; }
//		PipelineState& State()					{ return m_State; }


	private:

		GraphicsDeviceRef	m_refDevice;

		OreOreLib::Array<ShaderStage>	m_ShaderStages;
		OreOreLib::Array<VkPipelineShaderStageCreateInfo>	m_CreateInfos;

		// PushConstants
		OreOreLib::Array<VkPushConstantRange>	m_PushConstantRanges;

		// DescriptorSetLayouts
		ShaderParamLayout						m_ShaderParamLayout;
	
		// Render target In/Outs
		OreOreLib::Array<uint32_t>				m_InputRenderTargetIDs;
		OreOreLib::Array<uint32_t>				m_OutputRenderTargetIDs;

		// RenderStata
//		PipelineState							m_State;


		void InitShaderStage( ShaderStage& shaderstage, const OreOreLib::Array<char>& shadercode, VkShaderStageFlagBits stage );


	};


}// end of namespace vk


#endif // !SHADER_PASS_H
