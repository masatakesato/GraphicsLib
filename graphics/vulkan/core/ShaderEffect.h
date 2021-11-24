#ifndef SHADER_EFFECT_H
#define	SHADER_EFFECT_H

#include	<oreore/container/Pair.h>

#include	"Attachment.h"
#include	"ShaderPass.h"



namespace vk
{
	using SubpassDependencies = OreOreLib::Array<VkSubpassDependency>;





	enum class AttachmentOps
	{
		/*const uint8*/ Load_Store,
		/*const uint8*/ Load_None,
		/*const uint8*/ Clear_Store,
		/*const uint8*/ Clear_None,
		/*const uint8*/ None_Store,
		/*const uint8*/ None_None,
	};







	class ShaderEffect
	{
	public:

		ShaderEffect();
		ShaderEffect( GraphicsDevice& device );
		ShaderEffect( const ShaderEffect& )=delete;
		~ShaderEffect();


		void AddColorAttachmentDesc( uint8 slot, VkFormat format, VkSampleCountFlagBits msaaSamples, AttachmentOps ops, VkImageLayout layout=VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL );
		void AddDepthAttachmentDesc( uint8 slot, VkFormat format, VkSampleCountFlagBits msaaSamples, AttachmentOps ops, VkImageLayout layout=VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL );
		void AddReslveAttachmentDesc( uint8 slot, VkFormat format );



	private:

		GraphicsDeviceRef				m_refDevice;

		VkRenderPass					m_RenderPass;
		AttachmentDescArray				m_AttacmentDescs;


		OreOreLib::Array<ShaderPass>	m_ShaderPasses;// shader modules
		SubpassDependencies				m_Dependencies;// 






		const Pair<VkAttachmentLoadOp, VkAttachmentStoreOp> LoadStoreOps[] =
		{
			{ VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE },		// Load / Store
			{ VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_DONT_CARE },	// Load / DontCare
			{ VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE },		// Clear / Store
			{ VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE },	// Clear / DontCare
			{ VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_STORE },	// DontCare / Store
			{ VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE }// DontCare / DontCare
		};



	};


}// end of namespace vk 

#endif // !SHADER_EFFECT_H



