#ifndef ATTACHMENT_H
#define	ATTACHMENT_H

#include	<oreore/container/Array.h>
#include	<oreore/container/ArrayView.h>

#include	"RenderBuffer.h"



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




	struct LoadStoreOp
	{
		VkAttachmentLoadOp LoadOp;
		VkAttachmentStoreOp StoreOp;
	};


	namespace AttachmentOp
	{
		const LoadStoreOp Load_Store{ VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE };		// Load / Store
		const LoadStoreOp Load_DontCare{ VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_DONT_CARE };	// Load / DontCare
		const LoadStoreOp Clear_Store{ VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE };		// Clear / Store
		const LoadStoreOp Clear_DontCare{ VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE };	// Clear / DontCare
		const LoadStoreOp DontCare_Store{ VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_STORE };	// DontCare / Store
		const LoadStoreOp DontCare_DontCare{ VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE };// DontCare / DontCare
	};



	struct AttachmentDesc
	{
		LoadStoreOp		Operations;
		VkImageLayout	FinalLayout;
	};


	

	struct RenderTargetDesc
	{
		RenderBufferDesc	RenderBuffer;
		AttachmentDesc		Attachment;
	};



	// |----- ColorAttachments -----|-- DepthAttachment --|--- ResolveAttachments ---|

	class RenderPassAttachments
	{
	public:

		RenderPassAttachments();
		~RenderPassAttachments();

//void Init( int numAttachments );

void Init( std::initializer_list<RenderTargetDesc> redertargetdescs );

		void Init( int numColors, bool enableDepth, int numResolves=0 );
		void Release();

		bool HasDepth() const	{ return m_DepthDescs; }
		bool HasResolve() const	{ return m_ResolveDescs; }

		void SetColorAttachmentDesc( uint32 attachment, VkFormat format, VkSampleCountFlagBits msaaSamples, LoadStoreOp ops, bool presentable );
		void SetDepthAttachmentDesc( VkFormat format, VkSampleCountFlagBits msaaSamples, LoadStoreOp ops, VkImageLayout layout=VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL );

		void CreateColorAttachmentReferece( OreOreLib::Array<VkAttachmentReference>& refs, std::initializer_list<uint32_t> activeattachments );
		void CreateResolveAttachmentReference( OreOreLib::Array<VkAttachmentReference>& refs, std::initializer_list<uint32_t> activeattachments );
		void CreateDepthAttachmentReference( OreOreLib::Array<VkAttachmentReference>& refs );

		const OreOreLib::Array<VkAttachmentDescription>& AttachmentDescs()	const { return m_AttacmentDescs; }




	private:

		OreOreLib::Array<VkAttachmentDescription>	m_AttacmentDescs;
		
		OreOreLib::ArrayView<VkAttachmentDescription>	m_ColorDescs;
		OreOreLib::ArrayView<VkAttachmentDescription>	m_DepthDescs;
		OreOreLib::ArrayView<VkAttachmentDescription>	m_ResolveDescs;

		uint32_t	m_ActiveResolves;


		OreOreLib::Array<uint32_t>	m_ColorToResolve;// アタッチメントに対応したリゾルブのインデックスを格納する. マルチサンプルしない場合はVK_ATTACHMENT_UNUSED入れておく
//pResolveAttachments
// マルチサンプルしたい時に必要
// 専用のVkAttachmentDescriptionを用意する必要がある
// ColorAttachmentのインデックスから、ResolveAttachmentのインデックスへ変換するテーブルが必要

		void InitResolveAttachmentDesc( uint32 slot, VkFormat format, VkImageLayout layout );


	};



}// end of namespace vk


#endif // !ATTACHMENT_H
