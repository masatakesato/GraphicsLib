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


	private:

		GraphicsDeviceRef	m_refDevice;
		WindowRef			m_refWindow;

		SwapChain		m_SwapChain;
		CommandBuffers	m_CommandBuffers;

		uint32_t imageIndex;

		void RecreateSwapChain();


	};

}// end of namespace vk


#endif // !RENDERER_H
