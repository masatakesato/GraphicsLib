#include "GraphicsDevice.h"

#include	<oreore/common/TString.h>



GraphicsDevice::GraphicsDevice()
{
tcout << _T("GraphicsDevice::GraphicsDevice()...") << tendl;
	m_glrc	= NULL;
	m_hDC	= NULL;
}



GraphicsDevice::~GraphicsDevice()
{
	//m_hDC	= NULL;	// デバイスコンテキストをNULLにする
	//wglDeleteContext(m_glrc);// レンダリングコンテキストを削除する
}



// レンダリングコンテキストの初期化
bool GraphicsDevice::CreateContext(HWND hWnd, int ColorBits, int DepthBits)
{
tcout << _T("GraphicsDevice::CreateContext()...") << tendl;

	BOOL	bResult;
	
	PIXELFORMATDESCRIPTOR	pfd = {0};
	pfd.nSize		= sizeof(PIXELFORMATDESCRIPTOR);// サイズ
	pfd.nVersion	= 1;
	pfd.dwFlags		= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType	= PFD_TYPE_RGBA;
	pfd.cColorBits	= ColorBits;			// カラービット数
	pfd.cDepthBits	= DepthBits;			// デプスビット数
	pfd.dwLayerMask	= PFD_MAIN_PLANE;		//レイヤータイプ．WindowsではPFD_MAIN_PLANEのみ指定可．
	
	m_hDC = GetDC(hWnd);// デバイスコンテキストを取得する

	int pixelformat = ChoosePixelFormat(m_hDC, &pfd);
	if(pixelformat==0)
		return false;

	bResult = SetPixelFormat(m_hDC, pixelformat, &pfd);
	if(bResult==FALSE)
		return false;

	//================ レンダリングコンテキストを作成 ==============//
	m_glrc = wglCreateContext(m_hDC);
	if(m_glrc == NULL)
		return false;


	//================ レンダリングコンテキスト初期化 ==============//
	wglMakeCurrent(m_hDC, m_glrc);
	{
		glewInit();
	}
	wglMakeCurrent(NULL, NULL);


	//ReleaseDC(hWnd, m_hDC);
	
	return true;

}



void GraphicsDevice::CloseContext(HWND hWnd)
{
	if(m_hDC)
	{
		ReleaseDC(hWnd, m_hDC);
		m_hDC = NULL;
	}

	if(m_glrc)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_glrc);
		m_glrc = NULL;
	}
}



HGLRC GraphicsDevice::GetRenderingContext()
{
	HGLRC	rc = m_glrc;
	return rc;
}



void GraphicsDevice::ShareRenderingContext(HGLRC rc)
{
tcout << _T("GraphicsDevice::ShareRenderingContext()...") << tendl;
	wglShareLists(rc, m_glrc);

}