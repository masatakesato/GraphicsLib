﻿#ifndef SHADER_PASS_H
#define	SHADER_PASS_H

//#include	<oreore/common/TString.h>
//#include	<oreore/container/Array.h>

//#include	"GraphicsDevice.h"

#include	"ShaderParamLayout.h"
#include	"DescriptorSets.h"



namespace vk
{

	using AttachmentDescArray	= OreOreLib::Array<VkAttachmentDescription>;
	using AttachmentRefArray	= OreOreLib::Array<VkAttachmentReference>;



	class SubPassAttachments
	{
	public:

		SubPassAttachments()
		{

		}


		SubPassAttachments( const AttachmentDescArray& attachmentdescs )
			: m_refAttachmentDescs( attachmentdescs )
		{

		}


		const AttachmentRefArray& InputAttachments() const
		{
			return m_InputAttachments;
		}


		const AttachmentRefArray& ColorAttachments() const
		{
			return m_ColorAttachments;
		}


		const AttachmentRefArray& ResolveAttachments() const
		{
			return m_ResolveAttachments;
		}


		const VkAttachmentReference& DepthStencilAttachment() const
		{
			return m_DepthStencilAttachment;
		}



	private:

		OreOreLib::ReferenceWrapper< const AttachmentDescArray >	m_refAttachmentDescs;

		
		AttachmentRefArray	m_InputAttachments;

		AttachmentRefArray	m_ColorAttachments;
		AttachmentRefArray	m_ResolveAttachments;
		// https://qiita.com/Pctg-x8/items/2b3d5c8a861f42aa533f
		// for Multisampling. element count mus 
		// m_ResolveAttachments.Length() must be equal to m_ColorAttachments.Length()
		// set VkAttachmentReference::attachment to VK_ATTACHMENT_UNUSED if you want to invalidate multisampling

		VkAttachmentReference	m_DepthStencilAttachment;

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
		ShaderPass( const ShaderPass& )=delete;
		~ShaderPass();

		void Release();

		void AddShaderStage( VkShaderStageFlagBits stage, const tstring& filepath );
		void BuildCreateInfo();



		const OreOreLib::Array<VkPipelineShaderStageCreateInfo>& CreateInfoArray() const	{ return m_CreateInfos; }


		void InitDescriptorSetLayouts( std::initializer_list< std::initializer_list<VkDescriptorSetLayoutBinding> > bindings )
		{
			m_ShaderParamLayout.Init( m_refDevice->Device(), bindings );
		}


		const ShaderParamLayout& ParamLayout() const	{ return m_ShaderParamLayout; }




	private:

		GraphicsDeviceRef	m_refDevice;

		OreOreLib::Array<ShaderStage>	m_ShaderStages;
		OreOreLib::Array<VkPipelineShaderStageCreateInfo>	m_CreateInfos;


		SubPassAttachments	m_Attachments;
		ShaderParamLayout	m_ShaderParamLayout;
	

		void InitShaderStage( ShaderStage& shaderstage, const OreOreLib::Array<char>& shadercode, VkShaderStageFlagBits stage );


	};


}// end of namespace vk


#endif // !SHADER_PASS_H
