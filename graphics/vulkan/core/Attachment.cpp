#include	"Attachment.h"

#include	"../utils/HelperFunctions.h"



namespace vk
{

	//##################################################################################//
	//																					//
	//								AttachmentRefs										//
	//																					//
	//##################################################################################//

	AttachmentRefs::AttachmentRefs()
	{

	}

	

	AttachmentRefs::~AttachmentRefs()
	{
		Release();
	}



	void AttachmentRefs::Init( int numInputs, int numColorAttachments, bool depthStencil )
	{
		m_InputAttachments.Resize( numInputs );

		m_ColorAttachments.Resize( numInputs );
		m_ResolveAttachments.Resize( numInputs, { VK_ATTACHMENT_UNUSED } );

		if( depthStencil )
			m_DepthStencilAttachments.Resize(1);
	}



	void AttachmentRefs::Release()
	{
		m_InputAttachments.Release();
		m_ColorAttachments.Release();
		m_ResolveAttachments.Release();

		m_DepthStencilAttachments.Release();

	}



	//void AttachmentRefs::SetInputAttachments( std::initializer_list<VkAttachmentReference> ilist )
	//{
	//	m_InputAttachments.Init( ilist );
	//}



	//void AttachmentRefs::SetColorAttachments( std::initializer_list<VkAttachmentReference> ilist )
	//{
	//	m_ColorAttachments.Init( ilist );
	//}



	//void AttachmentRefs::SetResolveAttachments( std::initializer_list<VkAttachmentReference> ilist )
	//{
	//	ASSERT( m_ColorAttachments.Length() == (int)ilist.size() );
	//	m_ResolveAttachments.Init( ilist );
	//}



	//void AttachmentRefs::SetDepthAttachment( VkAttachmentReference attachref )
	//{
	//	m_DepthStencilAttachments.Init( 1, attachref );
	//}




	//##################################################################################//
	//																					//
	//								RenderPassAttachments								//
	//																					//
	//##################################################################################//

	RenderPassAttachments::RenderPassAttachments()
		: m_DepthSlot( VK_ATTACHMENT_UNUSED )
	{
		
	}



	RenderPassAttachments::~RenderPassAttachments()
	{
		Release();
	}



	void RenderPassAttachments::Init( const OreOreLib::Memory<RenderTargetDesc>& rederTargetDescs )
	{
		uint32 slotCount = 0;
		uint32 attachmentCount	= 0;
		for( const auto& rtdesc : rederTargetDescs )
		{
			// count up attachments
			if( rtdesc.MultiSampleFlag != VK_SAMPLE_COUNT_1_BIT && rtdesc.Resolve )
				attachmentCount+=2;
			else
				attachmentCount++;

			// count up attachments(except depth and resolve)
			if( !HasDepthComponent( rtdesc.Format ) )
				slotCount++;
		}


		m_ColorSlots.Resize( slotCount );
		m_AttacmentDescs.Resize( attachmentCount );

		auto slot = m_ColorSlots.begin();
		auto attachmentdesc = m_AttacmentDescs.begin();

		for( const auto& rtdesc : rederTargetDescs )
		{
			bool isdepth = HasDepthComponent( rtdesc.Format );
			bool ismultisample = rtdesc.MultiSampleFlag != VK_SAMPLE_COUNT_1_BIT && rtdesc.Resolve;


			if( isdepth )
				m_DepthSlot = static_cast<uint32_t>( attachmentdesc - m_AttacmentDescs.begin() );
			else
				*slot = { static_cast<uint32_t>( &rtdesc - rederTargetDescs.begin() ), false };


			attachmentdesc->format			= rtdesc.Format;
			attachmentdesc->samples			= rtdesc.MultiSampleFlag;
			attachmentdesc->loadOp			= rtdesc.Operations.LoadOp;
			attachmentdesc->storeOp			= rtdesc.Operations.StoreOp;
			attachmentdesc->stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachmentdesc->stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachmentdesc->initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
			//attachmentdesc->finalLayout		= rtdesc.FinalLayout;

			// Put resolve attachment next to multisample attachment
			if( ismultisample )
			{
				slot->HasResolve = true;

				// Set Multisample attachmentdesc's finalLayout
				if( rtdesc.FinalLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR )
					attachmentdesc->finalLayout	= isdepth ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				// Set Resolve attachmentdesc
				attachmentdesc++;

				attachmentdesc->format			= rtdesc.Format;
				attachmentdesc->samples			= VK_SAMPLE_COUNT_1_BIT;
				attachmentdesc->loadOp			= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				attachmentdesc->storeOp			= VK_ATTACHMENT_STORE_OP_DONT_CARE;// https://github.com/Overv/VulkanTutorial/issues/118
				attachmentdesc->stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				attachmentdesc->stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
				attachmentdesc->initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
				attachmentdesc->finalLayout		= rtdesc.FinalLayout;
			}
			else
			{
				attachmentdesc->finalLayout		= rtdesc.FinalLayout;
			}

			attachmentdesc++;
			slot++;
		}


		m_ClearValues.Resize( attachmentCount );
		ResetClearValues();
	}

	
	
	void RenderPassAttachments::Release()
	{
		m_DepthSlot	= VK_ATTACHMENT_UNUSED;
		m_ClearValues.Release();
		m_AttacmentDescs.Release();
	}



	void RenderPassAttachments::ClearColor( uint32_t slot, float r, float g, float b, float a )
	{
		if( slot < m_ColorSlots.Length() )
			m_ClearValues[ m_ColorSlots[ slot ].AttachmentID ].color = { r, g, b, a };
	}



	void RenderPassAttachments::ClearDepth( float depth, uint32_t stencil )
	{
		if( m_DepthSlot != VK_ATTACHMENT_UNUSED )
			m_ClearValues[ m_DepthSlot ].depthStencil = { depth, stencil };
	}



	//void RenderPassAttachments::CreateColorAttachmentReferece( OreOreLib::Array<VkAttachmentReference>& refs, std::initializer_list<uint32_t> slots )
	//{
	//	for( const auto& slot : slots )
	//	{
	//		if( slot >= m_Slots.Length() )	continue;
	//		refs.AddToTail( { m_Slots[ slot ].ID, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } );
	//	}
	//}


	
	//void RenderPassAttachments::CreateResolveAttachmentReference( OreOreLib::Array<VkAttachmentReference>& refs, std::initializer_list<uint32_t> slots )
	//{
	//	for( const auto& slot : slots )
	//	{
	//		if( slot >= m_Slots.Length() )	continue;

	//		if( m_Slots[ slot ].HasResolve )// found resolve attachment
	//			refs.AddToTail( { m_Slots[ slot ].ID + 1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } );
	//		else// no resolve attachment found
	//			refs.AddToTail( { VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED } );

	//	//	ASSERT( attachmentId < static_cast<uint32_t>(m_ColorToResolve.Length()) );
	//	//	const auto& attachment = m_ColorToResolve[ attachmentId ];
	//	//	refs.AddToTail( { attachment, attachment==VK_ATTACHMENT_UNUSED ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } );
	//	}	
	//}



	void RenderPassAttachments::CreateInputAttachmentReferece(	OreOreLib::Array<VkAttachmentReference>& inputRefs,
																const OreOreLib::Memory<uint32_t>& slots ) const
	{
		for( auto slot : slots )
		{
			if( slot >= m_ColorSlots.Length() || slot==~0u )	continue;

			inputRefs.AddToTail( { m_ColorSlots[ slot ].AttachmentID, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } );
		}
	}



	void RenderPassAttachments::CreateColorResolveAttachmentReferece(	OreOreLib::Array<VkAttachmentReference>& colorRefs,
																		OreOreLib::Array<VkAttachmentReference>& resolveRefs,
																		const OreOreLib::Memory<uint32_t>& slots ) const
	{
		for( auto slot : slots )
		{
			if( slot >= m_ColorSlots.Length() || slot==~0u )	continue;

			// Add color attachment reference
			colorRefs.AddToTail( { m_ColorSlots[ slot ].AttachmentID, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } );

			// Add resolve attachment reference
			if( m_ColorSlots[ slot ].HasResolve )// found resolve attachment
				resolveRefs.AddToTail( { m_ColorSlots[ slot ].AttachmentID + 1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } );
			else// no resolve attachment found
				resolveRefs.AddToTail( { VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED } );
		}	
	}




	void RenderPassAttachments::CreateDepthAttachmentReference( OreOreLib::Array<VkAttachmentReference>& refs ) const
	{
		if( m_DepthSlot != VK_ATTACHMENT_UNUSED )
			refs.AddToTail( { m_DepthSlot, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL } );
	}



	void RenderPassAttachments::InitAttachmentRef(	AttachmentRefs& attachmentRef,
													const OreOreLib::Memory<uint32_t>& inputs,
													const OreOreLib::Memory<uint32_t>& outputs ) const
	{
		CreateInputAttachmentReferece( attachmentRef.m_InputAttachments, inputs );
		CreateColorResolveAttachmentReferece( attachmentRef.m_ColorAttachments, attachmentRef.m_ResolveAttachments, outputs );
		CreateDepthAttachmentReference( attachmentRef.m_DepthStencilAttachments );
	}





	void RenderPassAttachments::ResetClearValues()
	{
		// Clear color values
		for( const auto& slot : m_ColorSlots )
			m_ClearValues[ slot.AttachmentID ].color = { 0.0f, 0.0f, 0.0f, 1.0f };

		// Clear depth value
		if( m_DepthSlot != VK_ATTACHMENT_UNUSED )
			m_ClearValues[ m_DepthSlot ].depthStencil = { 1.0f, 0 };
	}



}// end of namespace vk