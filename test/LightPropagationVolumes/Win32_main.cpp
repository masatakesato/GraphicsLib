#include	<oreore/Window.h>
#include	<oreore/WindowProcedure.h>
#include	<oreore/ViewGL.h>

#include	"ModelGL.h"
#include	"ControllerGL.h"



int MessageLoop(void);// ���b�Z�[�W���[�v
void AttachConsoleWindow(FILE *fp);// �R���\�[���o��


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//============ �R���\�[���E�B���h�E�쐬 =================//
#ifdef _DEBUG
	FILE *fp = NULL;
	AttachConsoleWindow(fp);
#endif

	
	//================ Model�C���X�^���X�쐬 ================//
	ModelGL *model = new ModelGL();
	

	//=============== View�C���X�^���X�쐬 ==================//
	ViewGL *view = new ViewGL();
	

	//============== Controller�C���X�^���X�쐬 =============//
	ControllerGL *controller = new ControllerGL(model, view);
	

	//=============== Window�C���X�^���X�쐬 ================//
	Window window = Window(hInstance, "WindowClass", "Main", controller, (WNDPROC)WinProc);
	window.SetWindowStyle(WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	window.SetClassStyle(CS_OWNDC);
	window.SetWidth(1280);
	window.SetHeight(720);

	window.Create();
	window.Show(nCmdShow);

	
	//================ ���b�Z�[�W���[�v�J�n ================//
	int exitCode = MessageLoop();
	
	//========================= �I�� =======================//
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