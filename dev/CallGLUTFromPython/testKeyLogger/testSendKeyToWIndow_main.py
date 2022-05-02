import threading
import subprocess
import time
import ctypes
import ctypes.wintypes
import win32con

#from keylogger.sender import *

from keylogger.const import *
from keylogger.sender import *
from WindowHandleHelper import *




##TODO: アプリウィンドウの起動完了を待つ方法を探す
#def CreateWindowHook( nCode, wParam, lParam ):

#    if( nCode == win32con.HCBT_CREATEWND ):
#        return  ctypes.windll.user32.CallNextHook()



from time import perf_counter
def Sleep( wait_sec ):
    until = perf_counter() + wait_sec
    while( perf_counter() < until ):
        pass





#TODO: 入力プロセス完了まで待機する. PostMessageの後で呼び出せばいい?
# https://programtalk.com/vs2/python/4060/EventGhost/eg/WinApi/SendKeys.py/
def WaitForInputProcessed( self, prochandle, milliseconds ):
    if( prochandle ):
        ctypes.windll.user32.WaitForInputIdle( prochandle, milliseconds )

    #while( ctypes.windll.user32.WaitForInputIdle( prochandle, milliseconds ) ):
    #    print('---')
 



# https://helperbyte.com/questions/330002/how-to-transfer-keyboard-shortcuts-ctrla-etc-an-inactive-window


KEY_UP_TO_DOWN = 0x00000000
KEY_DOWN_TO_UP = 0xC0000000
ALT_DOWN = 0x20000000

def SendAltSpace( hwnd ):

    __m_KeyState = ( ctypes.c_ubyte * 256 )()
    

    # Alt + Spaceでメニュー表示する
    scancode = ctypes.windll.user32.MapVirtualKeyW(Const.VK_LMENU, 0) << 16
    wparam = scancode | 0x0001 | KEY_UP_TO_DOWN | ALT_DOWN
    ctypes.windll.user32.PostMessageW( hWnd, win32con.WM_SYSKEYDOWN, win32con.VK_LMENU, wparam )

    #ctypes.windll.user32.PostMessageW( hWnd, win32con.WM_SYSKEYDOWN, win32con.VK_SPACE, 0x00390001 | 0x20000000 )# Alt押してる時はlparamの29ビット目を1にする
    #time.sleep(0.05)
    #ctypes.windll.user32.PostMessageW( hWnd, win32con.WM_SYSKEYUP, win32con.VK_SPACE, 0xC0390001 | 0x20000000 )

    # Presse Space
    scancode = ctypes.windll.user32.MapVirtualKeyW(Const.VK_SPACE, 0) << 16

    wparam = scancode | 0x0001 | KEY_UP_TO_DOWN | ALT_DOWN
    ctypes.windll.user32.PostMessageW( hwnd, win32con.WM_SYSKEYDOWN, Const.VK_SPACE, wparam )

    wparam = scancode | 0x0001 | KEY_DOWN_TO_UP | ALT_DOWN
    ctypes.windll.user32.PostMessageW( hwnd, win32con.WM_SYSKEYUP, Const.VK_SPACE, wparam )
    Sleep( 0.005 )#time.sleep( 0.00000000001 )#

    scancode = ctypes.windll.user32.MapVirtualKeyW(Const.VK_LMENU, 0) << 16
    wparam = scancode | 0x0001 | KEY_DOWN_TO_UP
    ctypes.windll.user32.PostMessageW( hWnd, win32con.WM_KEYUP, win32con.VK_MENU, wparam )




def SendKeys( hwnd, vkcode ):#, modifier ):

    __m_KeyState = ( ctypes.c_ubyte * 256 )()
    
    pid = ctypes.wintypes.DWORD()
    tid = ctypes.windll.user32.GetWindowThreadProcessId( hwnd, ctypes.byref(pid) )

    currentitd = ctypes.windll.kernel32.GetCurrentThreadId()#currentitd = threading.current_thread().ident#
    ctypes.windll.user32.AttachThreadInput( currentitd, tid, True )
    ctypes.windll.user32.GetKeyboardState( ctypes.byref(__m_KeyState) )

    # Set shift state to keydown
    __m_KeyState[ Const.VK_SHIFT ] |= 0x80
    ctypes.windll.user32.SetKeyboardState( ctypes.byref(__m_KeyState) )

    # Presse A
    ctypes.windll.user32.PostMessageW( hwnd, win32con.WM_KEYDOWN, Const.KEY_A, 0x0001 | 0x1E<<16 )
    ctypes.windll.user32.PostMessageW( hwnd, win32con.WM_KEYUP, Const.KEY_A, 0x0001 | 0x1E<<16 | KEY_DOWN_TO_UP )
    Sleep( 0.005 )#time.sleep( 0.00000000001 )#

    # Set shift state to keyup
    __m_KeyState[ Const.VK_SHIFT ] = 0x0
    ctypes.windll.user32.SetKeyboardState( ctypes.byref(__m_KeyState) )

    ctypes.windll.user32.AttachThreadInput( currentitd, tid, False )




# 非アクティブなウィンドウへのSendInputはできない. Sleepも必須
def SendInputs( hwnd, vkcode ):#, modifier ):

    __m_KeyState = ( ctypes.c_ubyte * 256 )()
    
    pid = ctypes.wintypes.DWORD()
    tid = ctypes.windll.user32.GetWindowThreadProcessId( hwnd, ctypes.byref(pid) )

    currentitd = ctypes.windll.kernel32.GetCurrentThreadId()#currentitd = threading.current_thread().ident#
    ctypes.windll.user32.AttachThreadInput( currentitd, tid, True )
    ctypes.windll.user32.GetKeyboardState( ctypes.byref(__m_KeyState) )


    # Set shift state to keydown
    #__m_KeyState[ Const.VK_SHIFT ] |= 0x80
    #ctypes.windll.user32.SetKeyboardState( ctypes.byref(__m_KeyState) )
    SendInput( Keyboard(Const.VK_SHIFT)  )
    #time.sleep(0.01)

    # Presse A
    SendInput( Keyboard(Const.KEY_A) )
    SendInput( Keyboard(Const.KEY_A, KEYEVENTF_KEYUP) )
    Sleep( 0.005 )#time.sleep( 0.00000000001 )#

    # Set shift state to keyup
    SendInput( Keyboard(Const.VK_SHIFT, KEYEVENTF_KEYUP)  )

    #__m_KeyState[ Const.VK_SHIFT ] = 0x0
    #ctypes.windll.user32.SetKeyboardState( ctypes.byref(__m_KeyState) )

    ctypes.windll.user32.AttachThreadInput( currentitd, tid, False )





def MAKELPARAM( low, high ):
    return ctypes.wintypes.LPARAM( ( (high & 0xFFFF) << 16 ) | (low & 0xFFFF) )


def MouseLeftDown( hwnd, x, y ):

    WM_LBUTTONDOWN      = 0x0201
    MK_LBUTTON  = 0x0001

    lpPoint = MAKELPARAM( x, y )#POINT( x, y )#
    #ctypes.windll.user32.ScreenToClient( hwnd, ctypes.byref(lpPoint) )

    ctypes.windll.user32.PostMessageW( hwnd, WM_LBUTTONDOWN, MK_LBUTTON, lpPoint )

    #print( "MouseLeftDown", lpPoint.x, lpPoint.y  )
    


def MouseLeftUp( hwnd, x, y ):
    WM_LBUTTONUP        = 0x0202
    MK_LBUTTON  = 0x0001

    lpPoint = MAKELPARAM( x, y )#POINT( x, y )
    #ctypes.windll.user32.ScreenToClient( hwnd, ctypes.byref(lpPoint) )
    
    ctypes.windll.user32.PostMessageW( hwnd, Const.WM_LBUTTONUP, Const.MK_LBUTTON, lpPoint )

    #print( "MouseLeftUp", lpPoint.x, lpPoint.y  )



def MouseMove( hwnd, dx, dy ):

    WM_MOUSEMOVE        = 0x0200
    MK_LBUTTON  = 0x0001

    lpPoint = MAKELPARAM( dx, dy )#POINT( dx, dy )
    ctypes.windll.user32.PostMessageW( hwnd, Const.WM_MOUSEMOVE, 0, lpPoint )#Const.MK_LBUTTON




if __name__=="__main__":

    app = subprocess.Popen(
        [r"C:\\Windows\\system32\\notepad.exe"] )
        #r"./app/ShapeController.exe" )
        #[r"./app/ProceduralPlanetRendering.exe"] )
        #[r"C:\\Windows\\System32\\mspaint.exe"] )
        #r"D:/Program Files (x86)/sakura/sakura.exe" )

#TODO: プロセスがアイドル状態になるまで待つ -> アプリ起動検出には使えない. HCBT_CREATEWND 
#ctypes.windll.user32.WaitForInputIdle( notepad._handle, 1000 ) ):
# https://stackoverflow.com/questions/33405201/waitforinputidle-doesnt-work-for-starting-mspaint-programmatically

    print( int(app._handle) )
    process_id = app.pid

    # Wait until app is ready
    window_handles = []
    while( GetWindowHandlesFromPID( process_id, window_handles ) == False ):
        print( window_handles )
        time.sleep(0.05)

    time.sleep(0.5)

    hWnds = GetChildHandles( window_handles[0] )
    hWnd = window_handles[0]#hWnds[1]


    # Alt + Spaceでメニュー表示する
    #ctypes.windll.user32.PostMessageW( hWnd, win32con.WM_SYSKEYDOWN, win32con.VK_MENU, 0 )
    #ctypes.windll.user32.PostMessageW( hWnd, win32con.WM_SYSKEYDOWN, win32con.VK_SPACE, 0x00390001 | 0x20000000 )# Alt押してる時はlparamの29ビット目を1にする
    #time.sleep(0.05)
    #ctypes.windll.user32.PostMessageW( hWnd, win32con.WM_SYSKEYUP, win32con.VK_SPACE, 0xC0390001 | 0x20000000 )
    #ctypes.windll.user32.PostMessageW( hWnd, win32con.WM_SYSKEYUP, win32con.VK_MENU, 0 )



    # Shift + a を送信する

    #SW_MINIMIZE =6
    #ctypes.windll.user32.ShowWindow(window_handles[0], SW_MINIMIZE)# 最小化したウィンドウ:PostMessageだけできる
    #ctypes.windll.user32.EnableWindow( window_handles[0], False )# Disableしたウィンドウ:SendInput/PostMessageできる
    #ctypes.windll.user32.SetForegroundWindow(window_handles[0])# フォーカス外れたウィンドウ:PostMessageだけできる
    time.sleep(0.5)

    # http://delfusa.main.jp/delfusafloor/archive/www.nifty.ne.jp_forum_fdelphi/samples/00027.html
    # https://stackoverflow.com/questions/13200362/how-to-send-ctrl-shift-alt-key-combinations-to-an-application-window-via-sen


    #VirtualKey = ctypes.windll.user32.MapVirtualKeyW(Const.KEY_A, 0);

    #ctypes.windll.user32.PostMessageW( hWnds[0], win32con.WM_IME_KEYDOWN, win32con.VK_SHIFT, 1 )#0x0001 | 0x2A<<16 )
    #ctypes.windll.user32.PostMessageW( hWnds[0], win32con.WM_KEYDOWN, Const.KEY_A, 0)#0x0001 | 0x1E<<16 )

    #ctypes.windll.user32.PostMessageW( hWnds[0], win32con.WM_KEYUP, Const.KEY_A, 0)#0x0001 | 0x1E<<16 | KEY_DOWN_TO_UP )
    #ctypes.windll.user32.PostMessageW( hWnds[0], win32con.WM_IME_KEYUP, win32con.VK_SHIFT, 0 )#KEY_DOWN_TO_UP )

    for i in range(200):
        SendKeys( hWnds[0], Const.KEY_A )
        #SendInputs( hWnds[0], Const.KEY_A )


    #SendAltSpace( hWnds[0] )

    s = Sender()










    ######################################## MSPaintにマウス入力を送信する例 ###################################



    #app = subprocess.Popen( [r"C:\\Windows\\System32\\mspaint.exe"] )


    ##TODO: プロセスがアイドル状態になるまで待つ -> アプリ起動検出には使えない. HCBT_CREATEWND 
    ##ctypes.windll.user32.WaitForInputIdle( notepad._handle, 1000 ) ):
    ## https://stackoverflow.com/questions/33405201/waitforinputidle-doesnt-work-for-starting-mspaint-programmatically

    #process_id = app.pid

    ## Wait until app is ready
    #window_handles = []
    #while( GetWindowHandlesFromPID( process_id, window_handles ) == False ):
    #    print( window_handles )
    #    time.sleep(0.05)

    #time.sleep(0.5)


    #def MouseWheel( hwnd, direction, mk_buttons=0x0 ):
    #    ctypes.windll.user32.PostMessageW( hwnd, Const.WM_MOUSEWHEEL, Const.WHEEL_DOWN | mk_buttons, 0 )


    ##https://codesequoia.wordpress.com/2009/06/07/control-mouse-programmatically/

    ## Get MSPaintView handle
    #hwndView = ctypes.windll.user32.FindWindowExW( window_handles[0], 0, "MSPaintView", None )
    ## Get drawable area handle from hwndView
    #hDrawArea = GetChildHandles( hwndView )[0]#ctypes.windll.user32.GetWindow( hwndView, win32con.GW_CHILD )


    ## DO mouse operation
    #MouseWheel( hDrawArea, -1, Const.MK_CONTROL )

    #MouseLeftDown( hDrawArea, 10, 10 )
    #MouseMove( hDrawArea, 25, 100 )
    ##MouseMove( hDrawArea, 5, 5 )
    #MouseLeftUp( hDrawArea, 100, 100 )









