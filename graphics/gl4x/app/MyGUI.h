#ifndef MY_GUI_H
#define	MY_GUI_H

#include	<windows.h>

#include	<oreore/mathlib/GraphicsMath.h>



class MyGUI
{
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


	static const int TextHeight;
	static const int ButtonHeight;
	static const int CheckBoxHeight;
	static const int ComboBoxHeight;
	static const int HorizontalSliderHeight;
	static const int VerticalSpacing;



private:

	int		m_ScreenWidth;// スクリーンの幅
	int		m_ScreenHeight;// スクリーンの高さ

	unsigned int m_fontBase;
	bool	m_State;// GUIの使用状態
	int		m_CursorX, m_CursorY;// マウスカーソルの座標
	int		m_MouseDownX, m_MouseDownY;// マウスボタンを押し込んだ時のカーソル座標
	int		m_MouseUpX, m_MouseUpY;// マウスボタンを離した時のカーソル座標

	bool	m_LeftMouseButtonPressed;// ボタンクリック
	

 
	static const int	SLIDER_SIZE;
	static const int	CHECKBOX_SIZE;

	// UI color states
	enum GUI_COLOR { COLOR_DEFAULT, COLOR_OVERLAP=1, COLOR_HOLD=2, COLOR_ON=3 }; 

	// UI color styles
	static const Vec4f color_string[2];
	static const Vec4f color_frame[2];
	static const Vec4f color_button[3];
	static const Vec4f color_checkbox[4];
	static const Vec4f color_listbox[4];
	static const Vec4f color_slider[2];



	bool CheckRange(float val, float val_min, float val_max);
	void DrawButton(int x, int y, int width, int height, char *text, int state);
	void DrawCheckBox(int x, int y, char *text, int status);
	void DrawHorizontalSlider(int x, int y, int width, char *text, float val, float val_min, float val_max, int status);
	void DrawListBox(int x, int y, int width, int numAttr, char *Attributes[], int id_selected, bool buttondown);

	void DrawDropDownIcon( float x, float y, float width, float height );
	void DrawFrame( float x, float y, float width, float height, GUI_COLOR col );





};



#endif // !MY_GUI_H