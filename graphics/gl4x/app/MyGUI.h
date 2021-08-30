#pragma once


#include	<windows.h>




class MyGUI
{
private:

	int		m_ScreenWidth;// スクリーンの幅
	int		m_ScreenHeight;// スクリーンの高さ

	unsigned int m_fontBase;
	bool	m_State;// GUIの使用状態
	int		m_CursorX, m_CursorY;// マウスカーソルの座標
	int		m_MouseDownX, m_MouseDownY;// マウスボタンを押し込んだ時のカーソル座標
	int		m_MouseUpX, m_MouseUpY;// マウスボタンを離した時のカーソル座標

	bool	m_LeftMouseButtonPressed;// ボタンクリック
	

	bool CheckRange(float val, float val_min, float val_max);
	void DrawButton(int x, int y, int width, int height, char *text, int state);
	void DrawCheckBox(int x, int y, char *text, int status);
	void DrawHorizontalSlider(int x, int y, int width, char *text, float val, float val_min, float val_max, int status);
	void DrawListBox(int x, int y, int width, int numAttr, char *Attributes[], int id_selected, bool buttondown);


public:
	
	MyGUI();
	~MyGUI();
	
	void InitFontDisplayList(HDC hdc);
	void SetScreenResolution(int width, int height);
	void mouseMove(int x, int y);
	void lButtonDown(int x, int y);
	void lButtonUp(int x, int y);

	void Begin();
	void End();
	

	void DoText(int x, int y, char *text);// テキストを描画する
	void DoButton(int x, int y, int width, int height, char *text, bool &state);// ボタンを描画する
	void DoCheckBox(int x, int y, char *text, bool &state);// チェックボックスを描画する
	void DoHorizontalSlider(int x, int y, int width, char *text, float val_min, float val_max, float &val, int numbins = 0);
	
	void DoListBox(int x, int y, int width, int numAttr, char *Attributes[], int &idx_curr, bool &showListBox);
	void DoComboBox(int x, int y, int width, int numAttr, char *Attributes[], int &idx_curr, bool &showListBox, char *caption=NULL);

	bool IsActive(){ return m_State;	};

};