#include	"ShaderPass.h"

#include	"../utils/FileIO.h"
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



	void ShaderPass::Release()
	{
		if( !m_refDevice.IsNull() && m_refDevice->Device() != VK_NULL_HANDLE )
		{
			for( auto& shaderstage : m_ShaderStages )
				vkDestroyShaderModule( m_refDevice->Device(), shaderstage.module, nullptr );
			m_ShaderStages.Release();

			m_refDevice.Reset();
		}



		m_ShaderParamLayout.Release();

	}



	void ShaderPass::AddShaderStage( VkShaderStageFlagBits stage, const tstring& filepath )
	{
		auto i = m_ShaderStages.AddToTail();
		auto shadercode = binaryFileRead( filepath );

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


	
	

	void ShaderPass::InitShaderStage( ShaderStage& shaderstage, const OreOreLib::Array<char>& shadercode, VkShaderStageFlagBits stage )
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType	= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize	= shadercode.Length();
		createInfo.pCode	= reinterpret_cast<const uint32_t*>( shadercode.begin() );

		VK_CHECK_RESULT( vkCreateShaderModule( m_refDevice->Device(), &createInfo, nullptr, &shaderstage.module ) );

		shaderstage.stage	= stage;
	}





}// end of namespace vk