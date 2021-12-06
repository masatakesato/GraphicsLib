#include	"Attachment.h"

#include	"../utils/HelperFunctions.h"



namespace vk
{

	//##################################################################################//
	//																					//
	//								SubpassAttachments									//
	//																					//
	//##################################################################################//


	//SubpassAttachments::SubpassAttachments()
	//{

	//}

	//

	//SubpassAttachments::~SubpassAttachments()
	//{
	//	Release();
	//}



	//void SubpassAttachments::Init( int numInputs, int numColorAttachments, bool depthStencil )
	//{
	//	m_InputAttachments.Resize( numInputs );

	//	m_ColorAttachments.Resize( numInputs );
	//	m_ResolveAttachments.Resize( numInputs, { VK_ATTACHMENT_UNUSED } );

	//	m_DepthStencilAttachment = {};
	//}



	//void SubpassAttachments::Release()
	//{
	//	m_InputAttachments.Release();
	//	m_ColorAttachments.Release();
	//	m_ResolveAttachments.Release();

	//}



	//void SubpassAttachments::SetInputAttachments( std::initializer_list<VkAttachmentReference> ilist )
	//{
	//	m_InputAttachments.Init( ilist );
	//}



	//void SubpassAttachments::SetColorAttachments( std::initializer_list<VkAttachmentReference> ilist )
	//{
	//	m_ColorAttachments.Init( ilist );
	//}



	//void SubpassAttachments::SetResolveAttachments( std::initializer_list<VkAttachmentReference> ilist )
	//{
	//	ASSERT( m_ColorAttachments.Length() == (int)ilist.size() );
	//	m_ResolveAttachments.Init( ilist );
	//}



	//void SubpassAttachments::SetDepthAttachment( VkAttachmentReference attachref )
	//{
	//	m_DepthStencilAttachment = attachref;
	//}




	//##################################################################################//
	//																					//
	//								RenderPassAttachments								//
	//																					//
	//##################################################################################//

	RenderPassAttachments::RenderPassAttachments()
	//	: m_ActiveResolves( 0 )
		: m_DepthSlot( VK_ATTACHMENT_UNUSED )
	{
		
	}



	RenderPassAttachments::~RenderPassAttachments()
	{
		Release();
	}



	void RenderPassAttachments::Init( OreOreLib::Array<RenderTargetDesc>& rederTargetDescs )
	{

//TODO: Color, Depth, Resolveぜんぶ一纏めにした配列が必要 -> 済. m_AttacmentDescs使ってる
//TODO: VkAttachmentReference配列はColor, Depth, Resolveそれぞれ個別に配列化する必要がある
//	TODO: rederTargetDescsの情報だけで分かる？ -> RenderTargetDescにResolveフラグをくっつけた

// TODO: MSAA用VkAttachmentDescriptorの直後に、対応するResolve用VkAttachmentDescriptorを配置する
//TODO: デプスとリゾルブを除いたアタッチメントスロットを登録する

		uint32 slotCount = 0;
		uint32 attachmentCount	= 0;
		for( const auto& rtdesc : rederTargetDescs )
		{
			// count up attachments
			if( rtdesc.RenderBuffer.MultiSampleFlag != VK_SAMPLE_COUNT_1_BIT && rtdesc.RenderBuffer.Resolve )
				attachmentCount+=2;
			else
				attachmentCount++;

			// count up attachments(except depth and resolve)
			if( !HasDepthComponent( rtdesc.RenderBuffer.Format ) )
				slotCount++;
		}


		m_Slots.Resize( slotCount );
		m_AttacmentDescs.Resize( attachmentCount );

		auto slot = m_Slots.begin();
		auto attachmentdesc = m_AttacmentDescs.begin();

		for( const auto& rtdesc : rederTargetDescs )
		{
			bool isdepth = HasDepthComponent( rtdesc.RenderBuffer.Format );
			bool ismultisample = rtdesc.RenderBuffer.MultiSampleFlag != VK_SAMPLE_COUNT_1_BIT && rtdesc.RenderBuffer.Resolve;


			if( isdepth )
				m_DepthSlot = static_cast<uint32_t>( /*&rtdesc*/attachmentdesc - m_AttacmentDescs.begin() );
			else
				*slot = { static_cast<uint32_t>( &rtdesc - rederTargetDescs.begin() ), false };


			attachmentdesc->format			= rtdesc.RenderBuffer.Format;
			attachmentdesc->samples			= rtdesc.RenderBuffer.MultiSampleFlag;
			attachmentdesc->loadOp			= rtdesc.Attachment.Operations.LoadOp;
			attachmentdesc->storeOp			= rtdesc.Attachment.Operations.StoreOp;
			attachmentdesc->stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachmentdesc->stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachmentdesc->initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
			//attachmentdesc->finalLayout		= rtdesc.Attachment.FinalLayout;

			// Put resolve attachment next to multisample attachment
			if( ismultisample )
			{
				slot->HasResolve = true;

				// Set Multisample AttachmentDesc's finalLayout
				if( rtdesc.Attachment.FinalLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR )
					attachmentdesc->finalLayout	= isdepth ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				// Set Resolve attachmentdesc
				attachmentdesc++;

				attachmentdesc->format			= rtdesc.RenderBuffer.Format;
				attachmentdesc->samples			= VK_SAMPLE_COUNT_1_BIT;
				attachmentdesc->loadOp			= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				attachmentdesc->storeOp			= VK_ATTACHMENT_STORE_OP_DONT_CARE;// https://github.com/Overv/VulkanTutorial/issues/118
				attachmentdesc->stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				attachmentdesc->stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
				attachmentdesc->initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
				attachmentdesc->finalLayout		= rtdesc.Attachment.FinalLayout;
			}
			else
			{
				attachmentdesc->finalLayout		= rtdesc.Attachment.FinalLayout;
			}

			attachmentdesc++;
			slot++;
		}


		m_ClearValues.Resize( attachmentCount );
		ResetClearValues();
	}



	//void RenderPassAttachments::Init( int numColors, bool enableDepth, int numResolves )
	//{
	//	m_ActiveResolves = 0;

	//	// Init AttachmentDescriptor Array
	//	m_AttacmentDescs.Resize( numColors + static_cast<int32>(enableDepth) + numResolves );

	//	// Init ArrayViews
	//	m_ColorDescs.Init( &m_AttacmentDescs[0], numColors );

	//	if( enableDepth )
	//		m_DepthDescs.Init( &m_AttacmentDescs[numColors], 1 );

	//	if( numResolves > 0 )
	//	{
	//		m_ResolveDescs.Init( &m_AttacmentDescs[ numColors + static_cast<int32>(enableDepth) ], numResolves );
	//		m_ColorToResolve.Resize( numColors, VK_ATTACHMENT_UNUSED );
	//	}
	//}

	
	
	void RenderPassAttachments::Release()
	{
		m_DepthSlot	= VK_ATTACHMENT_UNUSED;
//		m_ActiveResolves = 0;
//		m_ColorToResolve.Release();

		//m_ColorDescs.Release();
		//m_DepthDescs.Release();
		//m_ResolveDescs.Release();

		m_ClearValues.Release();
		m_AttacmentDescs.Release();
	}




	void RenderPassAttachments::ClearColor( int32_t slot, float r, float g, float b, float a )
	{
		if( slot < m_Slots.Length() )
			m_ClearValues[ m_Slots[ slot ].ID ].color = { r, g, b, a };
	}



	void RenderPassAttachments::ClearDepth( float depth, uint32_t stencil )
	{
		if( m_DepthSlot != VK_ATTACHMENT_UNUSED )
			m_ClearValues[ m_DepthSlot ].depthStencil = { depth, stencil };
	}





	//void RenderPassAttachments::SetColorAttachmentDesc( uint32 attachment, VkFormat format, VkSampleCountFlagBits msaaSamples, LoadStoreOp ops, bool presentable )
	//{
	//	ASSERT( attachment < m_ColorDescs.Length() );

	//	bool resolve = msaaSamples != VK_SAMPLE_COUNT_1_BIT;

	//	auto& desc = m_AttacmentDescs[ attachment ];
	//	desc.format			= format;
	//	desc.samples		= msaaSamples;
	//	desc.loadOp			= ops.LoadOp;
	//	desc.storeOp		= ops.StoreOp;
	//	desc.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	//	desc.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	//	desc.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
	//	desc.finalLayout	= (presentable && !resolve) ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//	if( presentable && resolve )
	//	{
	//		InitResolveAttachmentDesc( attachment, format, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR );
	//	}
	//}



	//void RenderPassAttachments::SetDepthAttachmentDesc( VkFormat format, VkSampleCountFlagBits msaaSamples, LoadStoreOp ops, VkImageLayout layout )
	//{
	//	ASSERT( m_DepthDescs );

	//	auto& desc = m_DepthDescs[0];
	//	desc.format			= format;
	//	desc.samples		= msaaSamples;
	//	desc.loadOp			= ops.LoadOp;
	//	desc.storeOp		= ops.StoreOp;
	//	desc.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	//	desc.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	//	desc.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
	//	desc.finalLayout	= layout;
	//}



	void RenderPassAttachments::CreateColorAttachmentReferece( OreOreLib::Array<VkAttachmentReference>& refs, std::initializer_list<uint32_t> slots )
	{

// TODO: リゾルブじゃないスロットだけフィルタリングしたい
		
		for( const auto& slot : slots )
		{
			if( slot >= m_Slots.Length() )	continue;
			refs.AddToTail( { m_Slots[ slot ].ID, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } );
		}
	}


	
	void RenderPassAttachments::CreateResolveAttachmentReference( OreOreLib::Array<VkAttachmentReference>& refs, std::initializer_list<uint32_t> slots )
	{
		for( const auto& slot : slots )
		{
			if( slot >= m_Slots.Length() )	continue;

			if( m_Slots[ slot ].HasResolve )// found resolve attachment
				refs.AddToTail( { m_Slots[ slot ].ID + 1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } );
			else// no resolve attachment found
				refs.AddToTail( { VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED } );

		//	ASSERT( attachmentId < static_cast<uint32_t>(m_ColorToResolve.Length()) );
		//	const auto& attachment = m_ColorToResolve[ attachmentId ];
		//	refs.AddToTail( { attachment, attachment==VK_ATTACHMENT_UNUSED ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } );
		}	
	}



	void RenderPassAttachments::CreateDepthAttachmentReference( OreOreLib::Array<VkAttachmentReference>& refs )
	{
		if( m_DepthSlot != VK_ATTACHMENT_UNUSED )
			refs.AddToTail( { m_DepthSlot, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL } );
	}



	//void RenderPassAttachments::InitResolveAttachmentDesc( uint32 attachment, VkFormat format, VkImageLayout layout )
	//{
	//	ASSERT( attachment < m_ColorDescs.Length() && m_ActiveResolves < (uint32_t)m_ResolveDescs.Length() );

	//	auto& resolveslot = m_ColorToResolve[ attachment ];
	//	if( resolveslot==VK_ATTACHMENT_UNUSED )
	//		resolveslot = static_cast<uint32_t>( m_ResolveDescs.begin() - m_AttacmentDescs.begin() ) + m_ActiveResolves;

	//	auto& desc = m_ResolveDescs[ m_ActiveResolves++ ];
	//	desc.format			= format;
	//	desc.samples		= VK_SAMPLE_COUNT_1_BIT;
	//	desc.loadOp			= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	//	desc.storeOp		= VK_ATTACHMENT_STORE_OP_DONT_CARE;// https://github.com/Overv/VulkanTutorial/issues/118
	//	desc.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	//	desc.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	//	desc.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
	//	desc.finalLayout	= layout;
	//}



	void RenderPassAttachments::ResetClearValues()
	{
		for( const auto& slot : m_Slots )
			m_ClearValues[ slot.ID ].color = { 0.0f, 0.0f, 0.0f, 1.0f };


		if( m_DepthSlot != VK_ATTACHMENT_UNUSED )
			m_ClearValues[ m_DepthSlot ].depthStencil = { 1.0f, 0 };


		//auto attachment = m_AttacmentDescs.begin();
		//for( uint32 i=0; i<m_ClearValues.Length(); ++i )
		//{
		//	if( HasDepthComponent( m_AttacmentDescs[i].format ) )
		//		m_ClearValues[i].depthStencil = { 1.0f, 0 };
		//	else
		//		m_ClearValues[i].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		//}
	}



}// end of namespace vk