#ifndef SWAP_CHAIN_H
#define	SWAP_CHAIN_H

#include	"GraphicsDevice.h"



namespace vulkan
{

	class SwapChain
	{
	public:

		SwapChain();
		SwapChain( GraphicsDevice& device, VkExtent2D extent );
		SwapChain( const SwapChain& obj )=delete;
		~SwapChain();

		void Init( GraphicsDevice& device, VkExtent2D extent );
		void Release();


		uint32_t NumImages() const		{ return static_cast<uint32_t>( m_Images.Length() ); }
		VkFormat ImageFormat() const	{ return m_ImageFormat; }
		VkImageView ImageView( int i ) const	{ return m_ImageViews[i]; }
		const VkExtent2D& Extent() const	{ return m_WindowExtent;}

		VkSwapchainKHR Handle() const	{ return m_SwapChain; }


	private:

		GraphicsDeviceRef				m_refDevice;
		VkExtent2D						m_WindowExtent;

		VkSwapchainKHR					m_SwapChain;
		VkExtent2D						m_SwapChainExtent;

		VkFormat						m_ImageFormat;


		OreOreLib::Array<VkImage>		m_Images;
		OreOreLib::Array<VkImageView>	m_ImageViews;
		

		void InitSwapChain();
		void InitImageViews();

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat( VkFormat format, VkColorSpaceKHR colorSpace, const OreOreLib::Array<VkSurfaceFormatKHR>& availableFormats );
		VkPresentModeKHR ChooseSwapPresentMode( VkPresentModeKHR presentmode, const OreOreLib::Array<VkPresentModeKHR>& availablePresentModes );
		VkExtent2D ChooseSwapExtent( const VkSurfaceCapabilitiesKHR& capabilities );

	};

}// end of namespace vulkan


#endif // !SWAP_CHAIN_H
