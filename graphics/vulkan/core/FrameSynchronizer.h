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
		FrameSynchronizer( const FrameSynchronizer& ) = delete;
		~FrameSynchronizer();

		void Init( GraphicsDevice& device, uint32_t maxConcurrentFrames );
		void Release();

		uint32_t MaxConcurrentFrames() const	{ return m_MaxConcurrentFrames; }
//		const VkSemaphore& ImageAvailableSemaphores( int i ) const { return imageAvailableSemaphores[i]; }
//		const VkSemaphore& RenderFinishedSemaphore( int i ) const { return renderFinishedSemaphores[i]; }
//		const VkFence& InFlightFence( int i ) const { return inFlightFences[i]; }


		const VkFence& CurrentInFlightFence() const { return inFlightFences[ currentFrame ]; }
		const VkSemaphore& CurrentImageAvailableSemaphore() const	{ return imageAvailableSemaphores[ currentFrame ]; }
		const VkSemaphore& CurrentRenderFinishedSemaphore() const	{ return renderFinishedSemaphores[ currentFrame ]; }

		void WaitForCurrentFrame();

		void SwitchCurrent()	{ currentFrame = ( currentFrame + 1) % m_MaxConcurrentFrames; }

		void ResetFence()	{ vkResetFences( m_refDevice->Device(), 1, &inFlightFences[ currentFrame ] ); }


	private:

		GraphicsDeviceRef	m_refDevice;
		uint32_t					m_MaxConcurrentFrames;

		OreOreLib::Array<VkSemaphore>	imageAvailableSemaphores;// MAX_CONCURRENT_FRAMES
		OreOreLib::Array<VkSemaphore>	renderFinishedSemaphores;// MAX_CONCURRENT_FRAMES
		OreOreLib::Array<VkFence>		inFlightFences;// MAX_CONCURRENT_FRAMES
//		OreOreLib::Array<VkFence>		imagesInFlight;// スワップチェイン画像枚数分

		uint32_t						currentFrame;
	};


}// end of namespace vk


#endif // !FRAME_SYNCHRONIZER_H
