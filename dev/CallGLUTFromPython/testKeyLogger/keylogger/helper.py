import ctypes


def MAKELPARAM( low, high ):
    return ctypes.wintypes.LPARAM( ( (high & 0xFFFF) << 16 ) | (low & 0xFFFF) )




# Mouse move
def MouseMove( hwnd, dx, dy, mk_buttons ):
    lpPoint = MAKELPARAM( dx, dy )#POINT( dx, dy )
    ctypes.windll.user32.PostMessageW( hwnd, Const.WM_MOUSEMOVE, mk_buttons, lpPoint )#Const.MK_LBUTTON

# Mouse move absolute



# Mouse left down
def MouseLeftDown( hwnd, x, y, mk_buttons, is_world_coord ):
    __MouseButton( hwnd, Const.WM_LBUTTONDOWN, Const.MK_LBUTTON | mk_buttons, x, y, is_world_coord )

# Mouse left up
def MouseLeftUp( hwnd, x, y, mk_buttons, is_world_coord ):
    __MouseButton( hwnd, Const.WM_LBUTTONUP, Const.MK_LBUTTON | mk_buttons, x, y, is_world_coord )

# Mouse left double click
def MouseLeftDoubleClick( hwnd, x, y, mk_buttons, is_world_coord ):
    __MouseButton( hwnd, Const.WM_LBUTTONDBLCLK, Const.MK_LBUTTON | mk_buttons, x, y, is_world_coord )



# Mouse right down
def MouseRightDown( hwnd, x, y, mk_buttons, is_world_coord ):
    __MouseButton( hwnd, Const.WM_RBUTTONDOWN, Const.MK_RBUTTON | mk_buttons, x, y, is_world_coord )

# Mouse right up
def MouseRightUp( hwnd, x, y, mk_buttons, is_world_coord ):
    __MouseButton( hwnd, Const.WM_RBUTTONUP, Const.MK_RBUTTON | mk_buttons, x, y, is_world_coord )

# Mouse right double click
def MouseRightDoubleClick( hwnd, x, y, mk_buttons, is_world_coord ):
    __MouseButton( hwnd, Const.WM_RBUTTONDBLCLK, Const.MK_RBUTTON | mk_buttons, x, y, is_world_coord )



# Mouse middle down
def MouseMiddleDown( hwnd, x, y, mk_buttons, is_world_coord ):
    __MouseButton( hwnd, Const.WM_MBUTTONDOWN, Const.MK_MBUTTON | mk_buttons, x, y, is_world_coord )

# Mouse middle up
def MouseMiddleUp( hwnd, x, y, mk_buttons, is_world_coord ):
    __MouseButton( hwnd, Const.WM_MBUTTONUP, Const.MK_MBUTTON | mk_buttons, x, y, is_world_coord )

# Mouse middle double click
def MouseMiddleDoubleClick( hwnd, x, y, mk_buttons, is_world_coord ):
    __MouseButton( hwnd, Const.WM_MBUTTONDBLCLK, Const.MK_MBUTTON | mk_buttons, x, y, is_world_coord )



# https://stackoverflow.com/questions/41817550/mouse-simulation-works-really-slow-can-it-be-faster
# Mouse X1 down
def MouseX1Down( hwnd, x, y, mk_buttons, is_world_coord ):
    __MouseButton( hwnd, Const.WM_XBUTTONDOWN, Const.XBUTTON1 | mk_buttons, x, y, is_world_coord )

# Mouse X1 up
def MouseX1Up( hwnd, x, y, mk_buttons, is_world_coord ):
    __MouseButton( hwnd, Const.WM_XBUTTONUP, Const.XBUTTON1 | mk_buttons, x, y, is_world_coord )

# Mouse X1 double click
def MouseX1DoubleClick( hwnd, x, y, mk_buttons, is_world_coord ):
    __MouseButton( hwnd, Const.WM_XBUTTONDBLCLK, Const.XBUTTON1 | mk_buttons, x, y, is_world_coord )



# Mouse X2 down
def MouseX1Down( hwnd, x, y, mk_buttons, is_world_coord ):
    __MouseButton( hwnd, Const.WM_XBUTTONDOWN, Const.XBUTTON2 | mk_buttons, x, y, is_world_coord )

# Mouse X2 up
def MouseX1Up( hwnd, x, y, mk_buttons, is_world_coord ):
    __MouseButton( hwnd, Const.WM_XBUTTONUP, Const.XBUTTON2 | mk_buttons, x, y, is_world_coord )

# Mouse X2 double click
def MouseX1DoubleClick( hwnd, x, y, mk_buttons, is_world_coord ):
    __MouseButton( hwnd, Const.WM_XBUTTONDBLCLK, Const.XBUTTON2 | mk_buttons, x, y, is_world_coord )



# Mouse scroll up
def MouseScrollUp( hwnd, direction, mk_buttons ):
    #lParam = MAKELPARAM( x, y )
    ctypes.windll.user32.PostMessageW( hwnd, Const.WM_MOUSEWHEEL, Const.WHEEL_UP | mk_buttons, 0 )

# Mouse scroll down        
def MouseScrollDown( hwnd, direction, keyflags=0x0 ):
    #lParam = MAKELPARAM( x, y )
    ctypes.windll.user32.PostMessageW( hwnd, Const.WM_MOUSEWHEEL, Const.WHEEL_DOWN | mk_buttons, 0 )



def __MouseButton( hwnd, msg, wpButtons, x, y, is_world_coord ):
    lpPoint = MAKELPARAM( x, y )
    if( is_world_coord ): ctypes.windll.user32.ScreenToClient( hwnd, ctypes.byref(lpPoint) )
    ctypes.windll.user32.PostMessageW( hwnd, msg, wpButtons, lpPoint )
