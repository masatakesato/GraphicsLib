#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


#include	<oreore/ui/win/Window.h>
#include	<oreore/ui/win/WindowProcedure.h>
//#include	<oreore/ui/win/Controller.h>
#include	"ControllerGL.h"



int MessageLoop(void);// メッセージループ
void AttachConsoleWindow(FILE *fp);// コンソール出力







int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
#if _DEBUG
	 _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif



	//============ コンソールウィンドウ作成 =================//
#ifdef _DEBUG
	FILE *fp = NULL;
	AttachConsoleWindow(fp);
#endif

	
	//================ Modelインスタンス作成 ================//
	ModelGL model;
	

	//=============== Viewインスタンス作成 ==================//
	ViewGL view;
	

	//============== Controllerインスタンス作成 =============//
	ControllerGL controller( &model, &view );
//	Controller controller;

	////=============== Windowインスタンス作成 ================//
	Window window = Window( hInstance, _T( "WindowClass" ), _T( "Main" ), &controller, (WNDPROC)WinProc );
	window.SetWindowStyle(WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	window.SetClassStyle(CS_OWNDC);
	window.SetWidth(1280);
	window.SetHeight(720);

	window.Create();
	window.Show(nCmdShow);


	//================ メッセージループ開始 ================//
	int exitCode = MessageLoop();
	
	//========================= 終了 =======================//
#ifdef _DEBUG
	FreeConsole();
	if(fp)	fclose(fp);
#endif

	return exitCode;
}

// http://stackoverflow.com/questions/2165230/should-i-use-directinput-or-windows-message-loop
// ゲームの場合のメッセージループは違う?

int MessageLoop(void)
{
	MSG msg = {0};

	while( GetMessage(&msg,0,0,0) > 0 )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}



void AttachConsoleWindow(FILE *fp)
{
	AttachConsole( GetCurrentProcessId() );
	AllocConsole();
	
	// 入力
	//freopen("CON", "r", stdin);

	// 出力
	freopen_s(&fp, "CON", "w", stderr);
	freopen_s(&fp, "CON", "w", stdout);

	cout << "/********************* Opening Console Window... *********************/" << endl;
}