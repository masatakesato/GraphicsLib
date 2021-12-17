#include	"ShaderPass.h"

#include	<oreore/io/FileIO.h>

#include	"../utils/HelperFunctions.h"



namespace vk
{

	ShaderPass::ShaderPass()
	{

	}



	ShaderPass::ShaderPass( GraphicsDevice& device )
		: m_refDevice( device )
	{

	}



	ShaderPass::~ShaderPass()
	{
		Release();

	}



	ShaderPass::ShaderPass( ShaderPass&& obj )
		: m_refDevice( obj.m_refDevice )
		, m_ShaderStages( (OreOreLib::Array<ShaderPass>&&) obj.m_ShaderStages )
		, m_CreateInfos( (OreOreLib::Array<VkPipelineShaderStageCreateInfo>&&) obj.m_CreateInfos )
		, m_ShaderParamLayout( obj.m_ShaderParamLayout )
	{

	}



	void ShaderPass::Release()
	{
		if( !m_refDevice.IsNull() && m_refDevice->Device() != VK_NULL_HANDLE )
		{
			m_PushConstantRanges.Release();

			for( auto& shaderstage : m_ShaderStages )
				vkDestroyShaderModule( m_refDevice->Device(), shaderstage.module, nullptr );
			m_ShaderStages.Release();

			m_CreateInfos.Release();

			m_State.Release();

			//m_refDevice.Reset();
		}



		m_ShaderParamLayout.Release();

		m_InputRenderTargetIDs.Release();
		m_OutputRenderTargetIDs.Release();

	}



	void ShaderPass::AddShaderStage( VkShaderStageFlagBits stage, const tstring& filepath )
	{
		auto i = m_ShaderStages.AddToTail();
		auto shadercode = OreOreLib::binaryFileRead( filepath );

		InitShaderStage( m_ShaderStages[i], shadercode, stage );


	}



	void ShaderPass::BuildCreateInfo()
	{
		if( !m_ShaderStages )
			return;

		m_CreateInfos.Resize( m_ShaderStages.Length() );

		auto shaderstage = m_ShaderStages.begin();
		for( auto& info : m_CreateInfos )
		{
			info.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			info.stage	= shaderstage->stage;
			info.module	= shaderstage->module;
			info.pName	= "main";

			shaderstage++;
		}

	}



	void ShaderPass::SetInputRenderTargetIDs( std::initializer_list<uint32> ilist )
	{
		m_InputRenderTargetIDs.Init( ilist.begin(), ilist.end() );
	}



	void ShaderPass::SetOutputRenderTargetIDs( std::initializer_list<uint32> ilist )
	{
		m_OutputRenderTargetIDs.Init( ilist.begin(), ilist.end() );
	}

	

	void ShaderPass::InitPushConstantRanges( std::initializer_list<VkPushConstantRange> constRanges )
	{
		m_PushConstantRanges.Init( constRanges.begin(), constRanges.end() );
	}



	void ShaderPass::InitDescriptorSetLayouts( std::initializer_list< std::initializer_list<VkDescriptorSetLayoutBinding> > bindings )
	{
		m_ShaderParamLayout.Init( m_refDevice->Device(), bindings );
	}



	void ShaderPass::InitShaderStage( ShaderStage& shaderstage, const OreOreLib::Array<char>& shadercode, VkShaderStageFlagBits stage )
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType	= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize	= shadercode.Length<size_t>();
		createInfo.pCode	= reinterpret_cast<const uint32_t*>( shadercode.begin() );

		VK_CHECK_RESULT( vkCreateShaderModule( m_refDevice->Device(), &createInfo, nullptr, &shaderstage.module ) );

		shaderstage.stage	= stage;
	}





}// end of namespace vk