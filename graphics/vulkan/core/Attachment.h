#ifndef ATTACHMENT_H
#define	ATTACHMENT_H

#include	<vulkan/vulkan.h>

#include	<oreore/container/Array.h>
#include	<oreore/container/ArrayView.h>



namespace vk
{

	enum class AttachmentOps
	{
		/*const uint8*/ Load_Store,
		/*const uint8*/ Load_None,
		/*const uint8*/ Clear_Store,
		/*const uint8*/ Clear_None,
		/*const uint8*/ None_Store,
		/*const uint8*/ None_None,
	};




	class SubpassAttachments
	{
	public:

		SubpassAttachments();
		SubpassAttachments( const SubpassAttachments& )=delete;
		~SubpassAttachments();

		void Init( int numInputs, int numColorAttachments, bool depthStencil );
		void Release();

		void SetInputAttachments( std::initializer_list<VkAttachmentReference> ilist );
		void SetColorAttachments( std::initializer_list<VkAttachmentReference> ilist );
		void SetResolveAttachments( std::initializer_list<VkAttachmentReference> ilist );
		void SetDepthAttachment( VkAttachmentReference attachref );


	private:

		OreOreLib::Array<VkAttachmentReference>	m_InputAttachments;

		OreOreLib::Array<VkAttachmentReference>	m_ColorAttachments;
		OreOreLib::Array<VkAttachmentReference>	m_ResolveAttachments;
		// https://qiita.com/Pctg-x8/items/2b3d5c8a861f42aa533f
		// for Multisampling.
		// m_ResolveAttachments.Length() must be equal to m_ColorAttachments.Length()
		// set VkAttachmentReference::attachment to VK_ATTACHMENT_UNUSED if you want to invalidate multisampling

		VkAttachmentReference	m_DepthStencilAttachment{};

	};





	//[0]                   [ m_DepthSlot ]         [ m_ResolveSlot ]
	// |----- ColorAttachments -----|-- DepthAttachment --|--- ResolveAttachments -----|

	class RenderPassAttachments
	{
	public:

		RenderPassAttachments();
		~RenderPassAttachments();

		void Init( int numColors, bool enableDepth, int numResolves=0 );
		void Release();

		bool HasDepth() const	{ return m_DepthDescs; }
		bool HasResolve() const	{ return m_ResolveDescs; }

		void SetColorAttachmentDesc( uint32 attachment, VkFormat format, VkSampleCountFlagBits msaaSamples, AttachmentOps ops, bool presentable );
		void SetDepthAttachmentDesc( VkFormat format, VkSampleCountFlagBits msaaSamples, AttachmentOps ops, VkImageLayout layout=VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL );


		// {0, 2, 3}
		void InitColorAttachmentReferece( OreOreLib::Array<VkAttachmentReference>& refs, std::initializer_list<uint32_t> activeattachments )
		{
			for( const auto& attachment : activeattachments )
			{
				refs.AddToTail( { attachment, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } );
			}
		}


		// {0, 2, 3}
		void InitResolveAttachmentReference( OreOreLib::Array<VkAttachmentReference>& refs, std::initializer_list<uint32_t> activeattachments )
		{
			for( const auto& attachment : activeattachments )
			{
				if( m_ColorToResolve[ attachment ] == -1 )
					refs.AddToTail( { VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED } );
				else
					refs.AddToTail( { static_cast<uint32_t>( m_ColorToResolve[ attachment ] ), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } );
			}	
		}


		void InitDepthAttachmentReference( OreOreLib::Array<VkAttachmentReference>& refs )
		{
			if( m_DepthDescs )
				refs.AddToTail( { static_cast<uint32_t>( m_ColorDescs.Length() ), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } );
		}




	private:

		OreOreLib::Array<VkAttachmentDescription>	m_AttacmentDescs;
		
		OreOreLib::ArrayView<VkAttachmentDescription>	m_ColorDescs;
		OreOreLib::ArrayView<VkAttachmentDescription>	m_DepthDescs;
		OreOreLib::ArrayView<VkAttachmentDescription>	m_ResolveDescs;

		int32	m_ActiveResolves;


		OreOreLib::Array<int32>	m_ColorToResolve;// マルチサンプルしない場合は-1, する場合はインデックス入れておく
//pResolveAttachments
// マルチサンプルしたい時に必要
// 専用のVkAttachmentDescriptionを用意する必要がある
// ColorAttachmentのインデックスから、ResolveAttachmentのインデックスへ変換するテーブルが必要

		void InitResolveAttachmentDesc( uint32 slot, VkFormat format, VkImageLayout layout );


	};



}// end of namespace vk


#endif // !ATTACHMENT_H
