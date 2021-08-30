#include	<io.h>
#include	<fcntl.h>
#include	<process.h>
#include	<windows.h>
#include	<iostream>
using namespace std;
#include	"resource.h"


#include	"PlanetRenderingEngine.h"

#define	APP_NAME TEXT("PlanetRenderingEngine")


//----------------------------------------------------------------------------------------------------
//	グローバル変数
//----------------------------------------------------------------------------------------------------
int			g_ScreenWidth	= 1280;	// ウィンドウ解像度
int			g_ScreenHeight	= 720;	// ウィンドウ解像度
HWND		g_hWnd	= NULL;

PlanetRenderingEngine	*g_Engine = NULL;


//----------------------------------------------------------------------------------------------------
//	グローバル関数プロトタイプ宣言
//----------------------------------------------------------------------------------------------------
HRESULT	InitWindow( HINSTANCE hInstance, int nCmdShow );	// ウィンドウ初期化
LRESULT CALLBACK	WndProc( HWND, UINT, WPARAM, LPARAM );	


//----------------------------------------------------------------------------------------------------
//	メイン
//----------------------------------------------------------------------------------------------------
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{	
	//============ コンソールウィンドウ作成 =================//
#ifdef _DEBUG // コンソールウィンドウを呼び出す（デバッグ時のみ）
	FILE *fp = NULL;
	AttachConsole( GetCurrentProcessId() );
	AllocConsole();
	
	// 入力
	//freopen("CON", "r", stdin);
	
	// 出力
	freopen_s(&fp, "CON", "w", stderr);
	freopen_s(&fp, "CON", "w", stdout);
#endif


	//====================== ミューテックスを用いた多重起動防止 ==================//
	HANDLE	hMutex;

	hMutex = CreateMutex(NULL, TRUE, APP_NAME);
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		MessageBox(NULL, TEXT("多重起動はできません"), NULL, MB_OK);
		return 0;
	}



	//====================== 惑星レンダリングエンジンの初期化 ====================//
	g_Engine = new PlanetRenderingEngine(g_ScreenWidth, g_ScreenHeight);

	//============================== ウィンドウ初期化 ===========================//
	if( FAILED( InitWindow(hInstance,nCmdShow) ) )// ウィンドウ初期化に失敗した場合はプログラム終了
		return 0;

	//============================= メッセージループ ============================//
	MSG msg = {0};
    while(GetMessage(&msg, 0, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
	
	CloseHandle(hMutex);

#ifdef _DEBUG 
	FreeConsole();
	if(fp)	fclose(fp);
#endif


	return (int)msg.wParam;
}



//----------------------------------------------------------------------------------------------------
//	ウィンドウ初期化
//----------------------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	//============== 構造体を使ってウィンドウの属性を初期化する ================//
	WNDCLASSEX wcex;//
	
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;								// ウィンドウプロシージャを所有するインスタンスを設定
	wcex.hIcon			= LoadIcon( hInstance, (LPCTSTR)IDI_TEST );	// 画面左上のアイコン
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);				// カーソルのアイコンを設定
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);				// 背景色を設定（ブラシオブジェクトを指定する）
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= _T("WindowClass");
	wcex.hIconSm		= NULL;
	
	if( !RegisterClassEx(&wcex) )
		return E_FAIL;

	//========================= ウィンドウを作成する ===========================//
	RECT rc = {0, 0, g_ScreenWidth, g_ScreenHeight};
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow( _T("WindowClass"), APP_NAME, WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
                           NULL );

	if(!g_hWnd)
		return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);
	
	return S_OK;
}


//----------------------------------------------------------------------------------------------------
//	ウィンドウプロシージャ
//----------------------------------------------------------------------------------------------------
LRESULT CALLBACK	WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT	ps;
	HDC			hDC;
	
	switch(message)
	{
		case WM_CREATE:// ウィンドウ初期化
			g_Engine->SetHandle( hWnd );// ハンドルをセット
			g_Engine->Create();			// スレッド作成
			g_Engine->Start();			// スレッド開始	
			break;

		case WM_PAINT:
			hDC = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;

		case WM_SIZE:
			g_Engine->SetScreenSize(LOWORD(lParam), HIWORD(lParam));
			break;
		
		case WM_LBUTTONDOWN:
			g_Engine->lButtonDown(lParam);
			break;
		
		case WM_LBUTTONUP:
			g_Engine->lButtonUp(lParam);
			break;

		case WM_RBUTTONDOWN:
			g_Engine->rButtonDown(lParam);
			break;
		
		case WM_RBUTTONUP:
			g_Engine->rButtonUp(lParam);
			break;
		
		case WM_MOUSEMOVE:
			g_Engine->mouseMove(lParam);
			break;

		case WM_KEYDOWN:
			g_Engine->keyDown(wParam);
			break;

		case WM_KEYUP:
			g_Engine->keyUp(wParam);
			break;

		case WM_ERASEBKGND:
			break;

		case WM_DESTROY:
			g_Engine->Stop();
			g_Engine->~PlanetRenderingEngine();
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}