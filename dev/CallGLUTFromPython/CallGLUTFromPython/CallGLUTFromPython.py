# http://timgolden.me.uk/python/win32_how_do_i/find-the-window-for-my-subprocess.html

## pywin32 module required. 
#import win32con
#import win32gui
#import win32process

#import subprocess
#import time


#if __name__ == '__main__':

#	notepad = subprocess.Popen( [r"nodepad.exe"] )

#	for hwnd in get_hwnds_for_pid( notepad.pid ):

#	pass




#https://stackoverflow.com/questions/17272572/get-window-handler-from-started-process

import win32process
import win32process as process
import win32gui
import sys

PORTABLE_APPLICATION_LOCATION = "C:\\Windows\\system32\\notepad.exe"
processHandler = -1


def callback(hwnd, procid):
	if procid in win32process.GetWindowThreadProcessId(hwnd):
		win32gui.ShowWindow( hwnd, 1 )#win32gui.SetForegroundWindow(hwnd)


def show_window_by_process(procid):
	win32gui.EnumWindows(callback, procid)



def runProgram():
	global processHandler
	#don't run a process more than once
	if (isLiveProcess(processHandler) ):
		#Bring focus back to running window!
		show_window_by_process(processHandler)
		print( "processHandler: " , processHandler )
		return
	try:
		startObj = process.STARTUPINFO()
		myProcessTuple = process.CreateProcess(PORTABLE_APPLICATION_LOCATION,None,None,None,8,8,None,None,startObj)
		processHandler = myProcessTuple[2]
		print( "processHandler: " , processHandler )
	except:
		print(sys.exc_info[0])



def isLiveProcess( processHandler ): #Process handler is dwProcessId
	processList = process.EnumProcesses()
	for aProcess in processList:
		if( aProcess == processHandler ):
			return True
	return False


runProgram()