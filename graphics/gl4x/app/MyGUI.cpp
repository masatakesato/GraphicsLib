#include	"MyGUI.h"

#include	<math.h>
#include	<iostream>
using namespace std;

#include	"MyGlPrimitive.h"
using namespace MyGlPrimitive;
#include	<GL/glut.h>


#define	SLIDER_SIZE				10
#define	CHECKBOX_SIZE			14


// GUIの色
enum GUI_COLOR { COLOR_DEFAULT, COLOR_OVERLAP=4, COLOR_HOLD=8, COLOR_ON=12 }; 

// 文字
static float color_string[8] =
{
	1.0f, 1.0f, 1.0f, 0.0f,
	0.3f, 0.3f, 0.3f, 0.0f,
};


// 枠
static float color_frame[8]	= 
{
	0.5, 0.5, 0.5, 0.5,
	0.5, 0.7, 0.5, 0.5,
};


// ボタン
static float color_button[12] =
{
	0.1, 0.1, 0.1, 0.5,// デフォルト色
	0.3, 0.3, 0.3, 0.5,// カーソルオーバーラップ
	0.1, 0.1, 0.1, 0.5,// ボタン押し込み
};

// チェックボックス
static float color_checkbox[16] =
{
	0.1, 0.0, 0.0, 0.5,// デフォルト（オフ）
	0.3, 0.0, 0.0, 0.5,// オーバーラップ
	0.2, 0.0, 0.0, 0.5,// ボタン押し込み
	0.7, 1.0, 0.7, 0.5,// オン
};

// リストボックス
static float color_listbox[16] = 
{
	0.1, 0.1, 0.1, 0.5,// デフォルト
	0.5, 0.0, 0.0, 0.5,// オーバーラップ
	0.5, 0.0, 0.0, 0.5,// ボタン押し込み
	0.5, 0.7, 0.5, 0.5,// 選択
};

// スライダーつまみ
static float color_slider[8] =
{
	0.2, 0.3, 0.0, 0.5,
	0.35, 0.5, 0.0, 0.5,
};

/*
#define max((a),(b)) ((a)>(b)?(a):(b))
#define min((a),(b)) ((a)<(b)?(a):(b))
*/

static void DrawPointer(float x, float y, float width, float height)
{
	float	ColorArray[12] =
	{
		0.1, 0.1, 0.1, 0.0,// 頂点0
		0.5, 0.7, 0.5, 0.7,// 頂点1
		0.1, 0.1, 0.1, 0.0,// 頂点2
	};
	
	float	VertexArray[6] = 
	{
		x,				y+height,	// 頂点0
		x+width*0.5,	y,			// 頂点1
		x+width,		y+height,	// 頂点2
	};

	GLuint	index[12] = { 0,1,2 };
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	
	glColorPointer(4, GL_FLOAT, 0, ColorArray);
	glVertexPointer(2, GL_FLOAT, 0, VertexArray);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, index);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}



static void DrawFrame(float x, float y, float width, float height, int offset)
{
	float	ColorArray[16] =
	{
		color_frame[offset], color_frame[offset+1], color_frame[offset+2], color_frame[offset+3],// 頂点0
		color_frame[offset], color_frame[offset+1], color_frame[offset+2], color_frame[offset+3],// 頂点1
		color_frame[offset], color_frame[offset+1], color_frame[offset+2], color_frame[offset+3],// 頂点2
		color_frame[offset], color_frame[offset+1], color_frame[offset+2], color_frame[offset+3],// 頂点3
	};
	
	float	VertexArray[8] = 
	{
		x,			y,				// 頂点0
		x,			y+height,		// 頂点1
		x+width,	y+height,	// 頂点2
		x+width,	y,		// 頂点3
	};

	GLuint	index[4] = { 0,1,2,3, };
	
	glLineWidth(1.5);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glColorPointer(4, GL_FLOAT, 0, ColorArray);
	glVertexPointer(2, GL_FLOAT, 0, VertexArray);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, index);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	glLineWidth(1.0);
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
//cout << "MyGUI::InitFontDisplayList()..." << endl;

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
//cout << "MyGUI::SetScreenResolution()..." << endl;
	m_ScreenWidth = width;
	m_ScreenHeight = height;

//std::cout << "ScreenWidth = " << m_ScreenWidth << std::endl;
//std::cout << "ScreenHeight = " << m_ScreenHeight << std::endl;
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




bool MyGUI::CheckRange(float val, float val_min, float val_max)
{
	return val_min<=val && val<=val_max;
}




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
	if(m_fontBase)	DrawString(x, y, &color_string[COLOR_DEFAULT], text, GLUT_BITMAP_HELVETICA_12, m_fontBase);
	else			DrawString(x, y, &color_string[COLOR_DEFAULT], text, GLUT_BITMAP_HELVETICA_12);
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
	
//std::cout << "flag = " << CursorState << ", " << ButtonState << ", " << ButtonDownState << ", " << ButtonUpState << std::endl;
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

//std::cout << state << std::endl;
	DrawFrame(x, y, width, height, COLOR_DEFAULT);

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
	bool	CursorState		= CheckRange(m_CursorX, x, x+CHECKBOX_SIZE) & CheckRange(m_CursorY, y, y+CHECKBOX_SIZE);
	bool	ButtonState		= m_LeftMouseButtonPressed;
	bool	ButtonDownState	= CheckRange(m_MouseDownX, x, x+CHECKBOX_SIZE) & CheckRange(m_MouseDownY, y, y+CHECKBOX_SIZE);
	bool	ButtonUpState	= CheckRange(m_MouseUpX, x, x+CHECKBOX_SIZE) & CheckRange(m_MouseUpY, y, y+CHECKBOX_SIZE);
	
//std::cout << "flag = " << CursorState << ", " << ButtonState << ", " << ButtonDownState << ", " << ButtonUpState << std::endl;

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
//std::cout << "State4: カーソルが領域内にあってボタンを離した" << std::endl;
		m_MouseUpX = m_MouseUpY = -1;
		m_MouseDownX = m_MouseDownY = -1;
	}

//std::cout << state << std::endl;

	//============================ GUIの描画 ==============================//
	// チェックボックス塗りつぶし
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
	
//std::cout << "flag = " << CursorState << ", " << ButtonState << ", " << ButtonDownState << ", " << ButtonUpState << std::endl;

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
		pos_relative = max(min(pos_relative, 1.0), 0.0);
		
		if(numbins > 0)
		{
			float bin = 1.0 / (float)numbins;
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
//std::cout << "flag = " << CursorState << ", " << ButtonState << ", " << ButtonDownState << ", " << ButtonUpState << std::endl;
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

//std::cout << "flag = " << CursorState << ", " << ButtonState << ", " << ButtonDownState << ", " << ButtonUpState << std::endl;
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
	bool	CursorState		= CheckRange(m_CursorX, x, x+width+20) & CheckRange(m_CursorY, y, y+20);
	bool	ButtonState		= m_LeftMouseButtonPressed;
	bool	ButtonDownState	= CheckRange(m_MouseDownX, x, x+width+20) & CheckRange(m_MouseDownY, y, y+20);
	bool	ButtonUpState	= CheckRange(m_MouseUpX, x, x+width+20) & CheckRange(m_MouseUpY, y, y+20);
	bool	IsSelected = false;

//std::cout << "flag = " << CursorState << ", " << ButtonState << ", " << ButtonDownState << ", " << ButtonUpState << std::endl;

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
	DrawPointer(x+width, y+5, 16, 10);

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

//std::cout << "idx_curr = " << idx_curr << std::endl;
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
	DrawFrame(x, y, width, height, COLOR_DEFAULT);
	// ボタンの塗りつぶし部分を描画する
	DrawQuad(x, y, width, height, &color_button[status]);
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
	DrawFrame(x, y, CHECKBOX_SIZE, CHECKBOX_SIZE, COLOR_DEFAULT);
	
	// チェックボックス塗りつぶし
	DrawQuad(x,y, CHECKBOX_SIZE, CHECKBOX_SIZE, &color_checkbox[status]);
	
	// テキスト
	DoText(x+CHECKBOX_SIZE + 5, y, text);
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
	pos = max(min(pos,1.0), 0.0)*width - SLIDER_SIZE/2 + 1;

	// つまみ
	DrawPoint(x+pos, y, SLIDER_SIZE, &color_slider[status]);// 内側
	DrawPoint((float)x+pos-1.5f, (float)y-1.5f, SLIDER_SIZE+3.0f, &color_frame[COLOR_OVERLAP]);// 外枠

	// バー
	DrawQuad(x, y+SLIDER_SIZE/2, width, 1.5, &color_button[COLOR_DEFAULT]);
	DrawFrame(x, y+SLIDER_SIZE/2, width, 1.0, COLOR_DEFAULT);
	

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
	DrawFrame(x, y, width, -20*numAttr, COLOR_DEFAULT);

	// リストボックス描画
	for(int i=0; i<numAttr; i++)
	{
		float pos_y = (y - 20) - i * 20;
		// テキスト
		//DrawString(x+5, pos_y+5, &color_string[COLOR_DEFAULT], Attributes[i], GLUT_STROKE_ROMAN, 0.1);
		DoText(x+5, pos_y+5, Attributes[i]);

		// 枠
		if(i==id_selected)
		{
			if(buttondown==true)
				DrawQuad(x, pos_y, width, 20, &color_listbox[COLOR_ON]);
			else
				DrawQuad(x, pos_y, width, 20, &color_listbox[COLOR_OVERLAP]);
		}
		else
		{
			DrawQuad(x, pos_y, width, 20, &color_listbox[COLOR_DEFAULT]);
		}
	}

}