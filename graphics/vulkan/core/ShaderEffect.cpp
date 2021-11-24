#include	"ShaderEffect.h"



namespace vk
{

	ShaderEffect::ShaderEffect()
		: m_refDevice()
	{
		m_AttacmentDescs.Reserve(16);
	}



	ShaderEffect::ShaderEffect( GraphicsDevice& device )
		: m_refDevice( device )
	{

	}



	ShaderEffect::~ShaderEffect()
	{

	}



	void ShaderEffect::AddColorAttachmentDesc( VkFormat format, VkSampleCountFlagBits msaaSamples, AttachmentOps ops, VkImageLayout layout )
	{
		const auto& loadstoreop = LoadStoreOps[ static_cast<int>(ops) ];

		m_AttacmentDescs.AddToTail();

		auto& attachmentDesc = m_AttacmentDescs.Back();

		attachmentDesc.format			= format;
		attachmentDesc.samples			= msaaSamples;
		attachmentDesc.loadOp			= loadstoreop.first;
		attachmentDesc.storeOp			= loadstoreop.second;
		attachmentDesc.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDesc.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDesc.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDesc.finalLayout		= layout;
	}



	void ShaderEffect::AddDepthAttachmentDesc( VkFormat format, VkSampleCountFlagBits msaaSamples, AttachmentOps ops, VkImageLayout layout )
	{
		const auto& loadstoreop = LoadStoreOps[ static_cast<int>(ops) ];

		m_AttacmentDescs.AddToTail();

		auto& attachmentDesc = m_AttacmentDescs.Back();

		attachmentDesc.format			= format;
		attachmentDesc.samples			= msaaSamples;
		attachmentDesc.loadOp			= loadstoreop.first;
		attachmentDesc.storeOp			= loadstoreop.second;
		attachmentDesc.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDesc.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDesc.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDesc.finalLayout		= layout;
	}



	void ShaderEffect::AddReslveAttachmentDesc( VkFormat format )
	{

		VkAttachmentDescription colorAttachmentResolve = {};
		colorAttachmentResolve.format			= format;
		colorAttachmentResolve.samples			= VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentResolve.loadOp			= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.storeOp			= VK_ATTACHMENT_STORE_OP_DONT_CARE;// https://github.com/Overv/VulkanTutorial/issues/118
		colorAttachmentResolve.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentResolve.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentResolve.finalLayout		= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	}





/*
	void createRenderPass()
	{

		// カラーバッファアタッチメントの設定
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format			= m_SwapChain.ImageFormat();
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
		colorAttachmentResolve.format			= m_SwapChain.ImageFormat();
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


