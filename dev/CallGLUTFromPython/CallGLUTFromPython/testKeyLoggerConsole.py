import win32process
import pyWinhook
import ctypes

import keylogger



class MyEventFilter( ):#keylogger.EventFilterBase ):

    # キーイベントで処理が必要な引数は取り込んでおく
    def __init__( self, flag=None ):
        self.quitFlag = [flag]


    def KeyDown( self, event ):

        print('MyEventFilter::OnKeyDown()...', event.Key )
    
        if( pyWinhook.HookConstants.IDToName( event.KeyID )=='F' ):
            self.quitFlag[0] = True
            return False

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

        return True

    
    #def RunThread( self ):
    #    import threading
    #    th = threading.Thread( target=self.threadFunc )#  args=None
    #    th.start()


    #def threadFunc( self ):
    #    print( self.quitFlag.value )





if __name__ == '__main__':

    # これは動く
    #logger = keylogger.KeyLogger()
    #filter = MyEventFilter( logger.m_QuitFlag )
    #filter.quitFlag = logger.m_QuitFlag
    #filter.RunThread()


    # こっちが動かない
    logger = keylogger.KeyLogger()

    filter = MyEventFilter()# logger.m_QuitFlag )
    #filter.quitFlag = logger.m_QuitFlag

    logger.BindEventFilter( filter )


    logger.Start()
    ##logger.Stop()
    #logger.Start()

    #logger.UnbindEventFilter()