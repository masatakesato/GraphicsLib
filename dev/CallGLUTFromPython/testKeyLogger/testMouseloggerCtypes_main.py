import threading
import win32con
import win32api
import ctypes
from ctypes import *
from ctypes.wintypes import DWORD, POINT

user32 = windll.user32
kernel32 = windll.kernel32


class MSLLHOOKSTRUCT( Structure ):
    _fields_ = (
        ( "pt", POINT ),    
        ( "mouseData", DWORD ),
        ( "flags", DWORD ),
        ( "time", DWORD ),
        ( "dwExtraInfo", DWORD )
    )

HOOKPROC = WINFUNCTYPE( HRESULT, ctypes.c_int, ctypes.wintypes.WPARAM, ctypes.wintypes.LPARAM )
#HOOKPROC = CFUNCTYPE( c_int, c_int, ctypes.wintypes.HINSTANCE, POINTER(c_void_p))

SetWindowsHookEx = ctypes.windll.user32.SetWindowsHookExA
SetWindowsHookEx.restype = ctypes.wintypes.HHOOK
SetWindowsHookEx.argtypes = [ c_int, HOOKPROC, ctypes.wintypes.HINSTANCE, ctypes.wintypes.DWORD ]




class EventBase:

    def __init__( self, msg, time, hwnd, windowtitle, injected ):
        self.__m_MessageName = None
        self.__m_Message = msg
        self.__m_Time = time
        self.__m_WindowHandle = hwnd
        self.__m_WindowName = windowtitle
        self.__m_Injected = injected



class MouseEvent( EventBase ):

    def __init__( self, msg, x, y, mousedata, flags, time, hwnd, windowtitle ):
        super(MouseEvent, self).__init__( msg, time, hwnd, windowtitle, flags & 0x01 )

        self.__m_Position = (x, y)
        self.__m_Wheel = 0 if mousedata==0 else 1 if mousedata>0 else -1

#*   MessageName: mouse left down ------> Messageの名前. MsgToName辞書
#*   Message: 256 ---------------------> wParam. WM_LBUTTONDOWNとかWM_LBUTTONUPとか.
#*   Time: 2426687 ---------------------> KBDLLHOOKSTRUCT.timeで取得可能
#*   Window handle: 3409886
#*   WindowName: C:\WINDOWS\system32\cmd.exe
#*    Position:    -------------------------> MSLLHOOKSTRUCT.pt.x/y で取得可能
#?    Wheel
#    Injected     --------------------> プログラムで生成されたコマンドかどうかフラグ. MSLLHOOKSTRUCT.flags & 0x01 で取得可能.



class KeyboardEvent( EventBase ):

    def __init__( self, msg, vkcode, scancode, flags, time, hwnd, windowtitle ):
        super(KeyboardEvent, self).__init__( msg, time, hwnd, windowtitle, flags & 0x20 )

        #self.__m_Ascii     = None
        self.__m_KeyID      = vkcode
        #self.__m_Key       = VkToKeyName[ vkcode ]
        self.__m_ScanCode   = scancode
        self.__m_Extended   = flags & 0x01
        self.__m_Alt        = flags & 0x20
        self.__m_Transition = flags & 0x80

#*   MessageName: key down ------------> Messageの名前. MsgToName辞書
#*   Message: 256 ---------------------> wParam. WM_KEYDOWNとかWM_KEYUPとか.
#*   Time: 2426687 ---------------------> KBDLLHOOKSTRUCT.timeで取得可能
#*   Window handle: 3409886
#*   WindowName: C:\WINDOWS\system32\cmd.exe
#*    Ascii: 0      ---------------------> 506行目からの処理と等価. 497行目からの処理使えばUnicodeもできる.
#*    Key: Right     --------------------> pyHookが変換テーブル使ってVKからキー名に変換してる. 余裕あったらtestKeydownSimulation_main.pyのコードから作る
#*    KeyID: 39       --------------------> Virtual key id. KBDLLHOOKSTRUCT::vkCodeで取得可能 ( = 0x27 )
#*    ScanCode: 77   --------------------> KBDLLHOOKSTRUCT.scanCodeで取得可能
#*    Extended: 1    --------------------> 拡張キーかどうか判定フラグ. KBDLLHOOKSTRUCT.flags & 0x01 で取得可能 
#*    Injected: 0     --------------------> プログラムで生成されたコマンドかどうかフラグ. KBDLLHOOKSTRUCT.flags & 0x10 で取得可能.
#*    Alt 0            -------------------> Altキー押されてるかどうかフラグ. KBDLLHOOKSTRUCT.flags & 0x20 で取得可能
#*    Transition 0    --------------------> Up/Downの状態遷移かどうかフラグ. KBDLLHOOKSTRUCT.flags & 0x80 で取得可能




class MouseLogger:

    def __init__( self ):
        self.lUser32 = user32
        self.hooked = None


    def InstallHookProc( self, pointer ):

        # hinstの入力に注意. https://stackoverflow.com/questions/49898751/setwindowshookex-gives-error-126-module-not-found-when
        # 明示的にやらないと126(module not found)が発生して関数フックできない
        hinst = ctypes.windll.LoadLibrary('user32')._handle
        #hinst = kernel32.GetModuleHandleW( None )._handle

        self.hooked = SetWindowsHookEx(#self.lUser32.SetWindowsHookExA(
            win32con.WH_MOUSE_LL,
            pointer,
            hinst,
            0
        )

        if( not self.hooked ):
            print( "Failed hook procedure installation:", str( kernel32.GetLastError() ) )
            return False

        print( "Installed hook procedure" )
        return True


    def UninstallHookProc( self ):
        print("-----")
        if( self.hooked is None ):
            
            return

        self.lUser32.UnhookWindowsHookEx( self.hooked )
        self.hooked = None
        print( "Uninstalled hook procedure" )



def HookProc( nCode, wParam, lParam ):
    
    MsgToName = {
        win32con.WM_MOUSEMOVE : 'mouse move',
        win32con.WM_LBUTTONDOWN : 'mouse left down', 
        win32con.WM_LBUTTONUP : 'mouse left up',
        win32con.WM_LBUTTONDBLCLK : 'mouse left double', 
        win32con.WM_RBUTTONDOWN : 'mouse right down',
        win32con.WM_RBUTTONUP : 'mouse right up', 
        win32con.WM_RBUTTONDBLCLK : 'mouse right double',
        win32con.WM_MBUTTONDOWN : 'mouse middle down', 
        win32con.WM_MBUTTONUP : 'mouse middle up',
        win32con.WM_MBUTTONDBLCLK : 'mouse middle double', 
        win32con.WM_MOUSEWHEEL : 'mouse wheel',
        win32con.WM_KEYDOWN : 'key down', 
        win32con.WM_KEYUP : 'key up',
        win32con.WM_CHAR : 'key char',
        win32con.WM_DEADCHAR : 'key dead char', 
        win32con.WM_SYSKEYDOWN : 'key sys down',
        win32con.WM_SYSKEYUP : 'key sys up', 
        win32con.WM_SYSCHAR : 'key sys char',
        win32con.WM_SYSDEADCHAR : 'key sys dead char'
    }

#*   MessageName: mouse left down ------> Messageの名前. MsgToName辞書
#*   Message: 256 ---------------------> wParam. WM_LBUTTONDOWNとかWM_LBUTTONUPとか.
#*   Time: 2426687 ---------------------> KBDLLHOOKSTRUCT.timeで取得可能
#*   Window handle: 3409886
#*   WindowName: C:\WINDOWS\system32\cmd.exe
#*    Position:    -------------------------> MSLLHOOKSTRUCT.pt.x/y で取得可能
#?    Wheel
#    Injected     --------------------> プログラムで生成されたコマンドかどうかフラグ. MSLLHOOKSTRUCT.flags & 0x01 で取得可能.

    if( nCode != win32con.HC_ACTION ):
        return ctypes.windll.user32.CallNextHookEx( None, nCode, wParam, ctypes.c_longlong(lParam) )

    ## Get active window information
    ## get window handle
    #hwnd = ctypes.windll.user32.GetForegroundWindow()

    ## get process id
    #pid = ctypes.wintypes.DWORD()    
    #tid = ctypes.windll.user32.GetWindowThreadProcessId( hwnd, ctypes.byref(pid) )

    ## get window title
    #length = user32.GetWindowTextLengthW( hwnd ) + 1
    #title = ctypes.create_unicode_buffer( length )
    #user32.GetWindowTextW( hwnd, title, length )

    #print( pid, title.value )


    
    kb = MSLLHOOKSTRUCT.from_address( lParam )
    #print( kb.pt.x, kb.pt.y )
    print( MsgToName[wParam], -1 if kb.mouseData > 0x80000000 else 1 )

    

    #print("mouseproc...")

    return ctypes.windll.user32.CallNextHookEx( None, nCode, wParam, ctypes.c_longlong(lParam) )


# https://jpdebug.com/p/2095065
# Ctrl+CでKeyboardInterrupt例外発生しないようにする方法
class CustomException( Exception ):
    pass

import signal
def handler( signum, frame ):
    print( "Please use exit to exit" )
    user32.PostQuitMessage(0)
    raise CustomException()




# TODO: 別スレッドで動かして停止処理かける必要がある
#  メッセージループ使って制御
# MouseLogger::Start
# MouseLogger::Stop
# カスタムコールバック関数をぶら下げられるようにしたい

# TODO: イベントクラスの設計
# class EventBase
# class MouseEvent( EvantBase )
# class KeyboardEvent( EvantBase )


if __name__=="__main__":

    signal.signal( signal.SIGINT, handler )

    MouseLogger = MouseLogger()
    pointer = HOOKPROC( HookProc )
    #MouseLogger.InstallHookProc( pointer )
    #msg = ctypes.wintypes.MSG()
    #user32.GetMessageA( byref(msg), 0, 0, 0 )
    

    if( MouseLogger.InstallHookProc( pointer ) ):
        try:
            msg = ctypes.wintypes.MSG()
            user32.GetMessageA( byref(msg), 0, 0, 0 )

        except CustomException:
            print("-----")
            MouseLogger.UninstallHookProc()

    else:
        pass
