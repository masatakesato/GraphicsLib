#ifndef RENDERER_H
#define	RENDERER_H

#include	"GraphicsDevice.h"



namespace vk
{

	class Renderer
	{
	public:

		Renderer();
		~Renderer();
		Renderer( const Renderer& ) = delete;

		void DrawFrame();



	private:

		GraphicsDeviceRef	m_refDevice;


		OreOreLib::Array<VkCommandBuffer>	m_CommandBuffers;


		void InitCommandBuffers( uint32 numbuffers );


	};

}// end of namespace vk


#endif // !RENDERER_H
