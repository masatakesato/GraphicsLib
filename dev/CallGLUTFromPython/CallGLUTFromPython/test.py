#import os
#import time
#import win32.win32gui as win32gui
#import sys
#import os
#from PyQt5.QtCore import *
#from PyQt5.QtWidgets import *
#from PyQt5.QtGui import *
#import subprocess

#import win32con
#import win32gui
#import win32process


##class MainWindow(QtWidgets.QMainWindow):
##    def __init__(self):
##        super(MainWindow, self).__init__()

##        widget = QtWidgets.QWidget()
##        layout = QtWidgets.QGridLayout(widget)
##        self.setCentralWidget(widget)

##        exePath = "C:\\Windows\\system32\\calc.exe"
##        # subprocess.Popen(exePath)
##        os.system('"%s"' % exePath)
##        hwnd = win32gui.FindWindow(0, 'Calculator')
##        time.sleep(0.05)
##        print(hwnd)
##        self.window = QtGui.QWindow.fromWinId(hwnd)    
##        self.windowcontainer = self.createWindowContainer(self.window, widget)

##        layout.addWidget(self.windowcontainer, 0, 0)

##if __name__ == '__main__':

##    app = QtWidgets.QApplication(sys.argv)
##    app.setStyle("fusion")
##    form = MainWindow()
##    form.setWindowTitle('Embed')
##    form.setGeometry(100, 100, 400, 600)
##    form.show()
##    sys.exit(app.exec_())


## Second Solution
##class MainWindow(QtWidgets.QMainWindow):
##     def __init__(self):
##         super(MainWindow, self).__init__()

##         widget = QtWidgets.QWidget()
##         layout = QtWidgets.QGridLayout(widget)
##         self.setCentralWidget(widget)

##         exePath = "C:\Windows\system32\calc.exe"
##         subprocess.Popen(exePath)
##         # os.system('"%s"' % exePath)
##         hwnd = win32gui.FindWindow(0, 'Calculator')
##         time.sleep(0.05)
##         window = QtGui.QWindow.fromWinId(hwnd)
##         self.createWindowContainer(window,self)
##         # self.setGeometry(500,500,450,400)
##         self.setWindowTitle("File Dialog")
##         self.show()

##if __name__ == '__main__':
##     app = QtWidgets.QApplication(sys.argv)
##     app.setStyle("fusion")
##     form = MainWindow()
##     form.setWindowTitle('My Embedded Calculator')
##     form.setGeometry(600, 600, 600, 600)
##     form.show()
##     sys.exit(app.exec_())


#def get_hwnds_for_pid (pid):
#	def callback (hwnd, hwnds):
#		if win32gui.IsWindowVisible (hwnd) and win32gui.IsWindowEnabled (hwnd):
#			_, found_pid = win32process.GetWindowThreadProcessId (hwnd)
#			if found_pid == pid:
#				hwnds.append (hwnd)
#		return True

#	hwnds = []
#	win32gui.EnumWindows (callback, hwnds)
#	return hwnds


## Solution 3
#class MainWindow( QMainWindow ):

#	def __init__(self):
#		super(MainWindow, self).__init__()

#		# create a process
#		exePath = "C:\\Windows\\system32\\notepad.exe"

#		notepad = subprocess.Popen( [exePath] )

#		#subprocess.Popen(exePath)
#		# os.system(exePath)
#		#hwnd = win32gui.FindWindow(0, "" )
#		#print(hwnd)
#		time.sleep(0.05)

#		#window = QtGui.QWindow.fromWinId(hwnd)
#		#self.createWindowContainer(window, self)
#		self.setGeometry(0, 0, 502, 752)
#		self.setWindowTitle('Embedded Window')

#		hwnds = get_hwnds_for_pid( notepad.pid )

#		for hwnd in get_hwnds_for_pid( notepad.pid ):
#			window_wrapper = QWindow.fromWinId( hwnd )
#			self.createWindowContainer( window_wrapper, self )
#			#window_widget.show()

#		self.activateWindow()


#if __name__ == '__main__':
#	app = QApplication(sys.argv)
#	form = MainWindow()
#	form.setWindowTitle(' Embedded ')
#	form.setGeometry(100, 100, 400, 600)
#	form.show()
#	sys.exit(app.exec_())




# https://code.tiblab.net/python/pyhook

import sys
import threading

from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *

import pythoncom, pyWinhook, ctypes

import win32api
import win32con
import win32gui
import win32process



class Window( QFrame ):

	def __init__(self, parent=None):
		super(Window, self).__init__(parent)

		self.blockKey = False

		self.hm = pyWinhook.HookManager()

		self.label = QLabel('hook:',self)

		#self.timer = QTimer(self)
		#self.timer.timeout.connect( self.hook )#self.hookThread )#
		#self.timer.start()

		self.th = threading.Thread( target=self.hook )#self.hookThread )
		self.th.daemon = True
		self.th.start()



	def closeEvent( self,event ):
		print( 'closeEvent' )
		ctypes.windll.user32.PostQuitMessage(0)


	#def setTimer( self ):
	#	print('setTimer')
	#	self.timer = QTimer(self)
	#	self.timer.timeout.connect(self.hook)
	#	self.timer.start()


	def hook( self ):
		print( 'hook' )
		self.hm.KeyDown = self.hookEvent
		self.hm.HookKeyboard()
		pythoncom.PumpMessages()
		#self.label.setText(self.label.text())#for update


	def hookEvent( self, event ):
		print('hookEvent')
		
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

		#ctrl_pressed = pyWinhook.GetKeyState( pyWinhook.HookConstants.VKeyToID('VK_LSHIFT') )
		#print( "shift_pressed: ", bool(ctrl_pressed), event.KeyID )

		#if( pyWinhook.HookConstants.IDToName( event.KeyID )=='F' ):
		#	self.blockKey = self.blockKey ^ True
		#	print( "blockKey:",  )

		if( event.Window == 67204 ):# 特定のウィンドウに対するキー入力を無効化できる
			return False

		self.label.setText( event.Key )#self.label.text() + event.Key )
		#ctypes.windll.user32.PostQuitMessage(0)
		return True


	def hookThread( self ):
		print( 'hookThread' )
		self.hm.KeyDown = self.hookEventThread
		self.hm.HookKeyboard()
		pythoncom.PumpMessages()
		#self.label.setText(self.label.text())#for update

	def hookEvent2( self, event ):
		print('hookEvent2')
		self.label.setText( event.Key )#self.label.text() + event.Key )
		#ctypes.windll.user32.PostQuitMessage(0)

	def hookEventThread( self, event ):
		print('hookEventThread')
		th = threading.Thread( target=self.hookEvent2, args=(event,))
		th.daedaemon = True
		th.start()
		return True



if __name__ == '__main__':
	app = QApplication( sys.argv )
	window = Window()
	window.show()
	sys.exit( app.exec_() )







## https://tousu.in/qa/?qa=1087440/
## キー大量に推し過ぎるとクラッシュする

## https://stackoverflow.com/questions/14449956/python-crash-with-pyhook-hookmanager-on-keydown-ctrlc

#import threading
#import pythoncom, pyWinhook, ctypes

#def OnKeyboardEvent(event):
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
#	print( '---' )

#	#ctypes.windll.user32.PostQuitMessage(0)

#	# return True to pass the event to other handlers
#	return True


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

#	hm = pyWinhook.HookManager()
#	hm.KeyDown = OnKeyboardEvent#OnKeyboardEventThread#
#	hm.HookKeyboard()
#	pythoncom.PumpMessages()

#	#while True:
#	#	try:
#	#		while True:
#	#			pythoncom.PumpWaitingMessages()
#	#	except KeyboardInterrupt:
#	#		print("TGR$T$#T#T$")
#	#		pass



#	print("gfdgfdggfdgfgd")

#	for i in range(10):
#		time.sleep(0.5)
#		print("-+++")