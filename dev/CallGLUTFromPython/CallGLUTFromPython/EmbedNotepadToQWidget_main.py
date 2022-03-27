# http://timgolden.me.uk/python/win32_how_do_i/find-the-window-for-my-subprocess.html

from ctypes.wintypes import *
import ctypes.wintypes

from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *

# pywin32 module required. 
import win32api
import win32con
import win32gui
import win32process

import sys
import subprocess
import time




class MyNativeEventFilter( QAbstractNativeEventFilter ):

	def __init__( self ):
		super(MyNativeEventFilter, self).__init__()


	def nativeEventFilter( self, eventType, message ):

		if( eventType == "windows_generic_MSG" ):
			msg = ctypes.wintypes.MSG.from_address( message.__int__() )

			print( msg.message )


			if( msg.message == win32con.WM_PARENTNOTIFY ):
				print( "WM_PARENTNOTIFY" )

			elif( msg.message == win32con.WM_MOUSEACTIVATE ):
				print( "WM_MOUSEACTIVATE" )



			elif( msg.message == win32con.WM_KEYDOWN ):
				print( "WM_KEYDOWN" )

			elif( msg.message == win32con.WM_KEYUP ):
				print( "WM_KEYUP" )

			elif( msg.message == win32con.WM_LBUTTONDOWN ):
				print( "WM_LBUTTONDOWN" )

		return False, 0





class MINMAXINFO(ctypes.Structure):
	_fields_ = [
		("ptReserved",      POINT),
		("ptMaxSize",       POINT),
		("ptMaxPosition",   POINT),
		("ptMinTrackSize",  POINT),
		("ptMaxTrackSize",  POINT),
	]


class MyWidget(QWidget):

	def __init__( self ):
		super( MyWidget, self ).__init__()
		#self.setFocusPolicy( Qt.NoFocus )
		self.setLayout( QVBoxLayout() ) 
		self.setGeometry(100, 100, 400,800)
		#self.resize( 400, 400)
		self.myEventFilter = MyNativeEventFilter()

		self.window_widget = None


	def BindHwnd( self, hwnd ):
		self.window_widget = QWidget.createWindowContainer( window_wrapper, self )
		#self.window_widget.installEventFilter( self )
		self.layout().addWidget( self.window_widget )
		ctypes.windll.user32.RegisterHotKey()



	def mousePressEvent( self, a0 ):
		print( "MyWidget::mousePressEvent" )
		return super().mousePressEvent(a0)


	def mouseReleaseEvent( self, a0 ):
		print( "MyWidget::mouseReleaseEvent" )
		return super().mouseReleaseEvent(a0)


	#def event( self, event ):
	#	print( "MyWidget::event" )
	#	return super().event( event )


	def keyPressEvent( self, event ):
		print( "MyWidget::keyPressEvent" )
		return super().keyPressEvent( event )



	def GET_X_PARAM( self, param ):
		return param & 0xffff


	def GET_Y_PARAM( self, param ):
		return param >> 16


	# https://jpdebug.com/p/365848
	# https://gitpress.io/u/1155/pyqt-example-NativeEvent
	#def nativeEvent( self, eventType, message ):
	#	retval, result = super(MyWidget, self).nativeEvent(eventType, message)

	#	print( eventType )

	#	if( eventType == "windows_generic_MSG" ):
	#		msg = ctypes.wintypes.MSG.from_address( message.__int__() )
	#		print( msg.lParam )
	#		x = self.GET_X_PARAM( msg.lParam ) #win32api.LOWORD(msg.lParam)# - self.frameGeometry().x()
	#		y = self.GET_Y_PARAM( msg.lParam ) #win32api.HIWORD(msg.lParam) - self.frameGeometry().y()
	#		#print( x, y )

	#		# https://mcz-xoxo.cocolog-nifty.com/blog/2015/01/post-5960.html
	#		#wparam = msg.wParam
	#		#print( wparam )
	#		#print( msg.message==win32con.WM_KEYDOWN )


	#		# nativeEventFilter
	#		# https://qtcoder.hatenadiary.org/entry/20140626/1403794191 これ？

	#	#	if( self.childAt(x, y) != None ):
	#	#		return retval, result
	#	#	
	#	#	if( msg.message == win32con.WM_NCCALCSIZE ):# ?
	#	#		return True, 0

	#	#	if( msg.message == win32con.WM_GETMINMAXINFO ):# window position or size has been changed
	#	#		info = ctypes.cast( msg.lParam, ctypes.POINTER(MINMAXINFO) ).contents
	#	#		# modify maximized window size to available size
	#	#		#info.ptMaxSize.x = self.
	#	#		#info.ptMaxSize.y = self.
	#	#		info.ptMaxPosition.x = 0
	#	#		info.ptMaxPosition.y = 0

	#	#print( "MyWidget::nativeEvent: ", message  )

	#	return retval, result


def get_hwnds_for_pid( pid ):

	def callback( hwnd, hwnds ):
		if( win32gui.IsWindowVisible(hwnd) and win32gui.IsWindowEnabled(hwnd) ):
			_, found_pid = win32process.GetWindowThreadProcessId( hwnd )
			if( found_pid == pid ):
				hwnds.append( hwnd )
		return True

	hwnds = []
	win32gui.EnumWindows( callback, hwnds )
	return hwnds



def get_child_hwnds( hwnd ):

	#def callback( handle, childlist ):
	#	childlist.append( handle )
	#	return True

	def callback( hwnd, hwnds ):
		if( win32gui.IsWindowVisible(hwnd) and win32gui.IsWindowEnabled(hwnd) ):
			hwnds.append( hwnd )
		return True

	child_hwnds = []
	win32gui.EnumChildWindows( hwnd, callback, child_hwnds )
	return child_hwnds








# https://bbs.huaweicloud.com/blogs/106187
# https://gitlab.com/Gluttton/SOS/tree/master/38096428/solution


# https://www.qtcentre.org/threads/67495-native-event-of-child-window/page2
# TODO: QWindowのmousePressed, keyPressedEventあたりを実装する?

# http://bttb.s1.valueserver.jp/wordpress/blog/2017/09/30/python%E3%81%A7%E5%AD%90%E3%82%A6%E3%82%A4%E3%83%B3%E3%83%89%E3%82%A6%E3%83%8F%E3%83%B3%E3%83%89%E3%83%AB%E3%82%92%E5%8F%96%E5%BE%97%E3%81%99%E3%82%8B%E6%96%B9%E6%B3%95enumchildwindows/
# Pythonで子ウインドウハンドルを取得する方法.

# https://code.tiblab.net/python/pyhook
# pyHookで引っ掛ける?

if __name__ == "__main__":

	app = QApplication( sys.argv )

	w = MyWidget()
	app.installNativeEventFilter( w.myEventFilter )

	notepad = subprocess.Popen( [r"C:\\Windows\\system32\\notepad.exe"] )#[r"C:\\Windows\\System32\\mspaint.exe"] ) #

	time.sleep (0.5)

	parent_window_handles = get_hwnds_for_pid( notepad.pid )

	child_handles = []
	child_handles = get_child_hwnds( parent_window_handles[0] )
	print( child_handles )# 0:テキストフィールド, 1:ステータスバー

	#c_handle = ctypes.windll.user32.FindWindowExW( parent_window_handles[0], 0, 'Edit', 0 )
	#print("子ウインドウハンドル: ", c_handle )


	#for hwnd in child_handles:#parent_window_handles:
	#	window_wrapper = QWindow.fromWinId( hwnd )
	#	window_widget = QWidget.createWindowContainer( window_wrapper )
	#	window_widget.show()

	window_wrapper = QWindow.fromWinId( parent_window_handles[0] )#child_handles[0] )#
	w.BindHwnd( window_wrapper )
	w.setGeometry(0, 0, 800, 600)
	w.show()

	sys.exit( app.exec_() )








#from PyQt5.QtCore import *
#from PyQt5.QtWidgets import *
#from PyQt5.QtGui import *



#https://stackoverflow.com/questions/17272572/get-window-handler-from-started-process

#import win32process
#import win32process as process
#import win32gui
#import sys

#PORTABLE_APPLICATION_LOCATION = "C:\\Windows\\system32\\notepad.exe"
#processHandler = -1


#def isLiveProcess( processHandler ): #Process handler is dwProcessId
#	processList = process.EnumProcesses()
#	for aProcess in processList:
#		if( aProcess == processHandler ):
#			return True
#	return False


#def callback( hwnd, procid ):
#	if procid in win32process.GetWindowThreadProcessId( hwnd ):
#		win32gui.ShowWindow( hwnd, 1 )#win32gui.SetForegroundWindow(hwnd)


#def show_window_by_process( procid ):
#	win32gui.EnumWindows( callback, procid )



#def runProgram():
#	global processHandler
#	#don't run a process more than once
#	if (isLiveProcess(processHandler) ):
#		#Bring focus back to running window!
#		show_window_by_process( processHandler )
#		print( "processHandler: " , processHandler )
#		return
#	try:
#		startObj = process.STARTUPINFO()
#		myProcessTuple = process.CreateProcess(PORTABLE_APPLICATION_LOCATION,None,None,None,8,8,None,None,startObj)
#		processHandler = myProcessTuple[2]
#		print( "processHandler: " , processHandler )
#	except:
#		print(sys.exc_info[0])





#if __name__ == "__main__":

#	runProgram()

#	app = QApplication( sys.argv )

##TODO: runProgramからhwnd引っ張り出す
#	window_wrapper = QWindow.fromWinId( hwnd )
#	#window_widget = createWindowContainer( window_wrapper )

#	#window_widget.show()

#	sys.exit(app.exec_())