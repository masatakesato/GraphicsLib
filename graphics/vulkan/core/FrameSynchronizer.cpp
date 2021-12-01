#include	"FrameSynchronizer.h"

#include	"../utils/HelperFunctions.h"



namespace vk
{

	FrameSynchronizer::FrameSynchronizer()
		: currentFrame( 0 )
	{

	}
	

	
	FrameSynchronizer::FrameSynchronizer( GraphicsDevice& device, uint32_t maxConcurrentFrames )
		: currentFrame( 0 )
	{
		Init( device, maxConcurrentFrames );
	}
	

	
	FrameSynchronizer::~FrameSynchronizer()
	{


	}



	void FrameSynchronizer::Init( GraphicsDevice& device, uint32_t maxConcurrentFrames )
	{
		m_refDevice				= device;
		m_MaxConcurrentFrames	= maxConcurrentFrames;

		m_PresentFinishedSemaphores.Resize( (int)maxConcurrentFrames );
		m_RenderFinishedSemaphores.Resize( (int)maxConcurrentFrames );
		inFlightFences.Resize( maxConcurrentFrames );

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType	= VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags	= VK_FENCE_CREATE_SIGNALED_BIT;

		for( uint32_t i=0; i<m_MaxConcurrentFrames; ++i )
		{
			VK_CHECK_RESULT( vkCreateSemaphore( m_refDevice->Device(), &semaphoreInfo, nullptr, &m_PresentFinishedSemaphores[i] ) );
			VK_CHECK_RESULT( vkCreateSemaphore( m_refDevice->Device(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i] ) );
			VK_CHECK_RESULT( vkCreateFence( m_refDevice->Device(), &fenceInfo, nullptr, &inFlightFences[i] ) );
		}
	}



	void FrameSynchronizer::Release()
	{
		if( !m_refDevice.IsNull() && m_refDevice->Device() != VK_NULL_HANDLE )
		{

			for( uint32 i=0; i<m_MaxConcurrentFrames; ++i )
			{
				if( m_RenderFinishedSemaphores[i] != VK_NULL_HANDLE )
				{
					vkDestroySemaphore( m_refDevice->Device(), m_RenderFinishedSemaphores[i], nullptr );
					m_RenderFinishedSemaphores[i] = VK_NULL_HANDLE;
				}

				if( m_PresentFinishedSemaphores[i] != VK_NULL_HANDLE )
				{
					vkDestroySemaphore( m_refDevice->Device(), m_PresentFinishedSemaphores[i], nullptr );
					m_PresentFinishedSemaphores[i] = VK_NULL_HANDLE;
				}

				if( inFlightFences[i] != VK_NULL_HANDLE )
				{
					vkDestroyFence( m_refDevice->Device(), inFlightFences[i], nullptr );
					inFlightFences[i] = VK_NULL_HANDLE;
				}
			}

			m_RenderFinishedSemaphores.Release();
			m_PresentFinishedSemaphores.Release();
			inFlightFences.Release();

			m_MaxConcurrentFrames = 0;
			//m_refDevice.Reset();
		}
	}



	void FrameSynchronizer::WaitForCurrentFrame()
	{
		// グラフィックスキューにサブミットしたcurrentFrameジョブ完了してinFlightFencesがシグナル状態になるまでCPU待機する
		vkWaitForFences( m_refDevice->Device(), 1, &inFlightFences[ currentFrame ], VK_TRUE, std::numeric_limits<uint64_t>::max() );
		
	}


}// end of namespace vk