#ifndef SHADER_EFFECT_H
#define	SHADER_EFFECT_H

#include	"SwapChain.h"
#include	"Attachment.h"
#include	"Framebuffer.h"
#include	"RenderTargets.h"
#include	"ShaderPass.h"
#include	"DescriptorBuffer.h"
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

		const uint32 SwapChainColorTarget = ~0u;
		const uint32 SwapChainDepthTarget = ~0u;

		ShaderEffect();
		ShaderEffect( GraphicsDevice& device, uint32 numPasses, uint32 numRenderTargets );
		ShaderEffect( GraphicsDevice& device, SwapChain& swapChain, uint32 numPasses, uint32 numRenderTargets );
		~ShaderEffect();
		ShaderEffect( const ShaderEffect& )=delete;

		void Init( GraphicsDevice& device, uint32 numPasses, uint32 numRenderTargets );
		void Init( GraphicsDevice& device, SwapChain& swapchain, uint32 numPasses, uint32 numRenderTargets );
		void Release();

		void InitRenderTargets( std::initializer_list<RenderTargetDesc> renderTargetDescs );
		void InitDependencies( std::initializer_list<ShaderPassDependency> dependencies );

		void SetSubpassInputRenderTargets( uint32 pass, std::initializer_list<uint32> ilist );
		void SetSubpassOutputRenderTargets( uint32 pass, std::initializer_list<uint32> ilist );

		void InitDescriptorSetLayouts( uint32 pass, std::initializer_list< std::initializer_list<VkDescriptorSetLayoutBinding> > bindings );

		void BindUniformBuffer( uint32 pass, uint32 set, uint32 binding, const OreOreLib::Array<UniformBuffer>& uniformBuffers );
		void BindCombinedImageSampler( uint32 pass, uint32 set, uint32 binding, VkImageView imageView, VkSampler sampler );

		void BuildDescriptorSets();
		void BuildRenderPass();
		void BuildFramebuffers();
		void BuildPipelines();


		void ReleaseOnSwapchainUpdate();
		void RecreateOnSwapchainUpdate();

		
		ShaderPass& Pass( uint32 pass=0 ) 							{ return m_ShaderPasses[ pass ]; }
		const RenderPassAttachments& Attachments() const			{ return m_Attachments; }

		const VkFramebuffer Framebuffer( uint32 i ) const			{ return m_Framebuffers.Buffer(i); }
		const VkRenderPass RenderPass() const						{ return m_RenderPass; }
		const VkPipeline Pipeline( uint32 pass ) const				{ return m_Pipelines[ pass ].Pipeline(); }
		const VkPipelineLayout PipelineLayout( uint32 pass ) const	{ return m_Pipelines[ pass ].Layout(); }
		PipelineState& RenderState( uint32 pass )					{ return m_ShaderPasses[ pass ].State(); }


		uint32_t NumSets( uint32 pass ) const	{ return m_DescriptorBuffers[ pass ].NumSets(); }
		const VkDescriptorSet& DescriptorSets( uint32 pass, uint32 swap_id ) const					{ return m_DescriptorBuffers[ pass ].DescriptorSets( swap_id ); }
		const VkDescriptorSet& DescriptorSet( uint32 pass, uint32 swap_id, uint32 set_id ) const	{ return m_DescriptorBuffers[ pass ].DescriptorSet( swap_id, set_id ); }


	private:

		GraphicsDeviceRef				m_refDevice;
		SwapChainRef					m_refSwapChain;

		RenderTargets					m_RenderTargets;


		OreOreLib::Array<ShaderPass>	m_ShaderPasses;// shader modules

		OreOreLib::Array<VkSubpassDependency>	m_SubpassDependencies;// サブパス間の依存関係配列


		//===================== SwapChain再生成に応じてもう一回作り直す必要があるオブジェクト群 =====================//

		OreOreLib::Array<RenderTargetDesc>			m_RenderTargetDescs;// スワップチェーン + 手動設定する全レンダーターゲットの情報配列
		OreOreLib::ArrayView<vk::RenderTargetDesc>	m_CustomRTDescView;// 手動設定レンダーターゲット情報
		OreOreLib::ArrayView<vk::RenderTargetDesc>	m_SwapChainRTDescView;// スワップチェーンのレンダーターゲット情報


		VkRenderPass						m_RenderPass;

		Framebuffers						m_Framebuffers;// スワップチェーンのVkImageViewを参照している.
		OreOreLib::Array<GraphicsPipeline>	m_Pipelines;// オブジェクト生成にVkRenderPass(スワップチェーン更新で再生成)が必要

		RenderPassAttachments				m_Attachments;// スワップチェーンから取得した情報を使う. MSAAオンオフ切り替えとかあると再生成必要

OreOreLib::Array<DescriptorBuffer>	m_DescriptorBuffers;// スワップチェーン画像数だけバックバッファ確保が必要

	};


}// end of namespace vk 

#endif // !SHADER_EFFECT_H



