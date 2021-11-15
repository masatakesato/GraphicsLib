#include	"GraphicsPipeline.h"



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



	// Rasterization settings
	void GraphicsPipeline::SetDepthClamp( VkBool32 flag ){}
	void GraphicsPipeline::SetRasterizerDiscard( VkBool32 flag ){}
	void GraphicsPipeline::SetPolygonMode( VkPolygonMode mode ){}
	void GraphicsPipeline::SetLineWidth( float width ){}
	void GraphicsPipeline::SetCullMode( VkCullModeFlags mode ){}
	void GraphicsPipeline::SetFrontFace( VkFrontFace mode ){}
	void GraphicsPipeline::SetDepthBias( VkBool32 flag, float constantfactor, float biasclamp, float slopefactor ){}


	// Multisampe settings
	void GraphicsPipeline::SetSampleShading( VkBool32 flag )						{ multisampling.sampleShadingEnable	= flag;	}
	void GraphicsPipeline::SetRasterizationSamples( VkSampleCountFlagBits flag )	{ multisampling.rasterizationSamples = flag; }
	void GraphicsPipeline::SetMinSampleShading( float val )							{ multisampling.minSampleShading = 1.0f; }// Optional
	void GraphicsPipeline::SetSampleMask( VkSampleMask* mask )						{ multisampling.pSampleMask = nullptr; }// Optional
	void GraphicsPipeline::SetAlphaToCoverage( VkBool32 flag )						{ multisampling.alphaToCoverageEnable = flag; } // Optional
	void GraphicsPipeline::SetAlphaToOne( VkBool32 flag )							{ multisampling.alphaToOneEnable = flag; }// Optional


	// Color blending
	void GraphicsPipeline::SetBlend( VkBool32 flag )									{ m_BlendState.colorBlendAttachment.blendEnable = flag; }
	void GraphicsPipeline::SetColorBlendFactor( VkBlendFactor src, VkBlendFactor dst )	{ m_BlendState.colorBlendAttachment.srcColorBlendFactor = src; m_BlendState.colorBlendAttachment.dstColorBlendFactor = dst; }// Optional
	void GraphicsPipeline::SetColorBlendOperation( VkBlendOp op )						{ m_BlendState.colorBlendAttachment.colorBlendOp	= op; }// Optional
	void GraphicsPipeline::SetAlphaBlendFactor( VkBlendFactor src, VkBlendFactor dst )	{ m_BlendState.colorBlendAttachment.srcAlphaBlendFactor = src; m_BlendState.colorBlendAttachment.dstAlphaBlendFactor = dst; }// Optional
	void GraphicsPipeline::SetAlphaBlendOperation( VkBlendOp op )						{ m_BlendState.colorBlendAttachment.alphaBlendOp = op; } //Optional
	void GraphicsPipeline::SetWriteMask( VkColorComponentFlagBits mask )				{ m_BlendState.colorBlendAttachment.colorWriteMask = mask; }

	void GraphicsPipeline::SetAttachmentCount( uint32_t count )							{ m_BlendState.colorBlending.attachmentCount	= count; }
	void GraphicsPipeline::SetBlendConstants( const Vec4f& color )	{
		m_BlendState.colorBlending.blendConstants[0] = color.r;
		m_BlendState.colorBlending.blendConstants[1] = color.g;
		m_BlendState.colorBlending.blendConstants[2] = color.b;
		m_BlendState.colorBlending.blendConstants[3] = color.a; }


	// Depth stencil
	void GraphicsPipeline::SetDepthTest( VkBool32 flag )										{ depthStencil.depthTestEnable = flag; }
	void GraphicsPipeline::SetDepthWrite( VkBool32 flag )										{ depthStencil.depthWriteEnable = flag; }
	void GraphicsPipeline::SetDepthCompareOperator( VkCompareOp op )							{ depthStencil.depthCompareOp = VK_COMPARE_OP_LESS; }
	void GraphicsPipeline::SetDepthBoundsTest( VkBool32 flag )									{ depthStencil.depthBoundsTestEnable = flag; }
	void GraphicsPipeline::SetDepthBounds( float minbounds, float maxbounds )					{ depthStencil.minDepthBounds = minbounds; depthStencil.maxDepthBounds = maxbounds; }
	void GraphicsPipeline::SetStencilTest( VkBool32 flag )										{ depthStencil.stencilTestEnable = flag; }
	void GraphicsPipeline::SetStencilOperators( VkStencilOpState front, VkStencilOpState back )	{ depthStencil.front = front; depthStencil.back = back; }

	
	// Dynamic states
	void GraphicsPipeline::SetDynamicStates( std::initializer_list<VkDynamicState> ilist )
	{
		m_DynamicStates.states.Init( ilist );

		m_DynamicStates.createInfo.dynamicStateCount	= m_DynamicStates.states.Length();
		m_DynamicStates.createInfo.pDynamicStates		= m_DynamicStates.states.begin();
		//m_DynamicStates.createInfo.flags = 0;
	}





	void GraphicsPipeline::Build( )
	{

TODO: パラメータ設定メソッドを実装する

//========= Shader input geometry( vertex ) ===========//
auto bindingDescription = MyVertexLayout::getBindingDesctiption();
auto attributeDescriptions = MyVertexLayout::getAttributeDescriptions();

VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
vertexInputInfo.sType							= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
vertexInputInfo.vertexBindingDescriptionCount	= 1;
vertexInputInfo.pVertexBindingDescriptions		= &bindingDescription;
vertexInputInfo.vertexAttributeDescriptionCount	= static_cast<uint32_t>( attributeDescriptions.Length() );
vertexInputInfo.pVertexAttributeDescriptions	= attributeDescriptions.begin();

VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
inputAssembly.sType						= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
inputAssembly.topology					= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
inputAssembly.primitiveRestartEnable	= VK_FALSE;


		//========= Viewport ===========//

		//viewport.width		= (float)m_SwapChain.Extent().width;
		//viewport.height		= (float)m_SwapChain.Extent().height;
		//viewport.minDepth	= 0.0f;
		//viewport.maxDepth	= 1.0f;

		//VkRect2D scissor	= {};
		//scissor.offset		= { 0, 0 };
		//scissor.extent		= m_SwapChain.Extent();


		VkPipelineViewportStateCreateInfo viewportState = {};

		//VkViewport viewport = {};
		//viewport.x			= 0.0f;
		//viewport.y			= 0.0f;
		viewportState.sType			= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount	= 1;
		viewportState.pViewports	= nullptr;//&viewport;
		viewportState.scissorCount	= 1;
		viewportState.pScissors		= nullptr;//&scissor;

		VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };




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


		//============== DynamicState ==================//
		VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
		dynamicStateInfo.sType	= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateInfo.pDynamicStates	= dynamicStates;
		dynamicStateInfo.dynamicStateCount = 2;
		dynamicStateInfo.flags = 0;



		//============= Shader input parameter layout ============//
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount			= m_ShaderParamLayout.NumSets();//1;
		pipelineLayoutInfo.pSetLayouts				= &m_ShaderParamLayout.DescriptorSetLayout(0);
		pipelineLayoutInfo.pushConstantRangeCount	= 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges		= nullptr; // Optional

		if( vkCreatePipelineLayout( m_refDevice->Device(), &pipelineLayoutInfo, nullptr, &pipelineLayout ) != VK_SUCCESS )
			throw std::runtime_error( "failed to create pipeline layout!" );







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

		if( vkCreateGraphicsPipelines( m_refDevice->Device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline ) != VK_SUCCESS )
			throw std::runtime_error( "failed to create graphics pipeline!" );



		// Cleanup temporary data
		//shaderPass.Release( m_Device );

	}







}// end of namespace vk


