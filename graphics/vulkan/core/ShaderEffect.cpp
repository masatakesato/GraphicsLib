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
	{
		Init( device, numPasses, numRenderTargets );
	}



	ShaderEffect::ShaderEffect( GraphicsDevice& device, SwapChain& swapChain, uint32_t numPasses, uint32_t numRenderTargets )
	{
		Init( device, swapChain, numPasses, numRenderTargets );
	}



	ShaderEffect::~ShaderEffect()
	{
		Release();
	}



	void ShaderEffect::Init( GraphicsDevice& device, uint32_t numPasses, uint32_t numRenderTargets )
	{
		m_refDevice	= device;
		m_ShaderPasses.Init( numPasses );

		m_Pipelines.Init( numPasses );

		m_RenderTargetDescs.Init( numRenderTargets );

		m_DescriptorBuffers.Init( numPasses );

		for( auto& shaderPass : m_ShaderPasses )
			shaderPass.BindDevice( device );

		m_CustomRTDescView.Init( m_RenderTargetDescs.begin(), numRenderTargets );
	}



	void ShaderEffect::Init( GraphicsDevice& device, SwapChain& swapchain, uint32_t numPasses, uint32_t numRenderTargets )
	{
		m_refDevice	= device;
		m_refSwapChain	= swapchain;
		m_ShaderPasses.Init( numPasses );

		m_Pipelines.Init( numPasses );

		m_RenderTargetDescs.Init( 2/*swapchain color, swapchain depth*/ + numRenderTargets );
		*const_cast<uint32_t*>(&SwapChainColorTarget)	= numRenderTargets;
		*const_cast<uint32_t*>(&SwapChainDepthTarget)	= numRenderTargets + 1;// TODO: Need to check if swapchain has depth component.

		m_DescriptorBuffers.Init( numPasses );

		for( auto& shaderPass : m_ShaderPasses )
			shaderPass.BindDevice( device );

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

		for( auto& pipeline : m_Pipelines )	pipeline.Release();
		m_Pipelines.Release();

		m_SubpassDependencies.Release();

		m_DescriptorBuffers.Release();

		m_refSwapChain.Reset();
	}



	void ShaderEffect::InitRenderTargets( std::initializer_list<RenderTargetDesc> renderTargetDescs )
	{
		// 先頭から順番にRenderTargetDescsを詰めていく
		OreOreLib::MemCopy( m_CustomRTDescView.begin(), renderTargetDescs.begin(), renderTargetDescs.size() );
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
		m_Pipelines.Init( m_ShaderPasses.Length() );
		ASSERT( pass < static_cast<uint32_t>( m_Pipelines.Length() ) );
		m_Pipelines[ pass ].Init( m_refDevice, m_ShaderPasses[ pass ], pipelineState, m_RenderPass, pass );
	}



	void ShaderEffect::SetSubpassInputRenderTargets( uint32_t pass, std::initializer_list<uint32_t> ilist )
	{
		// スワップチェーンがないのにSwapChainColorTarget指定されてるケースを除外する
		m_ShaderPasses[ pass ].SetInputRenderTargetIDs( ilist );
	}



	void ShaderEffect::SetSubpassOutputRenderTargets( uint32_t pass, std::initializer_list<uint32_t> ilist )
	{
		m_ShaderPasses[ pass ].SetOutputRenderTargetIDs( ilist );
	}



	void ShaderEffect::InitDescriptorSetLayouts( uint32_t pass, std::initializer_list< std::initializer_list<VkDescriptorSetLayoutBinding> > bindings )
	{
		m_ShaderPasses[ pass ].InitDescriptorSetLayouts( bindings );
	}



	void ShaderEffect::BindUniformBuffer( uint32_t pass, uint32_t set, uint32_t binding, const OreOreLib::Array<UniformBuffer>& uniformBuffers )
	{
		m_DescriptorBuffers[ pass ].BindUniformBuffer( set, binding, uniformBuffers );
	}



	void ShaderEffect::BindCombinedImageSampler( uint32_t pass, uint32_t set, uint32_t binding, VkImageView imageView, VkSampler sampler )
	{
		m_DescriptorBuffers[ pass ].BindCombinedImageSampler( set, binding, imageView, sampler );
	}



	void ShaderEffect::BuildDescriptorSets()
	{
		auto shaderPass = m_ShaderPasses.begin();
		for( auto& descSets : m_DescriptorBuffers )
		{
			descSets.Init( m_refDevice->Device(), m_refSwapChain.IsNull() ? 1 : m_refSwapChain->NumBuffers(), shaderPass->ParamLayout() );
			shaderPass++;
		}
	}



	void ShaderEffect::BuildRenderPass()
	{

		if( !m_refSwapChain.IsNull() )
			m_refSwapChain->ExposeRenderTargetDescs( m_SwapChainRTDescView );
		
		m_Attachments.Init( m_RenderTargetDescs );


		//========================= Create RenderPass ==========================//
		OreOreLib::Array<AttachmentRefs>		attachmentRefs( m_ShaderPasses.Length() );
		OreOreLib::Array<VkSubpassDescription>	subpassDescriptions( m_ShaderPasses.Length() );
		auto attachmentRef = attachmentRefs.begin();
		auto shaderPass = m_ShaderPasses.begin();

		for( auto& subpassDesc : subpassDescriptions )
		{
			m_Attachments.InitAttachmentRef( *attachmentRef, shaderPass->InputRenderTargetIDs(), shaderPass->OutputRenderTargetIDs() );
	
			subpassDesc.pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS;

			subpassDesc.inputAttachmentCount	= static_cast<uint32_t>( attachmentRef->Inputs().Length() );//inputAttachmentRefs.Length() );
			subpassDesc.pInputAttachments		= attachmentRef->Inputs().begin();//inputAttachmentRefs.begin();
			subpassDesc.colorAttachmentCount	= static_cast<uint32_t>( attachmentRef->Colors().Length() );
			subpassDesc.pColorAttachments		= attachmentRef->Colors().begin();
			subpassDesc.pResolveAttachments		= attachmentRef->Resolves().begin();
			subpassDesc.pDepthStencilAttachment	= attachmentRef->Depth().begin();

			shaderPass++;
			attachmentRef++;
		}

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType			= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount	= static_cast<uint32_t>( m_Attachments.AttachmentDescs().Length() );
		renderPassInfo.pAttachments		= m_Attachments.AttachmentDescs().begin();
		renderPassInfo.subpassCount		= static_cast<uint32_t>( subpassDescriptions.Length() );
		renderPassInfo.pSubpasses		= subpassDescriptions.begin();
		renderPassInfo.dependencyCount	= static_cast<uint32_t>( m_SubpassDependencies.Length() );
		renderPassInfo.pDependencies	= m_SubpassDependencies.begin();

		VK_CHECK_RESULT( vkCreateRenderPass( m_refDevice->Device(), &renderPassInfo, nullptr, &m_RenderPass ) );
	}



	void ShaderEffect::BuildFramebuffers()
	{
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



	void ShaderEffect::BuildPipelines()
	{
		if(!m_Pipelines) m_Pipelines.Init( m_ShaderPasses.Length() );

		for( uint32_t pass=0; pass<m_ShaderPasses.Length(); ++pass )
			m_Pipelines[ pass ].Init( m_refDevice, m_ShaderPasses[ pass ], m_RenderPass, pass );
	}



	void ShaderEffect::ReleaseOnSwapchainUpdate()
	{
		m_SwapChainRTDescView.Clear();
//		m_RenderTargetDescs.Clear();// スワップチェーン画像枚数が変わってたら再初期化必要.

		m_Attachments.Release();

		m_Framebuffers.Release();
		
		//for( auto& pipeline : m_Pipelines )
		//	pipeline.Release();
		m_Pipelines.Release();


		if( m_RenderPass != VK_NULL_HANDLE )
		{
			vkDestroyRenderPass( m_refDevice->Device(), m_RenderPass, nullptr );
			m_RenderPass = VK_NULL_HANDLE;
		}

		for( auto& descSets : m_DescriptorBuffers )
			descSets.Release();
		m_DescriptorBuffers.Clear();
	}


	void ShaderEffect::RecreateOnSwapchainUpdate()
	{
		//ReleaseOnSwapchainUpdate();

		BuildRenderPass();//createRenderPass();
		
		BuildPipelines();

		BuildDescriptorSets();
	}


}// end of namespace vk 


