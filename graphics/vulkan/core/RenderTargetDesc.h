#ifndef RENDER_TARGET_DESC_H
#define	RENDER_TARGET_DESC_H

#include	<vulkan/vulkan.h>



namespace vk
{


	namespace RenderBufferUsage
	{
		// color or depth, support input or not, transient or not
		const VkImageUsageFlags ColorWrite					= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		const VkImageUsageFlags ColorWrite_Transient		= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;

		const VkImageUsageFlags ColorReadWrite				= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		const VkImageUsageFlags ColorReadWrite_Transient	= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;

		const VkImageUsageFlags DepthWrite					= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		const VkImageUsageFlags DepthWrite_Transient		= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;

		const VkImageUsageFlags DepthReadWrite				= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		const VkImageUsageFlags DepthReadWrite_Transient	= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
	};


	struct RenderBufferDesc
	{
		VkExtent2D				Dim;
		VkFormat				Format;
		VkSampleCountFlagBits	MultiSampleFlag;
		bool					Resolve;
		VkImageUsageFlags		UsageFlags;
	};









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
		// if( Resolve )
		//	if( FinalLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR )
		//		-> MSAAバッファはVK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMALに変更
		//		-> リゾルブバッファをVK_IMAGE_LAYOUT_PRESENT_SRC_KHRに設定する
		//	else
		//		-> MSAAもリゾルブもFinalLayoutのまま作る

	};




	struct RenderTargetDesc
	{
		RenderBufferDesc	RenderBuffer;
		AttachmentDesc		Attachment;
	};





}// end of namespace vk

#endif // !RENDER_TARGET_DESC_H
