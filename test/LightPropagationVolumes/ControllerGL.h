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
	bool			m_ChangeClientArea;// �N���C�A���g�̈�̃T�C�Y�ύX�t���O
	
	static unsigned _stdcall ThreadFunction(void *Param);
	void RunThread();

	void ProcessCameraKeys();


public:

	ControllerGL();
	ControllerGL(ModelGL *model, ViewGL *view);
	~ControllerGL();
	
	// �N���C�A���g�̈�ݒ�
	unsigned virtual SetClientSize(int width, int height);
	

	unsigned virtual Close();

	// �X���b�h����
	unsigned virtual Create();
	unsigned virtual Start();
	unsigned virtual Pause();
	unsigned virtual Stop();
	// �L�[���̓C�x���g
	unsigned virtual keyDown(WPARAM wParam);	// �L�[����(for WM_KEYDOWN): 
	unsigned virtual keyUp(WPARAM wParam);		// �L�[���(for WM_KEYUP)
	// �}�E�X�C�x���g
	unsigned virtual lButtonDown(LPARAM lParam);// �}�E�X���{�^������(for WM_LBUTTONDOWN)
	unsigned virtual lButtonUp(LPARAM lParam);	// �}�E�X���{�^�����(for WM_LBUTTONUP)
	unsigned virtual mouseMove(LPARAM lParam);	// �}�E�X�J�[�\���ړ�(for WM_MOUSEMOVE

};



#endif	// CONTROLLER_GL_H //