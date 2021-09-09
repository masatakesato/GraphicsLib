#include	"MyGUI.h"

#include	<math.h>
#include	<iostream>
using namespace std;

#include	"../other/MyGlPrimitive.h"
using namespace MyGlPrimitive;



const int MyGUI::TextHeight				= 20;
const int MyGUI::ButtonHeight			= 20;
const int MyGUI::CheckBoxHeight			= 20;
const int MyGUI::ComboBoxHeight			= 20;
const int MyGUI::HorizontalSliderHeight	= 30;
const int MyGUI::VerticalSpacing		= 20;

const int MyGUI::SLIDER_SIZE			= 10;
const int MyGUI::CHECKBOX_SIZE			= 14;




// text color
const Vec4f MyGUI::color_string[2] =
{
	{ 1.0f, 1.0f, 1.0f, 0.0f },
	{ 0.3f, 0.3f, 0.3f, 0.0f },
};


// frame color
const Vec4f MyGUI::color_frame[2] = 
{
	{ 0.5f, 0.5f, 0.5f, 0.5f },
	{ 0.5f, 0.7f, 0.5f, 0.5f },
};


// buton color
const Vec4f MyGUI::color_button[3] =
{
	{ 0.1f, 0.1f, 0.1f, 0.5f },// デフォルト色
	{ 0.3f, 0.3f, 0.3f, 0.5f },// カーソルオーバーラップ
	{ 0.1f, 0.1f, 0.1f, 0.5f },// ボタン押し込み
};


// チェックボックス
const Vec4f MyGUI::color_checkbox[4] =
{
	{ 0.1f, 0.0f, 0.0f, 0.5f },// デフォルト（オフ）
	{ 0.3f, 0.0f, 0.0f, 0.5f },// オーバーラップ
	{ 0.2f, 0.0f, 0.0f, 0.5f },// ボタン押し込み
	{ 0.7f, 1.0f, 0.7f, 0.5f },// オン
};


// リストボックス
const Vec4f MyGUI::color_listbox[4] = 
{
	{ 0.1f, 0.1f, 0.1f, 0.5f },// デフォルト
	{ 0.5f, 0.0f, 0.0f, 0.5f },// オーバーラップ
	{ 0.5f, 0.0f, 0.0f, 0.5f },// ボタン押し込み
	{ 0.5f, 0.7f, 0.5f, 0.5f },// 選択
};


// スライダーつまみ
const Vec4f MyGUI::color_slider[2] =
{
	{ 0.2f, 0.3f, 0.0f, 0.5f },
	{ 0.35f, 0.5f, 0.0f, 0.5f },
};





void MyGUI::DrawDropDownIcon( float x, float y, float width, float height )
{
	float	ColorArray[12] =
	{
		0.2f, 0.3f, 0.2f, 0.0f,// 頂点0
		0.5f, 0.7f, 0.5f, 0.7f,// 頂点1
		0.2f, 0.3f, 0.2f, 0.0f,// 頂点2
	};
	
	float	VertexArray[6] = 
	{
		x,				y+height,	// 頂点0
		x+width*0.5f,	y,			// 頂点1
		x+width,		y+height,	// 頂点2
	};

	GLuint	index[3] = { 0,1,2 };

	glLineWidth(2.5f);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	
	glColorPointer(4, GL_FLOAT, 0, ColorArray);
	glVertexPointer(2, GL_FLOAT, 0, VertexArray);
	glDrawElements(GL_LINE_STRIP, 3, GL_UNSIGNED_INT, index);//glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, index);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glLineWidth(1.0f);
}



void MyGUI::DrawFrame( float x, float y, float width, float height, GUI_COLOR col )
{
	float ColorArray[16] =
	{
		color_frame[col].r, color_frame[col].g, color_frame[col].b, color_frame[col].a,// 頂点0
		color_frame[col].r, color_frame[col].g, color_frame[col].b, color_frame[col].a,// 頂点1
		color_frame[col].r, color_frame[col].g, color_frame[col].b, color_frame[col].a,// 頂点2
		color_frame[col].r, color_frame[col].g, color_frame[col].b, color_frame[col].a,// 頂点3
	};
	
	float VertexArray[8] = 
	{
		x,			y,			// 頂点0
		x,			y+height,	// 頂点1
		x+width,	y+height,	// 頂点2
		x+width,	y,			// 頂点3
	};

	GLuint	index[4] = { 0,1,2,3, };
	
	glLineWidth(1.5f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glColorPointer(4, GL_FLOAT, 0, ColorArray);
	glVertexPointer(2, GL_FLOAT, 0, VertexArray);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, index);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	glLineWidth(1.0f);
}





MyGUI::MyGUI()
{
	m_ScreenWidth = 0;
	m_ScreenHeight = 0;
	m_fontBase = NULL;

	m_LeftMouseButtonPressed = false;
	m_State = false;
}

MyGUI::~MyGUI()
{
	if(m_fontBase)	glDeleteLists(m_fontBase, 96);
}

//　デバイスコンテキストハンドルを使ってフォントのディスプレイリストを初期化する
void MyGUI::InitFontDisplayList(HDC hdc)
{
//tcout << _T("MyGUI::InitFontDisplayList()...") << tendl;

	HFONT font;
	HFONT oldfont;

	//　リストを作成
	m_fontBase = glGenLists(96);

	//　フォントを作成
	font = CreateFontA( 
		15, 
		0,
		0,
		0,
		FW_NORMAL,
		FALSE,
		FALSE,
		FALSE,
		ANSI_CHARSET,
		OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		FF_DONTCARE | DEFAULT_PITCH,
		"Arial" );

	//　
	oldfont = (HFONT)SelectObject(hdc, font);
	//　ビットマップフォント
	wglUseFontBitmapsA(hdc, 32, 96, m_fontBase);
	//
	SelectObject(hdc, oldfont);
	//
	DeleteObject(font);
}





void MyGUI::SetScreenResolution(int width, int height)
{
//tcout << _T("MyGUI::SetScreenResolution()...") << tendl;
	m_ScreenWidth = width;
	m_ScreenHeight = height;

//tcout << _T("ScreenWidth = ") << m_ScreenWidth << tendl;
//tcout << _T("ScreenHeight = ") << m_ScreenHeight << tendl;
}




void MyGUI::lButtonDown(int x, int y)
{
	m_LeftMouseButtonPressed = true;
	m_MouseDownX = x;
	m_MouseDownY = m_ScreenHeight - y;
}


void MyGUI::lButtonUp(int x, int y)
{
	m_LeftMouseButtonPressed = false;
	m_MouseUpX = x;
	m_MouseUpY = m_ScreenHeight - y;
}


void MyGUI::mouseMove(int x, int y)
{
	m_CursorX = x;
	m_CursorY = m_ScreenHeight - y;
}




//bool MyGUI::CheckRange(float val, float val_min, float val_max)
//{
//	return val_min<=val && val<=val_max;
//}




void MyGUI::Begin()
{
	glPolygonMode(GL_FRONT, GL_FILL);
	glEnable(GL_POINT_SMOOTH);
	
	glViewport(0, 0, m_ScreenWidth, m_ScreenHeight);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, m_ScreenWidth, 0, m_ScreenHeight, 0, 0.1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}


void MyGUI::End()
{
    glPopMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

	glDisable(GL_POINT_SMOOTH);
}




//-----------------------------------------------------------------------------------------------------//
// テキストを画面上に描画する
// x,y：画面上の描画座標位置
// width/height：ボタンの大きさ
// text：ボタン文字列
//state：ボタンの状態
//-----------------------------------------------------------------------------------------------------//
void MyGUI::DoText(int x, int y, char *text)
{
	if(m_fontBase)	DrawString( float(x), float(y), color_string[COLOR_DEFAULT], text, GLUT_BITMAP_HELVETICA_12, m_fontBase);
	else			DrawString( float(x), float(y), color_string[COLOR_DEFAULT], text, GLUT_BITMAP_HELVETICA_12);
}

//GLUT_STROKE_ROMAN
//GLUT_BITMAP_HELVETICA_12

//-----------------------------------------------------------------------------------------------------//
// ボタンを画面上に配置する
// x,y：画面上の描画座標位置
// width/height：ボタンの大きさ
// text：ボタン文字列
//state：ボタンの状態
//-----------------------------------------------------------------------------------------------------//
void MyGUI::DoButton(int x, int y, int width, int height, char *text, bool &state)
{
	int		flag			= NULL;
	bool	CursorState		= CheckRange(m_CursorX, x, x+width) & CheckRange(m_CursorY, y, y+height);
	bool	ButtonState		= m_LeftMouseButtonPressed;
	bool	ButtonDownState	= CheckRange(m_MouseDownX, x, x+width) & CheckRange(m_MouseDownY, y, y+height);
	bool	ButtonUpState	= CheckRange(m_MouseUpX, x, x+width) & CheckRange(m_MouseUpY, y, y+height);
	
//tcout << _T("flag = ") << CursorState << _T(", ") << ButtonState << _T(", ") << ButtonDownState << _T(", ") << ButtonUpState << tendl;
	state = false;

	//========================= マウス状態の検出 ===========================//
	if( CursorState==false && ButtonState==false /*&& ButtonDownState==false*/ && ButtonUpState==false )// 領域外で何もしない
	{
		m_State = false;
	}
	else if( CursorState==true && ButtonState==false && ButtonDownState==false && ButtonUpState==false )// 領域内で何もしない
	{
	}
	else if( CursorState==true && ButtonState==true && ButtonDownState==true /*&& ButtonUpState==false*/ )// 領域内押し込み
	{
		m_State = true;
	}
	else if( CursorState==false && ButtonState==true && ButtonDownState==false /*&& ButtonUpState==false*/ )// 領域外押し込み & 領域外ドラッグ
	{
	}
	else if( CursorState==true && ButtonState==false && ButtonDownState==true && ButtonUpState==true )// state4（コマンド実行）
	{
		// 処理が終わったら即座にstate1に遷移する
		m_MouseUpX = m_MouseUpY = -1;//ButtonDownState=false;
		m_MouseDownX = m_MouseDownY = -1;//ButtonDownState=false;
		state = true;
		m_State = false;
	}

//tcout << state << tendl;
	DrawFrame( float(x), float(y), float(width), float(height), COLOR_DEFAULT);

	//============================ ボタンの描画 ==============================//
	if(CursorState==true && ButtonState==true && ButtonDownState==true)
	{
		DrawButton(x, y, width, height, text, COLOR_HOLD);
	}
	else if(CursorState==true)
	{
		DrawButton(x, y, width, height, text, COLOR_OVERLAP);
	}
	else
	{
		DrawButton(x, y, width, height, text, COLOR_DEFAULT);
	}
}

//-----------------------------------------------------------------------------------------------------//
// チェックボックスを画面上に配置する
// x,y: スクリーン座標
// text：チェックボックスの説明文字列
// state：チェックボックスの状態（On/Off）
//-----------------------------------------------------------------------------------------------------//
void MyGUI::DoCheckBox(int x, int y, char *text, bool &state)
{
	int		flag			= NULL;
	bool	CursorState		= CheckRange( m_CursorX, x, x+CHECKBOX_SIZE ) & CheckRange( m_CursorY, y, y+CHECKBOX_SIZE );
	bool	ButtonState		= m_LeftMouseButtonPressed;
	bool	ButtonDownState	= CheckRange( m_MouseDownX, x, x+CHECKBOX_SIZE ) & CheckRange( m_MouseDownY, y, y+CHECKBOX_SIZE );
	bool	ButtonUpState	= CheckRange( m_MouseUpX, x, x+CHECKBOX_SIZE ) & CheckRange( m_MouseUpY, y, y+CHECKBOX_SIZE );
	
//tcout << _T("flag = ") << CursorState << _T(", ") << ButtonState << _T(", ") << ButtonDownState << _T(", ") << ButtonUpState << tendl;

	//========================= マウス状態の検出 ===========================//
	if( CursorState==false && ButtonState==false /*&& ButtonDownState==false*/ && ButtonUpState==false )// state0（何もしていない）
	{
		m_State = false;
	}
	else if( CursorState==true && ButtonState==false && ButtonDownState==false && ButtonUpState==false )// state1（領域内で何もしない）
	{
	}
	else if( CursorState==true && ButtonState==true && ButtonDownState==true /*&& ButtonUpState==false*/ )// state2（領域内ホールド）
	{
	}
	else if( CursorState==false && ButtonState==true && ButtonDownState==false /*&& ButtonUpState==false*/ )// state3（領域外ホールド）
	{
	}
	else if( CursorState==true && ButtonState==false && ButtonDownState==true && ButtonUpState==true )// state4（コマンド実行）
	{
		state = !state;
//tcout << _T("State4: Cursor released inside checkbox boudnary") << tendl;
		m_MouseUpX = m_MouseUpY = -1;
		m_MouseDownX = m_MouseDownY = -1;
	}

//tcout << state << tendl;

	//============================ GUIの描画 ==============================//
	// Fill check box with specified color
	if(state ==true)
	{
		DrawCheckBox(x, y, text, COLOR_ON);
	}
	else if(CursorState==true && ButtonState==true && ButtonDownState==true)
	{
		DrawCheckBox(x, y, text, COLOR_HOLD);
	}
	else if(CursorState==true)
	{
		DrawCheckBox(x, y, text, COLOR_OVERLAP);
	}
	else
	{
		DrawCheckBox(x, y, text, COLOR_DEFAULT);
	}
}


//-----------------------------------------------------------------------------------------------------//
// 水平スライダーを画面上に配置する
// x,y：スクリーン座標
// width：スライダーの長さ
// numbins：スナップ区間の数
//-----------------------------------------------------------------------------------------------------//
void MyGUI::DoHorizontalSlider(int x, int y, int width, char *text, float val_min, float val_max, float &val, int numbins)
{
	int		flag			= NULL;
	bool	CursorState		= CheckRange(m_CursorX, x-5, x+width+10) & CheckRange(m_CursorY, y, y+SLIDER_SIZE);
	bool	ButtonState		= m_LeftMouseButtonPressed;
	bool	ButtonDownState	= CheckRange(m_MouseDownX, x-5, x+width+10) & CheckRange(m_MouseDownY, y, y+SLIDER_SIZE);
	bool	ButtonUpState	= CheckRange(m_MouseUpX, x-5, x+width+10) & CheckRange(m_MouseUpY, y, y+SLIDER_SIZE);
	
//tcout << _T("flag = ") << CursorState << _T(", ") << ButtonState << _T(", ") << ButtonDownState << _T(", ") << ButtonUpState << tendl;

	if( CursorState==false && ButtonState==false /*&& ButtonDownState==false*/ && ButtonUpState==false )// 領域外で何もしない
	{
		m_State = false;
	}
	else if( CursorState==true && ButtonState==false && ButtonDownState==false && ButtonUpState==false )// 領域内で何もしない
	{
	}
	else if( CursorState==true && ButtonState==true && ButtonDownState==true /*&& ButtonUpState==false*/ )// 領域内押し込み
	{
		// つまみの位置を更新したい → カーソル位置からvalを逆算する
		float pos_relative = float(m_CursorX - x) / float(width);
		pos_relative = max(min(pos_relative, 1.f), 0.f);
		
		if(numbins > 0)
		{
			float bin = 1.f / (float)numbins;
			float amari = fmod(pos_relative, bin);
			pos_relative -= amari;
			if(amari >= 0.5 * bin) pos_relative += bin;
		}
		
		val = val_min + pos_relative * (val_max - val_min);
		
		m_State = true;
	}
	else if( CursorState==false && ButtonState==true && ButtonDownState==false /*&& ButtonUpState==false*/ )// 領域外押し込み & 領域外ドラッグ
	{
	}
	else if( CursorState==true && ButtonState==false && ButtonDownState==true && ButtonUpState==true )// state4（コマンド実行）
	{
		// 処理が終わったら即座にstate1に遷移する
		m_MouseUpX = m_MouseUpY = -1;
		m_MouseDownX = m_MouseDownY = -1;
		m_State = false;
	}

	//============================ GUIの描画 ==============================//
	if(CursorState==true)
	{
		DrawHorizontalSlider(x, y, width, text, val, val_min, val_max, COLOR_OVERLAP);
	}
	else
	{
		DrawHorizontalSlider(x, y, width, text, val, val_min, val_max, COLOR_DEFAULT);
	}
//tcout << _T("flag = ") << CursorState << _T(", ") << ButtonState << _T(", ") << ButtonDownState << _T(", ") << ButtonUpState << tendl;
}


//-----------------------------------------------------------------------------------------------------//
// リストボックスを画面上に配置する
// x,y：スクリーン座標
// width：ボックスの幅
// numAttr, Attributes[]: 項目
// idx_curr: 現在選択されている項目のインデックス
//-----------------------------------------------------------------------------------------------------//
void MyGUI::DoListBox(int x, int y, int width, int numAttr, char *Attributes[], int &idx_curr, bool &IsSelected)
{
	int		idx_tmp			= -1;
	int		height			= 20 * numAttr;
	bool	CursorState		= CheckRange(m_CursorX, x, x+width) & CheckRange(m_CursorY, y-height, y);
	bool	ButtonState		= m_LeftMouseButtonPressed;
	bool	ButtonDownState	= CheckRange(m_MouseDownX, x, x+width) & CheckRange(m_MouseDownY, y-height, y);
	bool	ButtonUpState	= CheckRange(m_MouseUpX, x, x+width) & CheckRange(m_MouseUpY, y-height, y);


	//========================= マウス状態の検出 ===========================//
	if( CursorState==false && ButtonState==false /*&& ButtonDownState==false*/ && ButtonUpState==false )// state0（何もしていない）
	{
		m_State = false;
	}
	else if( CursorState==true && ButtonState==false && ButtonDownState==false && ButtonUpState==false )// state1（領域内で何もしない）
	{
		idx_tmp = numAttr * (y-m_CursorY)/height;// 選択行を計算する
	}
	else if( CursorState==true && ButtonState==true && ButtonDownState==true /*&& ButtonUpState==false*/ )// state2（領域内ホールド）
	{
		idx_tmp = numAttr * (y-m_CursorY)/height;// 選択行を計算する
		m_State = true;
	}
	else if( CursorState==false && ButtonState==true && ButtonDownState==false /*&& ButtonUpState==false*/ )// state3（領域外ホールド）
	{
		IsSelected = true;// リストボックスの領域外でマウスボタンを押したら閉じる
	}
	else if( CursorState==true && ButtonState==false && ButtonDownState==true && ButtonUpState==true )// state4（コマンド実行）
	{
		IsSelected = !IsSelected;
		idx_tmp = numAttr * (y-m_CursorY)/height;
		idx_curr = numAttr * (y-m_CursorY)/height; // 選択行を計算する
		m_MouseUpX = m_MouseUpY = -1;
		m_MouseDownX = m_MouseDownY = -1;
		m_State = false;
	}

	DrawListBox(x, y, width, numAttr, Attributes, idx_tmp, ButtonDownState);

//tcout << _T("flag = ") << CursorState << _T(", ") << ButtonState << _T(", ") << ButtonDownState << _T(", ") << ButtonUpState << tendl;
}



//-----------------------------------------------------------------------------------------------------//
// コンボボックスを画面上に配置する
// x,y：スクリーン座標
// width：ボックスの幅
// numAttr, Attributes[]: 項目
// idx_curr: 現在選択されている項目のインデックス
// showListBox: リストボックス表示/非表示
//-----------------------------------------------------------------------------------------------------//
void MyGUI::DoComboBox(int x, int y, int width, int numAttr, char *Attributes[], int &idx_curr, bool &showListBox, char *caption)
{
	int		flag			= NULL;
	bool	CursorState		= CheckRange( m_CursorX, x, x+width+20 ) & CheckRange( m_CursorY, y, y+20 );
	bool	ButtonState		= m_LeftMouseButtonPressed;
	bool	ButtonDownState	= CheckRange( m_MouseDownX, x, x+width+20 ) & CheckRange( m_MouseDownY, y, y+20 );
	bool	ButtonUpState	= CheckRange(m_MouseUpX, x, x+width+20) & CheckRange(m_MouseUpY, y, y+20);
	bool	IsSelected = false;

//tcout << _T("flag = ") << CursorState << _T(", ") << ButtonState << _T(", ") << ButtonDownState << _T(", ") << ButtonUpState << tendl;

	//========================= マウス状態の検出 ===========================//
	if( CursorState==false && ButtonState==false /*&& ButtonDownState==false*/ && ButtonUpState==false )// state0（何もしていない）
	{
		m_State = false;
	}
	else if( CursorState==true && ButtonState==false && ButtonDownState==false && ButtonUpState==false )// state1（領域内で何もしない）
	{
	}
	else if( CursorState==true && ButtonState==true && ButtonDownState==true /*&& ButtonUpState==false*/ )// state2（領域内ホールド）
	{
		m_State = true;
	}
	else if( CursorState==false && ButtonState==true && ButtonDownState==false /*&& ButtonUpState==false*/ )// state3（領域外ホールド）
	{
	}
	else if( CursorState==true && ButtonState==false && ButtonDownState==true && ButtonUpState==true )// state4（コマンド実行）
	{
		// 開いてたら閉じる．閉じてたら開く
		showListBox = !showListBox;

		m_MouseUpX = m_MouseUpY = -1;
		m_MouseDownX = m_MouseDownY = -1;
		m_State = false;
	}

	//============================ GUIの描画 ==============================//
	// キャプション
	if(caption)	DoText(x, y+25, caption); 
	
	// ボタン
	DrawDropDownIcon( float(x+width), float(y+5), 12.f, 8.f );

	if(CursorState==true && ButtonState==true && ButtonDownState==true)
	{
		DrawButton(x, y, width+20, 20, Attributes[idx_curr], COLOR_HOLD);
	}
	else if(CursorState==true)
	{
		DrawButton(x, y, width+20, 20, Attributes[idx_curr], COLOR_OVERLAP);
	}
	else
	{
		DrawButton(x, y, width+20, 20, Attributes[idx_curr], COLOR_DEFAULT);
	}

	// リストボックス
	if(showListBox)
	{
		DoListBox(x, y, width, numAttr, Attributes, idx_curr, IsSelected);

		if(IsSelected)
		{
			showListBox = false;
			m_MouseUpX = m_MouseUpY = -1;
			m_MouseDownX = m_MouseDownY = -1;
		}
	}

//tcout << _T("idx_curr = ") << idx_curr << tendl;
}


//-----------------------------------------------------------------------------------------------------//
// ボタン形状を描画する
// x,y: スクリーン座標
// width,height: ボタンの縦横寸法
// text: ボタン名称
// status: ボタンの状態
//-----------------------------------------------------------------------------------------------------//
void MyGUI::DrawButton(int x, int y, int width, int height, char *text, int status)
{
	// テキストを描画する
	if(text)// DrawString(x+5, y+5, &color_string[COLOR_DEFAULT], text, GLUT_STROKE_ROMAN, 0.1);
	DoText(x+5, y+5, text);

	// ボタンの外枠部分を描画する
	DrawFrame( float(x), float(y), float(width), float(height), COLOR_DEFAULT );
	// ボタンの塗りつぶし部分を描画する
	DrawQuad( float(x), float(y), float(width), float(height), color_button[status] );
}


//-----------------------------------------------------------------------------------------------------//
// チェックボックス形状を描画する
// x,y: スクリーン座標
// text: チェックボックス名称
// status: ボタンの状態
//-----------------------------------------------------------------------------------------------------//
void MyGUI::DrawCheckBox(int x, int y, char *text, int status)
{
	// チェックボックス枠
	DrawFrame( float(x), float(y), float(CHECKBOX_SIZE), float(CHECKBOX_SIZE), COLOR_DEFAULT);
	
	// チェックボックス塗りつぶし
	DrawQuad( float(x), float(y), float(CHECKBOX_SIZE), float(CHECKBOX_SIZE), color_checkbox[status] );
	
	// テキスト
	DoText( x+CHECKBOX_SIZE + 5, y, text);
}


//-----------------------------------------------------------------------------------------------------//
// 水平スライダー形状を描画する
// x,y: スクリーン座標
// width: スライダーバー長さ
// text: チェックボックス名称
// val: つまみ位置の値
// val_min, float val_max:スライダーが取りうる値の範囲
//-----------------------------------------------------------------------------------------------------//
void MyGUI::DrawHorizontalSlider(int x, int y, int width, char *text, float val, float val_min, float val_max, int status)
{
	float	pos = (val - val_min) / (val_max - val_min);
	pos = max(min(pos,1.f), 0.f)*width - SLIDER_SIZE/2 + 1;

	// つまみ
	DrawPoint( x+pos, float(y), SLIDER_SIZE, color_slider[status] );// 内側
	DrawPoint( (float)x+pos-1.5f, (float)y-1.5f, SLIDER_SIZE+3.0f, color_frame[COLOR_OVERLAP] );// 外枠

	// バー
	DrawQuad( float(x), float(y+SLIDER_SIZE/2), float(width), 1.5f, color_button[COLOR_DEFAULT] );
	DrawFrame( (float)x, float(y+SLIDER_SIZE/2), float(width), 1.f, COLOR_DEFAULT);
	

	// テキスト
	char str[64];
	sprintf_s(str, 64, "%s%.3f", text, val);
	DoText(x+5, y+15, str);
}

//-----------------------------------------------------------------------------------------------------//
// リストボックス形状を描画する
// x,y: スクリーン座標
// width: スライダーバー長さ
// *Attributes[]: リスト表示する文字列（複数個）
// id_selected: カーソルが重なっている項目のインデックス
// buttondown：マウスボタンの状態
//-----------------------------------------------------------------------------------------------------//
void MyGUI::DrawListBox(int x, int y, int width, int numAttr, char *Attributes[], int id_selected, bool buttondown)
{
	// フレーム描画
	DrawFrame( float(x), float(y), float(width), float(-20*numAttr), COLOR_DEFAULT );

	// リストボックス描画
	for(int i=0; i<numAttr; i++)
	{
		float pos_y = float( (y - 20) - i * 20 );
		// テキスト
		//DrawString(x+5, pos_y+5, &color_string[COLOR_DEFAULT], Attributes[i], GLUT_STROKE_ROMAN, 0.1);
		DoText(x+5, (int)pos_y+5, Attributes[i]);

		// 枠
		if(i==id_selected)
		{
			if(buttondown==true)
				DrawQuad( float(x), pos_y, float(width), 20.f, color_listbox[COLOR_ON] );
			else
				DrawQuad( float(x), pos_y, float(width), 20.f, color_listbox[COLOR_OVERLAP] );
		}
		else
		{
			DrawQuad( float(x), pos_y, float(width), 20.f, color_listbox[COLOR_DEFAULT] );
		}
	}

}