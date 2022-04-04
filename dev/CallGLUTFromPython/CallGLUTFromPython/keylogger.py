import time
import threading
import functools

import pythoncom
import ctypes
import pyWinhook


import win32process


# https://stackoverflow.com/questions/23516150/pyhook-keylogger-thread-not-finishing



CONST_EVENT_FUNC_NAMES = (
    "KeyAll",
    "KeyChar",
    "KeyDown",
    "KeyUp",
    "MouseAll",
    "MouseAllButtons",
    "MouseAllButtonsDbl",
    "MouseAllButtonsDown",
    "MouseAllButtonsUp",
    "MouseLeftDbl",
    "MouseLeftDown",
    "MouseLeftUp",
    "MouseMiddleDbl",
    "MouseMiddleDown",
    "MouseMiddleUp",
    "MouseMove",
    "MouseRightDbl",
    "MouseRightDown",
    "MouseRightUp",
    "MouseWheel"
)




class KeyLogger:


    class QuitFlag:
        def __init__( self ):
            self.value = False


#public:
    
    def __init__( self ):
        self.hm = pyWinhook.HookManager()
        self.hm.KeyDown = self.__KeyDown#functools.partial( self.__KeyDownEvent, 


        self.__m_Thread = None
        self.__m_Lock = threading.Lock()

        self.__m_KeyDownEvent = None


        self.__m_refEventFilter = None


        self.m_QuitFlag = self.QuitFlag()



    def Start( self ):
        print( "KeyLogger::start()...", self.m_QuitFlag.value  )

        self.__m_Thread = threading.Thread( target=self.__hook )#  args=None
        self.th__m_Threaddaemon = True
        self.__m_Thread.start()


    def Stop( self ):
        print( "KeyLogger::stop()...")
        self.m_QuitFlag.value = True
        #self.__m_Thread.join()



    def BindEventFilter( self, filter ):
        self.__m_refEventFilter = filter

        for funcname in CONST_EVENT_FUNC_NAMES:
            try:
                callback = getattr( self.__m_refEventFilter, funcname )
                bindFunc = getattr( self.hm, "Subscribe" + funcname )
                bindFunc( functools.partial( callback, quitFlag=self.m_QuitFlag ) )
                print( "Registered callback: " + callback.__name__ )
                

            except:# Exception as e:
                pass
                #print( e )
                #print( "Registered callback: " + callback.__name__ )


    def UnbindEventFilter( self ):
        if( not self.__m_refEventFilter ):
            return
        self.__m_refEventFilter.m_QuitFlag = None
        self.__m_refEventFilter = None



#private:

    def __hook( self ):

        # Initialize hook
        self.hm.HookKeyboard()
        #self.hm.HookMouse()

        # Custom message loop instead of WM_QUIT waiting( pythoncom.PumpMessages() )
        while( not self.m_QuitFlag.value ):
            pythoncom.PumpWaitingMessages()
            time.sleep(0.001)


        # Uninitialize hook
        self.__unhook()


    def __unhook( self ):

        print( "KeyLogger::__unhook()...", self.m_QuitFlag.value )

        if( self.m_QuitFlag.value==True ):
            self.m_QuitFlag.value = False
            return 

        self.hm.UnhookKeyboard()
        #self.hm.UnhookMouse()
        ctypes.windll.user32.PostQuitMessage(0)



    def __KeyDown( self, event ):

        if( pyWinhook.HookConstants.IDToName( event.KeyID )=='Q' ):
            self.m_QuitFlag.value = True
            return False

        try:
            return self.__m_KeyDownEvent( event )

        except Exception as e:
            print( e )
            return False






class EventFilterBase:

    #def KeyAll( self, event, quitFlag ):
    #    return False


    #def KeyChar( self, event, quitFlag ):
    #    return False


    #def KeyDown( self, event, quitFlag ):
    #    return False


    #def KeyUp( self, event, quitFlag ):
    #    return False


    #def MouseAll( self, event, quitFlag ):
    #    return False


    #def MouseAllButtons( self, event, quitFlag ):
    #    return False


    #def MouseAllButtonsDbl( self, event, quitFlag ):
    #    return False


    #def MouseAllButtonsDown( self, event, quitFlag ):
    #    return False


    #def MouseAllButtonsUp( self, event, quitFlag ):
    #    return False


    #def MouseLeftDbl( self, event, quitFlag ):
    #    return False


    #def MouseLeftDown( self, event, quitFlag ):
    #    return False


    #def MouseLeftUp( self, event, quitFlag ):
    #    return False


    #def MouseMiddleDbl( self, event, quitFlag ):
    #    return False


    #def MouseMiddleDown( self, event, quitFlag ):
    #    return False


    #def MouseMiddleUp( self, event, quitFlag ):
    #    return False


    #def MouseMove( self, event, quitFlag ):
    #    return False


    #def MouseRightDbl( self, event, quitFlag ):
    #    return False


    #def MouseRightDown( self, event, quitFlag ):
    #    return False


    #def MouseRightUp( self, event, quitFlag ):
    #    return False


    #def MouseWheel( self, event, quitFlag ):
    #    return False

    pass