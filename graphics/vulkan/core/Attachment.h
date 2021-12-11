#ifndef ATTACHMENT_H
#define	ATTACHMENT_H

#include	<oreore/container/Array.h>

#include	"RenderTargetDesc.h"



namespace vk
{

	// |----- ColorAttachments(hasresolve) ---|--- ResolveAttachments ---|-- ... --|-- DepthAttachment --|

	class RenderPassAttachments
	{
	public:

		RenderPassAttachments();
		~RenderPassAttachments();

		void Init( const OreOreLib::Memory<RenderTargetDesc>& rederTargetDescs );
		void Release();

		uint32_t NumAttachments() const { return static_cast<uint32_t>( m_AttacmentDescs.Length() ); }
		bool HasDepth() const	{ return m_DepthSlot != VK_ATTACHMENT_UNUSED; }

		void ClearColor( uint32_t slot, float r, float g, float b, float a );
		void ClearDepth( float depth, uint32_t stencil );

		//void CreateColorAttachmentReferece( OreOreLib::Array<VkAttachmentReference>& refs, std::initializer_list<uint32_t> slots );
		void CreateColorResolveAttachmentReferece( OreOreLib::Array<VkAttachmentReference>& colorRefs, OreOreLib::Array<VkAttachmentReference>& resolveRefs, const OreOreLib::Memory<uint32_t>& slots ) const;
		//void CreateResolveAttachmentReference( OreOreLib::Array<VkAttachmentReference>& refs, std::initializer_list<uint32_t> slots );
		void CreateDepthAttachmentReference( OreOreLib::Array<VkAttachmentReference>& refs ) const;

		const OreOreLib::Array<VkAttachmentDescription>& AttachmentDescs()	const { return m_AttacmentDescs; }
		const OreOreLib::Array<VkClearValue>& ClearValues() const	{ return m_ClearValues; }


	private:

		struct Slot
		{
			uint32_t	AttachmentID;	// attachment index
			bool		HasResolve;	// Resolve buffer existence
		};

		OreOreLib::Array<VkAttachmentDescription>	m_AttacmentDescs;
		OreOreLib::Array<VkClearValue>				m_ClearValues;

		// ColorBuffer slot table( resolve and depth buffers are excluded )
		OreOreLib::Array<Slot>						m_ColorSlots;
		uint32_t									m_DepthSlot;


		void ResetClearValues();

	};



}// end of namespace vk


#endif // !ATTACHMENT_H
