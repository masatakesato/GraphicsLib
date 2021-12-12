#ifndef ATTACHMENT_H
#define	ATTACHMENT_H

#include	<oreore/container/Array.h>

#include	"RenderTargetDesc.h"



namespace vk
{

	class AttachmentRefs
	{
	public:

		AttachmentRefs();
		AttachmentRefs( const AttachmentRefs& ) = delete;
		~AttachmentRefs();

		void Init( int numInputs, int numColorAttachments, bool depthStencil );
		void Release();

		//void SetInputAttachments( std::initializer_list<VkAttachmentReference> ilist );
		//void SetColorAttachments( std::initializer_list<VkAttachmentReference> ilist );
		//void SetResolveAttachments( std::initializer_list<VkAttachmentReference> ilist );
		//void SetDepthAttachment( VkAttachmentReference attachref );

		const OreOreLib::Array<VkAttachmentReference>& Inputs() const	{ return m_InputAttachments; }
		const OreOreLib::Array<VkAttachmentReference>& Colors() const	{ return m_ColorAttachments; }
		const OreOreLib::Array<VkAttachmentReference>& Resolves() const	{ return m_ResolveAttachments; }
		const OreOreLib::Array<VkAttachmentReference>& Depth() const	{ return m_DepthStencilAttachments; }


	private:

		OreOreLib::Array<VkAttachmentReference>	m_InputAttachments;

		OreOreLib::Array<VkAttachmentReference>	m_ColorAttachments;
		OreOreLib::Array<VkAttachmentReference>	m_ResolveAttachments;
		// https://qiita.com/Pctg-x8/items/2b3d5c8a861f42aa533f
		// for Multisampling.
		// m_ResolveAttachments.Length() must be equal to m_ColorAttachments.Length()
		// set VkAttachmentReference::attachment to VK_ATTACHMENT_UNUSED if you want to invalidate multisampling

		OreOreLib::Array<VkAttachmentReference>	m_DepthStencilAttachments;

		friend class RenderPassAttachments;
	};











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

		void CreateInputAttachmentReferece( OreOreLib::Array<VkAttachmentReference>& inputRefs, const OreOreLib::Memory<uint32_t>& slots ) const;
		//void CreateColorAttachmentReferece( OreOreLib::Array<VkAttachmentReference>& refs, std::initializer_list<uint32_t> slots );
		void CreateColorResolveAttachmentReferece( OreOreLib::Array<VkAttachmentReference>& colorRefs, OreOreLib::Array<VkAttachmentReference>& resolveRefs, const OreOreLib::Memory<uint32_t>& slots ) const;
		//void CreateResolveAttachmentReference( OreOreLib::Array<VkAttachmentReference>& refs, std::initializer_list<uint32_t> slots );
		void CreateDepthAttachmentReference( OreOreLib::Array<VkAttachmentReference>& refs ) const;


		void InitAttachmentRef(	AttachmentRefs& attachmentRef, const OreOreLib::Memory<uint32_t>& inputs, const OreOreLib::Memory<uint32_t>& outputs ) const;


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
