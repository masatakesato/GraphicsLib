#include	"GraphicsPipeline.h"

#include	"../utils/HelperFunctions.h"



namespace vk
{

	GraphicsPipeline::GraphicsPipeline()
	{

	}
	

	
	GraphicsPipeline::GraphicsPipeline( GraphicsDevice& device )
		: m_refDevice( device )
	{

	}



	GraphicsPipeline::~GraphicsPipeline()
	{
		Release();
	}



	void GraphicsPipeline::Build( const ShaderPass& shaderpass, const PipelineState& pipelinestate )
	{


		//========= Shader input geometry( vertex ) ===========//
		//auto bindingDescription = MyVertexLayout::getBindingDesctiption();
		//auto attributeDescriptions = MyVertexLayout::getAttributeDescriptions();
		//
		//VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		//vertexInputInfo.sType							= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		//vertexInputInfo.vertexBindingDescriptionCount	= 1;
		//vertexInputInfo.pVertexBindingDescriptions		= &bindingDescription;
		//vertexInputInfo.vertexAttributeDescriptionCount	= static_cast<uint32_t>( attributeDescriptions.Length() );
		//vertexInputInfo.pVertexAttributeDescriptions	= attributeDescriptions.begin();
		//
		//VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		//inputAssembly.sType						= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		//inputAssembly.topology					= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		//inputAssembly.primitiveRestartEnable	= VK_FALSE;


//========= Viewport ===========//
//VkViewport viewport = {};
//viewport.x			= 0.0f;
//viewport.y			= 0.0f;
//viewport.width		= (float)m_SwapChain.Extent().width;
//viewport.height		= (float)m_SwapChain.Extent().height;
//viewport.minDepth	= 0.0f;
//viewport.maxDepth	= 1.0f;

//VkRect2D scissor	= {};
//scissor.offset		= { 0, 0 };
//scissor.extent		= m_SwapChain.Extent();


//VkPipelineViewportStateCreateInfo viewportState = {};



//viewportState.sType			= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//viewportState.viewportCount	= 1;
//viewportState.pViewports	= nullptr;//&viewport;
//viewportState.scissorCount	= 1;
//viewportState.pScissors		= nullptr;//&scissor;

//VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };




		//=========== Rasterization ==============//
		//VkPipelineRasterizationStateCreateInfo rasterizer = {};
		//rasterizer.sType					= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		//rasterizer.depthClampEnable			= VK_FALSE;
		//rasterizer.rasterizerDiscardEnable	= VK_FALSE;
		//rasterizer.polygonMode				= VK_POLYGON_MODE_FILL;
		//rasterizer.lineWidth				= 1.0f;
		//rasterizer.cullMode					= VK_CULL_MODE_BACK_BIT;//VK_CULL_MODE_NONE;//
		//rasterizer.frontFace				= VK_FRONT_FACE_COUNTER_CLOCKWISE;//VK_FRONT_FACE_CLOCKWISE;//
		//rasterizer.depthBiasEnable			= VK_FALSE;
		//rasterizer.depthBiasConstantFactor	= 0.0f; // Optional
		//rasterizer.depthBiasClamp			= 0.0f; // Optional
		//rasterizer.depthBiasSlopeFactor		= 0.0f; // Optional


		//=========== Multisampling ==============//
		//VkPipelineMultisampleStateCreateInfo multisampling = {};
		//multisampling.sType					= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		//multisampling.sampleShadingEnable	= VK_FALSE;
		//multisampling.rasterizationSamples	= msaaSamples;
		//multisampling.minSampleShading		= 1.0f; // Optional
		//multisampling.pSampleMask			= nullptr; // Optional
		//multisampling.alphaToCoverageEnable	=  VK_FALSE; // Optional
		//multisampling.alphaToOneEnable		= VK_FALSE; // Optional



		//============ Color blending ==============// 
//-> class BlendStateとかにまとめる
		//VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		//colorBlendAttachment.colorWriteMask			= VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		//colorBlendAttachment.blendEnable			= VK_FALSE;
		//colorBlendAttachment.srcColorBlendFactor	= VK_BLEND_FACTOR_ONE; // Optional
		//colorBlendAttachment.dstColorBlendFactor	= VK_BLEND_FACTOR_ZERO; // Optional
		//colorBlendAttachment.colorBlendOp			= VK_BLEND_OP_ADD; // Optional
		//colorBlendAttachment.srcAlphaBlendFactor	= VK_BLEND_FACTOR_ONE; // Optional
		//colorBlendAttachment.dstAlphaBlendFactor	= VK_BLEND_FACTOR_ZERO; // Optioanl
		//colorBlendAttachment.alphaBlendOp			= VK_BLEND_OP_ADD; //Optional

//		VkPipelineColorBlendStateCreateInfo colorBlending = {};
//		colorBlending.sType				= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//		colorBlending.logicOpEnable		= VK_FALSE;
//		colorBlending.logicOp			= VK_LOGIC_OP_COPY; // Optional
//
////なんで複数個登録できるの? -> アタッチメント毎に個別設定できるように. ただし、VkPhysicalDeviceFeatures::independentBlendがtrueの場合に限る
//		colorBlending.attachmentCount	= 1;
//		colorBlending.pAttachments		= &colorBlendAttachment;
//
//		colorBlending.blendConstants[0] = 0.0f; // Optional
//		colorBlending.blendConstants[1] = 0.0f; // Optional
//		colorBlending.blendConstants[2] = 0.0f; // Optional
//		colorBlending.blendConstants[3] = 0.0f; // Optional



		//============== Depth stencil ================//
		//VkPipelineDepthStencilStateCreateInfo depthStencil = {};
		//depthStencil.sType					= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		//depthStencil.depthTestEnable		= VK_TRUE;
		//depthStencil.depthWriteEnable		= VK_TRUE;
		//
		//depthStencil.depthCompareOp			= VK_COMPARE_OP_LESS;

		//depthStencil.depthBoundsTestEnable	= VK_FALSE;
		//depthStencil.minDepthBounds			= 0.0f;
		//depthStencil.maxDepthBounds			= 1.0f;

		//depthStencil.stencilTestEnable		= VK_FALSE;
		//depthStencil.front					= {};
		//depthStencil.back					= {};


		////============== DynamicState ==================//
		//VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
		//dynamicStateInfo.sType	= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		//dynamicStateInfo.pDynamicStates	= dynamicStates;
		//dynamicStateInfo.dynamicStateCount = 2;
		//dynamicStateInfo.flags = 0;
	

/*
//============= Shader input parameter layout ============//
VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
pipelineLayoutInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
pipelineLayoutInfo.setLayoutCount			= paramlayout.NumSets();//1;
pipelineLayoutInfo.pSetLayouts				= &paramlayout.DescriptorSetLayout(0);
pipelineLayoutInfo.pushConstantRangeCount	= 0; // Optional
pipelineLayoutInfo.pPushConstantRanges		= nullptr; // Optional

if( vkCreatePipelineLayout( m_refDevice->Device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout ) != VK_SUCCESS )
	throw std::runtime_error( "failed to create pipeline layout!" );


//============== Create GraphisPipeline ================//

VkGraphicsPipelineCreateInfo pipelineInfo = {};
pipelineInfo.sType					= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
pipelineInfo.stageCount				= 2;
pipelineInfo.pStages				= shaderpass.CreateInfoArray().begin();//shaderStages;
pipelineInfo.pVertexInputState		= &m_VertexInputState.vertexInputInfo;//&vertexInputInfo;
pipelineInfo.pInputAssemblyState	= &m_VertexInputState.inputAssemblyInfo;//&inputAssembly;
pipelineInfo.pViewportState			= &m_ViewportState.createInfo;//&viewportState;
pipelineInfo.pRasterizationState	= &m_RasterizeInfo;//&rasterizer;
pipelineInfo.pMultisampleState		= &multisampling;
pipelineInfo.pDepthStencilState		= &depthStencil;
pipelineInfo.pColorBlendState		= &m_BlendState.createInfo;//&colorBlending;
pipelineInfo.pDynamicState			= &m_DynamicStates.createInfo;//&dynamicStateInfo;//nullptr; // Optional
pipelineInfo.layout					= m_PipelineLayout;//pipelineLayout;
pipelineInfo.renderPass				= renderPass;
pipelineInfo.subpass				= 0; TODO: Specify subpass index
pipelineInfo.basePipelineHandle		= VK_NULL_HANDLE;// Optional
pipelineInfo.basePipelineIndex		= -1;// Optional

VK_CHECK_RESULT( vkCreateGraphicsPipelines( m_refDevice->Device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline ) );
*/

		// Cleanup temporary data
		//shaderPass.Release( m_Device );

	}




	void GraphicsPipeline::Release()
	{

	}




}// end of namespace vk


