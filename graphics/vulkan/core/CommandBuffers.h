#ifndef COMMAND_BUFFERS_H
#define	COMMAND_BUFFERS_H

#include	"GraphicsDevice.h"



namespace vk
{

	class CommandBuffers
	{
	public:

		CommandBuffers();
		CommandBuffers( GraphicsDevice& device, uint32 numBuffers );
		~CommandBuffers();
		CommandBuffers( const CommandBuffers& ) = delete;
		CommandBuffers( CommandBuffers&& obj );

		void Init( GraphicsDevice& device, uint32 numBuffers );
		void Release();

		uint32_t NumBuffers() const	{ return m_CommandBuffers.Length<uint32_t>(); }
		const VkCommandBuffer Buffer( OreOreLib::MemSizeType i ) const	{ return m_CommandBuffers[i]; }
		const OreOreLib::Array<VkCommandBuffer>& Buffers() const		{ return m_CommandBuffers; }




	private:

		GraphicsDeviceRef	m_refDevice;

		OreOreLib::Array<VkCommandBuffer> m_CommandBuffers;

	};

}// end of namespace vk

#endif // !COMMAND_BUFFERS_H
