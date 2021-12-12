﻿#ifndef SHADER_EFFECT_H
#define	SHADER_EFFECT_H

#include	"SwapChain.h"
#include	"Attachment.h"
#include	"Framebuffer.h"
#include	"RenderTargets.h"
#include	"ShaderPass.h"
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

/*
AttachmentReferenceで指定するattachment番号って何だっけ？　VkAttachmentDescription配列の要素番号. RenderPassAttachmentsインスタンスが持ってる
RenderPassAttachmentsは、レンダーターゲットとスワップチェーン両方のVkAttachmentDescriptionを保持する
・VkAttachmentReferenceを登録する際にスロット番号を指定するとどうなる？
　→レンダーターゲットだけが登録されている場合
　　→{ RT0, RT1, RT2... }
 　
　→スワップチェーン/レンダーターゲット双方が登録されている場合
	→{ Color Depth, RT0, RT1, RT2... } みたいな並び順になってる
	→スロット番号そのままだとスワップチェーンを指定してしまう。マズい！

・対策：
　→{ RT0, RT1, RT2... , Color Depth }の順番にRenderPassAttachmentsに登録する.
　→レンダーターゲットを先頭に配置すれば、スロット番号はそのまま使える
　→スワップチェーンから取得したアタッチメントにアクセスする際は、SwapChainSlot, DepthSlotの特殊スロット指定を使う


 */

//		void AddShaderPass( const ShaderPass& pass );

		void BuildRenderPass();


		void ReleaseOnSwapchainUpdate();
		void RecreateOnSwapchainUpdate();

		
		ShaderPass& Pass( uint32_t pass=0 ) 	{ return m_ShaderPasses[pass]; }
		const RenderPassAttachments& Attachments() const	{ return m_Attachments; }

		const VkFramebuffer Framebuffer( uint32_t i ) const	{ return m_Framebuffers.Buffer(i); }
		const VkRenderPass RenderPass() const				{ return m_RenderPass; }
		const VkPipeline Pipeline( uint32_t pass ) const	{ return m_Pipelines[ pass ].Pipeline(); }
		const VkPipelineLayout PipelineLayout( uint32_t pass ) const	{ return m_Pipelines[ pass ].Layout(); }


	private:

		GraphicsDeviceRef				m_refDevice;
		SwapChainRef					m_refSwapChain;

		RenderTargets					m_RenderTargets;


		OreOreLib::Array<ShaderPass>	m_ShaderPasses;// shader modules
		OreOreLib::Array<VkSubpassDescription>	m_SubpassDescriptions;
		OreOreLib::Array<VkSubpassDependency>	m_SubpassDependencies;
OreOreLib::Array<AttachmentRefs> m_AttachmentRefs;


		//===================== SwapChain再生成に応じてもう一回作り直す必要があるオブジェクト群 =====================//

		OreOreLib::Array<RenderTargetDesc>	m_RenderTargetDescs;// スワップチェーン + シェーダー構成に応じて手動設定するレンダーターゲットの情報配列
		
		OreOreLib::ArrayView<vk::RenderTargetDesc>	m_refTargetDescs;
		OreOreLib::ArrayView<vk::RenderTargetDesc>	m_refSwapChainTargetDescs;



		VkRenderPass					m_RenderPass;

		Framebuffers						m_Framebuffers;// スワップチェーンのVkImageViewを参照している.
		OreOreLib::Array<GraphicsPipeline>	m_Pipelines;


		RenderPassAttachments			m_Attachments;// スワップチェーンから取得した情報を使う. MSAAオンオフ切り替えとかあると再生成必要


	};


}// end of namespace vk 

#endif // !SHADER_EFFECT_H



