#include	<iostream>
using namespace std;


#include	<AntTweakBar.h>


#include	<oreore/QuadShader.h>
#include	<labworks/ProceduralShader.h>
//#include	<labworks/GeometricalNoiseGPU.h>
#include	<labworks/GeneralProceduralShader.h>

#include	<gl/freeglut.h>


using namespace OreOreLib;



ProceduralMaterial		g_ProcMat;	// プロシージャルテクスチャ制御パラメータへのポインタ
//GeometricalNoiseParam	g_ProcMat2;

QuadShader				g_QuadShader;
ProceduralShader		g_ProceduralShader;
//GeometricalNoiseGPU		g_GeomNoiseGenerator;



int					g_ScreenWidth	= 800;
int					g_ScreenHeight	= 800;


#define KEY_ESCAPE 27



void display()
{
	// RenderSceneのアップデート
	//g_SceneGraph->Update();
	//g_RenderScene->Update();
	
	// Clear Screen and Depth Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor( 0.2f, 0.2f, 0.2f, 0.0f ); 

	// シーンの描画
	//g_RenderScene->Render2Texture();
	//g_RenderScene->Render();
	g_ProceduralShader.SetParams( &g_ProcMat );
	g_ProceduralShader.Render();
	//g_ProceduralShader.Render2Texture( &g_Fbo );
	
	
	// AntTweakBarの描画
	TwDraw();

	// g_rotation += g_rotation_speed;
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

	g_ProceduralShader.InitShader( "../shader/ProceduralTexture.glsl" );
	g_QuadShader.InitShader( "../shader/QuadShader.glsl" );


	AllocateProceduralMaterial( &g_ProcMat, 2 );
	
	*g_ProcMat.RingInterval()	= 1.0f;
	*g_ProcMat.EdgeSmoothness()	= 0.0f;

	*g_ProcMat.Amplitude(0)		= 1.0f;
	*g_ProcMat.Stretch(0)		= 1.0f;
	*g_ProcMat.Roughness(0)		= 1.0f;

	*g_ProcMat.Amplitude(1)		= 1.0f;
	*g_ProcMat.Stretch(1)		= 1.0f;
	*g_ProcMat.Roughness(1)		= 1.0f;


	//g_ProcMat2.Scale.x			= 1.0f;
	//g_ProcMat2.Scale.y			= 1.0f;
	//g_ProcMat2.Scale.z			= 1.0f;

	//g_ProcMat.Octaves			= 1.0f;
	//g_ProcMat.Lacunarity		= 1.0f;
	//g_ProcMat.Gain				= 1.0f;

	//g_ProcMat2.Bin				= 1.0f;
	//g_ProcMat2.Gap				= 0.0f;
	//g_ProcMat2.Min				= 0.0f;
	//g_ProcMat2.Max				= 1.0f;


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
	glutCreateWindow( "ProceduralTexture" );					// create Window
	glewInit();
	glutDisplayFunc(display);						// register Display Function
	glutReshapeFunc(reshape);
	glutIdleFunc( display );						// register Idle Function
	//glutKeyboardFunc( keyboard );					// register Keyboard Handler
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

	
	TwAddVarRW( bar, "RingInterval", TW_TYPE_FLOAT, g_ProcMat.RingInterval(),
				" label='RingInterval' min=0.0 max=100.0 step=0.001 keyIncr=z keyDecr=Z ");
	
	TwAddVarRW( bar, "EdgeSmoothness", TW_TYPE_FLOAT, g_ProcMat.EdgeSmoothness(),
				" label='EdgeSmoothness' min=0.0 max=100.0 step=0.001 keyIncr=z keyDecr=Z ");

	TwAddVarRW( bar, "Amplitude1", TW_TYPE_FLOAT, g_ProcMat.Amplitude(0),
				" label='Amplitude1' min=0.0 max=10.0 step=0.001 keyIncr=z keyDecr=Z ");

	TwAddVarRW( bar, "Stretch1", TW_TYPE_FLOAT, g_ProcMat.Stretch(0),
				" label='Stretch1' min=0.0 max=10000.0 step=0.001 keyIncr=z keyDecr=Z ");

	TwAddVarRW( bar, "Roughness1", TW_TYPE_FLOAT, g_ProcMat.Roughness(0),
				" label='Roughness1' min=0.01 max=10000.0 step=0.001 keyIncr=z keyDecr=Z ");

	TwAddVarRW( bar, "Amplitude2", TW_TYPE_FLOAT, g_ProcMat.Amplitude(1),
				" label='Amplitude2' min=0.0 max=10.0 step=0.001 keyIncr=z keyDecr=Z ");

	TwAddVarRW( bar, "Stretch2", TW_TYPE_FLOAT, g_ProcMat.Stretch(1),
				" label='Stretch2' min=0.0 max=10000.0 step=0.001 keyIncr=z keyDecr=Z ");

	TwAddVarRW( bar, "Roughness2", TW_TYPE_FLOAT, g_ProcMat.Roughness(1),
				" label='Roughness2' min=0.01 max=10000.0 step=0.001 keyIncr=z keyDecr=Z ");


	//TwAddVarRW( bar, "ScaleX", TW_TYPE_FLOAT, &g_ProcMat2.Scale.x,
	//			" label='ScaleX' min=0.01 max=10000.0 step=0.001 keyIncr=z keyDecr=Z ");

	//TwAddVarRW( bar, "ScaleY", TW_TYPE_FLOAT, &g_ProcMat2.Scale.y,
	//			" label='ScaleY' min=0.01 max=10000.0 step=0.001 keyIncr=z keyDecr=Z ");

	//TwAddVarRW( bar, "ScaleZ", TW_TYPE_FLOAT, &g_ProcMat2.Scale.z,
	//			" label='ScaleZ' min=0.01 max=10000.0 step=0.001 keyIncr=z keyDecr=Z ");


	//TwAddVarRW( bar, "Bin", TW_TYPE_FLOAT, &g_ProcMat2.Bin,
	//			" label='Bin' min=0.01 max=10000.0 step=0.001 keyIncr=z keyDecr=Z ");

	//TwAddVarRW( bar, "Gap", TW_TYPE_FLOAT, &g_ProcMat2.Gap,
	//			" label='Gap' min=0.01 max=10000.0 step=0.001 keyIncr=z keyDecr=Z ");

	//TwAddVarRW( bar, "Min", TW_TYPE_FLOAT, &g_ProcMat2.Min,
	//			" label='Min' min=0.01 max=10000.0 step=0.001 keyIncr=z keyDecr=Z ");

	//TwAddVarRW( bar, "Max", TW_TYPE_FLOAT, &g_ProcMat2.Max,
	//			" label='Max' min=0.01 max=10000.0 step=0.001 keyIncr=z keyDecr=Z ");




//	static float aaa = 0.0f;
	
//	TwAddVarRW( bar, "test", TW_TYPE_FLOAT, &aaa,
//				" label='test' min=0.01 max=10000.0 step=0.02 ");
	
//	TwAddVarRW( bar, "Param4", TW_TYPE_DIR3F, g_refProcParam->Param4.xyz,
//				" label='Param4' opened=true help='Change the direction.' " );

	TwWindowSize( g_ScreenWidth, g_ScreenHeight);

	glutMainLoop();							// run GLUT mainloop


	TwTerminate();

	return 0;
}