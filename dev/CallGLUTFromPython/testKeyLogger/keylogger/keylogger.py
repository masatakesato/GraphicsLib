import threading
import ctypes
import win32con

from .listener import HookManager




class KeyLogger( HookManager ):
    
    def __init__( self ):
        super(KeyLogger, self).__init__()

        self.__m_Thread = None
        self.__m_ThredID = None



    def Start( self ):
        print( "KeyLogger::Start()..." )
        self.__m_Thread = threading.Thread( target=self.__Run )
        self.__m_Thread.start()



    def Stop( self ):
        print( "KeyLogger::Stop()...")
        self.lUser32.PostThreadMessageW( self.__m_Thread.ident, win32con.WM_QUIT, 0, 0 )
        self.__m_Thread.join()



    def __Run( self ):

        # Initialize hook
        self.HookKeyboard()
        self.HookMouse()

        # Wait for message
        msg = ctypes.wintypes.MSG()#MSG()#
        self.lUser32.GetMessageW( ctypes.byref(msg), 0, 0, 0 )
        #pythoncom.PumpMessages()

        # Uninitialize hook
        self.UnhookKeyboard()
        self.UnhookMouse()
