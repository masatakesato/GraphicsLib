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


unsigned ControllerGL::SetClientSize( int width, int height )
{
	WaitForSingleObject( m_hMutex, INFINITE );

	m_ClientWidth		= width;
	m_ClientHeight		= height;
	m_ChangeClientArea	= true;

	ReleaseMutex( m_hMutex );

	return 0;
}



unsigned ControllerGL::Create()
{
	m_View->CreateContext(m_hWnd, 24, 24, 24);
	

	wglMakeCurrent(m_View->GetHDC(), m_View->GetHRC());
	
	glewInit();
	Vec3f pos = {10,10,10}, dir = {-1,-1,-1}, up = {0,0,1};
	//Vec3f pos = {0,0,0}, dir = {0,0,1}, up = {0,1,0};

	m_Model->Init();
	m_Model->LoadShader("Shader/Shader.vert", NULL, "Shader/Shader.frag");
	//m_Model->SetViewport(0, 0, m_ClientWidth, m_ClientHeight);
	m_Model->InitCamera( pos, dir, up, float(m_ClientWidth) / float(m_ClientHeight), M_PI_4, 1.0e-3f, 1.0e+3f );

#ifdef _DEBUG		
	GLenum errCode;
		if((errCode=glGetError()) != GL_NO_ERROR)
		{
			cout << "gl error atControllerGL::Create()..." << gluErrorString(errCode) << endl;
			//return;
		}
#endif


	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	glClearColor(0.3, 0.3, 1.0, 1.0);


	wglMakeCurrent( 0, 0 );


	if(!m_hThread)
	{
		m_hThread = (HANDLE) _beginthreadex( 0, 0, ThreadFunction, this, CREATE_SUSPENDED, &m_ThreadID );
		m_hEvent = CreateEvent( 0, TRUE, TRUE, _T("ThreadFunction") );
		m_hMutex = CreateMutex( 0, FALSE, _T("ThreadPause") );
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
	//wglMakeCurrent(m_View->GetHDC(), m_View->GetHRC());
	//{
	//	if(m_Model){ delete	m_Model; }
	//}
	//wglMakeCurrent(0, 0);

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


unsigned ControllerGL::Destroy()
{
	::PostQuitMessage(0);       // メッセージループから抜け出す
	return 0;
}






unsigned _stdcall ControllerGL::ThreadFunction(void *Param)
{
	((ControllerGL *)Param)->RunThread();

	return 0;
}


void ControllerGL::RunThread()
{
	
wglMakeCurrent(m_View->GetHDC(), m_View->GetHRC());

	while(WaitForSingleObject(m_hEvent,0) != WAIT_OBJECT_0)
	{
		WaitForSingleObject(m_hMutex, INFINITE);

		// クライアント領域サイズ変更の反映
		if( m_ChangeClientArea==true )
		{
			m_Model->SetViewport( 0, 0, m_ClientWidth, m_ClientHeight );
			m_Model->SetCameraProjection( float(m_ClientWidth) / float(m_ClientHeight), M_PI_4, 1.0e-3f, 1.0e+3f );
			m_ChangeClientArea = false;
		}

		// キー入力操作の反映
		ProcessCameraKeys();

		// 描画処理
		m_Model->Draw();		
		
		// バッファのスワップ
		m_View->swapBuffers();	

		//Sleep(1);
		

		ReleaseMutex(m_hMutex);
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
		
		m_Model->RotateCamera( dx, dy );

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
			case 'W' :{	m_Model->TranslateCamera(+0.01f, 0, 0);	break; }
			case 'S' :{	m_Model->TranslateCamera(-0.01f, 0, 0);	break; }
			case 'A' :{	m_Model->TranslateCamera(0, +0.01f, 0);	break; }
			case 'D' :{ m_Model->TranslateCamera(0, -0.01f, 0);	break; }
			case 'R' :{	m_Model->TranslateCamera(0, 0, +0.01f);	break; }
			case 'F' :{	m_Model->TranslateCamera(0, 0, -0.01f);	break; }
			case 'Q' :{	m_Model->RollCamera(-0.01f);			break; }
			case 'E' :{	m_Model->RollCamera(+0.01f);			break; }
			default : {	break; }
		}// end of switch
		
	}// end of i loop

}