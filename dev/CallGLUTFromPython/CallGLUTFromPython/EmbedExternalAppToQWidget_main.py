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

import WindowHandleHelper




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
        #self.setGeometry(100, 100, 400,800)
        #self.resize( 400, 400)
        #self.myEventFilter = MyNativeEventFilter()

        self.window = None
        self.window_widget = None

        self.__p = None


        self.nonlayoutwidgets = []



    #def __del__( self ):
    #    #self.__p.terminate()
    #    if( self.window ):
    #        self.window.close()



    def BindHwnd( self, window_handle ):
        
        self.hwnd = window_handle
        #self._style = win32gui.GetWindowLong( self.hwnd, win32con.GWL_EXSTYLE )
        self.style = win32gui.GetWindowLong( self.hwnd, win32con.GWL_STYLE )
        self.exstyle = win32gui.GetWindowLong( self. hwnd, win32con.GWL_EXSTYLE )



        self.window =  QWindow.fromWinId( window_handle )
        self.window_widget = QWidget.createWindowContainer( self.window )

        self.window_widget.style = self.style
        self.window_widget.exstyle = self.exstyle


        self.window_widget.setFocusPolicy(Qt.StrongFocus)
        self.window_widget.setFocus()
        self.window_widget.grabKeyboard()

        self.nonlayoutwidgets.append( self.window_widget )


        #self.window_widget.installEventFilter( self )
        
        
        self.window_widget.setParent(self)#self.layout().addWidget( self.window_widget )

        ctypes.windll.user32.RegisterHotKey()
        #print( self.window_widget.layout() )
        #self.window_widget.chi


    def UnbindHWnd( self ):
        try:
            # 外部アプリ埋め込んだwindowと、はめ込んでるwindow_widgetを、下記の順番でSetParentする必要がある
            if( self.window_widget ):
                self.window_widget.setParent( None )
                self.window.setParent( None )
                win32gui.SetWindowLong( self.hwnd, win32con.GWL_STYLE, self.style | win32con.WS_VISIBLE )
                win32gui.SetWindowLong( self.hwnd, win32con.GWL_EXSTYLE, self.exstyle )


                self.window_widget = None
                self.window = None

                self.nonlayoutwidgets.clear()

        except Exception as e:
            import traceback
            traceback.print_exc()



    def mousePressEvent( self, a0 ):
        print( "MyWidget::mousePressEvent" )
        return super().mousePressEvent(a0)


    def mouseReleaseEvent( self, a0 ):
        print( "MyWidget::mouseReleaseEvent" )
        return super().mouseReleaseEvent(a0)


    def keyPressEvent( self, event ):
        print( "MyWidget::keyPressEvent" )
        return super().keyPressEvent( event )


    def resizeEvent(self, event):
        for w in self.nonlayoutwidgets:
            w.resize(event.size())
        event.accept()


    def closeEvent( self, event ):
        print( "MyWidget::closeEvent" )

        self.UnbindHWnd()

        #self.__p.terminate()
        if( self.window ):
        #    print('terminating window')
            self.window.close()

        return super(MyWidget, self).closeEvent(event)




if __name__ == "__main__":

    hwnd = int( input( " window handl id >>" ) )
    threadid, process_id = win32process.GetWindowThreadProcessId( hwnd )

    app = QApplication( sys.argv )

    w = MyWidget()

    w.BindHwnd( hwnd )
    w.setGeometry( 0, 0, 800, 600 )
    w.show()

    #w.UnbindHWnd()

    sys.exit( app.exec_() )
