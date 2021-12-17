#include	"PipelineState.h"



namespace vk
{

	PipelineState::PipelineState()
	{
		Init();
	}



	PipelineState::~PipelineState()
	{
		Release();
	}



	void PipelineState::Init()
	{
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

		m_bDynamicViewport = true;
		m_Viewports.Release();
		m_Scissors.Release();

		m_ViewportState = {};
		m_ViewportState.sType			= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		m_ViewportState.viewportCount	= 1;
		m_ViewportState.pViewports		= nullptr;//&viewport;
		m_ViewportState.scissorCount	= 1;
		m_ViewportState.pScissors		= nullptr;//&scissor;


		//=========== Rasterization ==============//
	
		m_RasterizationState = {};
		m_RasterizationState.sType						= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		m_RasterizationState.depthClampEnable			= VK_FALSE;
		m_RasterizationState.rasterizerDiscardEnable	= VK_FALSE;
		m_RasterizationState.polygonMode				= VK_POLYGON_MODE_FILL;
		m_RasterizationState.lineWidth					= 1.0f;
		m_RasterizationState.cullMode					= VK_CULL_MODE_BACK_BIT;//VK_CULL_MODE_NONE;//
		m_RasterizationState.frontFace					= VK_FRONT_FACE_COUNTER_CLOCKWISE;//VK_FRONT_FACE_CLOCKWISE;//
		m_RasterizationState.depthBiasEnable			= VK_FALSE;
		m_RasterizationState.depthBiasConstantFactor	= 0.0f; // Optional
		m_RasterizationState.depthBiasClamp				= 0.0f; // Optional
		m_RasterizationState.depthBiasSlopeFactor		= 0.0f; // Optional


		//============ Blending ==============//

		m_ColorBlendAttachmentStates.Resize(1);

		m_ColorBlendAttachmentStates[0] = {};
		m_ColorBlendAttachmentStates[0].colorWriteMask		= VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		m_ColorBlendAttachmentStates[0].blendEnable			= VK_FALSE;
		m_ColorBlendAttachmentStates[0].srcColorBlendFactor	= VK_BLEND_FACTOR_ONE; // Optional
		m_ColorBlendAttachmentStates[0].dstColorBlendFactor	= VK_BLEND_FACTOR_ZERO; // Optional
		m_ColorBlendAttachmentStates[0].colorBlendOp		= VK_BLEND_OP_ADD; // Optional
		m_ColorBlendAttachmentStates[0].srcAlphaBlendFactor	= VK_BLEND_FACTOR_ONE; // Optional
		m_ColorBlendAttachmentStates[0].dstAlphaBlendFactor	= VK_BLEND_FACTOR_ZERO; // Optioanl
		m_ColorBlendAttachmentStates[0].alphaBlendOp		= VK_BLEND_OP_ADD; //Optional

		m_ColorBlendState = {};
		m_ColorBlendState.sType				= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		m_ColorBlendState.logicOpEnable		= VK_FALSE;
		m_ColorBlendState.logicOp			= VK_LOGIC_OP_COPY; // Optional
		m_ColorBlendState.attachmentCount	= m_ColorBlendAttachmentStates.Length<uint32_t>();
		m_ColorBlendState.pAttachments		= m_ColorBlendAttachmentStates.begin();
		m_ColorBlendState.blendConstants[0] = 0.0f; // Optional
		m_ColorBlendState.blendConstants[1] = 0.0f; // Optional
		m_ColorBlendState.blendConstants[2] = 0.0f; // Optional
		m_ColorBlendState.blendConstants[3] = 0.0f; // Optional


		//============== Depth stencil ================//

		m_DepthStencilState = {};
		m_DepthStencilState.sType					= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		m_DepthStencilState.depthTestEnable			= VK_TRUE;
		m_DepthStencilState.depthWriteEnable		= VK_TRUE;
		m_DepthStencilState.depthCompareOp			= VK_COMPARE_OP_LESS;
		m_DepthStencilState.depthBoundsTestEnable	= VK_FALSE;
		m_DepthStencilState.minDepthBounds			= 0.0f;
		m_DepthStencilState.maxDepthBounds			= 1.0f;
		m_DepthStencilState.stencilTestEnable		= VK_FALSE;
		m_DepthStencilState.front					= {};
		m_DepthStencilState.back					= {};


		//=========== Multisample ==============//

		m_MultisampleState = {};
		m_MultisampleState.sType					= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		m_MultisampleState.sampleShadingEnable		= VK_FALSE;
		m_MultisampleState.rasterizationSamples		= VK_SAMPLE_COUNT_1_BIT;
		m_MultisampleState.minSampleShading			= 1.0f; // Optional
		m_MultisampleState.pSampleMask				= nullptr; // Optional
		m_MultisampleState.alphaToCoverageEnable	= VK_FALSE; // Optional
		m_MultisampleState.alphaToOneEnable			= VK_FALSE; // Optional


		//========= Vertex input ===========//

		m_VertexInputState = {};
		m_VertexInputState.sType	= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		m_InputAssemblyState = {};
		m_InputAssemblyState.sType					= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		m_InputAssemblyState.topology				= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		m_InputAssemblyState.primitiveRestartEnable	= VK_FALSE;


		//============== DynamicState ==================//

		m_States = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		m_DynamicState = {};
		m_DynamicState.sType				= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		m_DynamicState.dynamicStateCount	= m_States.Length<uint32_t>();
		m_DynamicState.pDynamicStates		= m_States.begin();
		m_DynamicState.flags				= 0;

	}


	void PipelineState::Release()
	{
		m_Viewports.Release();
		m_Scissors.Release();
		m_ColorBlendAttachmentStates.Release();
		m_States.Release();
	}



	//==================== ViewportState ====================//

	void PipelineState::SetDynamicViewport( bool flag )
	{
		m_bDynamicViewport = flag;

		if( flag==true )
		{
			if( !OreOreLib::Exists( m_States, VK_DYNAMIC_STATE_VIEWPORT ) )
				m_States.AddToTail( VK_DYNAMIC_STATE_VIEWPORT );

			if( !OreOreLib::Exists( m_States, VK_DYNAMIC_STATE_SCISSOR ) )
				m_States.AddToTail( VK_DYNAMIC_STATE_SCISSOR );
		}
		else
		{
			int idx = (int)OreOreLib::Find( m_States, VK_DYNAMIC_STATE_VIEWPORT );
			if( idx>=0 )	m_States.Remove( idx );

			idx = (int)OreOreLib::Find( m_States, VK_DYNAMIC_STATE_SCISSOR );
			if( idx>=0 )	m_States.Remove( idx );		
		}
	}

	void PipelineState::AddViewportRect( const VkViewport& viewport, const VkRect2D& scissor )
	{
		m_Viewports.AddToTail( viewport );
		m_Scissors.AddToTail( scissor );

		m_ViewportState.viewportCount	= m_Viewports.Length<uint32_t>();
		m_ViewportState.pViewports		= m_Viewports.begin();
		m_ViewportState.scissorCount	= m_Scissors.Length<uint32_t>();
		m_ViewportState.pScissors		= m_Scissors.begin();
	}



	//===============  Rasterization settings ===================//

	void PipelineState::SetDepthClamp( VkBool32 flag )
	{
		m_RasterizationState.depthClampEnable = flag;
	}

	void PipelineState::SetRasterizerDiscard( VkBool32 flag )
	{
		m_RasterizationState.rasterizerDiscardEnable = flag;
	}

	void PipelineState::SetPolygonMode( VkPolygonMode mode )
	{
		m_RasterizationState.polygonMode = mode;
	}

	void PipelineState::SetLineWidth( float width )
	{
		m_RasterizationState.lineWidth = width;
	}

	void PipelineState::SetCullMode( VkCullModeFlags mode )
	{
		m_RasterizationState.cullMode = mode;
	}

	void PipelineState::SetFrontFace( VkFrontFace mode )
	{
		m_RasterizationState.frontFace = mode;
	}

	void PipelineState::SetDepthBias( VkBool32 flag, float constantfactor, float biasclamp, float slopefactor )
	{
		m_RasterizationState.depthBiasEnable			= flag;
		m_RasterizationState.depthBiasConstantFactor	= constantfactor;
		m_RasterizationState.depthBiasClamp				= biasclamp;
		m_RasterizationState.depthBiasSlopeFactor		= slopefactor;
	}



	//==================== Color blending ======================//

	void PipelineState::SetAttachmentCount( uint32_t count )
	{
		//m_ColorBlendState.attachmentCount	= count;
		m_ColorBlendAttachmentStates.Resize( (int)count );

		for( auto& attachment : m_ColorBlendAttachmentStates )
		{
			attachment = {};
			attachment.colorWriteMask		= VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			attachment.blendEnable			= VK_FALSE;
			attachment.srcColorBlendFactor	= VK_BLEND_FACTOR_ONE; // Optional
			attachment.dstColorBlendFactor	= VK_BLEND_FACTOR_ZERO; // Optional
			attachment.colorBlendOp			= VK_BLEND_OP_ADD; // Optional
			attachment.srcAlphaBlendFactor	= VK_BLEND_FACTOR_ONE; // Optional
			attachment.dstAlphaBlendFactor	= VK_BLEND_FACTOR_ZERO; // Optioanl
			attachment.alphaBlendOp			= VK_BLEND_OP_ADD; //Optional
		}

		m_ColorBlendState.attachmentCount	= m_ColorBlendAttachmentStates.Length<uint32_t>();
		m_ColorBlendState.pAttachments		= m_ColorBlendAttachmentStates.begin();
	}

	void PipelineState::SetBlendConstants( const Vec4f& color )
	{
		m_ColorBlendState.blendConstants[0] = color.r;
		m_ColorBlendState.blendConstants[1] = color.g;
		m_ColorBlendState.blendConstants[2] = color.b;
		m_ColorBlendState.blendConstants[3] = color.a;
	}

	void PipelineState::SetBlend( VkBool32 flag, int attachment )
	{
		m_ColorBlendAttachmentStates[ attachment ].blendEnable = flag;
	}

	void PipelineState::SetColorBlendFactor( VkBlendFactor src, VkBlendFactor dst, int attachment )
	{
		m_ColorBlendAttachmentStates[ attachment ].srcColorBlendFactor = src;
		m_ColorBlendAttachmentStates[ attachment ].dstColorBlendFactor = dst;
	}

	void PipelineState::SetColorBlendOperation( VkBlendOp op, int attachment )
	{
		m_ColorBlendAttachmentStates[ attachment ].colorBlendOp = op;
	}

	void PipelineState::SetAlphaBlendFactor( VkBlendFactor src, VkBlendFactor dst, int attachment )
	{
		m_ColorBlendAttachmentStates[ attachment ].srcAlphaBlendFactor = src;
		m_ColorBlendAttachmentStates[ attachment ].dstAlphaBlendFactor = dst;
	}

	void PipelineState::SetAlphaBlendOperation( VkBlendOp op, int attachment )
	{
		m_ColorBlendAttachmentStates[ attachment ].alphaBlendOp = op;
	}

	void PipelineState::SetWriteMask( VkColorComponentFlagBits mask, int attachment )
	{
		m_ColorBlendAttachmentStates[ attachment ].colorWriteMask = mask;
	}



	//==================== Depth stencil ======================//

	void PipelineState::SetDepthTest( VkBool32 flag )
	{
		m_DepthStencilState.depthTestEnable = flag;
	}

	void PipelineState::SetDepthWrite( VkBool32 flag )
	{
		m_DepthStencilState.depthWriteEnable = flag;
	}

	void PipelineState::SetDepthCompareOperator( VkCompareOp op )
	{
		m_DepthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
	}

	void PipelineState::SetDepthBoundsTest( VkBool32 flag )
	{
		m_DepthStencilState.depthBoundsTestEnable = flag;
	}

	void PipelineState::SetDepthBounds( float minbounds, float maxbounds )
	{
		m_DepthStencilState.minDepthBounds = minbounds;
		m_DepthStencilState.maxDepthBounds = maxbounds;
	}

	void PipelineState::SetStencilTest( VkBool32 flag )
	{
		m_DepthStencilState.stencilTestEnable = flag;
	}

	void PipelineState::SetStencilOperators( VkStencilOpState front, VkStencilOpState back )
	{
		m_DepthStencilState.front = front;
		m_DepthStencilState.back = back;
	}
	


	//==================== Multisampe settings ===================//

	void PipelineState::SetSampleShading( VkBool32 flag )
	{
		m_MultisampleState.sampleShadingEnable	= flag;
	}

	void PipelineState::SetRasterizationSamples( VkSampleCountFlagBits flag )
	{
		m_MultisampleState.rasterizationSamples = flag;
	}

	void PipelineState::SetMinSampleShading( float val )
	{
		m_MultisampleState.minSampleShading = val;
	}

	void PipelineState::SetSampleMask( VkSampleMask* mask )
	{
		m_MultisampleState.pSampleMask = mask;
	}

	void PipelineState::SetAlphaToCoverage( VkBool32 flag )
	{
		m_MultisampleState.alphaToCoverageEnable = flag;
	}

	void PipelineState::SetAlphaToOne( VkBool32 flag )
	{
		m_MultisampleState.alphaToOneEnable = flag;
	}



	//==================== Vertex states ====================//

	void PipelineState::SetPrimitiveType( VkPrimitiveTopology topology )
	{
		m_InputAssemblyState.topology	= topology;
	}


	void PipelineState::SetVertexInputState(	const OreOreLib::Memory<VkVertexInputBindingDescription>& bindingDescs,
												const OreOreLib::Memory<VkVertexInputAttributeDescription>& attribDescs )
	{
		m_VertexInputState.vertexBindingDescriptionCount	= bindingDescs.Length<uint32_t>();
		m_VertexInputState.pVertexBindingDescriptions		= bindingDescs.begin();
		m_VertexInputState.vertexAttributeDescriptionCount	= attribDescs.Length<uint32_t>();
		m_VertexInputState.pVertexAttributeDescriptions		= attribDescs.begin();
	}



	//==================== Dynamic states ====================//

	void PipelineState::SetDynamicStates( std::initializer_list<VkDynamicState> ilist )
	{
		m_States.Init( ilist );

		m_DynamicState.dynamicStateCount	= m_States.Length<uint32_t>();
		m_DynamicState.pDynamicStates		= m_States.begin();
		//m_DynamicState.flags = 0;
	}



}// end of namespace vk


