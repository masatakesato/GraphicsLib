﻿import time
import threading

import pythoncom
import ctypes
import pyWinhook


import win32process


# https://stackoverflow.com/questions/23516150/pyhook-keylogger-thread-not-finishing

# http://pyhook.sourceforge.net/doc_1.5.0/ pyHook api document


#############################################################################
#                                                                           #
#                               Declarations                                #
#                                                                           #
#############################################################################

# Pyhook event function names
PYHOOK_EVENT_FUNC_NAMES = (
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


# forward declaration
class KeyLogger: pass
class EventFilterBase: pass




#############################################################################
#                                                                           #
#                           Keylogger implementation                        #
#                                                                           #
#############################################################################

class KeyLogger:

    # Mutable boolean
    class MutableBool:

        def __init__( self ):
            self.__m_Value = False

        def get( self ):
            return self.__m_Value

        def set( self, value ):
            self.__m_Value = value


#public:
    
    def __init__( self ):

        self.__m_HookManager = pyWinhook.HookManager()
 
        self.__m_Thread = None
        self.m_IsRunning = self.MutableBool()

        self.__m_refEventFilter = None


    def Start( self ):
        self.m_IsRunning.set( True )
        print( "KeyLogger::start()...", self.m_IsRunning.get()  )
        self.__m_Thread = threading.Thread( target=self.__hook )
        self.__m_Thread.start()


    def Stop( self ):
        print( "KeyLogger::stop()...")
        self.m_IsRunning.set( False )
        #self.__m_Thread.join()


    def BindEventFilter( self, filter: EventFilterBase ):
        self.__m_refEventFilter = filter
        self.__m_refEventFilter.BindRunningFlag( self.m_IsRunning )
        #try:
        #    bindRunningFlag = getattr( self.__m_refEventFilter, "BindRunningFunc" )
        #except:
        #    pass


        for funcname in PYHOOK_EVENT_FUNC_NAMES:
            try:
                callback = getattr( self.__m_refEventFilter, funcname )
                bindFunc = getattr( self.__m_HookManager, "Subscribe" + funcname )
                bindFunc( callback )
                print( "Registered callback: " + callback.__name__ )
            except:# Exception as e:
                pass #print( e )


    def UnbindEventFilter( self ):
        if( not self.__m_refEventFilter ):
            return
        self.__m_refEventFilter.BindRuningFlag( None )
        self.__m_refEventFilter = None



#private:

    def __hook( self ):

        # Initialize hook
        self.__m_HookManager.HookKeyboard()
        #self.__m_HookManager.HookMouse()

        # Custom message loop instead of WM_QUIT waiting( pythoncom.PumpMessages() )
        while( self.m_IsRunning.get() ):
            pythoncom.PumpWaitingMessages()
            time.sleep(0.001)

        # Uninitialize hook
        self.__unhook()


    def __unhook( self ):

        print( "KeyLogger::__unhook()...", self.m_IsRunning.get() )

        if( self.m_IsRunning.get()==False ):
            self.m_IsRunning.set( True )
            return 

        self.__m_HookManager.UnhookKeyboard()
        #self.__m_HookManager.UnhookMouse()
        ctypes.windll.user32.PostQuitMessage(0)




#############################################################################
#                                                                           #
#                       EventFilterBase implementation                      #
#                                                                           #
#############################################################################

class EventFilterBase:

    def __init__( self, isRunning=None ):
        self.m_refIsRunning = isRunning


    def BindRunningFlag( self, isRunning ):
        self.m_refIsRunning = isRunning



    #def KeyAll( self, event ):
    #    return False


    #def KeyChar( self, event ):
    #    return False


    #def KeyDown( self, event ):
    #    return False


    #def KeyUp( self, event ):
    #    return False


    #def MouseAll( self, event ):
    #    return False


    #def MouseAllButtons( self, event ):
    #    return False


    #def MouseAllButtonsDbl( self, event ):
    #    return False


    #def MouseAllButtonsDown( self, event ):
    #    return False


    #def MouseAllButtonsUp( self, event ):
    #    return False


    #def MouseLeftDbl( self, event ):
    #    return False


    #def MouseLeftDown( self, event ):
    #    return False


    #def MouseLeftUp( self, event ):
    #    return False


    #def MouseMiddleDbl( self, event ):
    #    return False


    #def MouseMiddleDown( self, event ):
    #    return False


    #def MouseMiddleUp( self, event ):
    #    return False


    #def MouseMove( self, event ):
    #    return False


    #def MouseRightDbl( self, event ):
    #    return False


    #def MouseRightDown( self, event ):
    #    return False


    #def MouseRightUp( self, event ):
    #    return False


    #def MouseWheel( self, event ):
    #    return False


    pass