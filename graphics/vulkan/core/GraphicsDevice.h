#ifndef GRAPHICS_DEVICE_H
#define	GRAPHICS_DEVICE_H

#include	<vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include	<GLFW/glfw3.h>

#include	<oreore/memory/ReferenceWrapper.h>
#include	<oreore/container/Array.h>



namespace vulkan
{

	struct QueueFamilyIndices
	{
		uint32_t	graphicsFamily;
		uint32_t	presentFamily;
		bool		graphicsFamilyHasValue = false;
		bool		presentFamilyHasValue = false;

		bool isComplete()	{ return graphicsFamilyHasValue && presentFamilyHasValue; }
	};




	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR				capabilities;
		OreOreLib::Array<VkSurfaceFormatKHR>	formats;
		OreOreLib::Array<VkPresentModeKHR>		presentModes;
	};




	class GraphicsDevice
	{
	public:

		GraphicsDevice();
		GraphicsDevice( GLFWwindow& window );
		GraphicsDevice( const GraphicsDevice& )=delete;
		~GraphicsDevice();

		void Init( GLFWwindow& window );
		void Release();


		VkSampleCountFlagBits GetMaxUsableSampleCount();
		SwapChainSupportDetails GetSwapChainSupport()	{ return QuerySwapChainSupport( m_PhysicalDevice ); }
		QueueFamilyIndices GetQueueFamilies()			{ return FindQueueFamilies( m_PhysicalDevice ); }


		const VkSurfaceKHR& Surface() const				{ return m_Surface; }
		const VkDevice& Device() const					{ return m_Device; }
		const VkPhysicalDevice& PhysicalDevice() const	{ return m_PhysicalDevice; }
		 
		const VkCommandPool& CommandPool() const		{ return m_CommandPool; }

		const VkQueue& GraphicsQueue() const	{ return m_GraphicsQueue; }
		const VkQueue& PresentQueue() const		{ return m_PresentQueue; }



	private:

		VkInstance					m_Instance;
		VkDebugUtilsMessengerEXT	m_DebugMessenger;
		VkSurfaceKHR				m_Surface;
		VkPhysicalDevice			m_PhysicalDevice;
		VkDevice					m_Device;

		VkCommandPool				m_CommandPool;
		VkQueue						m_GraphicsQueue;
		VkQueue						m_PresentQueue;

		OreOreLib::ReferenceWrapper<GLFWwindow>	m_refWindow;

		#ifdef NDEBUG
		const bool			m_bEnableValidationLayers = false;
		#else
		const bool			m_bEnableValidationLayers = true;
		#endif

		const OreOreLib::Array<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
		const OreOreLib::Array<const char*> m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };



		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateCommandPool();


		bool CheckValidationLayerSupport();
		OreOreLib::Array<const char*> GetRequiredExtensions();
		void PopulateDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT& createInfo );
		bool IsDeviceSuitable( VkPhysicalDevice physicalDevice );

		QueueFamilyIndices FindQueueFamilies( VkPhysicalDevice physicalDevice );
		bool CheckDeviceExtensionSupport( VkPhysicalDevice physicalDevice );
		SwapChainSupportDetails QuerySwapChainSupport( VkPhysicalDevice physicalDevice );

	};


	using GraphicsDeviceRef = OreOreLib::ReferenceWrapper<GraphicsDevice>;

}


#endif // !GRAPHICS_DEVICE_H
