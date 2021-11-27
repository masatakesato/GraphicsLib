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
		m_InputAttachments.Release();
		m_ColorAttachments.Release();
		m_ResolveAttachments.Release();

	}



	void SubpassAttachments::SetInputAttachments( std::initializer_list<VkAttachmentReference> ilist )
	{
		m_InputAttachments.Init( ilist );
	}



	void SubpassAttachments::SetColorAttachments( std::initializer_list<VkAttachmentReference> ilist )
	{
		m_ColorAttachments.Init( ilist );
	}



	void SubpassAttachments::SetResolveAttachments( std::initializer_list<VkAttachmentReference> ilist )
	{
		ASSERT( m_ColorAttachments.Length() == (int)ilist.size() );
		m_ResolveAttachments.Init( ilist );
	}



	void SubpassAttachments::SetDepthAttachment( VkAttachmentReference attachref )
	{
		m_DepthStencilAttachment = attachref;
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
		: m_ActiveResolves( 0 )
	{
		
	}



	RenderPassAttachments::~RenderPassAttachments()
	{
		Release();
	}



	void RenderPassAttachments::Init( int numColors, bool enableDepth, int numResolves )
	{
		m_ActiveResolves = 0;

		// Init AttachmentDescriptor Array
		m_AttacmentDescs.Resize( numColors + static_cast<int32>(enableDepth) + numResolves );

		// Init ArrayViews
		m_ColorDescs.Init( &m_AttacmentDescs[0], numColors );

		if( enableDepth )
			m_DepthDescs.Init( &m_AttacmentDescs[numColors], 1 );

		if( numResolves>0 )
			m_ResolveDescs.Init( &m_AttacmentDescs[ numColors + static_cast<int32>(enableDepth) ], numResolves );

		m_ColorToResolve.Resize( numColors, VK_ATTACHMENT_UNUSED );
	}

	
	
	void RenderPassAttachments::Release()
	{
		m_ActiveResolves = 0;
		m_ColorToResolve.Release();

		m_ColorDescs.Release();
		m_DepthDescs.Release();
		m_ResolveDescs.Release();

		m_AttacmentDescs.Release();
	}



	void RenderPassAttachments::SetColorAttachmentDesc( uint32 attachment, VkFormat format, VkSampleCountFlagBits msaaSamples, AttachmentOps ops, bool presentable )
	{
		ASSERT( attachment < m_ColorDescs.Length() );

		bool resolve = msaaSamples != VK_SAMPLE_COUNT_1_BIT;

		const auto& loadstoreop = LoadStoreOps[ static_cast<int>(ops) ];

		auto& desc = m_AttacmentDescs[ attachment ];
		desc.format			= format;
		desc.samples		= msaaSamples;
		desc.loadOp			= loadstoreop.first;
		desc.storeOp		= loadstoreop.second;
		desc.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		desc.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		desc.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
		desc.finalLayout	= (presentable && !resolve) ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		if( presentable && resolve )
		{
			InitResolveAttachmentDesc( attachment, format, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR );
		}
	}



	void RenderPassAttachments::SetDepthAttachmentDesc( VkFormat format, VkSampleCountFlagBits msaaSamples, AttachmentOps ops, VkImageLayout layout )
	{
		ASSERT( m_DepthDescs );

		const auto& loadstoreop = LoadStoreOps[ static_cast<int>(ops) ];

		auto& desc = m_DepthDescs[0];
		desc.format			= format;
		desc.samples		= msaaSamples;
		desc.loadOp			= loadstoreop.first;
		desc.storeOp		= loadstoreop.second;
		desc.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		desc.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		desc.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
		desc.finalLayout	= layout;
	}



	void RenderPassAttachments::CreateColorAttachmentReferece( OreOreLib::Array<VkAttachmentReference>& refs, std::initializer_list<uint32_t> activeattachments )
	{
		for( const auto& attachment : activeattachments )
		{
			refs.AddToTail( { attachment, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } );
		}
	}


	
	void RenderPassAttachments::CreateResolveAttachmentReference( OreOreLib::Array<VkAttachmentReference>& refs, std::initializer_list<uint32_t> activeattachments )
	{
		for( const auto& attachmentId : activeattachments )
		{
			ASSERT( attachmentId < static_cast<uint32_t>(m_ColorToResolve.Length()) );
			const auto& attachment = m_ColorToResolve[ attachmentId ];
			refs.AddToTail( { attachment, attachment==VK_ATTACHMENT_UNUSED ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } );
		}	
	}



	void RenderPassAttachments::CreateDepthAttachmentReference( OreOreLib::Array<VkAttachmentReference>& refs )
	{
		if( m_DepthDescs )
			refs.AddToTail( { static_cast<uint32_t>( m_ColorDescs.Length() ), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL } );
	}



	void RenderPassAttachments::InitResolveAttachmentDesc( uint32 attachment, VkFormat format, VkImageLayout layout )
	{
		ASSERT( attachment < m_ColorDescs.Length() && m_ActiveResolves < (uint32_t)m_ResolveDescs.Length() );

		auto& resolveslot = m_ColorToResolve[ attachment ];
		if( resolveslot==VK_ATTACHMENT_UNUSED )
			resolveslot = static_cast<uint32_t>( m_ResolveDescs.begin() - m_AttacmentDescs.begin() ) + m_ActiveResolves;

		auto& desc = m_ResolveDescs[ m_ActiveResolves++ ];
		desc.format			= format;
		desc.samples		= VK_SAMPLE_COUNT_1_BIT;
		desc.loadOp			= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		desc.storeOp		= VK_ATTACHMENT_STORE_OP_DONT_CARE;// https://github.com/Overv/VulkanTutorial/issues/118
		desc.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		desc.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		desc.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
		desc.finalLayout	= layout;
	}



}// end of namespace vk