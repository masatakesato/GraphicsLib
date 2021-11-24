#include	"Attachment.h"

#include	<oreore/container/Pair.h>



namespace vk
{

	//##################################################################################//
	//																					//
	//								SubpassAttachments									//
	//																					//
	//##################################################################################//


	SubpassAttachments::SubpassAttachments()
	{

	}

	

	SubpassAttachments::~SubpassAttachments()
	{
		Release();
	}



	void SubpassAttachments::Init( int numInputs, int numColorAttachments, bool depthStencil )
	{
		m_InputAttachments.Resize( numInputs );

		m_ColorAttachments.Resize( numInputs );
		m_ResolveAttachments.Resize( numInputs, { VK_ATTACHMENT_UNUSED } );

		m_DepthStencilAttachment = {};
	}



	void SubpassAttachments::Release()
	{

	}




	//##################################################################################//
	//																					//
	//								RenderPassAttachments								//
	//																					//
	//##################################################################################//


	const Pair<VkAttachmentLoadOp, VkAttachmentStoreOp> LoadStoreOps[] =
	{
		{ VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE },		// Load / Store
		{ VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_DONT_CARE },	// Load / DontCare
		{ VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE },		// Clear / Store
		{ VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE },	// Clear / DontCare
		{ VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_STORE },	// DontCare / Store
		{ VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE }// DontCare / DontCare
	};





	RenderPassAttachments::RenderPassAttachments()
		: m_ResolveSlot( -1 )
		, m_DepthSlot( -1 )
	{
		
	}



	RenderPassAttachments::~RenderPassAttachments()
	{

	}



	void RenderPassAttachments::Init( int numattachments )
	{

	}

	
	
	void RenderPassAttachments::Release()
	{

	}



	void RenderPassAttachments::SetColorAttachmentDesc( uint8 slot, VkFormat format, VkSampleCountFlagBits msaaSamples, AttachmentOps ops, VkImageLayout layout )
	{
		ASSERT( slot< m_AttacmentDescs.Length() );

		const auto& loadstoreop = LoadStoreOps[ static_cast<int>(ops) ];

		auto& desc = m_AttacmentDescs[ slot ];
		desc.format			= format;
		desc.samples		= msaaSamples;
		desc.loadOp			= loadstoreop.first;
		desc.storeOp		= loadstoreop.second;
		desc.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		desc.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		desc.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
		desc.finalLayout	= layout;
	}



	void RenderPassAttachments::SetDepthAttachmentDesc( uint8 slot, VkFormat format, VkSampleCountFlagBits msaaSamples, AttachmentOps ops, VkImageLayout layout )
	{
		ASSERT( slot< m_AttacmentDescs.Length() );

		m_DepthSlot = static_cast<int32>( slot );

		const auto& loadstoreop = LoadStoreOps[ static_cast<int>(ops) ];

		auto& desc = m_AttacmentDescs[ m_DepthSlot ];
		desc.format			= format;
		desc.samples		= msaaSamples;
		desc.loadOp			= loadstoreop.first;
		desc.storeOp		= loadstoreop.second;
		desc.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		desc.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		desc.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
		desc.finalLayout	= layout;
	}



	void RenderPassAttachments::SetResolveAttachmentDesc( uint8 slot, VkFormat format )
	{
		ASSERT( slot< m_AttacmentDescs.Length() );

		m_ResolveSlot = static_cast<int32>( slot );

		auto& desc = m_AttacmentDescs[ m_ResolveSlot ];
		desc.format			= format;
		desc.samples		= VK_SAMPLE_COUNT_1_BIT;
		desc.loadOp			= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		desc.storeOp		= VK_ATTACHMENT_STORE_OP_DONT_CARE;// https://github.com/Overv/VulkanTutorial/issues/118
		desc.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		desc.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		desc.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
		desc.finalLayout	= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	}



	bool RenderPassAttachments::DeleteDepthAttachment()
	{
		if( !HasDepth() )	return false;

		m_AttacmentDescs[ m_DepthSlot ] = {};

		return true;
	}



	bool RenderPassAttachments::DeleteResolveAttachment()
	{
		if( !HasResolve() )	return false;

		m_AttacmentDescs[ m_ResolveSlot ] = {};

		return true;
	}


}// end of namespace vk