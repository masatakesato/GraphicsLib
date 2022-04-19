import win32gui
import win32process



def GetWindowHandlesFromPID( pid, window_handles ):

	def callback( hwnd, hwnds ):
		if( win32gui.IsWindowVisible(hwnd) and win32gui.IsWindowEnabled(hwnd) ):
			_, found_pid = win32process.GetWindowThreadProcessId( hwnd )
			if( found_pid == pid ):
				hwnds.append( hwnd )
		return True

	window_handles.clear()
	win32gui.EnumWindows( callback, window_handles )
	return any( window_handles )



def GetChildHandles( hwnd ):

	def callback( hwnd, hwnds ):
		if( win32gui.IsWindowVisible(hwnd) and win32gui.IsWindowEnabled(hwnd) ):
			hwnds.append( hwnd )
		return True

	hwnds = []
	win32gui.EnumChildWindows( hwnd, callback, hwnds )
	return hwnds