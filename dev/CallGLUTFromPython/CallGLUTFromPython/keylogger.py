import time
import threading

import pythoncom
import ctypes
import pyWinhook


import win32process


# https://stackoverflow.com/questions/23516150/pyhook-keylogger-thread-not-finishing


STOP_KEY_HANDLER = False

class KeyLogger:

#public:

    def __init__( self ):
        self.hm = pyWinhook.HookManager()
        self.hm.KeyDown = self.__hookEvent

        self.__m_Thread = None


    def start( self ):
        self.__m_Thread = threading.Thread( target=self.__hook )#  args=None
        self.th__m_Threaddaemon = True
        self.__m_Thread.start()



    def stop( self ):
        print( "KeyLogger::stop()...")

        global STOP_KEY_HANDLER
        STOP_KEY_HANDLER = True
        #self.__m_Thread.join()



#private:

    def __hook( self ):
        print( 'hook' )

        self.hm.HookKeyboard()

        while( not STOP_KEY_HANDLER ):
            pythoncom.PumpWaitingMessages()
            time.sleep(0.01)

        self.killkey()

        #pythoncom.PumpMessages()# Wait for WM_QUIT
        #self.label.setText(self.label.text())#for update



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

        hwnd = event.Window
        tid, pid = win32process.GetWindowThreadProcessId( hwnd )

        #if( shift_pressed = pyWinhook.GetKeyState( pyWinhook.HookConstants.VKeyToID('VK_LSHIFT') ) )
        #    print( "shift_pressed: ", event.KeyID )

        if( pyWinhook.HookConstants.IDToName( event.KeyID )=='F' ):
            global STOP_KEY_HANDLER
            STOP_KEY_HANDLER = True
        #	print( "F pressed: ", event.KeyID )

        #if( event.Window == 67204 ):# 特定のウィンドウに対するキー入力を無効化できる
        #    return False


        return True



    def killkey( self ):
        global STOP_KEY_HANDLER

        if( STOP_KEY_HANDLER==False ):
            return 

        self.hm.UnhookKeyboard()
        ctypes.windll.user32.PostQuitMessage(0)

        print("invoked killkey()")


    #============== QtのQTimer使う場合は以下必要 ==================#

    #self.timer = QTimer(self)
    #self.timer.timeout.connect( self.hookThread )
    #self.timer.start()

    #def hookThread( self ):
    #    print( 'hookThread' )
    #    self.hm.KeyDown = self.kewDownEvent
    #    self.hm.HookKeyboard()
    #    pythoncom.PumpMessages()


    #def kewDownEvent( self, event ):
    #    print('hookEventThread')
    #    th = threading.Thread( target=self.hookEvent, args=(event,))
    #    th.daedaemon = True
    #    th.start()
    #    return True


    #def hookEvent( self, event ):
    #    print('hookEvent')

    #    print( 'MessageName:',event.MessageName )
    #    print( 'Message:',event.Message )
    #    print( 'Time:',event.Time )
    #    #...
