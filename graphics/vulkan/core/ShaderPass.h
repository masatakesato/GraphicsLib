#ifndef SHADER_PASS_H
#define	SHADER_PASS_H

//#include	<oreore/common/TString.h>
//#include	<oreore/container/Array.h>

//#include	"GraphicsDevice.h"

#include	"ShaderParamLayout.h"
#include	"DescriptorSets.h"



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





	class ShaderPass
	{
	public:

		struct ShaderStage
		{
			VkShaderModule module;
			VkShaderStageFlagBits stage;
		};


		ShaderPass();
		ShaderPass( GraphicsDevice& device );
		~ShaderPass();
		ShaderPass( const ShaderPass& ) = delete;
		ShaderPass( ShaderPass&& obj );

		void Release();

		void BindDevice( GraphicsDevice& device )	{ m_refDevice = device; }

		void AddShaderStage( VkShaderStageFlagBits stage, const tstring& filepath );
		void BuildCreateInfo();



		const OreOreLib::Array<VkPipelineShaderStageCreateInfo>& CreateInfos() const	{ return m_CreateInfos; }


		void InitDescriptorSetLayouts( std::initializer_list< std::initializer_list<VkDescriptorSetLayoutBinding> > bindings )
		{
			m_ShaderParamLayout.Init( m_refDevice->Device(), bindings );
		}


		const ShaderParamLayout& ParamLayout() const	{ return m_ShaderParamLayout; }




	private:

		GraphicsDeviceRef	m_refDevice;

		OreOreLib::Array<ShaderStage>	m_ShaderStages;
		OreOreLib::Array<VkPipelineShaderStageCreateInfo>	m_CreateInfos;


		ShaderParamLayout	m_ShaderParamLayout;
	

		void InitShaderStage( ShaderStage& shaderstage, const OreOreLib::Array<char>& shadercode, VkShaderStageFlagBits stage );


	};


}// end of namespace vk


#endif // !SHADER_PASS_H
