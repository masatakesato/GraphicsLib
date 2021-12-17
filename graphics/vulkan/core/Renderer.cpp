#include	"Renderer.h"

#include	"../utils/HelperFunctions.h"



namespace vk
{

	Renderer::Renderer()
	{

	}

	

	Renderer::~Renderer()
	{

	}






	void Renderer::InitCommandBuffers( uint32 numbuffers )
	{

		m_CommandBuffers.Resize( numbuffers );

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool			= m_refDevice->CommandPool();
		allocInfo.level					= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount	= m_CommandBuffers.Length<uint32_t>();

		VK_CHECK_RESULT( vkAllocateCommandBuffers( m_refDevice->Device(), &allocInfo, m_CommandBuffers.begin() ) );
	}

}// end of namespace vk