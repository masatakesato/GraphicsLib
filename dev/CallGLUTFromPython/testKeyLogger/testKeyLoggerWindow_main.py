# https://code.tiblab.net/python/pyhook

import sys
import threading

from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *

import pythoncom, pyWinhook, ctypes

import win32api
import win32con
import win32gui
import win32process


import pyhookkeylogger








class MyEventFilter( pyhookkeylogger.EventFilterBase ):

    def __init__( self, label ):
        super(MyEventFilter, self).__init__()

        self.label = None#label


    def KeyDown( self, event ):

        print('MyEventFilter::OnKeyDown()...', event.Key )

        print( 'MessageName:',event.MessageName )
        print( 'Message:',event.Message )
        print( 'Time:',event.Time )
        print( 'Window handle:',event.Window )
        print( 'WindowName:',event.WindowName )
        print( 'Ascii:', event.Ascii, chr(event.Ascii) )
        print( 'Key:', event.Key )
        print( 'KeyID:', event.KeyID )
        print( 'ScanCode:', event.ScanCode )
        print( 'Extended:', event.Extended )
        print( 'Injected:', event.Injected )
        print( 'Alt', event.Alt )
        print( 'Transition', event.Transition )
        print( '---' )

        #if( shift_pressed = pyWinhook.GetKeyState( pyWinhook.HookConstants.VKeyToID('VK_LSHIFT') ) )
        #    print( "shift_pressed: ", event.KeyID )

        if( pyWinhook.HookConstants.IDToName( event.KeyID )=='A' ):
            print( "A pressed: ", event.KeyID )

        #if( event.Window == 67204 ):# 特定のウィンドウに対するキー入力を無効化できる
        #    return False

        # ウィンドウハンドルからプロセスID, スレッドIDを捕まえる
        hwnd = event.Window
        tid, pid = win32process.GetWindowThreadProcessId( hwnd )

        self.label.setText( event.Key )

        return True



class Window( QFrame ):

    def __init__(self, parent=None):
        super(Window, self).__init__(parent)

        self.label = QLabel('hook:',self)

        self.__m_KeyLogger = None

        #self.__m_KeyLogger = pyhookkeylogger.KeyLogger()
        #self.__m_KeyLogger.bindKeyEvent( self.__hookEvent )
        #self.__m_KeyLogger.start()


    def bindKeyLogger( self, logger ):
        self.__m_KeyLogger = logger
        #self.__m_KeyLogger.bindKeyDownEvent( self.__hookEvent )
        self.__m_KeyLogger.Start()


    def closeEvent( self,event ):
        print( 'closeEvent' )
        self.__m_KeyLogger.Stop()


    def __hookEvent( self, event ):
        print('hookEvent')
        
        print( 'MessageName:',event.MessageName )
        print( 'Message:',event.Message )
        print( 'Time:',event.Time )
        print( 'Window handle:',event.Window )
        print( 'WindowName:',event.WindowName )
        print( 'Ascii:', event.Ascii, chr(event.Ascii) )
        print( 'Key:', event.Key )
        print( 'KeyID:', event.KeyID )
        print( 'ScanCode:', event.ScanCode )
        print( 'Extended:', event.Extended )
        print( 'Injected:', event.Injected )
        print( 'Alt', event.Alt )
        print( 'Transition', event.Transition )
        print( '---' )

        #if( shift_pressed = pyWinhook.GetKeyState( pyWinhook.HookConstants.VKeyToID('VK_LSHIFT') ) )
        #    print( "shift_pressed: ", event.KeyID )

        if( pyWinhook.HookConstants.IDToName( event.KeyID )=='A' ):
        	print( "A pressed: ", event.KeyID )

        #if( event.Window == 67204 ):# 特定のウィンドウに対するキー入力を無効化できる
        #    return False

        # ウィンドウハンドルからプロセスID, スレッドIDを捕まえる
        hwnd = event.Window
        tid, pid = win32process.GetWindowThreadProcessId( hwnd )


        self.label.setText( event.Key )

        return True



if __name__ == '__main__':
    app = QApplication( sys.argv )
    window = Window()

    filter = MyEventFilter( window.label )
    filter.label = window.label

    logger = pyhookkeylogger.KeyLogger()
    logger.BindEventFilter( filter )

    window.bindKeyLogger( logger )

    window.show()
    sys.exit( app.exec_() )



    #logger = pyhookkeylogger.KeyLogger()
    #logger.start()
    #print("----------------")
    ##logger.stop()
