import win32con
import ctypes
from ctypes import *
from ctypes.wintypes import DWORD


from event import KeyboardEvent


################################# Constants ##################################

class Constants:

    # Window Messages
    WM_MOUSEFIRST       = 0x0200
    WM_MOUSEMOVE        = 0x0200
    WM_LBUTTONDOWN      = 0x0201
    WM_LBUTTONUP        = 0x0202
    WM_LBUTTONDBLCLK    = 0x0203
    WM_RBUTTONDOWN      = 0x0204
    WM_RBUTTONUP        = 0x0205
    WM_RBUTTONDBLCLK    = 0x0206
    WM_MBUTTONDOWN      = 0x0207
    WM_MBUTTONUP        = 0x0208
    WM_MBUTTONDBLCLK    = 0x0209
    WM_MOUSEWHEEL       = 0x020A
    WM_MOUSELAST        = 0x020A

    WM_KEYFIRST         = 0x0100
    WM_KEYDOWN          = 0x0100
    WM_KEYUP            = 0x0101
    WM_CHAR             = 0x0102
    WM_DEADCHAR         = 0x0103
    WM_SYSKEYDOWN       = 0x0104
    WM_SYSKEYUP         = 0x0105
    WM_SYSCHAR          = 0x0106
    WM_SYSDEADCHAR      = 0x0107
    # WM_UNICHAR        = 0x0109 windows xp
    WM_KEYLAST          = 0x0109



    # Check flag for GetKeyState/GetAsyncKeyState result
    KEYSTATE_DOWN = 0x8000
    KEYSTATE_TOGGLE = 0x0001







    MsgToName = {
        WM_MOUSEMOVE :      "mouse move",
        WM_LBUTTONDOWN :    "mouse left down", 
        WM_LBUTTONUP :      "mouse left up",
        WM_LBUTTONDBLCLK :  "mouse left double", 
        WM_RBUTTONDOWN :    "mouse right down",
        WM_RBUTTONUP :      "mouse right up", 
        WM_RBUTTONDBLCLK :  "mouse right double",
        WM_MBUTTONDOWN :    "mouse middle down", 
        WM_MBUTTONUP :      "mouse middle up",
        WM_MBUTTONDBLCLK :  "mouse middle double", 
        WM_MOUSEWHEEL :     "mouse wheel",
        WM_KEYDOWN :        "key down", 
        WM_KEYUP :          "key up",
        WM_CHAR :           "key char",
        WM_DEADCHAR :       "key dead char", 
        WM_SYSKEYDOWN :     "key sys down",
        WM_SYSKEYUP :       "key sys up", 
        WM_SYSCHAR :        "key sys char",
        WM_SYSDEADCHAR :    "key sys dead char"
    }




HOOKPROC = WINFUNCTYPE( HRESULT, ctypes.c_int, ctypes.wintypes.WPARAM, ctypes.wintypes.LPARAM )
#HOOKPROC = CFUNCTYPE(c_int, c_int, ctypes.wintypes.HINSTANCE, POINTER(c_void_p))

SetWindowsHookEx = ctypes.windll.user32.SetWindowsHookExA
SetWindowsHookEx.restype = ctypes.wintypes.HHOOK
SetWindowsHookEx.argtypes = [ c_int, HOOKPROC, ctypes.wintypes.HINSTANCE, ctypes.wintypes.DWORD ]



user32 = windll.user32
kernel32 = windll.kernel32


class KBDLLHOOKSTRUCT( Structure ):
    _fields_ = (
        ( "vkCode", DWORD ),
        ( "scanCode", DWORD ),
        ( "flags", DWORD ),
        ( "time", DWORD ),
        ( "dwExtraInfo", DWORD )
    )







class KeyLogger:

    def __init__( self ):
        self.lUser32 = user32
        self.__m_HHook = None# handle to the hook procedure

        #self.__m_refCallbackFuncs = {}# WH_KEYBOARD_LL, WH_MOUSE_LLだけ? WH_MAX個必要?
        self.__m_CustomEventFuncs = {}# Key: WM_***


        pointer = HOOKPROC( self.__KeyboardHookProc )
        self.__InstallHookProc( pointer )



    def __del__( self ):
        self.__UninstallHookProc()



    def BindKeyDown( self, func ):
        self.__m_CustomEventFuncs[ WM_KEYDOWN ] = func
        self.__m_CustomEventFuncs[ WM_SYSKEYDOWN ] = func



    def BindKeyUp( self, func ):
        self.__m_CustomEventFuncs[ WM_KEYUP ] = func
        self.__m_CustomEventFuncs[ WM_SYSKEYUP ] = func



    def BindKeyChar( self, func ):
        self.__m_CustomEventFuncs[ WM_CHAR ] = func
        self.__m_CustomEventFuncs[ WM_DEADCHAR ] = func# dead keys
        self.__m_CustomEventFuncs[ WM_SYSCHAR ] = func # F10/Alt
        self.__m_CustomEventFuncs[ WM_SYSDEADCHAR ] = func



    def BindKeyAll( self, func ):
        self.BindKeyDown( func )
        self.BindKeyUp( func )
        self.BindKeyChar( func )




    def __InstallHookProc( self, pointer ):

        # hinstの入力に注意. https://stackoverflow.com/questions/49898751/setwindowshookex-gives-error-126-module-not-found-when
        # 明示的にやらないと126(module not found)が発生して関数フックできない
        hinst = ctypes.windll.LoadLibrary('user32')._handle
        #hinst = kernel32.GetModuleHandleW( None )

        self.__m_HHook = SetWindowsHookEx(#self.lUser32.SetWindowsHookExA(
            win32con.WH_KEYBOARD_LL,
            pointer,
            hinst,
            0
        )

        if( not self.hooked ):
            print( "Failed hook procedure installation:", str( kernel32.GetLastError() ) )
            return False

        print( "Installed hook procedure" )
        return True



    def __UninstallHookProc( self ):
        if( self.__m_HHook is None ):
            return

        self.lUser32.UnhookWindowsHookEx( self.__m_HHook )
        self.__m_HHook = None



# cLLKeyboardCallback: SetWindowsHookExでフックしてる関数. 
# callback_funcs[ WH_KEYBOARD_LL ]: キーイベント用関数 KeyboardSwitchへのポインタ.
# KeyboardSwitch: メッセージの種類に応じて__m_CustomEventFuncsから該当するカスタムイベントを呼び出す関数.



    def __KeyboardHookProc( self, nCode, wParam, lParam ):

    #TODO: メッセージ(wParam)の種類に応じてコールバック関数を切り替える.

        # Get active window information
        # get window handle
        hwnd = user32.GetForegroundWindow()

        # get process id
        pid = ctypes.wintypes.DWORD()
        tid = user32.GetWindowThreadProcessId( hwnd, ctypes.byref(pid) )

        # get window title
        length = user32.GetWindowTextLengthW( hwnd ) + 1
        title = ctypes.create_unicode_buffer( length )
        user32.GetWindowTextW( hwnd, title, length )

        print( pid, title.value )


        if( user32.GetKeyState(win32con.VK_ESCAPE) & STATE_KEYDOWN ):
            print("\nCtrl pressed. Exiting keylogger." )
            #user32.SendMessageW( hwnd, win32con.WM_CLOSE, 0, 0 )
            user32.PostQuitMessage(0)
            return 0

        if( nCode == win32con.HC_ACTION and wParam == win32con.WM_KEYDOWN ):
            kb = KBDLLHOOKSTRUCT.from_address( lParam )
            print( kb.time )
    #        user32.GetKeyState( win32con.VK_SHIFT )
    #        user32.GetKeyState( win32con.VK_MENU )
            keyboard_state = ( ctypes.c_char * 256 )()
            user32.GetKeyboardState( byref(keyboard_state) )

            # VK_CodeからUnicodeへの変換
            #str = create_unicode_buffer(8)
            #n = user32.ToUnicode( kb.vkCode, kb.scanCode, keyboard_state, str, 8-1, 0 )
            #if( n > 0 ):
            #    if( kb.vkCode == win32con.VK_RETURN ):
            #        print()
            #    else:
            #        print( ctypes.wstring_at(str), end="", flush=True )

            # VK_CodeからASCIIへの変換
            str_ascii = create_string_buffer(2)
            n_ascii = user32.ToAscii( kb.vkCode, kb.scanCode, keyboard_state, str_ascii, 0 )
            if( n_ascii > 0 ):
                if( kb.vkCode == win32con.VK_RETURN ):
                    print()
                else:
                    print( ctypes.string_at(str_ascii), end="", flush=True )



#TODO: Create event instance
#TODO: Call self.__m_CustomEventFuncs event function according to WindowMessage type wParam.
        kb = KBDLLHOOKSTRUCT.from_address( lParam )

        event = KeyboardEvent( wParam, kb.vkCode, kb.scanCode, kb.flags, kb.time, hwnd, title )
        result = self.__m_CustomEventFuncs[ wParam ]( event ) #self.__m_refCallbackFuncs[ win32con.WH_KEYBOARD_LL ]# KeyEvent

        if( result ):
            return user32.CallNextHookEx( self.__m_HHook, nCode, wParam, ctypes.c_longlong(lParam) )# メッセージそのまま通す
        else:
            return True# メッセージ伝播をブロックする




# Convert VK_Code toUnicode
def __VkToUinicode():
    buf = create_unicode_buffer(8)
    length = user32.ToUnicode( kb.vkCode, kb.scanCode, keyboard_state, buf, 8-1, 0 )

    if( length > 0 ):
        return ctypes.wstring_at( buf )

    # cannot convert to ascii
    return 0



# Convert VK_Code to ascii
def __VkToAscii():
    buf = create_string_buffer(2)
    length = user32.ToAscii( kb.vkCode, kb.scanCode, keyboard_state, buf, 0 )

    if( length > 0 ):
        return ctypes.string_at( buf )

    # cannot convert to ascii
    return 0
