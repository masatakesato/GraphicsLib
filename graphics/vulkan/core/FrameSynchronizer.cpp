#include	"FrameSynchronizer.h"

#include	"../utils/HelperFunctions.h"



namespace vk
{

	FrameSynchronizer::FrameSynchronizer()
		: currentFrame( 0 )
	{

	}
	

	
	FrameSynchronizer::FrameSynchronizer( GraphicsDevice& device, int numswaps, uint32_t maxConcurrentFrames )
		: currentFrame( 0 )
	{
		Init( device, numswaps, maxConcurrentFrames );
	}
	

	
	FrameSynchronizer::~FrameSynchronizer()
	{


	}



	void FrameSynchronizer::Init( GraphicsDevice& device, int numswaps, uint32_t maxConcurrentFrames )
	{
		m_refDevice				= device;
		m_MaxConcurrentFrames	= maxConcurrentFrames;

		imageAvailableSemaphores.Resize( (int)maxConcurrentFrames );
		renderFinishedSemaphores.Resize( (int)maxConcurrentFrames );
		inFlightFences.Resize( maxConcurrentFrames );
//		imagesInFlight.Resize( numswaps, VK_NULL_HANDLE );

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



	void FrameSynchronizer::WaitForCurrentFrame()
	{
		// グラフィックスキューにサブミットしたcurrentFrameジョブ完了してinFlightFencesがシグナル状態になるまでCPU待機する
		vkWaitForFences( m_refDevice->Device(), 1, &inFlightFences[ currentFrame ], VK_TRUE, std::numeric_limits<uint64_t>::max() );
		
	}


}// end of namespace vk