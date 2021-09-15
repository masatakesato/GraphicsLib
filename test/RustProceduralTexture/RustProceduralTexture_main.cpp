#include	<Windows.h>
#include	<iostream>

#include	<GL/glew.h>
#pragma comment(lib, "glew32.lib")
#include	<GL/freeglut.h>

#include	<AntTweakBar/AntTweakBar.h>


#include	<graphics/gl4x/resource/BufferLayout.h>
#include	<graphics/gl4x/resource/Texture.h>


#include	"RustShader.h"
#include	"PerlinNoiseCPU.h"
#include	"SimplexNoiseCPU.h"
#include	"CellNoiseCPU.h"





using namespace std;
using namespace OreOreLib;

#define KEY_ESCAPE 27


Texture2D			g_texSimplexNoise;
Texture2D			g_texPerlinNoise;
RustShader			g_RustShader;


PerlinNoiseCPU		g_PerlinNoiseGenerator;
SimplexNoiseCPU		g_SimplexNoiseGenerator;
CellNoiseCPU		g_CellNoiseGenerator;



static float		g_NoiseScale	= 20.0f;
static float		g_OffsetX		= 0.0f;
static float		g_OffsetY		= 0.0f;
static float		g_OffsetZ		= 0.0f;
static float		g_OffsetW		= 0.0f;
static float		g_Brightness	= 1.0f;



int					g_ScreenWidth	= 800;
int					g_ScreenHeight	= 800;

// mouse event
int			g_mouseX		= 0;
int			g_mouseY		= 0;
int			g_mouseDX		= 0;
int			g_mouseDY		= 0;

bool		g_MouseLeftDown	= false;


// keyboard event
int			g_keyDown[256];




void mouse( int button, int state, int x, int y )
{
	switch( button )
	{
	case GLUT_LEFT_BUTTON:

		switch (state)
		{
		case GLUT_DOWN:	//  ボタンを押した直後の一瞬だけ引っかかる
			g_MouseLeftDown	= true;
			g_mouseX = x;
			g_mouseY = y;
			break;
		
		case GLUT_UP:	//  ボタンを開放した直後の一瞬だけ引っかかる
			g_MouseLeftDown	= false;



			break;

		default:
			break;

		}
		break;

	default:
		break;
	}

}



// マウスカーソルが動いたときに呼び出される関数
void motion( int x , int y )
{
	g_mouseDX	= x - g_mouseX;
	g_mouseDY	= y - g_mouseY;

	g_mouseX	= x;
	g_mouseY	= y;
	
	//printf( "X = %d : Y = %d\n", x, y );
	//printf( "DX = %d : DY = %d\n", g_mouseDX, g_mouseDY );
}



void keydown( unsigned char key, int x, int y )
{
	g_keyDown[ key ] = 1;

	switch ( key ) 
	{
	case KEY_ESCAPE:
		exit ( 0 );   
		break;      

	default:      
		break;
	}
}


void keyup( unsigned char key, int x, int y )
{
	g_keyDown[ key ] = 0;
}



#define DELTA 1.0e-2f

void keyboardCommand()
{
	//CameraObject *pCamera = g_SceneGraph->GetCurrentCamera();

	//if( g_keyDown['w'] )	pCamera->MoveForward( DELTA );
	//if( g_keyDown['s'] )	pCamera->MoveForward( -DELTA );
	//if( g_keyDown['d'] )	pCamera->MoveHorizontal( -DELTA );
	//if( g_keyDown['a'] )	pCamera->MoveHorizontal( DELTA );
	//if( g_keyDown['r'] )	pCamera->MoveVertical( DELTA );
	//if( g_keyDown['f'] )	pCamera->MoveVertical( -DELTA );
	//if( g_keyDown['e'] )	pCamera->Roll( +DELTA );
	//if( g_keyDown['q'] )	pCamera->Roll( -DELTA );
	//
	//if( g_MouseLeftDown && (g_mouseDX != 0 || g_mouseDY != 0) )
	//	pCamera->SetOrientation( float(g_mouseDY)*DELTA, float(g_mouseDX)*DELTA );

	g_mouseDX = 0;
	g_mouseDY = 0;

}


void display()
{
	// キー入力取得
	keyboardCommand();

	
	// Clear Screen and Depth Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor( 0.2f, 0.2f, 0.2f, 0.0f ); 

	// シーンの描画
for( int y=0; y<g_texSimplexNoise.Height(); ++y )
	{
		for( int x=0; x<g_texSimplexNoise.Width(); ++x )
		{
			*(float *)g_texSimplexNoise.Data(x,y) = g_SimplexNoiseGenerator.fBm((double)x/(double)g_texSimplexNoise.Width()*g_NoiseScale + g_OffsetX,
																				(double)y/(double)g_texSimplexNoise.Height()*g_NoiseScale + g_OffsetY,
																				g_OffsetZ
																				//g_OffsetW
																				) * 0.25f;

			*(float *)g_texSimplexNoise.Data(x,y) += g_CellNoiseGenerator.fBm(	(double)x/(double)g_texSimplexNoise.Width()*g_NoiseScale + g_OffsetX,
																				(double)y/(double)g_texSimplexNoise.Height()*g_NoiseScale + g_OffsetY,
																				g_OffsetZ,
																				4, 2.0f, 0.5f
																				) * 0.5f;
			*(float *)g_texSimplexNoise.Data(x,y) *= g_Brightness;
		}// end of x loop
	}// end of y loop

g_texSimplexNoise.Transfer2GPU();







	g_RustShader.Render( g_texSimplexNoise.texID );

	// AntTweakBarの描画
	TwDraw();

	
	glutSwapBuffers();
}



void reshape( int width, int height )
{
	g_ScreenWidth	= width;
	g_ScreenHeight	= height;

    // Send the new window size to AntTweakBar
	TwWindowSize(width, height);
}



void initialize () 
{
	g_texSimplexNoise.Allocate( 512, 512, 0, 0, FORMAT_R32_FLOAT );
	g_texSimplexNoise.GenTexture();

	g_texPerlinNoise.Allocate( 512, 512, 0, 0, FORMAT_R32_FLOAT );
	g_texPerlinNoise.GenTexture();



	g_RustShader.InitShader( "../Shader/RustShader.glsl" );


	// specify the clear value for the depth buffer
	glClearDepth( 1.0f );														
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}


void keyboard ( unsigned char key, int mousePositionX, int mousePositionY )
{ 
  switch ( key ) 
  {
    case KEY_ESCAPE:        
      exit ( 0 );   
      break;      

    default:      
      break;
  }
}

float	g_Zoom = 0.0f;

int main(int argc, char **argv) 
{
	// Ant TweakBar
	TwBar *bar; // Pointer to the tweak bar


	// initialize and run program
	glutInit(&argc, argv);                                      // GLUT initialization
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );  // Display Mode
	glutInitWindowSize( g_ScreenWidth, g_ScreenHeight );		// set window size
	glutCreateWindow( "testProceduralNoise" );					// create Window
	glewInit();
	glutDisplayFunc(display);						// register Display Function
	glutReshapeFunc(reshape);
	glutIdleFunc( display );						// register Idle Function

//	glutMouseFunc( mouse );
//	glutMotionFunc( motion );

//	memset( g_keyDown, 0, sizeof( g_keyDown ) );
//	glutKeyboardFunc( keydown );						// register Keyboard Handler
//	glutKeyboardUpFunc( keyup );						// register Keyboard Handler

	initialize();


	TwInit(TW_OPENGL, NULL);

	// Set GLUT event callbacks
    // - Directly redirect GLUT mouse button events to AntTweakBar
    glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
	// - Directly redirect GLUT mouse motion events to AntTweakBar
    glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
    // - Directly redirect GLUT mouse "passive" motion events to AntTweakBar (same as MouseMotion)
    glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	// - Directly redirect GLUT key events to AntTweakBar
    glutKeyboardFunc((GLUTkeyboardfun)TwEventKeyboardGLUT);
	// - Directly redirect GLUT special key events to AntTweakBar
	glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);
	// - Send 'glutGetModifers' function pointer to AntTweakBar;
	//   required because the GLUT key event functions do not report key modifiers states.
	TwGLUTModifiersFunc(glutGetModifiers);


	bar = TwNewBar("TweakBar");

	TwDefine(" TweakBar size='200 400' color='96 216 224' "); // change default tweak bar size and color


	TwAddVarRW( bar, "NoiseScale", TW_TYPE_FLOAT, &g_NoiseScale,
				" label='NoiseScale' min=0.0 max=100.0 step=0.1 keyIncr=z keyDecr=Z ");


	TwAddVarRW( bar, "OffsetX", TW_TYPE_FLOAT, &g_OffsetX,
				" label='OffsetX' min=-1000.0 max=1000.0 step=0.05 keyIncr=z keyDecr=Z ");


	TwAddVarRW( bar, "OffsetY", TW_TYPE_FLOAT, &g_OffsetY,
				" label='OffsetY' min=-1000.0 max=1000.0 step=0.05 keyIncr=z keyDecr=Z ");


	TwAddVarRW( bar, "OffsetZ", TW_TYPE_FLOAT, &g_OffsetZ,
				" label='OffsetZ' min=-1000.0 max=1000.0 step=0.05 keyIncr=z keyDecr=Z ");

	TwAddVarRW( bar, "OffsetW", TW_TYPE_FLOAT, &g_OffsetW,
				" label='OffsetW' min=-1000.0 max=1000.0 step=0.05 keyIncr=z keyDecr=Z ");

	TwAddVarRW( bar, "Brightness", TW_TYPE_FLOAT, &g_Brightness,
				" label='Brightness' min=0.0 max=1000.0 step=0.01 keyIncr=z keyDecr=Z ");

	//
	//TwAddVarRW( bar, "RingInterval", TW_TYPE_FLOAT, g_procMat->RingInterval(),
	//			" label='RingInterval' min=0.0 max=100.0 step=0.001 keyIncr=z keyDecr=Z ");
	//
	//TwAddVarRW( bar, "EdgeSmoothness", TW_TYPE_FLOAT, g_procMat->EdgeSmoothness(),
	//			" label='EdgeSmoothness' min=0.0 max=100.0 step=0.001 keyIncr=z keyDecr=Z ");

	//TwAddVarRW( bar, "Amplitude1", TW_TYPE_FLOAT, g_procMat->Amplitude(0),
	//			" label='Amplitude1' min=0.0 max=10.0 step=0.001 keyIncr=z keyDecr=Z ");

	//TwAddVarRW( bar, "Stretch1", TW_TYPE_FLOAT, g_procMat->Stretch(0),
	//			" label='Stretch1' min=1.0 max=10000.0 step=0.25 keyIncr=z keyDecr=Z ");

	//TwAddVarRW( bar, "Roughness1", TW_TYPE_FLOAT, g_procMat->Roughness(0),
	//			" label='Roughness1' min=0.01 max=10000.0 step=0.02 keyIncr=z keyDecr=Z ");

	//TwAddVarRW( bar, "Amplitude2", TW_TYPE_FLOAT, g_procMat->Amplitude(1),
	//			" label='Amplitude2' min=0.0 max=10.0 step=0.001 keyIncr=z keyDecr=Z ");

	//TwAddVarRW( bar, "Stretch2", TW_TYPE_FLOAT, g_procMat->Stretch(1),
	//			" label='Stretch2' min=1.0 max=10000.0 step=0.25 keyIncr=z keyDecr=Z ");

	//TwAddVarRW( bar, "Roughness2", TW_TYPE_FLOAT, g_procMat->Roughness(1),
	//			" label='Roughness2' min=0.01 max=10000.0 step=0.02 keyIncr=z keyDecr=Z ");
	
//	static float aaa = 0.0f;
	
//	TwAddVarRW( bar, "test", TW_TYPE_FLOAT, &aaa,
//				" label='test' min=0.01 max=10000.0 step=0.02 ");
	
//	TwAddVarRW( bar, "Param4", TW_TYPE_DIR3F, g_refProcParam->Param4.xyz,
//				" label='Param4' opened=true help='Change the direction.' " );

//	TwWindowSize( g_ScreenWidth, g_ScreenHeight);

	glutMainLoop();							// run GLUT mainloop


	TwTerminate();

	return 0;
}