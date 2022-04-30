﻿# https://stackoverflow.com/questions/11906925/python-simulate-keydown

from .const import Const

import ctypes
from ctypes.wintypes import WORD, DWORD, LONG, POINT




ULONG_PTR = ctypes.POINTER(DWORD)




#############################################################################
#                                                                           #
#                           KEYBDINPUT structure                            #
#                                                                           #
#############################################################################

# https://docs.microsoft.com/ja-jp/windows/win32/api/winuser/ns-winuser-keybdinput

class KEYBDINPUT( ctypes.Structure ):
    _fields_ = (
            ( "wVk", WORD ),
            ( "wScan", WORD ),
            ( "dwFlags", DWORD ),
            ( "time", DWORD ),
            ( "dwExtraInfo", ULONG_PTR )
    )



# dwFlags 
KEYEVENTF_EXTENDEDKEY   = 0x0001
KEYEVENTF_KEYUP         = 0x0002
KEYEVENTF_SCANCODE      = 0x0008
KEYEVENTF_UNICODE       = 0x0004




#############################################################################
#                                                                           #
#                           MOUSEINPUT structure                            #
#                                                                           #
#############################################################################

# https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-mouseinput

class MOUSEINPUT( ctypes.Structure ):
    _fields_ = (
            ( "dx", LONG ),
            ( "dy", LONG ),
            ( "mouseData", DWORD ),
            ( "dwFlags", DWORD ),
            ( "time", DWORD ),
            ( "dwExtraInfo", ULONG_PTR )
    )



# mouseData
WHEEL_DELTA                 = 120
XBUTTON1                    = 0x0001
XBUTTON2                    = 0x0002

# dwFlags
MOUSEEVENTF_MOVE            = 0x0001
MOUSEEVENTF_LEFTDOWN        = 0x0002
MOUSEEVENTF_LEFTUP          = 0x0004
MOUSEEVENTF_RIGHTDOWN       = 0x0008
MOUSEEVENTF_RIGHTUP         = 0x0010
MOUSEEVENTF_MIDDLEDOWN      = 0x0020
MOUSEEVENTF_MIDDLEUP        = 0x0040
MOUSEEVENTF_XDOWN           = 0x0080
MOUSEEVENTF_XUP             = 0x0100
MOUSEEVENTF_WHEEL           = 0x0800
MOUSEEVENTF_HWHEEL          = 0x1000
MOUSEEVENTF_MOVE_NOCOALESCE = 0x2000
MOUSEEVENTF_VIRTUALDESK     = 0x4000
MOUSEEVENTF_ABSOLUTE        = 0x8000




#############################################################################
#                                                                           #
#                          HARDWAREINPUT structure                          #
#                                                                           #
#############################################################################

class HARDWAREINPUT( ctypes.Structure ):
    _fields_ = (
            ( "uMsg", DWORD ),
            ( "wParamL", WORD ),
            ( "wParamH", WORD )
    )




#############################################################################
#                                                                           #
#                               INPUT structure                             #
#                                                                           #
#############################################################################

class _INPUTunion( ctypes.Union ):
    _fields_ = (
            ( "mi", MOUSEINPUT ),
            ( "ki", KEYBDINPUT ),
            ( "hi", HARDWAREINPUT )
    )



class INPUT( ctypes.Structure ):
 _fields_ = (
            ( "type", DWORD ),
            ( "union", _INPUTunion ),
        )




 

#INPUT_MOUSE = 0
#INPUT_KEYBOARD = 1
#INPUT_HARDWARE = 2


#def Input( structure ):

#    if( isinstance(structure, MOUSEINPUT) ):
#        return INPUT( 0, _INPUTunion(mi=structure) )#INPUT( INPUT_MOUSE, _INPUTunion(mi=structure) )

#    if( isinstance(structure, KEYBDINPUT) ):
#        return INPUT( 1, _INPUTunion(ki=structure) )#INPUT( INPUT_KEYBOARD, _INPUTunion(ki=structure) )

#    if( isinstance(structure, HARDWAREINPUT) ):
#        return INPUT( 2, _INPUTunion(hi=structure) )#INPUT( INPUT_HARDWARE, _INPUTunion(hi=structure) )
    
#    raise TypeError("Cannot create INPUT structure!")



#def MouseInput( flags, dx, dy, data ):
#    return MOUSEINPUT( dx, dy, data, flags, 0, None )



#def KeybdInput( code, flags ):
#    return KEYBDINPUT( code, code, flags, 0, None )



#def HardwareInput( message, parameter ):
#    return HARDWAREINPUT(
#        message & 0xFFFFFFFF,
#        parameter & 0xFFFF,
#        parameter >> 16 & 0xFFFF )




#def Mouse( flags, dx=0, dy=0, data=0 ):
#    return Input( MouseInput( flags, dx, dy, data ) )
def Mouse( flags, dx=0, dy=0, data=0 ):
    return INPUT( 0, _INPUTunion( mi=MOUSEINPUT(dx, dy, data, flags, 0, None) ) )


#def Keyboard( code, flags=0 ):
#    return Input( KeybdInput( code, flags ) )
def Keyboard( code, flags=0 ):
    return INPUT( 1, _INPUTunion( ki=KEYBDINPUT(code, code, flags, 0, None) ) )


#def Hardware( message, parameters=0 ):
#    return Input( HardwareInput( message, parameter ) )
def Hardware( message, parameters=0 ):
    return INPUT( 2, _INPUTunion( hi=HARDWAREINPUT(message & 0xFFFFFFFF, parameter & 0xFFFF, parameter >> 16 & 0xFFFF) ) )




def SendInput( *inputs ):

    nInputs = len( inputs )
    LPINPUT = INPUT * nInputs
    pInputs = LPINPUT( *inputs )
    cbSize = ctypes.c_int( ctypes.sizeof(INPUT) )

    return ctypes.windll.user32.SendInput( nInputs, pInputs, cbSize )






#def PressKey( code ):
##    return SendInput( Keyboard( code ) )
#    return ctypes.windll.user32.PostMessageW( None, Const.WM_KEYDOWN, code, 0 )


#def ReleaseKey( code ):
#    return SendInput( Keyboard( code, KEYEVENTF_KEYUP ) )




# http://nonsoft.la.coocan.jp/SoftSample/CS.NET/SampleSendInput.html

# mouse command settings
#def MouseClick


def MouseMove( dx, dy ):
    return SendInput( Mouse( MOUSEEVENTF_MOVE, dx, dy ) )
    #return SendInput( INPUT( 0, _INPUTunion( mi=MOUSEINPUT(dx, dy, 0, MOUSEEVENTF_MOVE, 0, None) ) ) )





import helper


class Sender:

    def __init__( self ):
        self.__m_KeyState = ( ctypes.c_ubyte * 256 )()
        self.__m_MK_Flags = 0x00


        self.__m_CurrentThreadID = ctypes.windll.kernel32.GetCurrentThreadId()

        self.__m_refHwndThreadID = None



    def SendInput( ):
        pass


    def PressKey( self, vkcode ):
        pass


    def ReleaseKey( self, vkcode ):
        pass


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
            self.__m_MK_Flags = self.__m_MK_Flags | Const.MK_SHIFT if down else self.__m_MK_Flags & Const.MK_SHIFT_INV

        elif( vkey == Const.VK_CONTROL or vkey == Const.VK_LCONTROL or vkey == Const.VK_RCONTROL ):
            self.__m_KeyState[ vkey ] = 0x80 if down else 0x00
            self.__m_KeyState[ Const.VK_CONTROL ] = OR( self.__m_KeyState[ Const.VK_LCONTROL ], self.__m_KeyState[ Const.VK_RCONTROL ] )
#self.__m_MK_Flags = self.__m_MK_Flags | Const.MK_SHIFT if down else self.__m_MK_Flags & Const.MK_SHIFT_INV

        elif( vkey == Const.VK_NUMLOCK and not down ):
            self.__m_KeyState[ Const.VK_NUMLOCK ] = not self.__m_KeyState[ Const.VK_NUMLOCK ]

        elif( vkey == Const.VK_CAPITAL and not down ):
            self.__m_KeyState[ Const.VK_CAPITAL ] = not self.__m_KeyState[ Const.VK_CAPITAL ]

        elif( vkey == Const.VK_SCROLL and not down ):
            self.__m_KeyState[ Const.VK_SCROLL ] = not self.__m_KeyState[ Const.VK_SCROLL ]


    
    # Mouse move
    def MouseMove( self, hwnd, dx, dy ):
        ctypes.windll.user32.PostMessageW( hwnd, Const.WM_MOUSEMOVE, self.__m_MK_Flags, MAKELPARAM( dx, dy ) )

    # Mouse move absolute



    # Mouse left down
    def MouseLeftDown( self, hwnd, x, y, is_world_coord=False ):
        self.__m_MK_Flags |= Const.MK_LBUTTON
        self.__MouseButton( hwnd, Const.WM_LBUTTONDOWN, self.__m_MK_Flags, x, y, is_world_coord )
        
    # Mouse left up
    def MouseLeftUp( self, hwnd, x, y, mk_buttons=0x0, is_world_coord=False ):
        self.__m_MK_Flags &= Const.MK_LBUTTON_INV# Disable MK_LBUTTON
        self.__MouseButton( hwnd, Const.WM_LBUTTONUP, self.__m_MK_Flags, x, y, is_world_coord )

    # Mouse left double click
    def MouseLeftDoubleClick( self, hwnd, x, y, is_world_coord=False ):
        self.__MouseButton( hwnd, Const.WM_LBUTTONDBLCLK, Const.MK_LBUTTON | self.__m_MK_Flags, x, y, is_world_coord )
        # Windows double click behavior...
        #self.__m_MK_Flags |= Const.MK_LBUTTON
        #self.__MouseButton( hwnd, Const.WM_LBUTTONDBLCLK, self.__m_MK_Flags, x, y, is_world_coord )
        #self.MouseLeftUp( hwnd, x, y, is_world_coord )



    # Mouse right down
    def MouseRightDown( self, hwnd, x, y, is_world_coord=False ):
        self.__m_MK_Flags |= Const.MK_RBUTTON
        self.__MouseButton( hwnd, Const.WM_RBUTTONDOWN, self.__m_MK_Flags, x, y, is_world_coord )

    # Mouse right up
    def MouseRightUp( self, hwnd, x, y, is_world_coord=False ):
        self.__m_MK_Flags &= Const.MK_RBUTTON_INV# Disable MK_RBUTTON
        self.__MouseButton( hwnd, Const.WM_RBUTTONUP, self.__m_MK_Flags, x, y, is_world_coord )

    # Mouse right double click
    def MouseRightDoubleClick( self, hwnd, x, y, is_world_coord=False ):
        self.__MouseButton( hwnd, Const.WM_RBUTTONDBLCLK, Const.MK_RBUTTON | self.__m_MK_Flags, x, y, is_world_coord )
        # Windows double click behavior...
        #self.__m_MK_Flags |= Const.MK_RBUTTON
        #self.__MouseButton( hwnd, Const.WM_RBUTTONDBLCLK, self.__m_MK_Flags, x, y, is_world_coord )
        #self.MouseRightUp( hwnd, x, y, is_world_coord )



    # Mouse middle down
    def MouseMiddleDown( self, hwnd, x, y, is_world_coord=False ):
        self.__m_MK_Flags |= Const.MK_MBUTTON
        self.__MouseButton( hwnd, Const.WM_MBUTTONDOWN, self.__m_MK_Flags, x, y, is_world_coord )

    # Mouse middle up
    def MouseMiddleUp( self, hwnd, x, y, is_world_coord=False ):
        self.__m_MK_Flags &= Const.MK_MBUTTON_INV# Disable MK_MBUTTON
        self.__MouseButton( hwnd, Const.WM_MBUTTONUP, self.__m_MK_Flags, x, y, is_world_coord )

    # Mouse middle double click
    def MouseMiddleDoubleClick( self, hwnd, x, y, is_world_coord=False ):
        self.__MouseButton( hwnd, Const.WM_MBUTTONDBLCLK, Const.MK_MBUTTON | self.__m_MK_Flags, x, y, is_world_coord )
        # Windows double click behavior...
        #self.__m_MK_Flags |= Const.MK_MBUTTON
        #self.__MouseButton( hwnd, Const.WM_MBUTTONDBLCLK, self.__m_MK_Flags, x, y, is_world_coord )
        #self.MouseMiddleUp( hwnd, x, y, is_world_coord )



# https://stackoverflow.com/questions/41817550/mouse-simulation-works-really-slow-can-it-be-faster
    # Mouse X1 down
    def MouseX1Down( self, hwnd, x, y, is_world_coord=False ):
        self.__m_MK_Flags |= Const.MK_XBUTTON1
        self.__MouseButton( hwnd, Const.WM_XBUTTONDOWN, self.__m_MK_Flags, x, y, is_world_coord )

    # Mouse X1 up
    def MouseX1Up( self, hwnd, x, y, is_world_coord=False ):
        self.__m_MK_Flags &= Const.MK_XBUTTON1_INV# Disable MK_XBUTTON1
        self.__MouseButton( hwnd, Const.WM_XBUTTONUP, self.__m_MK_Flags, x, y, is_world_coord )

    # Mouse X1 double click
    def MouseX1DoubleClick( self, hwnd, x, y, is_world_coord=False ):
        self.__MouseButton( hwnd, Const.WM_XBUTTONDBLCLK, Const.XBUTTON1 | self.__m_MK_Flags, x, y, is_world_coord )
        # Windows double click behavior...
        #self.__m_MK_Flags |= Const.MK_XBUTTON1
        #self.__MouseButton( hwnd, Const.WM_XBUTTONDBLCLK, self.__m_MK_Flags, x, y, is_world_coord )
        #self.MouseX1Up( hwnd, x, y, is_world_coord )


    # Mouse X2 down
    def MouseX1Down( self, hwnd, x, y, is_world_coord=False ):
        self.__m_MK_Flags |= Const.MK_XBUTTON2
        self.__MouseButton( hwnd, Const.WM_XBUTTONDOWN, self.__m_MK_Flags, x, y, is_world_coord )

    # Mouse X2 up
    def MouseX1Up( self, hwnd, x, y, is_world_coord=False ):
        self.__m_MK_Flags &= Const.MK_XBUTTON2_INV# Disable MK_XBUTTON2
        self.__MouseButton( hwnd, Const.WM_XBUTTONUP, self.__m_MK_Flags, x, y, is_world_coord )

    # Mouse X2 double click
    def MouseX1DoubleClick( self, hwnd, x, y, is_world_coord=False ):
        self.__MouseButton( hwnd, Const.WM_XBUTTONDBLCLK, Const.XBUTTON2 | self.__m_MK_Flags, x, y, is_world_coord )
        # Windows double click behavior...
        #self.__m_MK_Flags |= Const.MK_XBUTTON2
        #self.__MouseButton( hwnd, Const.WM_XBUTTONDBLCLK, self.__m_MK_Flags, x, y, is_world_coord )
        #self.MouseX1Up( hwnd, x, y, is_world_coord )




#TODO: Cursor position is required
    # Mouse scroll up
    def MouseScrollUp( self, hwnd, direction ):
        #lParam = MAKELPARAM( x, y )
        ctypes.windll.user32.PostMessageW( hwnd, Const.WM_MOUSEWHEEL, Const.WHEEL_UP | self.__m_MK_Flags, 0 )

    # Mouse scroll down        
    def MouseScrollDown( self, hwnd, direction ):
        #lParam = MAKELPARAM( x, y )
        ctypes.windll.user32.PostMessageW( hwnd, Const.WM_MOUSEWHEEL, Const.WHEEL_DOWN | self.__m_MK_Flags, 0 )



    def __MouseButton( self, hwnd, msg, wpButtons, x, y, is_world_coord ):
        lpPoint = MAKELPARAM( x, y )
        if( is_world_coord ): ctypes.windll.user32.ScreenToClient( hwnd, ctypes.byref(lpPoint) )
        ctypes.windll.user32.PostMessageW( hwnd, msg, wpButtons, lpPoint )
