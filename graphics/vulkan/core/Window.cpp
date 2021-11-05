#include	"Window.h"

#include	<stdexcept>



namespace vulkan
{

	Window::Window( int w, int h, std::string name )
		: width(w)
		, height(h)
		, windowName( name )
	{
		InitWindow();
	}



	Window::~Window()
	{
		glfwDestroyWindow( window );
		glfwTerminate();
	}



	void Window::InitWindow()
	{
		glfwInit();
		glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );// disable opengl context creation (glfw is oridinally designed for OpenGL)
		glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );// tutorial 10 win cover manual resizing

		window = glfwCreateWindow( width, height, windowName.c_str(), nullptr, nullptr );
		glfwSetWindowUserPointer( window, this );
		glfwSetFramebufferSizeCallback( window, framebufferResizeCallback );

	}



	void Window::createWindowSurface( VkInstance instance, VkSurfaceKHR* surface )
	{
		if( glfwCreateWindowSurface( instance, window, nullptr, surface ) != VK_SUCCESS )
		{
			throw std::runtime_error("failed to create window surface");
		}
	}

		
	void Window::framebufferResizeCallback( GLFWwindow* window, int width, int height )
	{
		auto lveWindow = reinterpret_cast<Window*>( glfwGetWindowUserPointer(window) );
		lveWindow->framebufferResiszed = true;

		lveWindow->width = width;
		lveWindow->height = height;


	}


}// end of namespace vulkan