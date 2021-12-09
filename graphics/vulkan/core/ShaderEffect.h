#ifndef SHADER_EFFECT_H
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
		PipelineBarrier	Src;
		PipelineBarrier	Dst;
	};




	class ShaderEffect
	{
	public:

		ShaderEffect();
		ShaderEffect( GraphicsDevice& device, uint32_t numpasses );
		~ShaderEffect();
		ShaderEffect( const ShaderEffect& )=delete;

		void Release();

		void BindSwapChain( SwapChain& swapchain );
		void UnbindSwapChain();

		void InitRenderTargets( std::initializer_list<RenderTargetDesc> renderTargetDescs );
		void InitDependencies( std::initializer_list<ShaderPassDependency> dependencies );

void SetInputAttachments( uint32_t pass, std::initializer_list<VkAttachmentReference> ilist );
void SetColorAttachments( uint32_t pass, std::initializer_list<VkAttachmentReference> ilist );
void SetResolveAttachments( uint32_t pass, std::initializer_list<VkAttachmentReference> ilist );


//		void AddShaderPass( const ShaderPass& pass );

		void BuildRenderPass();



		ShaderPass& Pass( uint32_t i=0 ) 	{ return m_ShaderPasses[i]; }


	private:

		GraphicsDeviceRef				m_refDevice;
		SwapChainRef					m_refSwapChain;

		RenderTargets					m_RenderTargets;


		OreOreLib::Array<ShaderPass>	m_ShaderPasses;// shader modules
		OreOreLib::Array<VkSubpassDependency>	m_SubpassDependencies;


AttachmentRefs 

		//===================== SwapChain再生成に応じてもう一回作り直す必要があるオブジェクト群 =====================//
		VkRenderPass					m_RenderPass;


		OreOreLib::Array<GraphicsPipeline>	m_Pipelines;
		Framebuffers					m_Framebuffers;


		RenderPassAttachments			m_Attachments;// スワップチェーンから取得した情報を使う. MSAAオンオフ切り替えとかあると再生成必要





	};


}// end of namespace vk 

#endif // !SHADER_EFFECT_H



