#include	"ControllerGL.h"



ControllerGL::ControllerGL()
{
	m_Model		= NULL;
	m_View		= NULL;

	m_hThread	= NULL;
	m_ThreadID	= 0;
	m_hEvent	= NULL;
	m_hMutex	= NULL;

	m_ClientWidth	= 0;
	m_ClientHeight	= 0;
	m_ChangeClientArea	= false;
}



ControllerGL::ControllerGL(ModelGL *model, ViewGL *view)
{
	m_Model	= model;
	m_View	= view;

	m_hThread	= NULL;
	m_ThreadID	= 0;
	m_hEvent	= NULL;
	m_hMutex	= NULL;

	m_ClientWidth	= 0;
	m_ClientHeight	= 0;
	m_ChangeClientArea	= false;
}



ControllerGL::~ControllerGL()
{	
}


unsigned ControllerGL::SetClientSize(int width, int height)
{
	WaitForSingleObject(m_hMutex, INFINITE);

	m_ClientWidth = width;
	m_ClientHeight = height;

	m_Model->SetViewport(0, 0, width, height);
	m_Model->SetCameraProjection( float(m_ClientWidth) / float(m_ClientHeight), 3.141592f/4.0f, 0.001f, 1000.0f );
	m_ChangeClientArea = true;

	ReleaseMutex(m_hMutex);

	return 0;
}



unsigned ControllerGL::Create()
{
	//=================== レンダリングコンテキスト作成 =====================//
	m_View->CreateContext(m_hWnd, 24, 24, 24);
	

	//=================== OpenGL関連ブジェクト作成 =========================//
	wglMakeCurrent(m_View->GetHDC(), m_View->GetHRC());
	{
		Vec3f pos = {0,2.5,2.0}, dir = {0,-1,-0.66}, up = {0,0,1};

		glewInit();

		m_Model->Init();
		m_Model->LoadShader("Shader/Shader.vert", NULL, "Shader/Shader.frag");
		m_Model->SetViewport(0, 0, m_ClientWidth, m_ClientHeight);
		m_Model->InitCamera(pos, dir, up, float(m_ClientWidth) / float(m_ClientHeight), 3.141592f/4.0f, 0.001f, 1000.0f );

		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0, 0.0, 0.0, 0.0);
	}
	wglMakeCurrent(0, 0);


	//============================ スレッド作成 ===========================//
	if(!m_hThread)
	{
		m_hThread = (HANDLE) _beginthreadex(0, 0, ThreadFunction, this, CREATE_SUSPENDED, &m_ThreadID);
		m_hEvent = CreateEvent(NULL, TRUE, TRUE, "ThreadFunction");
		m_hMutex = CreateMutex(NULL, FALSE, "ThreadPause");
	}
	
	return 0;
}


unsigned ControllerGL::Start()
{
	if(m_hThread)
	{
		ResetEvent(m_hEvent);
		ResumeThread(m_hThread);
	}

	return 0;
}



unsigned ControllerGL::Pause()
{
	SuspendThread(m_hThread);

	return 0;
}



unsigned ControllerGL::Stop()
{
	if(m_hThread)
	{
		SetEvent(m_hEvent);
		WaitForSingleObject(m_hThread, INFINITE);
		
		CloseHandle(m_hThread);
		CloseHandle(m_hEvent);
		CloseHandle(m_hMutex);
	}
	
	return 0;
}


unsigned ControllerGL::Close()
{
	//============= スレッドの停止 =============//
	Stop();

	//======= OpenGl関連オブジェクトの解放 ======//
	wglMakeCurrent(m_View->GetHDC(), m_View->GetHRC());
	{
		if(m_Model){ delete	m_Model; }
	}
	wglMakeCurrent(0, 0);

	//====== レンダリングコンテキストの削除 ====//
//	if(m_View)
//	{
		m_View->CloseContext(m_hWnd);
		//delete	m_View;
		//m_View = NULL;
//	}
	
	
	DestroyWindow(m_hWnd);
	return 0;
}



unsigned _stdcall ControllerGL::ThreadFunction(void *Param)
{
	((ControllerGL *)Param)->RunThread();

	return 0;
}







float		g_FrameRate		= 0.0f;			// フレームレート
float		g_DeltaT		= 1.0f / 60.0f;	// 1フレームあたりの描画時間(初期値)



void ControllerGL::RunThread()
{
	/*
	DWORD Time_origin, Time_next;

	int numframe = 0;
	Time_origin = timeGetTime(); // CPU時間をミリ秒単位で取得する
	Time_next = Time_origin;
	*/

	
	wglMakeCurrent(m_View->GetHDC(), m_View->GetHRC());


	while(WaitForSingleObject(m_hEvent,0) != WAIT_OBJECT_0)
	{
		WaitForSingleObject(m_hMutex, INFINITE);

		ProcessCameraKeys();

		if(m_ChangeClientArea==true)
		{
			m_Model->SetViewport(0,0,m_ClientWidth,m_ClientHeight);
			m_ChangeClientArea = false;
		}

		m_Model->Draw();

		m_View->swapBuffers();
		//Sleep(1);

		ReleaseMutex(m_hMutex);

		/*
		//=================== フレーム数をカウントする ========================//
		Time_next = timeGetTime();
		numframe++;

		if(Time_next - Time_origin >= 100)// 100[ms]毎に実施
		{
			g_FrameRate = (float)numframe / (Time_next - Time_origin) * 1000.0f;
			Time_origin = Time_next;
			numframe = 0;
			g_DeltaT = 0.5f * (g_DeltaT + (1.0f / g_FrameRate));

			std::cout << "fps: " << g_FrameRate << std::endl;
		}
		*/
	}

	wglMakeCurrent(NULL, NULL);
}



unsigned ControllerGL::keyDown(WPARAM wParam)
{
	m_Keys[int(wParam)] = true;
	return 0;
}



unsigned ControllerGL::keyUp(WPARAM wParam)
{
	m_Keys[int(wParam)] = false;

	// モード変更
	if(int(wParam)=='T')	m_Model->SetMode();

	return 0;
}



unsigned ControllerGL::lButtonDown(LPARAM lParam)
{
	m_LeftMouseButtonPressed = true;
	m_MouseX = LOWORD(lParam);
	m_MouseY = HIWORD(lParam);

	return 0;
}


unsigned ControllerGL::lButtonUp(LPARAM lParam)
{
	m_LeftMouseButtonPressed = false;
	m_MouseX = LOWORD(lParam);
	m_MouseY = HIWORD(lParam);

	return 0;
}



unsigned ControllerGL::mouseMove(LPARAM lParam)
{
	int x, y;

	x = LOWORD(lParam) > m_ClientWidth ? m_MouseX : LOWORD(lParam);
	y = HIWORD(lParam) > m_ClientHeight ? m_MouseY : HIWORD(lParam);

	if(m_LeftMouseButtonPressed==true)
	{
		dx = float(m_MouseX - x) / 100.0f;
		dy = float(m_MouseY - y) / 100.0f;
		
		m_Model->RotateCamera(dx,dy);

		m_MouseX = x;
		m_MouseY = y;
	}

	return 0;
}


void ControllerGL::ProcessCameraKeys()
{
	for(int	i=0; i<MAX_KEYS; i++)
	{
		if(m_Keys[i]==false) continue;

		switch(i)
		{
			case 'W' :{	m_Model->TransrateCamera(+0.01f, 0, 0);	break; }
			case 'S' :{	m_Model->TransrateCamera(-0.01f, 0, 0);	break; }
			case 'A' :{	m_Model->TransrateCamera(0, +0.01f, 0);	break; }
			case 'D' :{ m_Model->TransrateCamera(0, -0.01f, 0);	break; }
			case 'R' :{	m_Model->TransrateCamera(0, 0, +0.01f);	break; }
			case 'F' :{	m_Model->TransrateCamera(0, 0, -0.01f);	break; }
			case 'Q' :{	m_Model->RollCamera(-0.01f);			break; }
			case 'E' :{	m_Model->RollCamera(+0.01f);			break; }
			default : {	break; }
		}// end of switch
		
	}// end of i loop

}