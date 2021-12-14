#ifndef FRAME_SYNCHRONIZER_H
#define	FRAME_SYNCHRONIZER_H

#include	<oreore/container/NDArray.h>

#include	"GraphicsDevice.h"



namespace vk
{

	class FrameSynchronizer
	{
	public:

		FrameSynchronizer();
		FrameSynchronizer( GraphicsDevice& device, uint32_t maxConcurrentFrames );
		~FrameSynchronizer();
		FrameSynchronizer( const FrameSynchronizer& ) = delete;

		void Init( GraphicsDevice& device, uint32_t maxConcurrentFrames );
		void Release();

		uint32_t MaxConcurrentFrames() const	{ return m_MaxConcurrentFrames; }
//		const VkSemaphore& ImageAvailableSemaphores( int i ) const { return imageAvailableSemaphores[i]; }
//		const VkSemaphore& RenderFinishedSemaphore( int i ) const { return renderFinishedSemaphores[i]; }
//		const VkFence& InFlightFence( int i ) const { return inFlightFences[i]; }


		const VkFence& CurrentInFlightFence() const { return inFlightFences[ currentFrame ]; }
		const VkSemaphore& CurrentPresentFinishedSemaphore() const	{ return m_PresentFinishedSemaphores[ currentFrame ]; }
		const VkSemaphore& CurrentRenderFinishedSemaphore() const	{ return m_RenderFinishedSemaphores[ currentFrame ]; }

		void WaitForCurrentFrame();

		void SwitchCurrent()	{ currentFrame = ( currentFrame + 1) % m_MaxConcurrentFrames; }

		void ResetFence()	{ vkResetFences( m_refDevice->Device(), 1, &inFlightFences[ currentFrame ] ); }


	private:

		GraphicsDeviceRef	m_refDevice;
		uint32_t					m_MaxConcurrentFrames;

		OreOreLib::Array<VkSemaphore>	m_PresentFinishedSemaphores;// 描画処理完了(PresentQueueにサブミットしたジョブの完了)時にシグナル状態になるセマフォ
		OreOreLib::Array<VkSemaphore>	m_RenderFinishedSemaphores;// プレゼント処理完了(RenderQueueにサブミットしたジョブの完了)時にシグナル状態になるセマフォ
		OreOreLib::Array<VkFence>		inFlightFences;// スワップチェーンのフェンス.

		uint32_t						currentFrame;
	};



	using FrameSynchronizerRef = OreOreLib::ReferenceWrapper<FrameSynchronizer>;


}// end of namespace vk


#endif // !FRAME_SYNCHRONIZER_H
