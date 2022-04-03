
# https://tousu.in/qa/?qa=1087440/
# キー大量に推し過ぎるとクラッシュする

# https://stackoverflow.com/questions/14449956/python-crash-with-pyhook-hookmanager-on-keydown-ctrlc

#import threading
import pythoncom, pyWinhook, ctypes

def OnKeyboardEvent(event):
    print( 'MessageName:',event.MessageName )
    print( 'Message:',event.Message )
    print( 'Time:',event.Time )
    print( 'Window:',event.Window )
    print( 'WindowName:',event.WindowName )
    print( 'Ascii:', event.Ascii, chr(event.Ascii) )
    print( 'Key:', event.Key )
    print( 'KeyID:', event.KeyID )
    print( 'ScanCode:', event.ScanCode )
    print( 'Extended:', event.Extended )
    print( 'Injected:', event.Injected )
    print( 'Alt', event.Alt )
    print( 'Transition', event.Transition )
    print( '---' )

    if( event.KeyID == 113 ):
        print( "F2 pressed" )

    # return True to pass the event to other handlers
    return True


#def OnKeyboardEvent2(event):
#	print( 'MessageName:',event.MessageName )
#	print( 'Message:',event.Message )
#	print( 'Time:',event.Time )
#	print( 'Window:',event.Window )
#	print( 'WindowName:',event.WindowName )
#	print( 'Ascii:', event.Ascii, chr(event.Ascii) )
#	print( 'Key:', event.Key )
#	print( 'KeyID:', event.KeyID )
#	print( 'ScanCode:', event.ScanCode )
#	print( 'Extended:', event.Extended )
#	print( 'Injected:', event.Injected )
#	print( 'Alt', event.Alt )
#	print( 'Transition', event.Transition )
#	print( '++++++++++++++++' )

#	#ctypes.windll.user32.PostQuitMessage(0)
#	#pythoncom.PumpMessages()


#def OnKeyboardEventThread( event ):
#	th = threading.Thread( target=OnKeyboardEvent2, args=(event,)).start()
#	print('*********************')
#	return True


#import time

#if __name__=="__main__":
#    hm = pyWinhook.HookManager()
#    hm.KeyDown = OnKeyboardEvent#OnKeyboardEventThread#
#    hm.HookKeyboard()
#    pythoncom.PumpMessages()

#	#while True:
#	#	try:
#	#		while True:
#	#			pythoncom.PumpWaitingMessages()
#	#	except KeyboardInterrupt:
#	#		print("TGR$T$#T#T$")
#	#		pass


#    for i in range(10):
#        time.sleep(0.5)
#        print("-+++")








# キー同時押しでクラッシュする例　https://code.tiblab.net/python/pyhook

import sys
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
import pythoncom, pyWinhook, ctypes

class Window(QMainWindow):
    def __init__(self, parent=None):
        super(Window, self).__init__(parent)

        self.label = QLabel('hook:',self)

        self.timer = QTimer(self)
        self.timer.timeout.connect(self.hook)
        self.timer.start( )


    def closeEvent(self,event):
        ctypes.windll.user32.PostQuitMessage(0)


    def hook(self):
        print('hook')
        self.hm = pyWinhook.HookManager()
        self.hm.KeyDown = self.hookEvent
        self.hm.HookKeyboard()
        pythoncom.PumpMessages()
        self.label.setText(self.label.text())#for update


    def hookEvent(self,event):
        self.label.setText(self.label.text()+event.Key)
        #ctypes.windll.user32.PostQuitMessage(0)# ここに置くのはダメ!
        return True

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = Window()
    window.show()
    sys.exit(app.exec_())


# QTimerのせい? -> 違う
# 原因: hookEvent内部で ctypes.windll.user32.PostQuitMessage(0) 呼び出してるから
