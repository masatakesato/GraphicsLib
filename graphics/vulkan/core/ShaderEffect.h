#ifndef SHADER_EFFECT_H
#define	SHADER_EFFECT_H

#include	"SwapChain.h"
#include	"Attachment.h"
#include	"Framebuffer.h"
#include	"RenderTargets.h"
#include	"ShaderPass.h"
#include	"DescriptorSets.h"
#include	"UniformBuffer.h"
#include	"GraphicsPipeline.h"



namespace vk
{
	struct ShaderPassDependency
	{
		uint32			SrcIndex;
		uint32			DstIndex;
		PipelineBarrier	SrcBarrier;
		PipelineBarrier	DstBarrier;
	};




	class ShaderEffect
	{
	public:

		const uint32_t SwapChainColorTarget = ~0u;
		const uint32_t SwapChainDepthTarget = ~0u;

		ShaderEffect();
		ShaderEffect( GraphicsDevice& device, uint32_t numPasses, uint32_t numRenderTargets );
		ShaderEffect( GraphicsDevice& device, SwapChain& swapchain, uint32_t numPasses, uint32_t numRenderTargets );
		~ShaderEffect();
		ShaderEffect( const ShaderEffect& )=delete;

		void Init( GraphicsDevice& device, uint32_t numPasses, uint32_t numRenderTargets );
		void Init( GraphicsDevice& device, SwapChain& swapchain, uint32_t numPasses, uint32_t numRenderTargets );
		void Release();

		void InitRenderTargets( std::initializer_list<RenderTargetDesc> renderTargetDescs );
		void InitDependencies( std::initializer_list<ShaderPassDependency> dependencies );
		void InitGraphicsPipeline( uint32_t pass, const PipelineState& pipelineState );

		void SetSubpassInputRenderTargets( uint32_t pass, std::initializer_list<uint32_t> ilist );
		void SetSubpassOutputRenderTargets( uint32_t pass, std::initializer_list<uint32_t> ilist );

		void InitDescriptorSetLayouts( uint32_t pass, std::initializer_list< std::initializer_list<VkDescriptorSetLayoutBinding> > bindings );

		void BindUniformBuffer( uint32_t pass, uint32_t set, uint32_t binding, const OreOreLib::Array<UniformBuffer>& uniformBuffers );
		void BindCombinedImageSampler( uint32_t pass, uint32_t set, uint32_t binding, VkImageView imageView, VkSampler sampler );


		void BuildRenderPass();
		void BuildDescriptorSets();


		void ReleaseOnSwapchainUpdate();
		void RecreateOnSwapchainUpdate();

		
		ShaderPass& Pass( uint32_t pass=0 ) 	{ return m_ShaderPasses[pass]; }
		const RenderPassAttachments& Attachments() const	{ return m_Attachments; }

		const VkFramebuffer Framebuffer( uint32_t i ) const	{ return m_Framebuffers.Buffer(i); }
		const VkRenderPass RenderPass() const				{ return m_RenderPass; }
		const VkPipeline Pipeline( uint32_t pass ) const	{ return m_Pipelines[ pass ].Pipeline(); }
		const VkPipelineLayout PipelineLayout( uint32_t pass ) const	{ return m_Pipelines[ pass ].Layout(); }
		const VkDescriptorSet& DescriptorSet( uint32_t pass, uint32_t swap_id, uint32_t set_id ) const	{ return m_DescriptorSets[pass].DescriptorSet(swap_id, set_id); }


	private:

		GraphicsDeviceRef				m_refDevice;
		SwapChainRef					m_refSwapChain;

		RenderTargets					m_RenderTargets;


		OreOreLib::Array<ShaderPass>	m_ShaderPasses;// shader modules

		OreOreLib::Array<VkSubpassDependency>	m_SubpassDependencies;
		OreOreLib::Array<AttachmentRefs>		m_AttachmentRefs;// temporary buffer required for VkRenderPass creation
		OreOreLib::Array<VkSubpassDescription>	m_SubpassDescriptions;// temporary buffer required for VkRenderPass creation


		//===================== SwapChain再生成に応じてもう一回作り直す必要があるオブジェクト群 =====================//

		OreOreLib::Array<RenderTargetDesc>			m_RenderTargetDescs;// スワップチェーン + 手動設定する全レンダーターゲットの情報配列
		OreOreLib::ArrayView<vk::RenderTargetDesc>	m_CustomRTDescView;// 手動設定レンダーターゲット情報
		OreOreLib::ArrayView<vk::RenderTargetDesc>	m_SwapChainRTDescView;// スワップチェーンのレンダーターゲット情報


		VkRenderPass						m_RenderPass;

		Framebuffers						m_Framebuffers;// スワップチェーンのVkImageViewを参照している.
		OreOreLib::Array<GraphicsPipeline>	m_Pipelines;

		RenderPassAttachments				m_Attachments;// スワップチェーンから取得した情報を使う. MSAAオンオフ切り替えとかあると再生成必要

OreOreLib::Array<DescriptorSets>	m_DescriptorSets;

	};


}// end of namespace vk 

#endif // !SHADER_EFFECT_H



