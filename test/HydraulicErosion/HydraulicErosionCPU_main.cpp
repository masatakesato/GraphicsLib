#include	<iostream>
using namespace std;


#include	<oreore/GLPrimitives.h>
#include	<labworks/ErosionShader.h>
using namespace OreOreLib;

#include	"ErosionSolverCPU.h"



#include	<GL/glew.h>
#pragma comment(lib, "glew32.lib")
#include	<GL/glut.h>


#define KEY_ESCAPE 27



GLVertexArrayObject		g_ScreenSpaceQuad;

// 浸食シミュレータ
ErosionSimulator	*g_ErosionSim		= NULL;

// reference to simulation buffer
refErosionBuffer	g_refErosionBuffer = {0};

// シミュレーション過程のテクスチャを描画するシェーダー
ErosionShader		*g_erosionShader		= NULL;






typedef struct
{
    int width;
	int height;
	char* title;

	float field_of_view_angle;
	float z_near;
	float z_far;
} glutWindow;

glutWindow win;




void display()
{
	// Clear Screen and Depth Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor( 0.2f, 0.2f, 0.2f, 0.0f ); 

	// シミュレーション実行
	g_ErosionSim->DropRandomWater();
	g_ErosionSim->Update();//g_ErosionSim->TestAdvection();//

	
	//cout << g_ErosionSim->GetToalWaterAmount() << endl;

	g_refErosionBuffer.refHeight->Transfer2GPU();
	g_refErosionBuffer.refTerrainNormal->Transfer2GPU();
	g_refErosionBuffer.refWaterLevel->Transfer2GPU();
	g_refErosionBuffer.refWaterNormal->Transfer2GPU();
	//g_refErosionBuffer.refPressureTex->Transfer2GPU();
	//g_refErosionBuffer.refAccelTex->Transfer2GPU();
	
	g_refErosionBuffer.refVelocity->Transfer2GPU();
	//g_refErosionBuffer.refCapacityTex->Transfer2GPU();
	g_refErosionBuffer.refSediment->Transfer2GPU();
	g_refErosionBuffer.refRegolith->Transfer2GPU();


	// シーンの描画
	g_erosionShader->Render();

	glutSwapBuffers();
}


void initialize () 
{

	Vec3f u={1,0,0};
	Vec3f v={0,1,0};
	Vec3f n;

	CrossProduct( n, u, v );



	// initilaize erosion simulator
	g_ErosionSim = new ErosionSimulator( 512, 512, 1.0f, 1 );
g_ErosionSim->InitHeightExamples();
//g_ErosionSim->InitAdvectionTest();//
g_ErosionSim->SetMackCormakAdvection( true );

	// get reference to height map
	g_refErosionBuffer.refHeight		= g_ErosionSim->GetTerrainHeightMap();
	g_refErosionBuffer.refHeight->SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );
	g_refErosionBuffer.refHeight->GenHardwareTexture();
	g_refErosionBuffer.refHeight->Transfer2GPU();

	// get reference to terrain normal
	g_refErosionBuffer.refTerrainNormal	= g_ErosionSim->GetTerrainNormal();
	g_refErosionBuffer.refTerrainNormal->GenHardwareTexture();
	g_refErosionBuffer.refTerrainNormal->Transfer2GPU();

	// get reference to water level
	g_refErosionBuffer.refWaterLevel	= g_ErosionSim->GetWaterLevelMap();
	g_refErosionBuffer.refWaterLevel->GenHardwareTexture();
	g_refErosionBuffer.refWaterLevel->Transfer2GPU();

	// get reference to water surface normal
	g_refErosionBuffer.refWaterNormal	= g_ErosionSim->GetWaterNormal();
	g_refErosionBuffer.refWaterNormal->GenHardwareTexture();
	g_refErosionBuffer.refWaterNormal->Transfer2GPU();


#ifdef _DEBUG
	// get reference to pressure
	g_refErosionBuffer.refPressure	= g_ErosionSim->GetPressureMap();
	g_refErosionBuffer.refPressure->GenHardwareTexture();
	g_refErosionBuffer.refPressure->Transfer2GPU();

	// get reference to accelation map
	g_refErosionBuffer.refAccel		= g_ErosionSim->GetAccelationMap();
	g_refErosionBuffer.refAccel->GenHardwareTexture();
	g_refErosionBuffer.refAccel->Transfer2GPU();
#endif

	// get reference to flow amount map
	g_refErosionBuffer.refFlow	= g_ErosionSim->GetFlowAmountMap();
	g_refErosionBuffer.refFlow->GenHardwareTexture();
	g_refErosionBuffer.refFlow->Transfer2GPU();

	// get water velocity map
	g_refErosionBuffer.refVelocity	= g_ErosionSim->GetVelocityMap();
	g_refErosionBuffer.refVelocity->GenHardwareTexture();
	g_refErosionBuffer.refVelocity->Transfer2GPU();

	// get sediment transport capacity
	g_refErosionBuffer.refCapacity	= g_ErosionSim->GetCapacity();
	g_refErosionBuffer.refCapacity->GenHardwareTexture();
	g_refErosionBuffer.refCapacity->Transfer2GPU();

	// get sediment
	g_refErosionBuffer.refSediment	= g_ErosionSim->GetSedimentMap();
	g_refErosionBuffer.refSediment->GenHardwareTexture();
	g_refErosionBuffer.refSediment->Transfer2GPU();

	// get regolith
	g_refErosionBuffer.refRegolith	= g_ErosionSim->GetRegolith();
	g_refErosionBuffer.refRegolith->GenHardwareTexture();
	g_refErosionBuffer.refRegolith->Transfer2GPU();

	// initialize quad shader
	CreateScreenSpaceQuad( g_ScreenSpaceQuad );
	g_erosionShader = new ErosionShader();
	g_erosionShader->InitShader( _T("../shader/glsl/ErosionShader.glsl"), GLSL_330 );
	g_erosionShader->BindTexture( (const refErosionBuffer *)&g_refErosionBuffer );
	g_erosionShader->BindScreenSpaceQuad( &g_ScreenSpaceQuad );

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

int main(int argc, char **argv) 
{
	// set window values
	win.width = 1024;
	win.height = 1024;
	win.title = "";
	win.field_of_view_angle = 45;
	win.z_near = 1.0f;
	win.z_far = 500.0f;

	// initialize and run program
	glutInit(&argc, argv);                                      // GLUT initialization
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );  // Display Mode
	glutInitWindowSize(win.width,win.height);				// set window size
	glutCreateWindow(win.title);					// create Window
	glewInit();
	glutDisplayFunc(display);						// register Display Function
	glutIdleFunc( display );						// register Idle Function
    glutKeyboardFunc( keyboard );						// register Keyboard Handler
	initialize();
	glutMainLoop();							// run GLUT mainloop
	

	return 0;
}