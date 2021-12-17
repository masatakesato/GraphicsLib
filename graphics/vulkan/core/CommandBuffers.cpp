#include	"CommandBuffers.h"

#include	"../utils/HelperFunctions.h"



namespace vk
{



	CommandBuffers::CommandBuffers()
	{

	}



	CommandBuffers::CommandBuffers( GraphicsDevice& device, uint32 numBuffers )
	{
		Init( device, numBuffers );
	}



	CommandBuffers::~CommandBuffers()
	{

	}



	CommandBuffers::CommandBuffers( CommandBuffers&& obj )
	{

	}



	void CommandBuffers::Init( GraphicsDevice& device, uint32 numBuffers )
	{
		m_refDevice	= device;

		m_CommandBuffers.Resize( numBuffers );

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool			= m_refDevice->CommandPool();
		allocInfo.level					= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount	= m_CommandBuffers.Length<uint32_t>();

		VK_CHECK_RESULT( vkAllocateCommandBuffers( m_refDevice->Device(), &allocInfo, m_CommandBuffers.begin() ) );
	}



	void CommandBuffers::Release()
	{
		if( IsValidDevice( m_refDevice ) )
		{
			vkFreeCommandBuffers( m_refDevice->Device(), m_refDevice->CommandPool(), m_CommandBuffers.Length<uint32_t>(), m_CommandBuffers.begin() );
			m_CommandBuffers.Release();

		}
	}






}// end of namespace vk