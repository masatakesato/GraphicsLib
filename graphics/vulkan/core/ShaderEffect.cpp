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
		, m_RenderTargetDescs( numRenderTargets )
	{
		for( auto& pass : m_ShaderPasses )
			pass.BindDevice( device );
	}



	ShaderEffect::ShaderEffect( GraphicsDevice& device, SwapChain& swapchain, uint32_t numPasses, uint32_t numRenderTargets )
		: m_refDevice( device )
		, m_refSwapChain( swapchain )
		, m_ShaderPasses( numPasses )
		, m_RenderTargetDescs( 2/*swapchain color, swapchain depth*/ + numRenderTargets )
		, SwapChainColorTarget( numRenderTargets )
		, SwapChainDepthTarget( numRenderTargets + 1 )
	{
		for( auto& pass : m_ShaderPasses )
			pass.BindDevice( device );

		m_SwapChainRenderTargetDescs.Init( m_RenderTargetDescs.end()-2, 2 );
	}



	ShaderEffect::~ShaderEffect()
	{
		Release();
	}



	void ShaderEffect::Init( GraphicsDevice& device, uint32_t numPasses, uint32_t numRenderTargets )
	{
		m_refDevice	= device;
		m_ShaderPasses.Init( numPasses );
		m_RenderTargetDescs.Init( numRenderTargets );

		for( auto& pass : m_ShaderPasses )
			pass.BindDevice( device );
	}



	void ShaderEffect::Init( GraphicsDevice& device, SwapChain& swapchain, uint32_t numPasses, uint32_t numRenderTargets )
	{
		m_refDevice	= device;
		m_refSwapChain	= swapchain;
		m_ShaderPasses.Init( numPasses );
		m_RenderTargetDescs.Init( 2/*swapchain color, swapchain depth*/ + numRenderTargets );

		*const_cast<uint32_t*>(&SwapChainColorTarget)	= numRenderTargets;
		*const_cast<uint32_t*>(&SwapChainDepthTarget)	= numRenderTargets + 1;// TODO: Need to check if swapchain has depth component.

		for( auto& pass : m_ShaderPasses )
			pass.BindDevice( device );

		m_SwapChainRenderTargetDescs.Init( m_RenderTargetDescs.end()-2, 2 );
	}



	void ShaderEffect::Release()
	{
		m_ShaderPasses.Release();
		m_RenderTargetDescs.Release();
		m_RenderTargets.Release();

		m_refSwapChain.Reset();

	}



	void ShaderEffect::InitRenderTargets( std::initializer_list<RenderTargetDesc> renderTargetDescs )
	{
		// 先頭から順番にRenderTargetDescsを詰めていく
		OreOreLib::MemCopy( m_RenderTargetDescs.begin(), renderTargetDescs.begin(), renderTargetDescs.size() );
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
			dependency->srcStageMask	= dep.Src.StageFlag;// VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency->srcAccessMask	= dep.Src.AccessFlag;
			dependency->dstStageMask	= dep.Dst.StageFlag;//VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency->dstAccessMask	= dep.Dst.AccessFlag;//VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			dependency++;
		}
	}



	void ShaderEffect::SetSubpassInputRenderTargets( uint32_t pass, std::initializer_list<uint32_t> ilist )
	{
		// スワップチェーンがないのにSwapChainColorTarget指定されてるケースを除外する
//		m_ShaderPasses[ pass ].SetInputRenderTargetIDs( ilist );
	}



	void ShaderEffect::SetSubpassOutputRenderTargets( uint32_t pass, std::initializer_list<uint32_t> ilist )
	{
//		m_ShaderPasses[ pass ].SetOutputRenderTargetIDs( ilist );
	}



	void ShaderEffect::BuildRenderPass()
	{
		bool multiSampleEnabled = m_refSwapChain->MultiSampleCount() != VK_SAMPLE_COUNT_1_BIT;

		if( !m_refSwapChain.IsNull() )
		{
			m_refSwapChain->ExposeRenderTargetDescs( m_SwapChainRenderTargetDescs );
		}
		
		m_Attachments.Init( m_RenderTargetDescs );


//TODO: サブパス毎に有効化するスロット情報の設定が必要.( デプスアタッチメント使うかどうかも含めて )
OreOreLib::Array<VkAttachmentReference> colorAttachmentRefs, resolveAttachmentRefs, depthAttachmentRefs;
//これだとスワップチェーン無しでレンダーターゲット設定してる場合でもアタッチメントリファレンス登録されてしまう

		m_Attachments.CreateColorResolveAttachmentReferece( colorAttachmentRefs, resolveAttachmentRefs, { /*0,*/ SwapChainColorTarget }/*m_ShaderPasses[0].OutputRenderTargetIDs()*/ );//{ /*0,*/ SwapChainColorTarget } );

m_Attachments.CreateDepthAttachmentReference( depthAttachmentRefs );

/*
		// サブパスで使う入出力アタッチメントを記述する
		VkSubpassDescription subpassDesc = {};
		subpassDesc.pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS;

		subpassDesc.inputAttachmentCount	= 0;
		subpassDesc.pInputAttachments		= nullptr;// シェーダーで入力データとして使いたいアタッチメント群(G-Bufferみたいなやつ)
		subpassDesc.colorAttachmentCount	= static_cast<uint32_t>( colorAttachmentRefs.Length() );//1;
		subpassDesc.pColorAttachments		= colorAttachmentRefs.begin();//&colorAttachmentRef;
		subpassDesc.pResolveAttachments		= resolveAttachmentRefs.begin();//&colorAttachmentResolveRef;
		subpassDesc.pDepthStencilAttachment	= depthAttachmentRefs.begin();//&depthAttachmentRef;


		VkSubpassDependency dependency = {};
		dependency.srcSubpass		= VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass		= 0;
		dependency.srcStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask	= 0;
		dependency.dstStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask	= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType			= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount	= static_cast<uint32_t>( m_Attachments.AttachmentDescs().Length() );//static_cast<uint32_t>( attachmentDescriptions.Length() ) ;
		renderPassInfo.pAttachments		= m_Attachments.AttachmentDescs().begin();//attachmentDescriptions.begin();
		renderPassInfo.subpassCount		= 1;
		renderPassInfo.pSubpasses		= &subpassDesc;
		renderPassInfo.dependencyCount	= 1;
		renderPassInfo.pDependencies	= &dependency;

		VK_CHECK_RESULT( vkCreateRenderPass( m_refDevice->Device(), &renderPassInfo, nullptr, &m_RenderPass ) );
*/
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


