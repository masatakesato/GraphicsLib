#include	"GraphicsPipeline.h"

#include	"../utils/HelperFunctions.h"



namespace vk
{
	
	GraphicsPipeline::GraphicsPipeline()
		: m_Pipeline( VK_NULL_HANDLE )
		, m_PipelineLayout( VK_NULL_HANDLE )
	{

	}
	


	GraphicsPipeline::GraphicsPipeline( GraphicsPipeline&& obj )
		: m_refDevice( obj.m_refDevice )
		, m_Pipeline( obj.m_Pipeline )
		, m_PipelineLayout( obj.m_PipelineLayout )
	{
		obj.m_refDevice.Reset();
		obj.m_Pipeline			= VK_NULL_HANDLE;
		obj.m_PipelineLayout	= VK_NULL_HANDLE;
	}



	GraphicsPipeline::~GraphicsPipeline()
	{
		Release();
	}



	void GraphicsPipeline::Init( GraphicsDevice& device, const ShaderPass& shaderPass, VkRenderPass renderPass, uint32_t subpass )
	{
		//m_refDevice = device;

		////============= Shader input parameter layout ============//
		//VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		//pipelineLayoutInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		//pipelineLayoutInfo.setLayoutCount			= static_cast<uint32_t>( shaderPass.ParamLayout().NumSets() );
		//pipelineLayoutInfo.pSetLayouts				= shaderPass.ParamLayout().DescriptorSetLayouts().begin();
		//pipelineLayoutInfo.pushConstantRangeCount	= static_cast<uint32_t>( shaderPass.PushConstantRanges().Length() );
		//pipelineLayoutInfo.pPushConstantRanges		= shaderPass.PushConstantRanges().begin();

		//VK_CHECK_RESULT( vkCreatePipelineLayout( m_refDevice->Device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout ) );


		////============== Create GraphisPipeline ================//

		//VkGraphicsPipelineCreateInfo pipelineInfo = {};
		//pipelineInfo.sType					= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		//pipelineInfo.stageCount				= shaderPass.CreateInfos().Length();
		//pipelineInfo.pStages				= shaderPass.CreateInfos().begin();
		//pipelineInfo.pVertexInputState		= &shaderPass.State().m_VertexInputState;
		//pipelineInfo.pInputAssemblyState	= &shaderPass.State().m_InputAssemblyState;
		//pipelineInfo.pViewportState			= &shaderPass.State().m_ViewportState;
		//pipelineInfo.pRasterizationState	= &shaderPass.State().m_RasterizationState;
		//pipelineInfo.pMultisampleState		= &shaderPass.State().m_MultisampleState;
		//pipelineInfo.pDepthStencilState		= &shaderPass.State().m_DepthStencilState;
		//pipelineInfo.pColorBlendState		= &shaderPass.State().m_ColorBlendState;
		//pipelineInfo.pDynamicState			= &shaderPass.State().m_DynamicState;
		//pipelineInfo.layout					= m_PipelineLayout;
		//pipelineInfo.renderPass				= renderPass;
		//pipelineInfo.subpass				= subpass;
		//pipelineInfo.basePipelineHandle		= VK_NULL_HANDLE;// Optional
		//pipelineInfo.basePipelineIndex		= -1;// Optional

		//VK_CHECK_RESULT( vkCreateGraphicsPipelines( m_refDevice->Device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline ) );
	}



	void GraphicsPipeline::Init( GraphicsDevice& device, const ShaderPass& shaderPass,  const PipelineState& pipelineState, VkRenderPass renderPass, uint32_t subpass )
	{
		m_refDevice = device;

		//============= Shader input parameter layout ============//
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount			= static_cast<uint32_t>( shaderPass.ParamLayout().NumSets() );
		pipelineLayoutInfo.pSetLayouts				= shaderPass.ParamLayout().DescriptorSetLayouts().begin();
		pipelineLayoutInfo.pushConstantRangeCount	= static_cast<uint32_t>( shaderPass.PushConstantRanges().Length() );
		pipelineLayoutInfo.pPushConstantRanges		= shaderPass.PushConstantRanges().begin();

		VK_CHECK_RESULT( vkCreatePipelineLayout( m_refDevice->Device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout ) );


		//============== Create GraphisPipeline ================//

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType					= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount				= shaderPass.CreateInfos().Length();
		pipelineInfo.pStages				= shaderPass.CreateInfos().begin();
		pipelineInfo.pVertexInputState		= &pipelineState.m_VertexInputState;
		pipelineInfo.pInputAssemblyState	= &pipelineState.m_InputAssemblyState;
		pipelineInfo.pViewportState			= &pipelineState.m_ViewportState;
		pipelineInfo.pRasterizationState	= &pipelineState.m_RasterizationState;
		pipelineInfo.pMultisampleState		= &pipelineState.m_MultisampleState;
		pipelineInfo.pDepthStencilState		= &pipelineState.m_DepthStencilState;
		pipelineInfo.pColorBlendState		= &pipelineState.m_ColorBlendState;
		pipelineInfo.pDynamicState			= &pipelineState.m_DynamicState;
		pipelineInfo.layout					= m_PipelineLayout;
		pipelineInfo.renderPass				= renderPass;
		pipelineInfo.subpass				= subpass;
		pipelineInfo.basePipelineHandle		= VK_NULL_HANDLE;// Optional
		pipelineInfo.basePipelineIndex		= -1;// Optional

		VK_CHECK_RESULT( vkCreateGraphicsPipelines( m_refDevice->Device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline ) );
	}




	void GraphicsPipeline::Release()
	{
		if( !m_refDevice.IsNull() && m_refDevice->Device() != VK_NULL_HANDLE )
		{
			if( m_Pipeline != VK_NULL_HANDLE )
			{
				vkDestroyPipeline( m_refDevice->Device(), m_Pipeline, nullptr );
				m_Pipeline = VK_NULL_HANDLE;
			}

			if( m_PipelineLayout != VK_NULL_HANDLE )
			{
				vkDestroyPipelineLayout( m_refDevice->Device(), m_PipelineLayout, nullptr );
				m_PipelineLayout = VK_NULL_HANDLE;
			}

			//m_refDevice.Reset();
		}
	}




}// end of namespace vk


