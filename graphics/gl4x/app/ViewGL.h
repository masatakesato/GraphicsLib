#ifndef	VIEW_H
#define	VIEW_H


#include	<windows.h>
#pragma comment(lib, "gdi32.lib")	// ChoosePixelFormat, SetPixelFormat
#pragma comment(lib, "User32.lib")	// GetDC, ReleaseDC
#pragma comment(lib, "Opengl32.lib")// wgl**


class ViewGL
{
private:

	HDC		m_hDC;	// デバイスコンテキストへのハンドル
	HGLRC	m_glrc;	// レンダリングコンテキスト


public:

	ViewGL();
	~ViewGL();

	bool	CreateContext(HWND hWnd, int ColorBits, int DepthBits, int StencilBits);
	void	CloseContext(HWND hWnd);

	HDC		GetHDC() { return m_hDC; };
	HGLRC	GetHRC() { return m_glrc; };

	void	swapBuffers() { SwapBuffers(m_hDC); };

};




#endif	// VIEW_H //