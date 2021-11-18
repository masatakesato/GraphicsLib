#include	"GraphicsPipeline.h"

#include	"../utils/HelperFunctions.h"



namespace vk
{

	GraphicsPipeline::GraphicsPipeline()
		: m_Pipeline( VK_NULL_HANDLE )
		, m_PipelineLayout( VK_NULL_HANDLE )
	{

	}
	

	
	GraphicsPipeline::GraphicsPipeline( GraphicsDevice& device )
		: m_refDevice( device )
		, m_Pipeline( VK_NULL_HANDLE )
		, m_PipelineLayout( VK_NULL_HANDLE )
	{

	}



	GraphicsPipeline::~GraphicsPipeline()
	{
		Release();
	}



	void GraphicsPipeline::Init( const ShaderPass& shaderpass, const PipelineState& pipelinestate, VkRenderPass renderPass, uint32_t subpass )
	{

		//============= Shader input parameter layout ============//
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount			= static_cast<uint32_t>( shaderpass.ParamLayout().NumSets() );
		pipelineLayoutInfo.pSetLayouts				= shaderpass.ParamLayout().DescriptorSetLayouts().begin();
		pipelineLayoutInfo.pushConstantRangeCount	= 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges		= nullptr; // Optional

		VK_CHECK_RESULT( vkCreatePipelineLayout( m_refDevice->Device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout ) );


		//============== Create GraphisPipeline ================//

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType					= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount				= shaderpass.CreateInfos().Length();
		pipelineInfo.pStages				= shaderpass.CreateInfos().begin();
		pipelineInfo.pVertexInputState		= &pipelinestate.m_VertexInputState;
		pipelineInfo.pInputAssemblyState	= &pipelinestate.m_InputAssemblyState;
		pipelineInfo.pViewportState			= &pipelinestate.m_ViewportState;
		pipelineInfo.pRasterizationState	= &pipelinestate.m_RasterizationState;
		pipelineInfo.pMultisampleState		= &pipelinestate.m_MultisampleState;
		pipelineInfo.pDepthStencilState		= &pipelinestate.m_DepthStencilState;
		pipelineInfo.pColorBlendState		= &pipelinestate.m_ColorBlendState;
		pipelineInfo.pDynamicState			= &pipelinestate.m_DynamicState;
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

			m_refDevice.Reset();
		}
	}




}// end of namespace vk


