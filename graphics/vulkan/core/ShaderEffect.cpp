#include	"ShaderEffect.h"

#include	<oreore/container/ArrayView.h>

#include	"../utils/HelperFunctions.h"



namespace vk
{

	ShaderEffect::ShaderEffect()
		: m_RenderPass( VK_NULL_HANDLE )
	{
		
	}



	ShaderEffect::ShaderEffect( GraphicsDevice& device, uint32_t numPasses, uint32_t numRenderTargets )
		: m_refDevice( device )
		, m_ShaderPasses( numPasses )
		, m_SubpassDescriptions( numPasses )
		, m_SubpassDependencies( numPasses )

		, m_AttachmentRefs( numPasses )
		, m_Pipelines( numPasses )

		, m_RenderTargetDescs( numRenderTargets )
		
	{
		for( uint32_t i=0; i<numPasses; ++i )
		{
			m_ShaderPasses[i].BindDevice( device );
			m_Pipelines[i].BindDevice( device );
		}

		m_CustomRTDescView.Init( m_RenderTargetDescs.begin(), numRenderTargets );
	}



	ShaderEffect::ShaderEffect( GraphicsDevice& device, SwapChain& swapchain, uint32_t numPasses, uint32_t numRenderTargets )
		: m_refDevice( device )
		, m_refSwapChain( swapchain )
		, m_ShaderPasses( numPasses )
		, m_SubpassDescriptions( numPasses )
		, m_SubpassDependencies( numPasses )

		, m_AttachmentRefs( numPasses )
		, m_Pipelines( numPasses )

		, m_RenderTargetDescs( 2/*swapchain color, swapchain depth*/ + numRenderTargets )
		, SwapChainColorTarget( numRenderTargets )
		, SwapChainDepthTarget( numRenderTargets + 1 )
	{
		for( uint32_t i=0; i<numPasses; ++i )
		{
			m_ShaderPasses[i].BindDevice( device );
			m_Pipelines[i].BindDevice( device );
		}

		m_CustomRTDescView.Init( m_RenderTargetDescs.begin(), numRenderTargets );
		m_SwapChainRTDescView.Init( m_RenderTargetDescs.end()-2, 2 );
	}



	ShaderEffect::~ShaderEffect()
	{
		Release();
	}



	void ShaderEffect::Init( GraphicsDevice& device, uint32_t numPasses, uint32_t numRenderTargets )
	{
		m_refDevice	= device;
		m_ShaderPasses.Init( numPasses );
		m_SubpassDescriptions.Init( numPasses );
		m_SubpassDependencies.Init( numPasses );

		m_AttachmentRefs.Init( numPasses );
		m_Pipelines.Init( numPasses );

		m_RenderTargetDescs.Init( numRenderTargets );

		for( uint32_t i=0; i<numPasses; ++i )
		{
			m_ShaderPasses[i].BindDevice( device );
			m_Pipelines[i].BindDevice( device );
		}

		m_CustomRTDescView.Init( m_RenderTargetDescs.begin(), numRenderTargets );
	}



	void ShaderEffect::Init( GraphicsDevice& device, SwapChain& swapchain, uint32_t numPasses, uint32_t numRenderTargets )
	{
		m_refDevice	= device;
		m_refSwapChain	= swapchain;
		m_ShaderPasses.Init( numPasses );
		m_SubpassDescriptions.Init( numPasses );
		m_SubpassDependencies.Init( numPasses );

		m_AttachmentRefs.Init( numPasses );
		m_Pipelines.Init( numPasses );

		m_RenderTargetDescs.Init( 2/*swapchain color, swapchain depth*/ + numRenderTargets );
		*const_cast<uint32_t*>(&SwapChainColorTarget)	= numRenderTargets;
		*const_cast<uint32_t*>(&SwapChainDepthTarget)	= numRenderTargets + 1;// TODO: Need to check if swapchain has depth component.

		for( uint32_t i=0; i<numPasses; ++i )
		{
			m_ShaderPasses[i].BindDevice( device );
			m_Pipelines[i].BindDevice( device );
		}

		m_CustomRTDescView.Init( m_RenderTargetDescs.begin(), numRenderTargets );
		m_SwapChainRTDescView.Init( m_RenderTargetDescs.end()-2, 2 );
	}



	void ShaderEffect::Release()
	{
		m_ShaderPasses.Release();
		m_CustomRTDescView.Release();
		m_SwapChainRTDescView.Release();
		m_RenderTargetDescs.Release();
		m_RenderTargets.Release();

		m_Framebuffers.Release();

		m_SubpassDependencies.Release();
		m_SubpassDescriptions.Release();

		m_AttachmentRefs.Release();

		m_refSwapChain.Reset();

	}



	void ShaderEffect::InitRenderTargets( std::initializer_list<RenderTargetDesc> renderTargetDescs )
	{
		// 先頭から順番にRenderTargetDescsを詰めていく
		OreOreLib::MemCopy( m_CustomRTDescView.begin(), renderTargetDescs.begin(), renderTargetDescs.size() );
		//m_RenderTargetDescs.Init( renderTargetDescs.begin(), renderTargetDescs.end() );
		m_RenderTargets.Init( m_refDevice, renderTargetDescs );
	}



	void ShaderEffect::InitDependencies( std::initializer_list<ShaderPassDependency> dependencies )
	{
		m_SubpassDependencies.Init( static_cast<OreOreLib::MemSizeType>( dependencies.size() ) );

		auto dependency = m_SubpassDependencies.begin();
		for( const auto& dep : dependencies )
		{
			dependency->srcSubpass		= dep.SrcIndex;
			dependency->dstSubpass		= dep.DstIndex;
			dependency->srcStageMask	= dep.SrcBarrier.StageFlag;// VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency->srcAccessMask	= dep.SrcBarrier.AccessFlag;
			dependency->dstStageMask	= dep.DstBarrier.StageFlag;//VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency->dstAccessMask	= dep.DstBarrier.AccessFlag;//VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			dependency++;
		}
	}



	void ShaderEffect::InitGraphicsPipeline( uint32_t pass, const PipelineState& pipelineState )
	{
		ASSERT( pass < static_cast<uint32_t>( m_Pipelines.Length() ) );
		m_Pipelines[ pass ].Init( m_ShaderPasses[ pass ], pipelineState, m_RenderPass, pass );
	}



	//void ShaderEffect::InitFramebuffers()
	//{

	//}



	void ShaderEffect::SetSubpassInputRenderTargets( uint32_t pass, std::initializer_list<uint32_t> ilist )
	{
		// スワップチェーンがないのにSwapChainColorTarget指定されてるケースを除外する
		m_ShaderPasses[ pass ].SetInputRenderTargetIDs( ilist );
	}



	void ShaderEffect::SetSubpassOutputRenderTargets( uint32_t pass, std::initializer_list<uint32_t> ilist )
	{
		m_ShaderPasses[ pass ].SetOutputRenderTargetIDs( ilist );
	}



	void ShaderEffect::BuildRenderPass()
	{

		if( !m_refSwapChain.IsNull() )
			m_refSwapChain->ExposeRenderTargetDescs( m_SwapChainRTDescView );
		
		m_Attachments.Init( m_RenderTargetDescs );


		//========================= Create RenderPass ==========================//

		auto attachmentrefs = m_AttachmentRefs.begin();
		auto shaderpass = m_ShaderPasses.begin();
		for( auto& subpassDesc : m_SubpassDescriptions )
		{
			m_Attachments.InitAttachmentRef( *attachmentrefs, shaderpass->InputRenderTargetIDs(), shaderpass->OutputRenderTargetIDs() );
	
			subpassDesc.pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS;

			subpassDesc.inputAttachmentCount	= static_cast<uint32_t>( attachmentrefs->Inputs().Length() );//inputAttachmentRefs.Length() );
			subpassDesc.pInputAttachments		= attachmentrefs->Inputs().begin();//inputAttachmentRefs.begin();
			subpassDesc.colorAttachmentCount	= static_cast<uint32_t>( attachmentrefs->Colors().Length() );
			subpassDesc.pColorAttachments		= attachmentrefs->Colors().begin();
			subpassDesc.pResolveAttachments		= attachmentrefs->Resolves().begin();
			subpassDesc.pDepthStencilAttachment	= attachmentrefs->Depth().begin();

			shaderpass++;
			attachmentrefs++;
		}

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType			= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount	= static_cast<uint32_t>( m_Attachments.AttachmentDescs().Length() );
		renderPassInfo.pAttachments		= m_Attachments.AttachmentDescs().begin();
		renderPassInfo.subpassCount		= static_cast<uint32_t>( m_SubpassDescriptions.Length() );
		renderPassInfo.pSubpasses		= m_SubpassDescriptions.begin();
		renderPassInfo.dependencyCount	= static_cast<uint32_t>( m_SubpassDependencies.Length() );
		renderPassInfo.pDependencies	= m_SubpassDependencies.begin();

		VK_CHECK_RESULT( vkCreateRenderPass( m_refDevice->Device(), &renderPassInfo, nullptr, &m_RenderPass ) );


		//========================= Create Framebuffers ==========================//

		m_Framebuffers.Init( m_refDevice, m_RenderPass, m_refSwapChain.IsNull() ? 1 : m_refSwapChain->NumBuffers() );


		OreOreLib::Array<VkImageView> views( m_Attachments.NumAttachments() );

		OreOreLib::ArrayView<VkImageView> rendretargetviews( &views[0], m_RenderTargets.NumBuffers() );
		OreOreLib::ArrayView<VkImageView> swapchainviews( &views[ m_RenderTargets.NumBuffers() ], m_refSwapChain->NumAttachments() );


		const auto& extent = !m_refSwapChain.IsNull() ? m_refSwapChain->Extent() : m_RenderTargets.MaxDim();

		for( int i=0; i<m_Framebuffers.NumBuffers(); ++i )
		{
			if( !m_refSwapChain.IsNull() )
				m_refSwapChain->ExposeImageViews( swapchainviews, i );

			m_RenderTargets.ExposeImageViews( rendretargetviews );

			m_Framebuffers.InitFramebuffer( i, extent.width, extent.height, views );
		}

	}



	void ShaderEffect::ReleaseOnSwapchainUpdate()
	{

		m_SwapChainRTDescView.Clear();
//		m_RenderTargetDescs.Clear();// 
//		m_RenderTargets.Release();//TODO: want to reuse.

		m_Attachments.Release();
		for( auto& ref : m_AttachmentRefs )
			ref.Release();

		m_Framebuffers.Release();

		for( auto& pipeline : m_Pipelines )
			pipeline.Release();

		if( m_RenderPass != VK_NULL_HANDLE )
		{
			vkDestroyRenderPass( m_refDevice->Device(), m_RenderPass, nullptr );
			m_RenderPass = VK_NULL_HANDLE;
		}

	
		//m_UniformBuffers.Release();

		// m_ShaderParamDescs.Release();

	}


	void ShaderEffect::RecreateOnSwapchainUpdate()
	{
		ReleaseOnSwapchainUpdate();

		BuildRenderPass();//createRenderPass();
		
		//BuildGraphicsPipeline();//createGraphicsPipeline();
		
		m_Framebuffers.Init( m_refDevice, m_RenderPass, m_refSwapChain->NumBuffers() );//InitFramebuffers();//createFramebuffers();
		
		//BuildUniformBuffers();//createUnformBuffers();

		//createDescriptorPoolAndDescriptorSets();
//		for( auto&)
//		m_ShaderParamDescs.Init( m_Device.Device(), m_refSwapChain->NumBuffers(), m_ShaderPasses[i].ParamLayout() );

//		bindUniformToDescriptorSets();

//		createCommandBuffers();

	}







/*
	void createRenderPass()
	{

		// カラーバッファアタッチメントの設定
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format			= m_refSwapChain->ImageFormat();
		colorAttachment.samples			= msaaSamples;
		colorAttachment.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout		= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment	= 0;
		colorAttachmentRef.layout		= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


		// デプスバッファアタッチメントの設定
		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format			= findDepthFormat();
		depthAttachment.samples			= msaaSamples;
		depthAttachment.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp			= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment	= 1;
		depthAttachmentRef.layout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


		// 
		VkAttachmentDescription colorAttachmentResolve = {};
		colorAttachmentResolve.format			= m_refSwapChain->ImageFormat();
		colorAttachmentResolve.samples			= VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentResolve.loadOp			= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentResolve.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentResolve.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentResolve.finalLayout		= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentResolveRef = {};
		colorAttachmentResolveRef.attachment	= 2;
		colorAttachmentResolveRef.layout		= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;



		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount	= 1;
		subpass.pColorAttachments		= &colorAttachmentRef;
		subpass.pDepthStencilAttachment	= &depthAttachmentRef;
		subpass.pResolveAttachments		= &colorAttachmentResolveRef;


		VkSubpassDependency dependency = {};
		dependency.srcSubpass		= VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass		= 0;
		dependency.srcStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask	= 0;
		dependency.dstStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask	= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


		StaticArray<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType			= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount	= static_cast<uint32_t>( attachments.Length() ) ;
		renderPassInfo.pAttachments		= attachments.begin();
		renderPassInfo.subpassCount		= 1;
		renderPassInfo.pSubpasses		= &subpass;
		renderPassInfo.dependencyCount	= 1;
		renderPassInfo.pDependencies	= &dependency;

		if( vkCreateRenderPass( m_Device.Device(), &renderPassInfo, nullptr, &renderPass ) != VK_SUCCESS )
			throw std::runtime_error( "failed to create render pass!" );

	}
*/



}// end of namespace vk 


