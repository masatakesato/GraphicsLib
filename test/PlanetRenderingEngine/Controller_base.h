#ifndef CONTROLLER_BASE_H
#define CONTROLLER_BASE_H


// 入力処理の基本クラス
#include	<windows.h>



#define MAX_KEYS 256




class Controller_base
{
private:

protected:
	
	// ハンドル
	HWND	m_hWnd;

	// 入力状態
	bool	m_Keys[MAX_KEYS];			// キーボード入力状態
	bool	m_LeftMouseButtonPressed;	// マウス左ボタン状態
	bool	m_RightMouseButtonPressed;	// マウス右ボタン状態
	int		m_MouseX, m_MouseY;			// マウスカーソル位置
	float	dx, dy;						// カーソル移動量

public:
	
	Controller_base(){ m_hWnd = NULL; };
	void SetHandle(HWND handle){ m_hWnd = handle; };


	unsigned virtual keyDown(WPARAM wParam)	{ return 0; };	// for WM_KEYDOWN: keyCode, detailInfo
	unsigned virtual keyUp(WPARAM wParam)	{ return 0; };	// for WM_KEYUP: keyCode, detailInfo

	unsigned virtual lButtonDown(LPARAM lParam)		{ return 0; };	// for WM_LBUTTONDOWN: state, x, y
	unsigned virtual lButtonUp(LPARAM lParam)		{ return 0; };	// for WM_LBUTTONUP: state, x, y
	unsigned virtual rButtonDown(LPARAM lParam)		{ return 0; };	// for WM_RBUTTONDOWN: state, x, y
	unsigned virtual rButtonUp(LPARAM lParam)		{ return 0; };	// for WM_RBUTTONUP: state, x, y
	unsigned virtual mouseMove(LPARAM lParam)		{ return 0; };	// カーソル移動
};






#endif // CONTROLLER_H //