#ifndef ATTACHMENT_H
#define	ATTACHMENT_H

#include	<vulkan/vulkan.h>

#include	<oreore/container/Array.h>



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







	class RenderPassAttachments
	{
	public:

		RenderPassAttachments();
		~RenderPassAttachments();

		void Init( int numattachments );
		void Release();

		bool HasDepth() const	{ return m_DepthSlot>=0; }
		bool HasResolve() const	{ return m_ResolveSlot>=0; }

		void SetColorAttachmentDesc( uint8 slot, VkFormat format, VkSampleCountFlagBits msaaSamples, AttachmentOps ops, VkImageLayout layout=VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL );
		void SetDepthAttachmentDesc( uint8 slot, VkFormat format, VkSampleCountFlagBits msaaSamples, AttachmentOps ops, VkImageLayout layout=VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL );
		void SetResolveAttachmentDesc( uint8 slot, VkFormat format );


	private:

		OreOreLib::Array<VkAttachmentDescription>	m_AttacmentDescs;

		int32	m_DepthSlot;
		int32	m_ResolveSlot;


		bool DeleteDepthAttachment();
		bool DeleteResolveAttachment();


	};



}// end of namespace vk


#endif // !ATTACHMENT_H
