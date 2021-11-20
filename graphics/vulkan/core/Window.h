#ifndef WINDOW_H
#define	WINDOW_H


#define GLFW_INCLUDE_VULKAN
#include	<GLFW/glfw3.h>

#include	<oreore/common/TString.h>



namespace vk
{

	class Window
	{
	public:

		Window( int w, int h, const charstring& name );
		~Window();

		Window( const Window& ) = delete;
		Window &operator=( const Window& ) = delete;


		bool shouldClose(){ return glfwWindowShouldClose(window); }
		VkExtent2D getExtent() { return { static_cast<uint32_t>(width),  static_cast<uint32_t>(height) }; }
		bool wasWindowResized(){ return framebufferResiszed; }
		void ResetWindowResizedFlag(){ framebufferResiszed=false; }


		void createWindowSurface( VkInstance instance, VkSurfaceKHR* surface );


	private:

		static void framebufferResizeCallback( GLFWwindow* window, int width, int height );

		GLFWwindow*	window;

		int width;
		int height;
		bool framebufferResiszed=false;

		const charstring windowName;

		void InitWindow();

	};


}// end of namespace vk



#endif // !WINDOW_H
