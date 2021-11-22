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
		FrameSynchronizer( GraphicsDevice& device, int numswaps, int maxConcurrentFrames );
		FrameSynchronizer( const FrameSynchronizer& ) = delete;
		~FrameSynchronizer();

		void Init( GraphicsDevice& device, int numswaps, int maxConcurrentFrames );
		void Release();

		uint32_t MaxConcurrentFrames() const	{ return static_cast<uint32_t>( imageAvailableSemaphores.Length() ); }
		const VkSemaphore& ImageAvailableSemaphores( int i ) const { return imageAvailableSemaphores[i]; }
		const VkSemaphore& RenderFinishedSemaphore( int i ) const { return renderFinishedSemaphores[i]; }
		const VkFence& InFlightFence( int i ) const { return inFlightFences[i]; }
		const VkFence& ImagesInFlight( int i ) const { return imagesInFlight[i]; }


	private:

		GraphicsDeviceRef	m_refDevice;
		int					m_MaxConcurrentFrames;

		OreOreLib::Array<VkSemaphore>	imageAvailableSemaphores;// MAX_CONCURRENT_FRAMES
		OreOreLib::Array<VkSemaphore>	renderFinishedSemaphores;// MAX_CONCURRENT_FRAMES
		OreOreLib::Array<VkFence>		inFlightFences;// MAX_CONCURRENT_FRAMES
		OreOreLib::Array<VkFence>		imagesInFlight;// スワップチェイン画像枚数分

	};


}// end of namespace vk


#endif // !FRAME_SYNCHRONIZER_H
