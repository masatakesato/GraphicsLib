#include	"ShaderPass.h"

#include	<oreore/io/FileIO.h>

#include	"../utils/HelperFunctions.h"



namespace vk
{


	//##################################################################################//
	//																					//
	//								AttachmentRefs									//
	//																					//
	//##################################################################################//


	AttachmentRefs::AttachmentRefs()
	{

	}

	

	AttachmentRefs::~AttachmentRefs()
	{
		Release();
	}



	void AttachmentRefs::Init( int numInputs, int numColorAttachments, bool depthStencil )
	{
		m_InputAttachments.Resize( numInputs );

		m_ColorAttachments.Resize( numInputs );
		m_ResolveAttachments.Resize( numInputs, { VK_ATTACHMENT_UNUSED } );

		m_DepthStencilAttachment = {};
	}



	void AttachmentRefs::Release()
	{
		m_InputAttachments.Release();
		m_ColorAttachments.Release();
		m_ResolveAttachments.Release();

	}



	void AttachmentRefs::SetInputAttachments( std::initializer_list<VkAttachmentReference> ilist )
	{
		m_InputAttachments.Init( ilist );
	}



	void AttachmentRefs::SetColorAttachments( std::initializer_list<VkAttachmentReference> ilist )
	{
		m_ColorAttachments.Init( ilist );
	}



	void AttachmentRefs::SetResolveAttachments( std::initializer_list<VkAttachmentReference> ilist )
	{
		ASSERT( m_ColorAttachments.Length() == (int)ilist.size() );
		m_ResolveAttachments.Init( ilist );
	}



	void AttachmentRefs::SetDepthAttachment( VkAttachmentReference attachref )
	{
		m_DepthStencilAttachment = attachref;
	}












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
			for( auto& shaderstage : m_ShaderStages )
				vkDestroyShaderModule( m_refDevice->Device(), shaderstage.module, nullptr );
			m_ShaderStages.Release();

			m_CreateInfos.Release();

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



	void ShaderPass::SetInputRenderTargetIDs( std::initializer_list<uint32_t> ilist )
	{
		m_InputRenderTargetIDs.Init( ilist.begin(), ilist.end() );
	}



	void ShaderPass::SetOutputRenderTargetIDs( std::initializer_list<uint32_t> ilist )
	{
		m_OutputRenderTargetIDs.Init( ilist.begin(), ilist.end() );
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