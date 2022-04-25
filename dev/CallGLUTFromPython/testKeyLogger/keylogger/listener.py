import win32con
import ctypes
from ctypes import *
from ctypes.wintypes import DWORD

from .const import Const
from .event import KeyboardEvent



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
        self.__m_KeyState = ( ctypes.c_char * 256 )()
        self.__m_KeyHookHandle = None# handle to the hook procedure
        self.__m_KeyHookFuncPtr = None
        self.__m_CustomEventFuncs = {}# Key: WM_***

        self.__InstallHookProc()



    def __del__( self ):
        self.__UninstallKeyHookProc()



    def BindKeyDown( self, func ):
        self.__m_CustomEventFuncs[ Const.WM_KEYDOWN ] = func
        self.__m_CustomEventFuncs[ Const.WM_SYSKEYDOWN ] = func



    def BindKeyUp( self, func ):
        self.__m_CustomEventFuncs[ Const.WM_KEYUP ] = func
        self.__m_CustomEventFuncs[ Const.WM_SYSKEYUP ] = func



    def BindKeyChar( self, func ):
        self.__m_CustomEventFuncs[ Const.WM_CHAR ] = func
        self.__m_CustomEventFuncs[ Const.WM_DEADCHAR ] = func# dead keys
        self.__m_CustomEventFuncs[ Const.WM_SYSCHAR ] = func # F10/Alt
        self.__m_CustomEventFuncs[ Const.WM_SYSDEADCHAR ] = func



    def BindKeyAll( self, func ):
        self.BindKeyDown( func )
        self.BindKeyUp( func )
        self.BindKeyChar( func )



    def __InstallKeyHookProc( self ):

        # hinstの入力に注意. https://stackoverflow.com/questions/49898751/setwindowshookex-gives-error-126-module-not-found-when
        # 明示的にやらないと126(module not found)が発生して関数フックできない
        hinst = ctypes.windll.LoadLibrary('user32')._handle
        #hinst = kernel32.GetModuleHandleW( None )

        self.__m_KeyHookFuncPtr = HOOKPROC( self.__KeyboardHookProc )

        self.__m_KeyHookHandle = SetWindowsHookEx(#self.lUser32.SetWindowsHookExA(#
            win32con.WH_KEYBOARD_LL,
            self.__m_KeyHookFuncPtr,
            hinst,
            0
        )


        if( not self.__m_KeyHookHandle ):
            print( "Failed hook procedure installation:", str( kernel32.GetLastError() ) )
            return False

        print( "Installed hook procedure" )
        return True



    def __UninstallKeyHookProc( self ):
        print( "Uninstalled hook procedure" )
        if( self.__m_KeyHookHandle is None ):
            return

        self.lUser32.UnhookWindowsHookEx( self.__m_KeyHookHandle )
        self.__m_KeyHookHandle = None
        self.__m_KeyHookFuncPtr = None



    def __KeyboardHookProc( self, nCode, wParam, lParam ):

        # Get active window information
        # get window handle
        hwnd = user32.GetForegroundWindow()

        # get process id
        #pid = ctypes.wintypes.DWORD()
        #tid = user32.GetWindowThreadProcessId( hwnd, ctypes.byref(pid) )

        # get window title
        length = user32.GetWindowTextLengthW( hwnd ) + 1
        title = ctypes.create_unicode_buffer( length )
        user32.GetWindowTextW( hwnd, title, length )

        #print( pid, title.value )

        #if( user32.GetKeyState(Const.VK_ESCAPE) & Const.STATE_KEYDOWN ):
        #    print("\nCtrl pressed. Exiting keylogger." )
        #    #user32.SendMessageW( hwnd, win32con.WM_CLOSE, 0, 0 )
        #    user32.PostQuitMessage(0)
        #    return 0


        kb = KBDLLHOOKSTRUCT.from_address( lParam )

        e = KeyboardEvent( wParam, kb.vkCode, kb.scanCode, kb.flags, kb.time, hwnd, title )
        func = self.__m_CustomEventFuncs.get( wParam )
        result = func( e ) if func else True #self.__m_refCallbackFuncs[ win32con.WH_KEYBOARD_LL ]# KeyEvent

        if( result ):
            self.__UpdateKeyState( kb.vkCode, wParam )
            return user32.CallNextHookEx( self.__m_KeyHookHandle, nCode, wParam, ctypes.c_longlong(lParam) )# メッセージそのまま通す
        else:
            return True# メッセージ伝播をブロックする



    # Convert VK_Code toUnicode
    def __VkToUinicode( self ):

        #user32.GetKeyboardState( byref(self.__m_KeyState) )
        buf = create_unicode_buffer(8)

        length = user32.ToUnicode( kb.vkCode, kb.scanCode, self.__m_KeyState, buf, 8-1, 0 )

        if( length > 0 ):
            return ctypes.wstring_at( buf )

        # cannot convert to ascii
        return 0



    # Convert VK_Code to ascii
    def __VkToAscii( self ):

        #user32.GetKeyboardState( byref(self.__m_KeyState) )
        buf = create_string_buffer(2)

        length = user32.ToAscii( kb.vkCode, kb.scanCode, self.__m_KeyState, buf, 0 )

        if( length > 0 ):
            return ctypes.string_at( buf )

        # cannot convert to ascii
        return 0



    def __SetKeyState( self, vkey, down ):

        # Bitwise OR operation for bytes https://techoverflow.net/2020/09/27/how-to-perform-bitwise-boolean-operations-on-bytes-in-python3/
        def OR(a, b):
            result_int = int.from_bytes(a, byteorder="big") | int.from_bytes(b, byteorder="big")
            return result_int.to_bytes(max(len(a), len(b)), byteorder="big")

	    #(1 > 0) ? True : False
        if( vkey == Const.VK_MENU or vkey == Const.VK_LMENU or vkey == Const.VK_RMENU ):
            self.__m_KeyState[ vkey ] = 0x80 if down else 0x00
            self.__m_KeyState[ Const.VK_MENU ] = OR( self.__m_KeyState[ Const.VK_LMENU ], self.__m_KeyState[ Const.VK_RMENU ] )

        elif( vkey == Const.VK_SHIFT or vkey == Const.VK_LSHIFT or vkey == Const.VK_RSHIFT):
            self.__m_KeyState[ vkey ] = 0x80 if down else 0x00
            self.__m_KeyState[ Const.VK_SHIFT ] = OR( self.__m_KeyState[ Const.VK_LSHIFT ], self.__m_KeyState[ Const.VK_RSHIFT ] )

        elif( vkey == Const.VK_CONTROL or vkey == Const.VK_LCONTROL or vkey == Const.VK_NUMLOCK ):
            self.__m_KeyState[ vkey ] = 0x80 if down else 0x00
            self.__m_KeyState[ Const.VK_CONTROL ] = OR( self.__m_KeyState[ Const.VK_LCONTROL ], self.__m_KeyState[ Const.VK_NUMLOCK ] )

        elif( vkey == Const.VK_NUMLOCK and not down ):
            self.__m_KeyState[ Const.VK_NUMLOCK ] = not self.__m_KeyState[ Const.VK_NUMLOCK ]

        elif( vkey == Const.VK_CAPITAL and not down ):
            self.__m_KeyState[ Const.VK_CAPITAL ] = not self.__m_KeyState[ Const.VK_CAPITAL ]

        elif( vkey == Const.VK_SCROLL and not down ):
            self.__m_KeyState[ Const.VK_SCROLL ] = not self.__m_KeyState[ Const.VK_SCROLL ]



    def __UpdateKeyState( self, vkey, msg ):

        if( msg == Const.WM_KEYDOWN or msg == Const.WM_SYSKEYDOWN ):
            self.__SetKeyState( vkey, 1 )

        elif(msg == Const.WM_KEYUP or msg == Const.WM_SYSKEYUP):
            self.__SetKeyState( vkey, 0 )

