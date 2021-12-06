#ifndef ATTACHMENT_H
#define	ATTACHMENT_H

#include	<oreore/container/Array.h>
//#include	<oreore/container/ArrayView.h>

#include	"RenderTargetDesc.h"



namespace vk
{

	//class SubpassAttachments
	//{
	//public:

	//	SubpassAttachments();
	//	SubpassAttachments( const SubpassAttachments& )=delete;
	//	~SubpassAttachments();

	//	void Init( int numInputs, int numColorAttachments, bool depthStencil );
	//	void Release();

	//	void SetInputAttachments( std::initializer_list<VkAttachmentReference> ilist );
	//	void SetColorAttachments( std::initializer_list<VkAttachmentReference> ilist );
	//	void SetResolveAttachments( std::initializer_list<VkAttachmentReference> ilist );
	//	void SetDepthAttachment( VkAttachmentReference attachref );


	//private:

	//	OreOreLib::Array<VkAttachmentReference>	m_InputAttachments;

	//	OreOreLib::Array<VkAttachmentReference>	m_ColorAttachments;
	//	OreOreLib::Array<VkAttachmentReference>	m_ResolveAttachments;
	//	// https://qiita.com/Pctg-x8/items/2b3d5c8a861f42aa533f
	//	// for Multisampling.
	//	// m_ResolveAttachments.Length() must be equal to m_ColorAttachments.Length()
	//	// set VkAttachmentReference::attachment to VK_ATTACHMENT_UNUSED if you want to invalidate multisampling

	//	VkAttachmentReference	m_DepthStencilAttachment{};

	//};










	



	// |----- ColorAttachments(hasresolve) ---|--- ResolveAttachments ---|-- ... --|-- DepthAttachment --|

	class RenderPassAttachments
	{
	public:

		RenderPassAttachments();
		~RenderPassAttachments();

//void Init( int numAttachments );

void Init( OreOreLib::Array<RenderTargetDesc>& rederTargetDescs );

//void Init( int numColors, bool enableDepth, int numResolves=0 );
		void Release();


		uint32_t NumAttachments() const { return static_cast<uint32_t>( m_AttacmentDescs.Length() ); }
		bool HasDepth() const	{ return m_DepthSlot != VK_ATTACHMENT_UNUSED; }//m_DepthDescs; }
		//bool HasResolve() const	{ return m_ResolveDescs; }
		

		void ClearColor( int32_t slot, float r, float g, float b, float a );
		void ClearDepth( float depth, uint32_t stencil );


//void SetColorAttachmentDesc( uint32 attachment, VkFormat format, VkSampleCountFlagBits msaaSamples, LoadStoreOp ops, bool presentable );
//void SetDepthAttachmentDesc( VkFormat format, VkSampleCountFlagBits msaaSamples, LoadStoreOp ops, VkImageLayout layout=VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL );

		void CreateColorAttachmentReferece( OreOreLib::Array<VkAttachmentReference>& refs, std::initializer_list<uint32_t> slots );
		void CreateResolveAttachmentReference( OreOreLib::Array<VkAttachmentReference>& refs, std::initializer_list<uint32_t> slots );
		void CreateDepthAttachmentReference( OreOreLib::Array<VkAttachmentReference>& refs );

		const OreOreLib::Array<VkAttachmentDescription>& AttachmentDescs()	const { return m_AttacmentDescs; }
		const OreOreLib::Array<VkClearValue>& ClearValues() const	{ return m_ClearValues; }


	private:

		struct Slot
		{
			uint32_t	ID;
			bool		HasResolve;
		};

		OreOreLib::Array<VkAttachmentDescription>	m_AttacmentDescs;
		OreOreLib::Array<VkClearValue>				m_ClearValues;

		//OreOreLib::ArrayView<VkAttachmentDescription>	m_ColorDescs;
		//OreOreLib::ArrayView<VkAttachmentDescription>	m_DepthDescs;
		//OreOreLib::ArrayView<VkAttachmentDescription>	m_ResolveDescs;

		//uint32_t	m_ActiveResolves;
		uint32_t	m_DepthSlot;

		OreOreLib::Array<Slot>	m_Slots;// first: color attachment slot index, second: has resolve buffer flag
		// リゾルブ以外のバッファインデックス.	//m_ColorToResolve;// アタッチメントに対応したリゾルブのインデックスを格納する. マルチサンプルしない場合はVK_ATTACHMENT_UNUSED入れておく



		void ResetClearValues();


											//pResolveAttachments
// マルチサンプルしたい時に必要
// 専用のVkAttachmentDescriptionを用意する必要がある
// ColorAttachmentのインデックスから、ResolveAttachmentのインデックスへ変換するテーブルが必要

//void InitResolveAttachmentDesc( uint32 slot, VkFormat format, VkImageLayout layout );


	};



}// end of namespace vk


#endif // !ATTACHMENT_H
