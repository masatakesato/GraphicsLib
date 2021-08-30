#ifndef	CONTROLLER_H
#define	CONTROLLER_H

#include	<windows.h>


class Controller
{
public:
	
	Controller(){ m_hWnd = NULL;	for(int i=0; i<MAX_KEYS; i++) m_Keys[i] = false; }
	virtual ~Controller(){ ::DestroyWindow( m_hWnd ); }
	void SetHandle(HWND handle){ m_hWnd = handle; }

	unsigned virtual Close()							{ DestroyWindow(m_hWnd); return 0; }
	unsigned virtual Destroy()							{ return 0; }


	// クライアント領域設定
	unsigned virtual SetClientSize(int width, int heght){ return 0; }

	// スレッド操作
	unsigned virtual Create()							{ return 0; }
	unsigned virtual Start()							{ return 0; }
	unsigned virtual Pause()							{ return 0; }
	unsigned virtual Stop()								{ return 0; }

	// キー入力イベント
	unsigned virtual keyDown(WPARAM wParam)				{ return 0; }	// キー押込(for WM_KEYDOWN): 
	unsigned virtual keyUp(WPARAM wParam)				{ return 0; }	// キー解放(for WM_KEYUP)

	// マウスイベント
	unsigned virtual lButtonDown(LPARAM lParam)			{ return 0; }	// マウス左ボタン押込(for WM_LBUTTONDOWN)
	unsigned virtual lButtonUp(LPARAM lParam)			{ return 0; }	// マウス左ボタン解放(for WM_LBUTTONUP)
	unsigned virtual rButtonDown(LPARAM lParam)			{ return 0; }	// マウス右ボタン押込(for WM_RBUTTONDOWN)
	unsigned virtual rButtonUp(LPARAM lParam)			{ return 0; }	// マウス右ボタン押込(for WM_RBUTTONUP)
	unsigned virtual mouseMove(LPARAM lParam)			{ return 0; }	// マウスカーソル移動(for WM_MOUSEMOVE)


protected:

	// ハンドル
	HWND	m_hWnd;

	// 入力状態
	enum	{ MAX_KEYS = 256 };
	bool	m_Keys[MAX_KEYS];			// キーボード入力状態
	bool	m_LeftMouseButtonPressed;	// マウス左ボタン状態
	bool	m_RightMouseButtonPressed;	// マウス右ボタン状態
	int		m_MouseX, m_MouseY;			// マウスカーソル位置
	float	dx, dy;						// カーソル移動量

};



#endif	// CONTROLLER_H //