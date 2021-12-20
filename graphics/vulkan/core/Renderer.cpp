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







	//void Renderer::PrepareFrame()
	//{
	//	VkResult result = m_SwapChain.AquireNextImage( imageIndex );

	//	if( result == VK_ERROR_OUT_OF_DATE_KHR )// Surfaceに変更が加わって、スワップチェーンが合わなくなりましたよフラグ
	//	{
	//		RecreateSwapChain();
	//		return;
	//	}
	//	else if( result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR )
	//	{
	//		VK_CHECK_RESULT( result );//throw std::runtime_error( "failed to acquire swap chain image!" );
	//	}
	//}



	//void Renderer::SubmitFrame()
	//{
	//	VkResult result = m_SwapChain.QueuePresent( imageIndex );
	//	
	//	if( result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_refWindow->Resized() )
	//	{
	//		m_refWindow->ResetWindowResizedFlag();
	//		RecreateSwapChain();
	//	}
	//	else if( result != VK_SUCCESS )
	//	{
	//		VK_CHECK_RESULT( result );//throw std::runtime_error( "failed to present swap chain image!" );
	//	}
	//}


}// end of namespace vk