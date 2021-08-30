#ifndef GRAPHICS_DEVICE_H
#define GRAPHICS_DEVICE_H


#include	<windows.h>
#include	<iostream>
using namespace std;

#include	<GL/glew.h>
#include	<GL/wglew.h>
#include	<GL/glut.h>
#pragma comment(lib, "glew32.lib")


// OpenGLレンダリングコンテキストクラス（Windowsアプリケーションで必要）
class GraphicsDevice
{
private:

	HGLRC	m_glrc;	// レンダリングコンテキストのハンドル
	HDC		m_hDC;	// デバイスコンテキストのハンドル


public:

	GraphicsDevice();
	~GraphicsDevice();


	bool	CreateContext(HWND hWnd, int ColorBits, int DepthBits);
	void	CloseContext(HWND hWnd);
	
	HGLRC	GetRenderingContext();
	void	ShareRenderingContext(HGLRC rc);

	inline void Begin() { wglMakeCurrent(m_hDC, m_glrc); };
	inline void End()	{ wglMakeCurrent(NULL, NULL); };
	inline void swapBuffers() { SwapBuffers(m_hDC); }

};



#endif // GRAPHICS_DEVICE_H //