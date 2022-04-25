﻿


class EventBase:

    def __init__( self, msg, time, hwnd, windowtitle, injected ):
        self.__m_MessageName = None
        self.__m_Message = msg
        self.__m_Time = time
        self.__m_WindowHandle = hwnd
        #self.__m_ProcessID  =
        #self.__m_ThreadID  =
        self.__m_WindowName = windowtitle
        self.__m_Injected = injected




class MouseEvent( EventBase ):

    def __init__( self, msg, x, y, mousedata, flags, time, hwnd, windowtitle ):
        super(MouseEvent, self).__init__( msg, time, hwnd, windowtitle, flags & 0x01 )

        self.__m_Position = (x, y)
        self.__m_Wheel = 0 if mousedata==0 else -1 if mousedata > 0x80000000 else 1


#*   MessageName: mouse left down ------> Messageの名前. MsgToName辞書
#*   Message: 256 ---------------------> wParam. WM_LBUTTONDOWNとかWM_LBUTTONUPとか.
#*   Time: 2426687 ---------------------> KBDLLHOOKSTRUCT.timeで取得可能
#*   Window handle: 3409886
#*   WindowName: C:\WINDOWS\system32\cmd.exe
#*    Position:    -------------------------> MSLLHOOKSTRUCT.pt.x/y で取得可能
#?    Wheel
#    Injected     --------------------> プログラムで生成されたコマンドかどうかフラグ. MSLLHOOKSTRUCT.flags & 0x01 で取得可能.


    def Wheel( self ):
        return self.__m_Wheel




class KeyboardEvent( EventBase ):

    def __init__( self, msg, vkcode, scancode, flags, time, hwnd, windowtitle ):
        super(KeyboardEvent, self).__init__( msg, time, hwnd, windowtitle, flags & 0x10 )

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



