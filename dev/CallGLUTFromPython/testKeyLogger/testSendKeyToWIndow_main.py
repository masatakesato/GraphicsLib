﻿import threading
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

PROCESS_QUERY_INFORMATION = 0x1000
PROCESS_ALL_ACCESS = 0x001F0FFF
SYNCHRONIZE = 0x00100000
INFINITE = 0xFFFFFFFF

def SendKeys( hwnd, vkcode ):#, modifier ):

    __m_KeyState = ( ctypes.c_ubyte * 256 )()
    
    #threadId = 
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
    
    #threadId = 
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

    app = subprocess.Popen([r"C:\\Windows\\System32\\mspaint.exe"] )
    #[r"C:\\Windows\\system32\\notepad.exe"] )
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

    #KEY_UP_TO_DOWN = 0x00000000
    #KEY_DOWN_TO_UP = 0xC0000000
    #ALT_DOWN = 0x20000000

    #VirtualKey = ctypes.windll.user32.MapVirtualKeyA(Const.KEY_A, 0);

    #ctypes.windll.user32.PostMessageW( hWnds[0], win32con.WM_IME_KEYDOWN, win32con.VK_SHIFT, 1 )#0x0001 | 0x2A<<16 )
    #ctypes.windll.user32.PostMessageW( hWnds[0], win32con.WM_KEYDOWN, Const.KEY_A, 0)#0x0001 | 0x1E<<16 )

    #ctypes.windll.user32.PostMessageW( hWnds[0], win32con.WM_KEYUP, Const.KEY_A, 0)#0x0001 | 0x1E<<16 | KEY_DOWN_TO_UP )
    #ctypes.windll.user32.PostMessageW( hWnds[0], win32con.WM_IME_KEYUP, win32con.VK_SHIFT, 0 )#KEY_DOWN_TO_UP )

    #for i in range(200):
    #    SendKeys( hWnds[0], Const.KEY_A, notepad._handle )
        #SendInputs( hWnds[0], Const.KEY_A, notepad._handle )


    def MouseWheel( hwnd, direction, mk_buttons=0x0 ):
        #WHEEL_POS = 0x00780000
        #WHEEL_NEG = 0xff880000
        #lParam = MAKELPARAM( x, y )
        ctypes.windll.user32.PostMessageW( hwnd, Const.WM_MOUSEWHEEL, Const.WHEEL_DOWN | mk_buttons, 0 )



    #https://codesequoia.wordpress.com/2009/06/07/control-mouse-programmatically/

    # Get MSPaintView handle
    hwndView = ctypes.windll.user32.FindWindowExW( window_handles[0], 0, "MSPaintView", None )
    # Get drawable area handle from hwndView
    hDrawArea = GetChildHandles( hwndView )[0]#ctypes.windll.user32.GetWindow( hwndView, win32con.GW_CHILD )

    MouseWheel( hDrawArea, -1, Const.MK_CONTROL )



    # DO mouse operation
    MouseLeftDown( hDrawArea, 10, 10 )

    MouseMove( hDrawArea, 25, 100 )
    #MouseMove( hDrawArea, 5, 5 )

    MouseLeftUp( hDrawArea, 100, 100 )








# https://docs.microsoft.com/ja-jp/windows/win32/inputdev/about-keyboard-input#keystroke-message-flags
# LParam
# 0-15bit: 回数.   1回なら 0x0001
# 16-23bit: スキャンコード. VirtualKeyからの変換テーブルが必要. aなら1E # https://bsakatu.net/doc/virtual-key-of-windows/
# 24bit: 拡張キーフラグ. 拡張キーの場合は"1", そうでない場合は"0"
# 25-28bit: 未使用
# 29bit: Altキー押し込まれてる間は"1", そうでない場合は"0"
# 30bit: 直前のキー状態. キーダウンしている場合は"1", キーアップしている場合は"0"
# 31bit: 遷移状態フラグ. WM_KEYDOWN/WM_SYSMKEYDOWNを実行する場合は"0", WM_KEYUP/WM_SYSKEYUPを実行する場合は"1"に設定する




# F4キーを押し込むlParamの例: 0x003E0001  ( 0000 0000 0011 1110 0000 0000 0000 0001 )
# 0x0001 -> 1回
# 3E -> F4キーのスキャンコード
# 拡張キーフラグ -> 0
# Altキー押し込み -> 0
# 直前のキー状態 -> 0(キーアップ)
# 遷移状態 -> 0(WM_KEYDOWN/WM_SYSMKEYDOWN)



# F4キーを解放するlParamの例例: 0xC03E0001 ( 1100 0000 0011 1110 0000 0000 0000 0001 )
# 0x0001 -> 1回
# 3E -> F4キーのスキャンコード
# 拡張キーフラグ -> 0
# Altキー押し込み -> 0
# 直前のキー状態 -> 1(キーダウン)
# 遷移状態 -> 1(WM_KEYUP/WM_SYSKEYUP)



