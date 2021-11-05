#include	"GraphicsDevice.h"

#include	<oreore/container/Set.h>




namespace vulkan
{

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(	VkDebugUtilsMessageSeverityFlagBitsEXT messageSerevity,
															VkDebugUtilsMessageTypeFlagsEXT messageType,
															const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
															void* pUserData )
	{
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}



	VkResult CreateDebugUtilsMessengerEXT(	VkInstance instance,
											const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
											const VkAllocationCallbacks* pAllocator,
											VkDebugUtilsMessengerEXT* pDebugMessenger )
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr( instance, "vkCreateDebugUtilsMessengerEXT" );
		if( func != nullptr )
			return func( instance, pCreateInfo, pAllocator, pDebugMessenger );
		else
			return VK_ERROR_EXTENSION_NOT_PRESENT;
	}



	void DestroyDebugUtilsMessengerEXT( VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator )
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr( instance, "vkDestroyDebugUtilsMessengerEXT" );
		if( func != nullptr )
			func( instance, debugMessenger, pAllocator );
	}




	GraphicsDevice::GraphicsDevice( GLFWwindow&	window )
		: m_refWindow( window )
	{

	}



	GraphicsDevice::~GraphicsDevice()
	{

	}






	VkSampleCountFlagBits GraphicsDevice::GetMaxUsableSampleCount()
	{
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties( m_PhysicalDevice, &physicalDeviceProperties );

		VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
									physicalDeviceProperties.limits.framebufferDepthSampleCounts;

		if( counts & VK_SAMPLE_COUNT_64_BIT )	return VK_SAMPLE_COUNT_64_BIT;
		if( counts & VK_SAMPLE_COUNT_32_BIT )	return VK_SAMPLE_COUNT_32_BIT;
		if( counts & VK_SAMPLE_COUNT_16_BIT )	return VK_SAMPLE_COUNT_16_BIT;
		if( counts & VK_SAMPLE_COUNT_8_BIT )	return VK_SAMPLE_COUNT_8_BIT;
		if( counts & VK_SAMPLE_COUNT_4_BIT )	return VK_SAMPLE_COUNT_4_BIT;
		if( counts & VK_SAMPLE_COUNT_2_BIT )	return VK_SAMPLE_COUNT_2_BIT;

		return VK_SAMPLE_COUNT_1_BIT;
	}









	void GraphicsDevice::CreateInstance()
	{
		if( m_bEnableValidationLayers && !CheckValidationLayerSupport() )
			throw std::runtime_error( "validation layers requested, but not available!" );


		VkApplicationInfo appInfo ={};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION( 1, 0, 0 );
		appInfo.apiVersion = VK_API_VERSION_1_0;


		VkInstanceCreateInfo createInfo ={};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;


		auto extensions = GetRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>( extensions.Length() );
		createInfo.ppEnabledExtensionNames = extensions.begin();


		for( const auto& extension : extensions )
			std::cout << "\t" << extension << std::endl;

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;

		if( m_bEnableValidationLayers )
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>( m_ValidationLayers.Length() );
			createInfo.ppEnabledLayerNames = m_ValidationLayers.begin();

			PopulateDebugMessengerCreateInfo( debugCreateInfo );
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		if( vkCreateInstance( &createInfo, nullptr, &m_Instance ) != VK_SUCCESS )
		{
			throw std::runtime_error( "failed to create instance!" );
		}

	}



	void GraphicsDevice::SetupDebugMessenger()
	{
		if( !m_bEnableValidationLayers )	return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		PopulateDebugMessengerCreateInfo( createInfo );

		if( CreateDebugUtilsMessengerEXT( m_Instance, &createInfo, nullptr, &m_DebugMessenger ) != VK_SUCCESS )
			throw std::runtime_error( "failed to set up debug messenger!" );

	}



	void GraphicsDevice::CreateSurface()
	{
		if( glfwCreateWindowSurface( m_Instance, &m_refWindow, nullptr, &m_Surface) != VK_SUCCESS )
			throw std::runtime_error( "failed to create window surface!" );
	}



	void GraphicsDevice::PickPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices( m_Instance, &deviceCount, nullptr );

		if( deviceCount == 0 )
			throw std::runtime_error( "failed to find GPUs with Vulkan support!" );

		OreOreLib::Array<VkPhysicalDevice> devices( deviceCount );
		vkEnumeratePhysicalDevices( m_Instance, &deviceCount, devices.begin() );

		for( const auto& device : devices )
		{
			if( IsDeviceSuitable( device ) )
			{
				m_PhysicalDevice = device;
				break;
			}
		}

		if( m_PhysicalDevice == VK_NULL_HANDLE )
			throw std::runtime_error( "failed to find a suitable GPU!" );
	}



	void GraphicsDevice::CreateLogicalDevice()
	{
		QueueFamilyIndices indices = FindQueueFamilies( m_PhysicalDevice );

		OreOreLib::Array<VkDeviceQueueCreateInfo> queueCreateInfos;
		OreOreLib::Set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

		float queuePriority = 1.0f;
		for( uint32_t queueFamily : uniqueQueueFamilies )
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType				= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex	= queueFamily;
			queueCreateInfo.queueCount			= 1;
			queueCreateInfo.pQueuePriorities	= &queuePriority;

			queueCreateInfos.AddToTail( queueCreateInfo );
		}


		VkPhysicalDeviceFeatures deviceFeatures = {};

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount		= static_cast<uint32_t>( queueCreateInfos.Length() );
		createInfo.pQueueCreateInfos		= queueCreateInfos.begin();

		createInfo.pEnabledFeatures			= &deviceFeatures;
		createInfo.enabledExtensionCount	= static_cast<uint32_t>( m_DeviceExtensions.Length() );
		createInfo.ppEnabledExtensionNames	= m_DeviceExtensions.begin();

		if( m_bEnableValidationLayers )
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>( m_ValidationLayers.Length() );
			createInfo.ppEnabledLayerNames = m_ValidationLayers.begin();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		if( vkCreateDevice( m_PhysicalDevice, &createInfo, nullptr, &m_Device ) != VK_SUCCESS )
			throw std::runtime_error( "failed to create logical device" );

		vkGetDeviceQueue( m_Device, indices.graphicsFamily, 0, &m_GraphicsQueue );
		vkGetDeviceQueue( m_Device, indices.presentFamily, 0, &m_PresentQueue );
	}





	//##############################################################################//
	//																				//
	//							Private helper functions							//
	//																				//
	//##############################################################################//

	bool GraphicsDevice::CheckValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties( &layerCount, nullptr );

		OreOreLib::Array<VkLayerProperties> availableLayers( layerCount );
		vkEnumerateInstanceLayerProperties( &layerCount, availableLayers.begin() );


		for( const char* layerName : m_ValidationLayers )
		{
			bool layerFound = false;

			for( const auto& layerProperties: availableLayers )
			{
				if( strcmp( layerName, layerProperties.layerName )==0 )
				{
					layerFound = true;
					break;
				}
			}

			if( !layerFound )
			{
				return false;
			}
		}

		return true;
	}



	OreOreLib::Array<const char*> GraphicsDevice::GetRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount );

		OreOreLib::Array<const char*> extensions( glfwExtensions, glfwExtensions + glfwExtensionCount );

		if( m_bEnableValidationLayers )
			extensions.AddToTail( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );

		return extensions;
	}



	void GraphicsDevice::PopulateDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT& createInfo )
	{
		createInfo ={};
		createInfo.sType			= VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

		createInfo.messageSeverity	=	VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
										VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
										VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

		createInfo.messageType	=	VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
									VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
									VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

		createInfo.pfnUserCallback = debugCallback;
	}



	bool GraphicsDevice::IsDeviceSuitable( VkPhysicalDevice physicalDevice )
	{
		QueueFamilyIndices indices = FindQueueFamilies( physicalDevice );

		// extensionsSupported
		bool extensionsSupported = CheckDeviceExtensionSupport( physicalDevice );

		// swapChainAdequate
		bool swapChainAdequate = false;
		if( extensionsSupported )
		{
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport( physicalDevice );
			swapChainAdequate = !swapChainSupport.formats.Empty() && !swapChainSupport.presentModes.Empty();
		}

		// Anisotropic filtering
		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures( physicalDevice, &supportedFeatures );


		return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
	}



	QueueFamilyIndices GraphicsDevice::FindQueueFamilies( VkPhysicalDevice physicalDevice )
	{
		QueueFamilyIndices indices;

		// Aqcuire Queuefamily property list.
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice, &queueFamilyCount, nullptr );

		OreOreLib::Array<VkQueueFamilyProperties> queueFamilies( queueFamilyCount );
		vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice, &queueFamilyCount, queueFamilies.begin() );


		// Find 
		int i=0;
		for( const auto& queueFamily : queueFamilies )
		{
			if( queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT )
				indices.graphicsFamily = i;

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR( physicalDevice, i, m_Surface, &presentSupport );

			if( queueFamily.queueCount > 0 && presentSupport )
				indices.presentFamily = i;

			if( indices.isComplete() )
				break;

			i++;
		}


		return indices;
	}



	bool GraphicsDevice::CheckDeviceExtensionSupport( VkPhysicalDevice physicalDevice )
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties( physicalDevice, nullptr, &extensionCount, nullptr );

		OreOreLib::Array<VkExtensionProperties> availableExtensions( extensionCount );
		vkEnumerateDeviceExtensionProperties( physicalDevice, nullptr, &extensionCount, availableExtensions.begin() );

		OreOreLib::Set<std::string> requiredExtesions( m_DeviceExtensions.begin(), m_DeviceExtensions.end() );

		for( const auto& extension : availableExtensions )
			requiredExtesions.Remove( extension.extensionName );

		return requiredExtesions.Empty();
	}



	SwapChainSupportDetails GraphicsDevice::QuerySwapChainSupport( VkPhysicalDevice physicalDevice )
	{
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR( physicalDevice, m_Surface, &details.capabilities );

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, m_Surface, &formatCount, nullptr );
		if( formatCount != 0 )
		{
			details.formats.Init( formatCount );
			vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, m_Surface, &formatCount, details.formats.begin() );
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice, m_Surface, &presentModeCount, nullptr );
		if( presentModeCount != 0 )
		{
			details.presentModes.Init( presentModeCount );
			vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice, m_Surface, &presentModeCount, details.presentModes.begin() );
		}

		return details;
	}


}// end of namespace vulkan