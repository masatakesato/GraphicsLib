#ifndef SWAP_CHAIN_H
#define	SWAP_CHAIN_H

#include	<vulkan/vulkan.h>

#include	<oreore/container/Array.h>



namespace vulkan
{

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR				capabilities;
		OreOreLib::Array<VkSurfaceFormatKHR>	formats;
		OreOreLib::Array<VkPresentModeKHR>		presentModes;
	};



	struct QueueFamilyIndices
	{
		uint32_t	graphicsFamily;
		uint32_t	presentFamily;
		bool		graphicsFamilyHasValue = false;
		bool		presentFamilyHasValue = false;

		bool isComplete()	{ return graphicsFamilyHasValue && presentFamilyHasValue; }
	};





	class SwapChain
	{
	public:

		SwapChain();
		SwapChain( VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface );
		SwapChain( const SwapChain& obj )=delete;
		~SwapChain();

		void Init( VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface );
		void Release();


		uint32_t NumImages() const		{ return static_cast<uint32_t>( m_Images.Length() ); }
		VkFormat ImageFormat() const	{ return m_ImageFormat; }
		VkImageView ImageView( int i ) const	{ return m_ImageViews[i]; }
		const VkExtent2D& Extent() const	{ return m_Extent;}

		VkSwapchainKHR Handle() const	{ return m_SwapChain; }


	private:

		VkDevice						m_refDevice;

		VkSwapchainKHR					m_SwapChain;
		VkExtent2D						m_Extent;

		VkFormat						m_ImageFormat;
		OreOreLib::Array<VkImage>		m_Images;
		OreOreLib::Array<VkImageView>	m_ImageViews;

		VkExtent2D						m_WindowExtent;


		void InitSwapChain( VkPhysicalDevice physicalDevice, VkSurfaceKHR surface );
		void InitImageViews();


		SwapChainSupportDetails QuerySwapChainSupport( VkPhysicalDevice device, VkSurfaceKHR surface );
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat( VkFormat format, VkColorSpaceKHR colorSpace, const OreOreLib::Array<VkSurfaceFormatKHR>& availableFormats );
		VkPresentModeKHR ChooseSwapPresentMode( VkPresentModeKHR presentmode, const OreOreLib::Array<VkPresentModeKHR>& availablePresentModes );
		VkExtent2D ChooseSwapExtent( const VkSurfaceCapabilitiesKHR& capabilities );

	};

}// end of namespace vulkan


#endif // !SWAP_CHAIN_H
