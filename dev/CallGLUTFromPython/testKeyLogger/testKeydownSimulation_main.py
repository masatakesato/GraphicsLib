# https://stackoverflow.com/questions/11906925/python-simulate-keydown
import time

from keylogger.sender import *



if __name__=="__main__":

    SendInput( Keyboard(Const.VK_CONTROL) )
    time.sleep( 0.05 )
    SendInput( Keyboard(Const.VK_CONTROL, KEYEVENTF_KEYUP) )


    SendInput( Mouse( MOUSEEVENTF_MIDDLEDOWN ) )
    SendInput( Mouse( MOUSEEVENTF_MIDDLEUP ) )

    SendInput( Mouse( MOUSEEVENTF_MIDDLEDOWN ) )
    SendInput( Mouse( MOUSEEVENTF_MIDDLEUP ) )

    #SendInput( Mouse( MOUSEEVENTF_MOVE, 20, 20 ) )
    MouseMove( 20, 20 )

    #SendInput( Keyboard(KEY_C ) )
    #SendInput( Keyboard(KEY_C ) )

    #SendInput(Keyboard(VK_RETURN))