#include	"ViewGL.h"


ViewGL::ViewGL()
{
}


ViewGL::~ViewGL()
{
}


bool ViewGL::CreateContext(HWND hWnd, int ColorBits, int DepthBits, int StencilBits)
{
	//================ ピクセルフォーマットを決め，適正化チェックする =================//
	PIXELFORMATDESCRIPTOR	pfd = {0};
	pfd.nSize			= sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion		= 1;
	pfd.dwFlags			= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType		= PFD_TYPE_RGBA;
	pfd.cColorBits		= ColorBits;
	pfd.cDepthBits		= DepthBits;
	pfd.cStencilBits	= StencilBits;
	pfd.dwLayerMask		= PFD_MAIN_PLANE;

	m_hDC = GetDC(hWnd);

	//========================== ピクセルフォーマットを選択する =======================//
	int pixelformat = ChoosePixelFormat(m_hDC, &pfd);
	if(pixelformat==0) return false;

	//=============== デバイスコンテキストにピクセルフォーマットを設定する ============//
	BOOL bResult = SetPixelFormat(m_hDC, pixelformat, &pfd);
	if(bResult==FALSE) return false;
	
	//========================= レンダリングコンテキストを作成する ====================//
	m_glrc = wglCreateContext(m_hDC);
	if(m_glrc==0) return false;


	return true;
}


void ViewGL::CloseContext(HWND hWnd)
{
	if(!m_glrc || !m_hDC)	return;

	wglMakeCurrent(0, 0);
	wglDeleteContext(m_glrc);
	ReleaseDC(hWnd, m_hDC);

	hWnd	= NULL;
	m_glrc	= NULL;
}