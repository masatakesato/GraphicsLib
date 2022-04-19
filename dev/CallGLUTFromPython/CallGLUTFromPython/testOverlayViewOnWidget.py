import sys
import time
import traceback
import subprocess
import ctypes

from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtOpenGL import *

from OpenGL.GL import *
from OpenGL.GLU import *

import WindowHandleHelper



class glWidget(QGLWidget):
    def __init__(self, parent=None):
        QGLWidget.__init__(self, parent)
        self.setMinimumSize(640, 480)

    def paintGL(self):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glLoadIdentity()
        glTranslatef(-4.5, 0.5, -6.0)
        glColor3f( 1.0, 1.5, 0.0 );
        glPolygonMode(GL_FRONT, GL_FILL);
        glBegin(GL_TRIANGLES)
        glVertex3f(2.0,-1.2,0.0)
        glVertex3f(2.6,0.0,0.0)
        glVertex3f(2.9,-1.2,0.0)
        glEnd()
        glFlush()

    def initializeGL(self):
        glClearDepth(1.0)              
        glDepthFunc(GL_LESS)
        glEnable(GL_DEPTH_TEST)
        glShadeModel(GL_SMOOTH)
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()                    
        gluPerspective(45.0,1.33,0.1, 100.0) 
        glMatrixMode(GL_MODELVIEW)


    def paintEvent( self, event ):
        print( "glWidget::paintEvent()..." )
        return super(glWidget, self).paintEvent( event )




class ExternalAppWidget( QWidget ):

    def __init__( self, parent=None ):
        super( ExternalAppWidget, self ).__init__( parent=parent )

        self.setLayout( QVBoxLayout() ) 
        #self.myEventFilter = MyNativeEventFilter()

        self.__m_AppWindow = None
        self.__m_WindowWidget = None

        self.__p = None


        #self.nonlayoutwidgets = []



    #def __del__( self ):
    #    #self.__p.terminate()
    #    if( self.__m_AppWindow ):
    #        self.__m_AppWindow.close()



    def BindHwnd( self, window_handle, processhandle ):
        
        self.hwnd = window_handle
        #self._style = win32gui.GetWindowLong( self.hwnd, win32con.GWL_EXSTYLE )
        
        self.__m_AppWindow =  QWindow.fromWinId( window_handle )
        self.__m_WindowWidget = QWidget.createWindowContainer( self.__m_AppWindow, self )

        #self.__m_WindowWidget.setFocusPolicy(Qt.StrongFocus)
        #self.__m_WindowWidget.setFocus()
        #self.__m_WindowWidget.grabKeyboard()

        #self.nonlayoutwidgets.append( self.__m_WindowWidget )


        #self.__p = processhandle

        #self.__m_WindowWidget.installEventFilter( self )
        
        #https://programtalk.com/python-examples/ctypes.windll.user32.CreateWindowExW/
        ctypes.windll.user32.EnableWindow( self.hwnd, 0 )

        self.layout().addWidget( self.__m_WindowWidget )#self.__m_WindowWidget.setParent( self )#



    def UnbindHWnd( self ):
        try:
            # 外部アプリ埋め込んだwindowと、はめ込んでる__m_WindowWidgetを、下記の順番でSetParentする必要がある
            if( self.__m_WindowWidget ):
                self.__m_WindowWidget.setParent( None )
                self.__m_AppWindow.setParent( None )
                #win32gui.SetWindowLong( self.hwnd, win32con.GWL_EXSTYLE, self._style )

                self.__m_WindowWidget = None
                self.__m_AppWindow = None
                self.hwnd = None
                #self.nonlayoutwidgets.clear()
                ctypes.windll.user32.EnableWindow( self.hwnd, 0 )

        except Exception as e:
            import traceback
            traceback.print_exc()



    def mousePressEvent( self, event ):
        print( "ExternalAppWidget::mousePressEvent" )
        return super(ExternalAppWidget, self).mousePressEvent(event)


    def mouseReleaseEvent( self, event ):
        print( "ExternalAppWidget::mouseReleaseEvent" )
        return super(ExternalAppWidget, self).mouseReleaseEvent(event)


    def keyPressEvent( self, event ):
        print( "ExternalAppWidget::keyPressEvent" )
        return super(ExternalAppWidget, self).keyPressEvent( event )


    #def resizeEvent(self, event):
    #    for w in self.nonlayoutwidgets:
    #        w.resize(event.size())
    #    event.accept()


    def closeEvent( self, event ):
        print( "MyWidget::closeEvent" )

        self.UnbindHWnd()

        #self.__p.terminate()

        return super(ExternalAppWidget, self).closeEvent( event )





    #def GET_X_PARAM( self, param ):
    #    return param & 0xffff


    #def GET_Y_PARAM( self, param ):
    #    return param >> 16


	# https://jpdebug.com/p/365848
	# https://gitpress.io/u/1155/pyqt-example-NativeEvent
	#def nativeEvent( self, eventType, message ):
	#	retval, result = super(ExternalAppWidget, self).nativeEvent(eventType, message)

	#	print( eventType )

	#	if( eventType == "windows_generic_MSG" ):
	#		msg = ctypes.wintypes.MSG.from_address( message.__int__() )
	#		print( msg.lParam )
	#		x = self.GET_X_PARAM( msg.lParam ) #win32api.LOWORD(msg.lParam)# - self.frameGeometry().x()
	#		y = self.GET_Y_PARAM( msg.lParam ) #win32api.HIWORD(msg.lParam) - self.frameGeometry().y()
	#		#print( x, y )

	#		# https://mcz-xoxo.cocolog-nifty.com/blog/2015/01/post-5960.html
	#		#wparam = msg.wParam
	#		#print( wparam )
	#		#print( msg.message==win32con.WM_KEYDOWN )


	#		# nativeEventFilter
	#		# https://qtcoder.hatenadiary.org/entry/20140626/1403794191 これ？

	#	#	if( self.childAt(x, y) != None ):
	#	#		return retval, result
	#	#	
	#	#	if( msg.message == win32con.WM_NCCALCSIZE ):# ?
	#	#		return True, 0

	#	#	if( msg.message == win32con.WM_GETMINMAXINFO ):# window position or size has been changed
	#	#		info = ctypes.cast( msg.lParam, ctypes.POINTER(MINMAXINFO) ).contents
	#	#		# modify maximized window size to available size
	#	#		#info.ptMaxSize.x = self.
	#	#		#info.ptMaxSize.y = self.
	#	#		info.ptMaxPosition.x = 0
	#	#		info.ptMaxPosition.y = 0

	#	#print( "ExternalAppWidget::nativeEvent: ", message  )

	#	return retval, result





class BackgroundWidget( QWidget ):

    def __init__(self, parent=None):
        super(BackgroundWidget, self).__init__(parent=parent)
        self.nonlayoutwidgets = []


    def mousePressEvent( self, event ):
        print( "BackgroundWidget::mousePressEvent" )
        return super(BackgroundWidget, self).mousePressEvent( event )





class MyFrame( QFrame ):

    def __init__(self, parent=None):
        super(MyFrame, self).__init__(parent=parent)
        self.nonlayoutwidgets = []


    def resizeEvent( self, event ):
        print( "MyFrame::resizeEvent()..." )
        r = self.layout().contentsRect()
        for w in self.nonlayoutwidgets:
            w.resize( r.width(), r.height() )# event.size() )#
            print( w.geometry() )
        event.accept()


    #def moveEvent( self, event ):
    #    super(MyFrame, self).moveEvent( event )
    #    print( "MyFrame::moveEvent()..." )
    #    for w in self.nonlayoutwidgets:
    #        w.move( event.pos() )
    #        print( w.geometry() )
    #    event.accept()

    def moveChildren( self, pos ):
        print( "MyFrame::moveChildren()..." )
        r = self.layout().contentsRect()
        for w in self.nonlayoutwidgets:
            w.move( pos.x() + r.x(), pos.y() + r.y() )
            print( w.geometry() )



    def addChildWidget(self, w):
        w.setParent(self)
        self.nonlayoutwidgets.append(w)


    def mousePressEvent( self, event ):
        print( "MyFrame::mousePressEvent" )
        return super(MyFrame, self).mousePressEvent( event )




class MyView( QGraphicsView ):

    def __init__( self, parent=None ):
        super(MyView, self).__init__(parent=parent)
        self.setEnabled(True)

        #self.setAttribute( Qt.WA_TransparentForMouseEvents, False )
        #self.setOptimizationFlags( QGraphicsView.DontSavePainterState )
        self.setViewportUpdateMode( QGraphicsView.SmartViewportUpdate )
        self.setCacheMode( QGraphicsView.CacheBackground )


    def mousePressEvent( self, event ):
        print( "MyView::mousePressEvent" )
        return super().mousePressEvent(event)


    def keyPressEvent( self, event ):
        print( "MyView::keyPressEvent" )
        return super().keyPressEvent(event)


    def paintEvent( self, event ):
        print( "MyView::paintEvent()..." )
        return super(MyView, self).paintEvent( event )


    #def resizeEvent(self, event):
    #    print( "MyView::resizeEvent()..." )
    #    return super(MyView, self).resizeEvent( event )


    #def moveEvent( self, event ):
    #    #super(MyView, self).moveEvent( event )
    #    print( "MyView::moveEvent()..." )
    #    return super(MyView, self).moveEvent( event )




class MyScene( QGraphicsScene ):

    def __init__( self, *args, **kwargs ):
        super(MyScene, self).__init__(*args, **kwargs)


    def mousePressEvent( self, event ):
        print( "MyScene::mousePressEvent" )
        return super().mousePressEvent(event)



class windowOverlay(QWidget):

    view = None

    def __init__(self, parent=None):
        super(windowOverlay, self).__init__(parent)

        #================ Setup background widget ================#

        #################### Normal QWidget test #####################
        #self.backgroundWidget = BackgroundWidget()
        #self.backgroundWidget.setStyleSheet( 'background-color: rgb(130,130,130);' )
        #self.backgroundWidget.setLayout( QGridLayout() )

        #for i in range(3):
        #    for j in range(4):
        #        w = QWidget()
        #        w.setFixedSize( 50, 50 )
        #        w.setStyleSheet( 'background-color: rgb(30,60,30);' )
        #        #self.backgroundWidget.layout().addWidget(w)
        #        self.backgroundWidget.layout().addWidget( w, i, j )


        ############### Embedded application background test ###############

        #notepad = subprocess.Popen( r"D:/Program Files (x86)/sakura/sakura.exe" )

        #window_handles = []
        #while( WindowHandleHelper.GetWindowHandlesFromPID( notepad.pid, window_handles ) == False ):
        #    print( window_handles )
        #    time.sleep(0.05)

        #time.sleep(0.5)

        #self.backgroundWidget = ExternalAppWidget( self )
        #self.backgroundWidget.BindHwnd( window_handles[0], notepad.pid )
        ##self.backgroundWidget.show()


        #################### OpenGL background test ########################
        self.backgroundWidget = glWidget( self )



        #================ Setup GraphicsScene/View ===============#
        self.scene = MyScene()
        self.view = MyView( )
        self.view.setStyleSheet( 'background: rgba(255, 255, 64, 0);' )#self.view.setStyleSheet( 'background: transparent;' )
        
        #self.view.setParent( self )# Qt.CoverWindow前にやらなきゃダメ
        #self.view.hide()
        self.view.setScene( self.scene )
        
        # https://forum.qt.io/topic/95347/qt-qvulkanwindow-overlay-widgets/3
        # https://stackoverflow.com/questions/52691818/qt-qvulkanwindow-overlay-widgets
        # https://forum.qt.io/topic/89472/qwidget-createwindowcontainer-how-to-put-this-widget-createdby-this-api-to-stackunder-another-qwidget/2
        # https://forum.qt.io/topic/98974/draw-on-other-program/8
# https://gist.github.com/lebedov/5179201 CUDAでフレームバッファだけ転送する? -> でもOpenGLウィジェットの手前にボタンとか配置できないんでは？
  # -> https://stackoverflow.com/questions/11604062/semi-transparent-qwidget-over-qglwidget-strange-results
        #self.view.setAttribute( Qt.WA_NoSystemBackground )# バックグラウンドなし
        #self.view.setAttribute( Qt.WA_TranslucentBackground )
        #self.view.setWindowFlags( Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint)# | Qt.CoverWindow )
        
        #================ compose myFrame ==================#
        #layout = QStackedLayout()
        #layout.setStackingMode( QStackedLayout.StackAll )

        self.myFrame = MyFrame()
        self.myFrame.setLayout( QVBoxLayout() )
        
        #self.myFrame.layout().addWidget( self.view )# NG
        #self.myFrame.addChildWidget( self.view )

        self.myFrame.layout().addWidget( self.backgroundWidget )
        

        self.myFrame.addChildWidget( self.view )#
        self.view.setAttribute( Qt.WA_NoSystemBackground )# バックグラウンドなし
        self.view.setAttribute( Qt.WA_TranslucentBackground )
        self.view.setWindowFlags( Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.CoverWindow )



        self.button = QPushButton("Toggle Overlay")
        self.button.setFixedHeight(50)

        self.verticalLayout = QVBoxLayout( self )# QStackedLayout(self)#
        #self.layout().setStackingMode( QStackedLayout.StackAll )
        self.verticalLayout.addWidget( self.myFrame )#self.view )#
        self.verticalLayout.addWidget( self.button )




        # define rect item
        rect = QGraphicsRectItem()
        rect.setRect( 0, 0, 60, 20 )
        rect.setOpacity( 0.5 )
        rect.setBrush( Qt.white )
        rect.setFlag( QGraphicsItem.ItemSendsGeometryChanges )
        rect.setFlag( QGraphicsItem.ItemIsMovable )
        rect.setFlag( QGraphicsItem.ItemIsSelectable )


        self.scene.addItem( rect )

       
        self.button.clicked.connect( lambda: self.button_off() if self.view.isVisible() else self.button_on() )
        


    def button_on( self ):
        self.view.show()


    def button_off( self ):
        self.view.hide()


    #def resizeEvent( self, event ):
    #    super(windowOverlay, self).resizeEvent( event )
    #    print( "windowOverlay::resizeEvent()..." )
    #    if( self.view ): self.view.resize( self.myFrame.width(), self.myFrame.height() )
    #    event.accept()


    #def moveEvent( self, event ):
    #    super(windowOverlay, self).moveEvent( event )
    #    print( "windowOverlay::moveEvent()..." )
    #    if( self.view ): self.view.move( event.pos() )
    #    event.accept()


    def moveEvent( self, event ):
        super(windowOverlay, self).moveEvent( event )
        print( "windowOverlay::moveEvent()..." )
        if( self.view ): self.myFrame.moveChildren( event.pos() )
        event.accept()


    



if __name__ == "__main__":

    app = QApplication( sys.argv )

    #notepad = subprocess.Popen( r"D:/Program Files (x86)/sakura/sakura.exe" )

    #window_handles = []
    #while( WindowHandleHelper.GetWindowHandlesFromPID( notepad.pid, window_handles ) == False ):
    #    print( window_handles )
    #    time.sleep(0.05)

    #time.sleep(0.5)

    #appWidget = ExternalAppWidget()
    #appWidget.BindHwnd( window_handles[0], notepad.pid )
    #appWidget.setGeometry( 0, 0, 800, 600 )
    #appWidget.show()

    main = windowOverlay()
    main.show()

    sys.exit( app.exec_() )