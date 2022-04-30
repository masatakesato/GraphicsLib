import win32con
import ctypes
from ctypes import *
from ctypes.wintypes import DWORD, POINT, WPARAM, LPARAM, HHOOK, HINSTANCE

from .const import Const
from .event import KeyboardEvent, MouseEvent



HOOKPROC = WINFUNCTYPE( HRESULT, ctypes.c_int, WPARAM, LPARAM )
#HOOKPROC = CFUNCTYPE(c_int, c_int, ctypes.wintypes.HINSTANCE, POINTER(c_void_p))

SetWindowsHookEx = ctypes.windll.user32.SetWindowsHookExA
SetWindowsHookEx.restype = ctypes.wintypes.HHOOK
SetWindowsHookEx.argtypes = [ c_int, HOOKPROC, HINSTANCE, DWORD ]


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




class MSLLHOOKSTRUCT( Structure ):
    _fields_ = (
        ( "pt", POINT ),    
        ( "mouseData", DWORD ),
        ( "flags", DWORD ),
        ( "time", DWORD ),
        ( "dwExtraInfo", DWORD )
    )





class HookManager:

    def __init__( self ):

        self.lUser32 = user32

        self.__m_KeyState = ( ctypes.c_char * 256 )()
        self.__m_KeyHookHandle = None# handle to the hook procedure
        self.__m_KeyHookFuncPtr = None

        self.__m_MouseHookHandle = None
        self.__m_MouseHookFuncPtr = None

        self.__m_CustomEventFuncs = {}# Key: WM_***



    def __del__( self ):
        self.UnhookKeyboard()
        self.UnhookMouse()



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



    def BindMouseMove( self, func ):
        self.__m_CustomEventFuncs[ Const.WM_MOUSEMOVE ] = func



    def BindMouseLeftDown( self, func ):
        self.__m_CustomEventFuncs[ Const.WM_LBUTTONDOWN ] = func



    def BindMouseLeftUp( self, func ):
        self.__m_CustomEventFuncs[ Const.WM_LBUTTONUP ] = func



    def BindMouseLeftDbl( self, func ):
        self.__m_CustomEventFuncs[ Const.WM_LBUTTONDBLCLK ] = func



    def BindMouseRightDown( self, func ):
        self.__m_CustomEventFuncs[ Const.WM_RBUTTONDOWN ] = func



    def BindMouseRightUp( self, func ):
        self.__m_CustomEventFuncs[ Const.WM_RBUTTONUP ] = func



    def BindMouseRightDbl( self, func ):
        self.__m_CustomEventFuncs[ Const.WM_RBUTTONDBLCLK ] = func



    def BindMouseMiddleDown( self, func ):
        self.__m_CustomEventFuncs[ Const.WM_MBUTTONDOWN ] = func



    def BindMouseMiddleUp( self, func ):
        self.__m_CustomEventFuncs[ Const.WM_MBUTTONUP ] = func



    def BindMouseMiddleDbl( self, func ):
        self.__m_CustomEventFuncs[ Const.WM_MBUTTONDBLCLK ] = func



    def BindMouseWheel( self, func ):
        self.__m_CustomEventFuncs[ Const.WM_MOUSEWHEEL ] = func



    def BindMouseAllButtonsDown( self, func ):
        self.BindMouseLeftDown( func )
        self.BindMouseRightDown( func )
        self.BindMouseMiddleDown( func )



    def BindMouseAllButtonsUp( self, func ):
        self.BindMouseLeftUp( func )
        self.BindMouseRightUp( func )
        self.BindMouseMiddleUp( func )



    def BindMouseAllButtonsDbl( self, func ):
        self.BindMouseLeftDbl( func )
        self.BindMouseRightDbl( func )
        self.BindMouseMiddleDbl( func )



    def BindMouseAllButtons( self, func ):
        self.BindMouseAllButtonsDown( func )
        self.BindMouseAllButtonsUp( func )
        self.BindMouseAllButtonsDbl( func )



    def BindMouseAll( self, func ):
        self.BindMouseMove( func )
        self.BindMouseWheel( func )
        self.BindMouseAllButtons( func )



    def HookKeyboard( self ):

        print( "Hooking keyboard procedure...", end="" )

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
            print( "Failed: ", str( kernel32.GetLastError() ) )
            return False

        print( "Done." )
        
        return True



    def HookMouse( self ):

        print( "Hooking mouse procedure...", end="" )

        # hinstの入力に注意. https://stackoverflow.com/questions/49898751/setwindowshookex-gives-error-126-module-not-found-when
        # 明示的にやらないと126(module not found)が発生して関数フックできない
        hinst = ctypes.windll.LoadLibrary('user32')._handle
        #hinst = kernel32.GetModuleHandleW( None )._handle

        self.__m_MouseHookFuncPtr = HOOKPROC( self.__MouseHookProc )

        self.__m_MouseHookHandle = SetWindowsHookEx(#self.lUser32.SetWindowsHookExA(
            win32con.WH_MOUSE_LL,
            self.__m_MouseHookFuncPtr,
            hinst,
            0
        )

        if( not self.__m_MouseHookHandle ):
            print( "Failed: ", str( kernel32.GetLastError() ) )
            return False

        print( "Done." )
        
        return True



    def UnhookKeyboard( self ):
        print( "Unhooking keyboard procedure" )
        if( self.__m_KeyHookHandle is None ):
            return

        self.lUser32.UnhookWindowsHookEx( self.__m_KeyHookHandle )
        self.__m_KeyHookHandle = None
        self.__m_KeyHookFuncPtr = None



    def UnhookMouse( self ):
        print( "Unhooking mouse procedure" )
        if( self.__m_MouseHookHandle is None ):
            return

        self.lUser32.UnhookWindowsHookEx( self.__m_MouseHookHandle )
        self.__m_MouseHookHandle = None
        self.__m_MouseHookFuncPtr = None



    def __KeyboardHookProc( self, nCode, wParam, lParam ):

        # Get active window information
        # get window handle
        hwnd = self.lUser32.GetForegroundWindow()

        # get process id
        #pid = ctypes.wintypes.DWORD()
        #tid = self.lUser32.GetWindowThreadProcessId( hwnd, ctypes.byref(pid) )

        # get window title
        length = self.lUser32.GetWindowTextLengthW( hwnd ) + 1
        title = ctypes.create_unicode_buffer( length )
        self.lUser32.GetWindowTextW( hwnd, title, length )

        #print( pid, title.value )

        #if( self.lUser32.GetKeyState(Const.VK_ESCAPE) & Const.STATE_KEYDOWN ):
        #    print("\nCtrl pressed. Exiting keylogger." )
        #    #self.lUser32.SendMessageW( hwnd, win32con.WM_CLOSE, 0, 0 )
        #    self.lUser32.PostQuitMessage(0)
        #    return 0


        kb = KBDLLHOOKSTRUCT.from_address( lParam )

        e = KeyboardEvent( wParam, kb.vkCode, kb.scanCode, self.__VkToAscii(kb), kb.flags, kb.time, hwnd, title.value )
        func = self.__m_CustomEventFuncs.get( wParam )
        result = func( e ) if func else True# call custom keyevent func

        if( result ):
            self.__UpdateKeyState( kb.vkCode, wParam )
            return self.lUser32.CallNextHookEx( self.__m_KeyHookHandle, nCode, wParam, ctypes.c_longlong(lParam) )# メッセージそのまま通す
        else:
            return True# メッセージ伝播をブロックする



    def __MouseHookProc( self, nCode, wParam, lParam ):

        # Get active window information
        # get window handle
        hwnd = self.lUser32.GetForegroundWindow()

        # get process id
        #pid = ctypes.wintypes.DWORD()
        #tid = self.lUser32.GetWindowThreadProcessId( hwnd, ctypes.byref(pid) )

        # get window title
        length = self.lUser32.GetWindowTextLengthW( hwnd ) + 1
        title = ctypes.create_unicode_buffer( length )
        self.lUser32.GetWindowTextW( hwnd, title, length )

        #print( pid, title.value )

        ms = MSLLHOOKSTRUCT.from_address( lParam )

        e = MouseEvent( wParam, ms.pt.x, ms.pt.y, ms.mouseData, ms.flags, ms.time, hwnd, title.value )
        func = self.__m_CustomEventFuncs.get( wParam )
        result = func( e ) if func else True# call custom mouseevent func

        if( result ):
            return self.lUser32.CallNextHookEx( self.__m_MouseHookHandle, nCode, wParam, ctypes.c_longlong(lParam) )# メッセージそのまま通す
        else:
            return True# メッセージ伝播をブロックする



    # Convert VK_Code toUnicode
    def __VkToUinicode( self, kb ):

        #self.lUser32.GetKeyboardState( byref(self.__m_KeyState) )
        buf = create_unicode_buffer(8)

        length = self.lUser32.ToUnicode( kb.vkCode, kb.scanCode, self.__m_KeyState, buf, 8-1, 0 )

        if( length > 0 ):
            return int.from_bytes( ctypes.string_at( buf ), byteorder="big" )

        # cannot convert to ascii
        return 0



    # Convert VK_Code to ascii
    def __VkToAscii( self, kb ):

        #self.lUser32.GetKeyboardState( byref(self.__m_KeyState) )
        buf = create_string_buffer(2)

        length = self.lUser32.ToAscii( kb.vkCode, kb.scanCode, self.__m_KeyState, buf, 0 )

        if( length > 0 ):
            return int.from_bytes( ctypes.string_at( buf ), byteorder="big" )

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

        elif( vkey == Const.VK_CONTROL or vkey == Const.VK_LCONTROL or vkey == Const.VK_RCONTROL ):
            self.__m_KeyState[ vkey ] = 0x80 if down else 0x00
            self.__m_KeyState[ Const.VK_CONTROL ] = OR( self.__m_KeyState[ Const.VK_LCONTROL ], self.__m_KeyState[ Const.VK_RCONTROL ] )

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
