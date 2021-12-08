#include	"CommandBuffers.h"

#include	"../utils/HelperFunctions.h"



namespace vk
{



	CommandBuffers::CommandBuffers()
	{

	}



	CommandBuffers::CommandBuffers( GraphicsDevice& device, uint32 numbuffers )
	{
		Init( device, numbuffers );
	}



	CommandBuffers::~CommandBuffers()
	{

	}



	CommandBuffers::CommandBuffers( CommandBuffers&& obj )
	{

	}



	void CommandBuffers::Init( GraphicsDevice& device, uint32 numbuffers )
	{
		m_refDevice	= device;

		m_CommandBuffers.Resize( numbuffers );

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool			= m_refDevice->CommandPool();
		allocInfo.level					= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount	= static_cast<uint32_t>( m_CommandBuffers.Length() );

		VK_CHECK_RESULT( vkAllocateCommandBuffers( m_refDevice->Device(), &allocInfo, m_CommandBuffers.begin() ) );
	}



	void CommandBuffers::Release()
	{
		if( IsValidDevice( m_refDevice ) )
		{
			vkFreeCommandBuffers( m_refDevice->Device(), m_refDevice->CommandPool(), static_cast<uint32_t>( m_CommandBuffers.Length() ), m_CommandBuffers.begin() );
			m_CommandBuffers.Release();

		}
	}






}// end of namespace vk