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

		void InitRenderTargets( OreOreLib::Memory<RenderTargetDesc>& renderTargetDescs );

//		void AddShaderPass( const ShaderPass& pass );
		void AddShaderPassConnection( uint32_t srd, uint32_t dst );


		ShaderPass& Pass( uint32_t i=0 ) 	{ return m_ShaderPasses[i]; }


	private:

		GraphicsDeviceRef				m_refDevice;
		SwapChainRef					m_refSwapChain;

		RenderTargets					m_RenderTargets;


		OreOreLib::Array<ShaderPass>	m_ShaderPasses;// shader modules
		OreOreLib::Array<VkSubpassDependency>	m_SubpassDependencies;


		//===================== SwapChain再生成に応じてもう一回作り直す必要があるオブジェクト群 =====================//
		VkRenderPass					m_RenderPass;


		OreOreLib::Array<GraphicsPipeline>	m_Pipelines;
		Framebuffers					m_Framebuffers;


		RenderPassAttachments			m_Attachments;// スワップチェーンから取得した情報を使う. MSAAオンオフ切り替えとかあると再生成必要





	};


}// end of namespace vk 

#endif // !SHADER_EFFECT_H



