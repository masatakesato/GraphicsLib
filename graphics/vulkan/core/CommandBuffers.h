#ifndef COMMAND_BUFFERS_H
#define	COMMAND_BUFFERS_H

#include	"GraphicsDevice.h"



namespace vk
{

	class CommandBuffers
	{
	public:

		CommandBuffers();
		CommandBuffers( GraphicsDevice& device, uint32 numbuffers );
		~CommandBuffers();
		CommandBuffers( const CommandBuffers& ) = delete;
		CommandBuffers( CommandBuffers&& obj );

		void Init( GraphicsDevice& device, uint32 numbuffers );
		void Release();

		const VkCommandBuffer Buffer( OreOreLib::MemSizeType i ) const	{ return m_CommandBuffers[i]; }


	private:

		GraphicsDeviceRef	m_refDevice;

		OreOreLib::Array<VkCommandBuffer> m_CommandBuffers;

	};

}// end of namespace vk

#endif // !COMMAND_BUFFERS_H
