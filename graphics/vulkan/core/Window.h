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


		void Release();

		bool ShouldClose(){ return glfwWindowShouldClose(window); }
		const VkExtent2D& Extent() const { return m_Extent; }
		//bool IsValidExtent() const	{ return m_Extent.width>0 && m_Extent.height>0; }
		bool Resized() const	{ return framebufferResiszed; }
		void ResetWindowResizedFlag(){ framebufferResiszed=false; }

		void CreateWindowSurface( VkInstance instance, VkSurfaceKHR* surface );

		void WaitForRaise() const;


	private:

		static void framebufferResizeCallback( GLFWwindow* window, int width, int height );

		GLFWwindow*	window;
		const charstring windowName;
		VkExtent2D	m_Extent;

		bool framebufferResiszed=false;


		void InitWindow();

	};


}// end of namespace vk



#endif // !WINDOW_H
