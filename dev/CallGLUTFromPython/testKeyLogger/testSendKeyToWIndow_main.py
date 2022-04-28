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

PROCESS_QUERY_INFORMATION = 0x1000
PROCESS_ALL_ACCESS = 0x001F0FFF
SYNCHRONIZE = 0x00100000
INFINITE = 0xFFFFFFFF

def SendKeys( hwnd, vkcode, ha):#, modifier ):

    __m_KeyState = ( ctypes.c_ubyte * 256 )()
    
    #threadId = 
    pid = ctypes.wintypes.DWORD()
    tid = ctypes.windll.user32.GetWindowThreadProcessId( hwnd, ctypes.byref(pid) )
    processHandle = ctypes.windll.kernel32.OpenProcess( SYNCHRONIZE | PROCESS_ALL_ACCESS, False, pid )


    currentitd = ctypes.windll.kernel32.GetCurrentThreadId()#currentitd = threading.current_thread().ident#
    ctypes.windll.user32.AttachThreadInput( currentitd, tid, True )
    ctypes.windll.user32.GetKeyboardState( ctypes.byref(__m_KeyState) )


    # Set shift state to keydown
    __m_KeyState[ Const.VK_SHIFT ] |= 0x80
    ctypes.windll.user32.SetKeyboardState( ctypes.byref(__m_KeyState) )
    #time.sleep(0.01)

    # Presse A
    ctypes.windll.user32.PostMessageW( hwnd, win32con.WM_KEYDOWN, Const.KEY_A, 0x0001 | 0x1E<<16 )
    ctypes.windll.user32.PostMessageW( hwnd, win32con.WM_KEYUP, Const.KEY_A, 0x0001 | 0x1E<<16 | KEY_DOWN_TO_UP )
    Sleep( 0.01 )#time.sleep( 0.00000000001 )#
    #while( ctypes.windll.user32.WaitForInputIdle( processHandle, 1 ) == False ):
    #    pass

    # Set shift state to keyup
    __m_KeyState[ Const.VK_SHIFT ] = 0x0
    ctypes.windll.user32.SetKeyboardState( ctypes.byref(__m_KeyState) )

    ctypes.windll.user32.AttachThreadInput( currentitd, tid, False )




if __name__=="__main__":

    notepad = subprocess.Popen( [r"C:\\Windows\\system32\\notepad.exe"] )#
    #r"./app/ShapeController.exe" )#[r"./app/ProceduralPlanetRendering.exe"] )#[r"C:\\Windows\\System32\\mspaint.exe"] ) #
    #r"D:/Program Files (x86)/sakura/sakura.exe" )

#TODO: プロセスがアイドル状態になるまで待つ -> アプリ起動検出には使えない. HCBT_CREATEWND 
#ctypes.windll.user32.WaitForInputIdle( notepad._handle, 1000 ) ):
# https://stackoverflow.com/questions/33405201/waitforinputidle-doesnt-work-for-starting-mspaint-programmatically

    print( int(notepad._handle) )
    process_id = notepad.pid

    # Wait until app is ready
    window_handles = []
    while( GetWindowHandlesFromPID( process_id, window_handles ) == False ):
        print( window_handles )
        time.sleep(0.05)

    #time.sleep(0.5)

    hWnds = GetChildHandles( window_handles[0] )
    hWnd = window_handles[0]#hWnds[1]


    # Alt + Spaceでメニュー表示する
    #ctypes.windll.user32.PostMessageW( hWnd, win32con.WM_SYSKEYDOWN, win32con.VK_MENU, 0 )
    #ctypes.windll.user32.PostMessageW( hWnd, win32con.WM_SYSKEYDOWN, win32con.VK_SPACE, 0x00390001 | 0x20000000 )# Alt押してる時はlparamの29ビット目を1にする
    #time.sleep(0.05)
    #ctypes.windll.user32.PostMessageW( hWnd, win32con.WM_SYSKEYUP, win32con.VK_SPACE, 0xC0390001 | 0x20000000 )
    #ctypes.windll.user32.PostMessageW( hWnd, win32con.WM_SYSKEYUP, win32con.VK_MENU, 0 )



    # Shift + a を送信する

    SW_MINIMIZE =6
    ctypes.windll.user32.ShowWindow(window_handles[0], SW_MINIMIZE)# 最小化したウィンドウでもPostMessageできる
    ctypes.windll.user32.EnableWindow( window_handles[0], False )# DisableしたウィンドウでもPostMessageできる
    #ctypes.windll.user32.SetForegroundWindow(window_handles[0])
    time.sleep(1)

    # http://delfusa.main.jp/delfusafloor/archive/www.nifty.ne.jp_forum_fdelphi/samples/00027.html
    # https://stackoverflow.com/questions/13200362/how-to-send-ctrl-shift-alt-key-combinations-to-an-application-window-via-sen

    KEY_UP_TO_DOWN = 0x00000000
    KEY_DOWN_TO_UP = 0xC0000000

    ALT_DOWN = 0x20000000





    VirtualKey = ctypes.windll.user32.MapVirtualKeyA(Const.KEY_A, 0);

    #ctypes.windll.user32.PostMessageW( hWnds[0], win32con.WM_IME_KEYDOWN, win32con.VK_SHIFT, 1 )#0x0001 | 0x2A<<16 )
    #ctypes.windll.user32.PostMessageW( hWnds[0], win32con.WM_KEYDOWN, Const.KEY_A, 0)#0x0001 | 0x1E<<16 )

    #ctypes.windll.user32.PostMessageW( hWnds[0], win32con.WM_KEYUP, Const.KEY_A, 0)#0x0001 | 0x1E<<16 | KEY_DOWN_TO_UP )
    #ctypes.windll.user32.PostMessageW( hWnds[0], win32con.WM_IME_KEYUP, win32con.VK_SHIFT, 0 )#KEY_DOWN_TO_UP )

    for i in range(200):
        SendKeys( hWnds[0], Const.KEY_A, notepad._handle )



    # 非アクティブなウィンドウへのSendInputはできない
    #pid = ctypes.wintypes.DWORD()
    #tid = ctypes.windll.user32.GetWindowThreadProcessId( hWnds[0], ctypes.byref(pid) )
    
    #currentitd = ctypes.windll.kernel32.GetCurrentThreadId()#currentitd = threading.current_thread().ident#
    #ctypes.windll.user32.AttachThreadInput( currentitd, tid, True )

    #SendInput( Keyboard(Const.KEY_A) )
    #SendInput( Keyboard(Const.KEY_A, KEYEVENTF_KEYUP) )

    #ctypes.windll.user32.AttachThreadInput( currentitd, tid, False )



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



