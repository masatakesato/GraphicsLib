#ifndef RENDERER_H
#define	RENDERER_H

//#include	"GraphicsDevice.h"
#include	"SwapChain.h"
#include	"CommandBuffers.h"



namespace vk
{

	class Renderer
	{
	public:

		Renderer();
		~Renderer();
		Renderer( const Renderer& ) = delete;

		void DrawFrame();

		void PrepareFrame();
		void SubmitFrame();

		SwapChain& SwapChain();



	private:

		GraphicsDeviceRef	m_refDevice;
		WindowRef			m_refWindow;

		vk::SwapChain		m_SwapChain;
		CommandBuffers		m_CommandBuffers;

		FrameSynchronizer	m_Synchronizer;

		uint32_t imageIndex;

		void RecreateSwapChain();


	};

}// end of namespace vk


#endif // !RENDERER_H
