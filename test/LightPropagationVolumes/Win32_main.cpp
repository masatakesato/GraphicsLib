#include	<oreore/Window.h>
#include	<oreore/WindowProcedure.h>
#include	<oreore/ViewGL.h>

#include	"ModelGL.h"
#include	"ControllerGL.h"



int MessageLoop(void);// メッセージループ
void AttachConsoleWindow(FILE *fp);// コンソール出力


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//============ コンソールウィンドウ作成 =================//
#ifdef _DEBUG
	FILE *fp = NULL;
	AttachConsoleWindow(fp);
#endif

	
	//================ Modelインスタンス作成 ================//
	ModelGL *model = new ModelGL();
	

	//=============== Viewインスタンス作成 ==================//
	ViewGL *view = new ViewGL();
	

	//============== Controllerインスタンス作成 =============//
	ControllerGL *controller = new ControllerGL(model, view);
	

	//=============== Windowインスタンス作成 ================//
	Window window = Window(hInstance, "WindowClass", "Main", controller, (WNDPROC)WinProc);
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

	freopen_s(&fp, "CON", "w", stderr);
	freopen_s(&fp, "CON", "w", stdout);
	
	cout << "/********************* Opening Console Window... *********************/" << endl;
}