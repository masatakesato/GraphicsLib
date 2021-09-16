#include	<iostream>
using namespace std;

#include	<AntTweakBar/AntTweakBar.h>

#include	<oreore/common/TString.h>

#include	<graphics/gl4x/common/GLSamplerState.h>
#include	<graphics/gl4x/resource/GLDataFormat.h>
#include	<graphics/gl4x/resource/Texture.h>
#include	<graphics/gl4x/resource/GLFramebufferObject.h>
#include	<graphics/gl4x/resource/GLBindPointManager.h>

#include	<graphics/gl4xext/procedural/WorleyNoiseGPU.h>
#include	<graphics/gl4xext/rendering/QuadShader.h>
#include	<graphics/gl4xext/rendering/EdgeDetectionShader.h>

#include	<gl/freeglut.h>


using namespace OreOreLib;


WorleyNoiseGPU			g_WorleyNoiseShader;
EdgeDetectionShader		g_EdgeDetectionShader;
GLSamplerState			g_SamplerLinear;

Texture2D				g_OutputTexture;
GLFrameBufferObject		g_Fbo;
FrameBufferAttachment	g_Attachment;

int					g_ScreenWidth	= 800;
int					g_ScreenHeight	= 800;
int					g_FilterRadius	= 1;

fBmNoiseParam		g_NoiseParam;



#define KEY_ESCAPE 27



void display()
{	
	// Clear Screen and Depth Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor( 0.0f, 0.8f, 0.0f, 0.0f ); 

	g_WorleyNoiseShader.SetNoiseParam( g_NoiseParam );

	g_Fbo.Bind();
	g_WorleyNoiseShader.Render();
	g_Fbo.Unbind();


	g_EdgeDetectionShader.SetFilterRadius( g_FilterRadius );
	g_EdgeDetectionShader.Render();

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
	GLBindPointManager::InitBindPoints();

	//===================== テクスチャ出力用のバッファ初期化 =======================//
	g_OutputTexture.Allocate( g_ScreenWidth, g_ScreenHeight, 0, 0, DATA_FORMAT::FORMAT_R32G32B32A32_FLOAT );
	g_OutputTexture.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );
	g_OutputTexture.SetAddressMode( TEXTURE_ADDRESS_MODE::TEX_ADDRESS_MIRROR, TEXTURE_ADDRESS_MODE::TEX_ADDRESS_MIRROR );
	g_OutputTexture.GenHardwareTexture();
	
	GLuint channels[] = { GL_COLOR_ATTACHMENT0, };
	g_Attachment.Init( 1, channels, &g_OutputTexture.texID );

	g_Fbo.Init( g_OutputTexture.Width(), g_OutputTexture.Height(), false );
	g_Fbo.Bind();
	g_Attachment.BindAttachment();
	g_Fbo.Unbind();


	//==============================================================================//
	g_NoiseParam.Scale			= 10.0f;
	g_NoiseParam.Angle			= 0.0f;
	g_NoiseParam.Stretch		= 0.0f;
	g_NoiseParam.Octaves		= 1.0f;
	g_NoiseParam.Lacunarity		= 1.5f;
	g_NoiseParam.Gain			= 0.5f;

	//==============================================================================//
	g_EdgeDetectionShader.InitShader( _T("../../../graphics/gl4xext/glsl/rendering/EdgeDetectionShader.glsl"), GLSL_VERSION::GLSL_430 );
	g_EdgeDetectionShader.SetFilterRadius( 1 );
	g_EdgeDetectionShader.BindInputTexture( &g_OutputTexture );

	g_WorleyNoiseShader.InitShader( _T("../../../graphics/gl4xext/glsl/procedural/WorleyNoise_main.glsl"), GLSL_VERSION::GLSL_430 );
	g_WorleyNoiseShader.SetMode( 0 );

	g_SamplerLinear.Init();

	// specify the clear value for the depth buffer
	glClearDepth( 1.0f );														
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );


	g_SamplerLinear.Bind( 0 );

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
	glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS );
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


	bar = TwNewBar("Main");

	TwDefine( " Main label='Texture Parameters' size='200 400' color='96 216 224' " ); // change default tweak bar size and color


	// ノイズ関数合成パラメータを並べる
	TwAddVarRW( bar, "Radius", TW_TYPE_INT32, &g_FilterRadius,
					" label='Radius' min=0 max=64 step=1 keyIncr=z keyDecr=Z " );


	// fBm noise param
	TwAddVarRW( bar, string( "Scale" ).c_str(), TW_TYPE_FLOAT, &g_NoiseParam.Scale,
		string( " label='Scale' min=0.0 max=1000.0 step=0.01 keyIncr=z keyDecr=Z " ).c_str() );

	TwAddVarRW( bar, string( "Angle" ).c_str(), TW_TYPE_FLOAT, &g_NoiseParam.Angle,
		string( " label='Angle' min=0.0 max=360.0 step=0.1 keyIncr=z keyDecr=Z " ).c_str() );

	TwAddVarRW( bar, string( "Stretch" ).c_str(), TW_TYPE_FLOAT, &g_NoiseParam.Stretch,
		string( " label='Stretch' min=0.0 max=10000.0 step=0.1 keyIncr=z keyDecr=Z " ).c_str() );

	TwAddVarRW( bar, string( "Octaves" ).c_str(), TW_TYPE_FLOAT, &g_NoiseParam.Octaves,
		string( " label='Octaves' min=1.0 max=4.0 step=0.01 keyIncr=z keyDecr=Z " ).c_str() );

	TwAddVarRW( bar, string( "Lacunarity" ).c_str(), TW_TYPE_FLOAT, &g_NoiseParam.Lacunarity,
		string( " label='Lacunarity' min=1.0 max=5.0 step=0.01 keyIncr=z keyDecr=Z " ).c_str() );

	TwAddVarRW( bar, string( "Gain" ).c_str(), TW_TYPE_FLOAT, &g_NoiseParam.Gain,
		string( " label='Gain' min=0.0 max=2.0 step=0.01 keyIncr=z keyDecr=Z " ).c_str() );



	
	TwWindowSize( g_ScreenWidth, g_ScreenHeight);

	glutMainLoop();							// run GLUT mainloop


	TwTerminate();

	GLBindPointManager::ReleaseBindPoints();
	g_Fbo.Release();
	g_EdgeDetectionShader.Release();
	g_WorleyNoiseShader.Release();

	return 0;
}