#ifndef	CONTROLLER_GL_H
#define	CONTROLLER_GL_H


#include	<oreore/Controller.h>
#include	"ModelGL.h"
#include	<oreore/ViewGL.h>

#include	<process.h>


class ControllerGL : Controller
{
private:

	ModelGL			*m_Model;
	ViewGL			*m_View;

	HANDLE			m_hThread;
	unsigned int	m_ThreadID;
	HANDLE			m_hEvent;
	HANDLE			m_hMutex;

	int				m_ClientWidth;
	int				m_ClientHeight;
	bool			m_ChangeClientArea;// クライアント領域のサイズ変更フラグ
	
	static unsigned _stdcall ThreadFunction(void *Param);
	void RunThread();

	void ProcessCameraKeys();


public:

	ControllerGL();
	ControllerGL(ModelGL *model, ViewGL *view);
	~ControllerGL();
	
	// クライアント領域設定
	unsigned virtual SetClientSize(int width, int height);
	

	unsigned virtual Close();

	// スレッド操作
	unsigned virtual Create();
	unsigned virtual Start();
	unsigned virtual Pause();
	unsigned virtual Stop();
	// キー入力イベント
	unsigned virtual keyDown(WPARAM wParam);	// キー押込(for WM_KEYDOWN): 
	unsigned virtual keyUp(WPARAM wParam);		// キー解放(for WM_KEYUP)
	// マウスイベント
	unsigned virtual lButtonDown(LPARAM lParam);// マウス左ボタン押込(for WM_LBUTTONDOWN)
	unsigned virtual lButtonUp(LPARAM lParam);	// マウス左ボタン解放(for WM_LBUTTONUP)
	unsigned virtual mouseMove(LPARAM lParam);	// マウスカーソル移動(for WM_MOUSEMOVE

};



#endif	// CONTROLLER_GL_H //