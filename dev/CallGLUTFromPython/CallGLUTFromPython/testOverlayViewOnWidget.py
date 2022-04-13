import sys
import traceback

from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *



class TestWidget( QWidget ):

    def __init__(self, parent=None):
        super(QWidget, self).__init__(parent=parent)
        self.nonlayoutwidgets = []


    def mousePressEvent( self, event ):
        print( "TestWidget::mousePressEvent" )
        return super(TestWidget, self).mousePressEvent( event )





class MyFrame(QFrame):

    def __init__(self, parent=None):
        super(MyFrame, self).__init__(parent=parent)
        self.nonlayoutwidgets = []


    def resizeEvent(self, event):
        for w in self.nonlayoutwidgets:
            w.resize(event.size())
        event.accept()


    def addChildWidget(self, w):
        w.setParent(self)
        self.nonlayoutwidgets.append(w)


    def mousePressEvent( self, event ):
        print( "MyFrame::mousePressEvent" )
        return super(MyFrame, self).mousePressEvent( event )



class MyView( QGraphicsView ):

    def __init__( self ):
        super(MyView, self).__init__()
        self.setEnabled(True)

        #self.setAttribute( Qt.WA_TransparentForMouseEvents, False )


    def mousePressEvent( self, event ):
        print( "MyView::mousePressEvent" )
        return super().mousePressEvent(event)


    def keyPressEvent( self, event ):
        print( "MyView::keyPressEvent" )
        return super().keyPressEvent(event)



class MyScene( QGraphicsScene ):

    def __init__( self, *args, **kwargs ):
        super(MyScene, self).__init__(*args, **kwargs)


    def mousePressEvent( self, event ):
        print( "MyScene::mousePressEvent" )
        return super().mousePressEvent(event)



class windowOverlay(QWidget):

    def __init__(self, parent=None):
        super(windowOverlay, self).__init__(parent)

        #=================== Setup widget =======================#
        self.widget = TestWidget()
        self.widget.setStyleSheet('background-color: rgb(130,130,130);')
        self.widget.setLayout( QGridLayout() )

        for i in range(3):
            for j in range(4):
                w = QWidget()
                w.setFixedSize(50, 50)
                w.setStyleSheet('background-color: rgb(30,60,30);')
                #self.widget.layout().addWidget(w)
                self.widget.layout().addWidget(w, i,j)


        #================ Setup GraphicsScene/View ===============#
        self.scene = MyScene()
        self.view = MyView()
        self.view.hide()
        self.view.setScene( self.scene )


        #================ compose myFrame ==================#
        self.myFrame = MyFrame()
        self.myFrame.setLayout( QVBoxLayout() )#QBoxLayout( QBoxLayout.LeftToRight ) )
        self.myFrame.layout().addWidget( self.widget )

        self.myFrame.addChildWidget( self.view )# set parent widget without applying layout


        self.button = QPushButton("Toggle Overlay")
        self.button.setFixedHeight(200)

        self.verticalLayout = QVBoxLayout(self)
        self.verticalLayout.addWidget( self.myFrame )#self.view )#
        self.verticalLayout.addWidget(self.button)


        # define rect item
        rect = QGraphicsRectItem()
        rect.setRect( 0, 0, 60, 20 )
        rect.setBrush( Qt.white )
        rect.setFlag( QGraphicsItem.ItemSendsGeometryChanges )
        rect.setFlag( QGraphicsItem.ItemIsMovable )
        rect.setFlag( QGraphicsItem.ItemIsSelectable )


        self.scene.addItem( rect )

        self.view.setStyleSheet( 'background: rgba(255, 255, 64, 25);' )
        #self.view.setStyleSheet( 'background: transparent;' )


        self.button.clicked.connect( lambda: self.button_off() if self.view.isVisible() else self.button_on() )
        


    def button_on( self ):
        self.view.show()
        #self.widget.setAttribute( Qt.WA_TransparentForMouseEvents, True )
        #for i in range(3):
        #    for j in range(4):
        #        w = QWidget()
        #        w.setFixedSize(50, 50)
        #        w.setStyleSheet('background-color: rgb(30,60,30);')
        #        self.myFrame.layout().addWidget(w, i,j)
        #        w.lower()

    def button_off( self ):
        self.view.hide()
        #self.widget.setAttribute( Qt.WA_TransparentForMouseEvents, False )
        



if __name__ == "__main__":
    import sys

    app = QApplication(sys.argv)
    main = windowOverlay()
    main.show()
    sys.exit(app.exec_())