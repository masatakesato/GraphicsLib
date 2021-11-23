#include	"Window.h"

#include	"../utils/HelperFunctions.h"



namespace vk
{

	Window::Window( int w, int h, const charstring& name )
		: m_Extent{ static_cast<uint32_t>(w),  static_cast<uint32_t>(h) }
		, windowName( name )
	{
		InitWindow();
	}



	Window::~Window()
	{
		Release();
		//glfwTerminate();
	}



	void Window::InitWindow()
	{
		glfwInit();
		glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );// disable opengl context creation (glfw is oridinally designed for OpenGL)
		glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );// tutorial 10 win cover manual resizing

		window = glfwCreateWindow( static_cast<int32>(m_Extent.width), static_cast<int32>(m_Extent.height), windowName.c_str(), nullptr, nullptr );
		glfwSetWindowUserPointer( window, this );
		glfwSetFramebufferSizeCallback( window, framebufferResizeCallback );

	}



	void Window::Release()
	{
		if( window )
		{
			glfwDestroyWindow( window );
			window = nullptr;
		}
		glfwTerminate();// デストラクタで実行した方がいい?
	}



	void Window::CreateWindowSurface( VkInstance instance, VkSurfaceKHR* surface )
	{
		VK_CHECK_RESULT( glfwCreateWindowSurface( instance, window, nullptr, surface ) );
	}



	void Window::WaitForRaise() const
	{
		while( m_Extent.width==0 || m_Extent.height==0 )
			glfwWaitEvents();
	}


		
	void Window::framebufferResizeCallback( GLFWwindow* window, int width, int height )
	{
		auto pWindow = reinterpret_cast<Window*>( glfwGetWindowUserPointer(window) );
		pWindow->framebufferResiszed = true;

		pWindow->m_Extent.width = static_cast<uint32_t>(width);
		pWindow->m_Extent.height = static_cast<uint32_t>(height);
		//lveWindow->width = width;
		//lveWindow->height = height;
	}


}// end of namespace vk