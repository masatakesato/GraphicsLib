#include	"FrameSynchronizer.h"

#include	"../utils/HelperFunctions.h"



namespace vk
{

	FrameSynchronizer::FrameSynchronizer()
	{

	}
	

	
	FrameSynchronizer::FrameSynchronizer( GraphicsDevice& device, int numswaps, int maxConcurrentFrames )
	{
		Init( device, numswaps, maxConcurrentFrames );
	}
	

	
	FrameSynchronizer::~FrameSynchronizer()
	{


	}



	void FrameSynchronizer::Init( GraphicsDevice& device, int numswaps, int maxConcurrentFrames )
	{
		m_refDevice				= device;
		m_MaxConcurrentFrames	= maxConcurrentFrames;

		imageAvailableSemaphores.Resize( maxConcurrentFrames );
		renderFinishedSemaphores.Resize( maxConcurrentFrames );
		inFlightFences.Resize( maxConcurrentFrames );
		imagesInFlight.Resize( numswaps, VK_NULL_HANDLE );

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType	= VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags	= VK_FENCE_CREATE_SIGNALED_BIT;

		for( int i=0; i<m_MaxConcurrentFrames; ++i )
		{
			VK_CHECK_RESULT( vkCreateSemaphore( m_refDevice->Device(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i] ) );
			VK_CHECK_RESULT( vkCreateSemaphore( m_refDevice->Device(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i] ) );
			VK_CHECK_RESULT( vkCreateFence( m_refDevice->Device(), &fenceInfo, nullptr, &inFlightFences[i] ) );
		}
	}



	void FrameSynchronizer::Release()
	{
		if( !m_refDevice.IsNull() && m_refDevice->Device() != VK_NULL_HANDLE )
		{

			for( size_t i=0; i<m_MaxConcurrentFrames; ++i )
			{
				if( renderFinishedSemaphores[i] != VK_NULL_HANDLE )
				{
					vkDestroySemaphore( m_refDevice->Device(), renderFinishedSemaphores[i], nullptr );
					renderFinishedSemaphores[i] = VK_NULL_HANDLE;
				}

				if( imageAvailableSemaphores[i] != VK_NULL_HANDLE )
				{
					vkDestroySemaphore( m_refDevice->Device(), imageAvailableSemaphores[i], nullptr );
					imageAvailableSemaphores[i] = VK_NULL_HANDLE;
				}

				if( inFlightFences[i] != VK_NULL_HANDLE )
				{
					vkDestroyFence( m_refDevice->Device(), inFlightFences[i], nullptr );
					inFlightFences[i] = VK_NULL_HANDLE;
				}
			}

			renderFinishedSemaphores.Release();
			imageAvailableSemaphores.Release();
			inFlightFences.Release();

			m_MaxConcurrentFrames = 0;
			//m_refDevice.Reset();
		}
	}




}// end of namespace vk