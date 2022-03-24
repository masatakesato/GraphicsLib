# http://timgolden.me.uk/python/win32_how_do_i/find-the-window-for-my-subprocess.html

from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *

# pywin32 module required. 
import win32con
import win32gui
import win32process

import sys
import subprocess
import time


def get_hwnds_for_pid (pid):
	def callback (hwnd, hwnds):
		if win32gui.IsWindowVisible (hwnd) and win32gui.IsWindowEnabled (hwnd):
			_, found_pid = win32process.GetWindowThreadProcessId (hwnd)
			if found_pid == pid:
				hwnds.append (hwnd)
		return True

	hwnds = []
	win32gui.EnumWindows (callback, hwnds)
	return hwnds


if __name__ == "__main__":

	app = QApplication( sys.argv )

	notepad = subprocess.Popen( [r"C:\\Windows\\system32\\notepad.exe"] )

	time.sleep (2.0)

#TODO: runProgramからhwnd引っ張り出す
	for hwnd in get_hwnds_for_pid( notepad.pid ):
		window_wrapper = QWindow.fromWinId( hwnd )
		window_widget = QWidget.createWindowContainer( window_wrapper )
		window_widget.show()

	sys.exit( app.exec_() )








#from PyQt5.QtCore import *
#from PyQt5.QtWidgets import *
#from PyQt5.QtGui import *



#https://stackoverflow.com/questions/17272572/get-window-handler-from-started-process

#import win32process
#import win32process as process
#import win32gui
#import sys

#PORTABLE_APPLICATION_LOCATION = "C:\\Windows\\system32\\notepad.exe"
#processHandler = -1


#def isLiveProcess( processHandler ): #Process handler is dwProcessId
#	processList = process.EnumProcesses()
#	for aProcess in processList:
#		if( aProcess == processHandler ):
#			return True
#	return False


#def callback( hwnd, procid ):
#	if procid in win32process.GetWindowThreadProcessId( hwnd ):
#		win32gui.ShowWindow( hwnd, 1 )#win32gui.SetForegroundWindow(hwnd)


#def show_window_by_process( procid ):
#	win32gui.EnumWindows( callback, procid )



#def runProgram():
#	global processHandler
#	#don't run a process more than once
#	if (isLiveProcess(processHandler) ):
#		#Bring focus back to running window!
#		show_window_by_process( processHandler )
#		print( "processHandler: " , processHandler )
#		return
#	try:
#		startObj = process.STARTUPINFO()
#		myProcessTuple = process.CreateProcess(PORTABLE_APPLICATION_LOCATION,None,None,None,8,8,None,None,startObj)
#		processHandler = myProcessTuple[2]
#		print( "processHandler: " , processHandler )
#	except:
#		print(sys.exc_info[0])





#if __name__ == "__main__":

#	runProgram()

#	app = QApplication( sys.argv )

##TODO: runProgramからhwnd引っ張り出す
#	window_wrapper = QWindow.fromWinId( hwnd )
#	#window_widget = createWindowContainer( window_wrapper )

#	#window_widget.show()

#	sys.exit(app.exec_())