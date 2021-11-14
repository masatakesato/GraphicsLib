#include	"GraphicsPipeline.h"



namespace vk
{

	// Rasterization settings
	void GraphicsPipeline::SetDepthClamp( VkBool32 flag ){}
	void GraphicsPipeline::SetRasterizerDiscard( VkBool32 flag ){}
	void GraphicsPipeline::SetPolygonMode( VkPolygonMode mode ){}
	void GraphicsPipeline::SetLineWidth( float width ){}
	void GraphicsPipeline::SetCullMode( VkCullModeFlags mode ){}
	void GraphicsPipeline::SetFrontFace( VkFrontFace mode ){}
	void GraphicsPipeline::SetDepthBias( VkBool32 flag, float constantfactor, float biasclamp, float slopefactor ){}


	// Multisampe settings
	void GraphicsPipeline::SetSampleShading( VkBool32 flag )	{ multisampling.sampleShadingEnable	= flag;	}
	void GraphicsPipeline::SetRasterizationSamples( VkSampleCountFlagBits flag )	{ multisampling.rasterizationSamples = flag; }
	void GraphicsPipeline::SetMinSampleShading( float val )	{ multisampling.minSampleShading = 1.0f; }// Optional
	void GraphicsPipeline::SetSampleMask( VkSampleMask* mask )	{ multisampling.pSampleMask = nullptr; }// Optional
	void GraphicsPipeline::SetAlphaToCoverage( VkBool32 flag )	{ multisampling.alphaToCoverageEnable = flag; } // Optional
	void GraphicsPipeline::SetAlphaToOne( VkBool32 flag )	{ multisampling.alphaToOneEnable = flag; }// Optional


	// Color blending
	void GraphicsPipeline::SetBlend( VkBool32 flag )	{ colorBlendAttachment.blendEnable = flag; }
	void GraphicsPipeline::SetColorBlendFactor( VkBlendFactor src, VkBlendFactor dst )	{ colorBlendAttachment.srcColorBlendFactor = src; colorBlendAttachment.dstColorBlendFactor = dst; }// Optional
	void GraphicsPipeline::SetColorBlendOperation( VkBlendOp op )	{ colorBlendAttachment.colorBlendOp	= op; }// Optional
	void GraphicsPipeline::SetAlphaBlendFactor( VkBlendFactor src, VkBlendFactor dst )	{ colorBlendAttachment.srcAlphaBlendFactor = src; colorBlendAttachment.dstAlphaBlendFactor = dst; }// Optional
	void GraphicsPipeline::SetAlphaBlendOperation( VkBlendOp op )	{ colorBlendAttachment.alphaBlendOp = op; } //Optional
	void GraphicsPipeline::SetWriteMask( VkColorComponentFlagBits mask )	{ colorBlendAttachment.colorWriteMask = mask; }




	void GraphicsPipeline::Init()
	{


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
-> class BlendStateとかにまとめる
		//VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		//colorBlendAttachment.colorWriteMask			= VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		//colorBlendAttachment.blendEnable			= VK_FALSE;
		//colorBlendAttachment.srcColorBlendFactor	= VK_BLEND_FACTOR_ONE; // Optional
		//colorBlendAttachment.dstColorBlendFactor	= VK_BLEND_FACTOR_ZERO; // Optional
		//colorBlendAttachment.colorBlendOp			= VK_BLEND_OP_ADD; // Optional
		//colorBlendAttachment.srcAlphaBlendFactor	= VK_BLEND_FACTOR_ONE; // Optional
		//colorBlendAttachment.dstAlphaBlendFactor	= VK_BLEND_FACTOR_ZERO; // Optioanl
		//colorBlendAttachment.alphaBlendOp			= VK_BLEND_OP_ADD; //Optional

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType				= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable		= VK_FALSE;
		colorBlending.logicOp			= VK_LOGIC_OP_COPY; // Optional

なんで複数個登録できるの? -> アタッチメント毎に個別設定できるように. ただし、VkPhysicalDeviceFeatures::independentBlendがtrueの場合に限る
		colorBlending.attachmentCount	= 1;
		colorBlending.pAttachments		= &colorBlendAttachment;

		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional


		//============= Shader input parameter layout ============//
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount			= m_ShaderParamLayout.NumSets();//1;
		pipelineLayoutInfo.pSetLayouts				= &m_ShaderParamLayout.DescriptorSetLayout(0);
		pipelineLayoutInfo.pushConstantRangeCount	= 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges		= nullptr; // Optional

		if( vkCreatePipelineLayout( m_Device.Device(), &pipelineLayoutInfo, nullptr, &pipelineLayout ) != VK_SUCCESS )
			throw std::runtime_error( "failed to create pipeline layout!" );


		//============== Depth stencil ================//
		VkPipelineDepthStencilStateCreateInfo depthStencil = {};
		depthStencil.sType					= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable		= VK_TRUE;
		depthStencil.depthWriteEnable		= VK_TRUE;
		
		depthStencil.depthCompareOp			= VK_COMPARE_OP_LESS;

		depthStencil.depthBoundsTestEnable	= VK_FALSE;
		depthStencil.minDepthBounds			= 0.0f;
		depthStencil.maxDepthBounds			= 1.0f;

		depthStencil.stencilTestEnable		= VK_FALSE;
		depthStencil.front					= {};
		depthStencil.back					= {};


		//============== DynamicState ==================//
		VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
		dynamicStateInfo.sType	= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateInfo.pDynamicStates	= dynamicStates;
		dynamicStateInfo.dynamicStateCount = 2;
		dynamicStateInfo.flags = 0;


		//============== Create GraphisPipeline ================//

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType					= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount				= 2;
		pipelineInfo.pStages				= shaderPass.CreateInfoArray().begin();//shaderStages;
		pipelineInfo.pVertexInputState		= &vertexInputInfo;
		pipelineInfo.pInputAssemblyState	= &inputAssembly;
		pipelineInfo.pViewportState			= &viewportState;
		pipelineInfo.pRasterizationState	= &rasterizer;
		pipelineInfo.pMultisampleState		= &multisampling;
		pipelineInfo.pDepthStencilState		= &depthStencil;
		pipelineInfo.pColorBlendState		= &colorBlending;
		pipelineInfo.pDynamicState			= &dynamicStateInfo;//nullptr; // Optional
		pipelineInfo.layout					= pipelineLayout;
		pipelineInfo.renderPass				= renderPass;
		pipelineInfo.subpass				= 0;
		pipelineInfo.basePipelineHandle		= VK_NULL_HANDLE;// Optional
		pipelineInfo.basePipelineIndex		= -1;// Optional

		if( vkCreateGraphicsPipelines( m_Device.Device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline ) != VK_SUCCESS )
			throw std::runtime_error( "failed to create graphics pipeline!" );



		// Cleanup temporary data
		//vkDestroyShaderModule( m_Device.Device(), vertShaderModule, nullptr );
		//vkDestroyShaderModule( m_Device.Device(), fragShaderModule, nullptr );

		shaderPass.Release( m_Device );

	}


}// end of namespace vk


