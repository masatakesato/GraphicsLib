#include	<GL/freeglut.h>// or glut.h

#include	<oreore/common/TString.h>
#include	<oreore/mathlib/GraphicsMath.h>
#include	<oreore/container/Array.h>

#include	<graphics/gl4x/scene/MeshLoader.h>
#include	<graphics/gl4x/scene/Camera.h>
#include	<graphics/authoring/controller/ControlPoint.h>
#include	"ControlPointExt.h"

using namespace GraphicsLib;
using namespace OreOreLib;



// Lightsource
const GLfloat lightPos[] = { 10 , 10 , 10 , 0 };
const GLfloat lightCol[] = { 1 , 1 , 1 , 1 };


// Camera
Camera	g_Camera;

// Obj Mesh Loader
MeshLoader	g_MeshLoader;

// Mesh data
Array<VertexLayout> g_Vertices;
Array<uint32>	g_Indices;


// Control Points
Array<RadialControlPoint2D<float>>	g_ControlPoints;


Vec4f	g_TestPoint;
Mat4f	g_MatView, g_MatProj, g_MatViewProj, g_MatInvViewProj;




// Mouse
#define MAX_KEYS 256
bool	gKeys[ MAX_KEYS ];// pushed key flags
int		g_CursorX = 0;// cursor screen position x
int		g_CursorY = 0;// cursor screen position y
int		g_CursorDX	= 0;
int		g_CursorDY	= 0;
bool	LeftMouseButtonPressed = false;
float	g_CamSpeed = 0.1f;
bool	g_bDrawVertexBuffer = false;


// Interaction mode
enum INTERACTION_MODE
{
	INTERACTION_MODE_VIEW,
	INTERACTION_MODE_ADD_VIEW,
	INTERACTION_MODE_ADD_CONTROLLER,
	INTERACTION_MODE_MODIFY_CONTROLLER,
};

INTERACTION_MODE	g_InteractionMode = INTERACTION_MODE_VIEW;



static int	g_ScreenWidth	= 1280;
static int	g_ScreenHeight	= 720;



void DrawObjMesh( const MeshLoader& mesh )
{
	auto& vertices		= mesh.GetVertices();
	auto& normals		= mesh.GetNormals();
	auto& faces			= mesh.GetFaceIndices();
	auto& mats			= mesh.GetMaterials();
	auto& matsubsets	= mesh.GetMaterialSubsets();

	int pre_mat = -1, cur_mat = 0;

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glBegin( GL_TRIANGLES );

	for( auto& Face_Iter : faces/* != mesh.GetFaceIndices().end()*/ )// face draw loop
	{
		//　マテリアルがあるとき
		if( !mesh.GetMaterials().Empty() )
		{
			//　インデックスを格納
			cur_mat = matsubsets[ Face_Iter.matsub_index ].material_index;
			
			//　前と異なる色のとき
			if(pre_mat != cur_mat)
			{
				//　Ambient Color
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mats[cur_mat].GetAmbient()->rgba);

				//　Diffuse Color
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mats[cur_mat].GetDiffuse()->rgba);

				//　Specular Color
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mats[cur_mat].GetSpecular()->rgba);
				
				//　Emission
				//glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mats[cur_mat].GetLuminance()->rgb );

				//　Shininess
				glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mats[cur_mat].GetSpecularIntensity());

				//　更新
				pre_mat = cur_mat;
			}
			
		}
		
		// Draw polygons
		const auto& vertAttrIndices = Face_Iter.VertexAttribIndex;
		for( auto& VertAttr_Iter : vertAttrIndices )
		{
			// 法線ベクトル
			if( Face_Iter.use_normal==true )
				glNormal3fv( normals[ VertAttr_Iter.z ].xyz);

			if( VertAttr_Iter.x < 0 || VertAttr_Iter.x > vertices.Length<int32>()-1 )
				tcout << _T("out of range: ") << VertAttr_Iter.x << tendl;

			//　頂点
			glVertex3f(	vertices[ VertAttr_Iter.x ].x,
						vertices[ VertAttr_Iter.x ].y, 
						vertices[ VertAttr_Iter.x ].z );
		}

	}

	glEnd();
}



// Draw world space xyz axis
void DRAW_XYZ()
{
	glBegin(GL_LINES);
		glColor3f(1,0,0);// x
		glVertex3f(0,0,0);
		glVertex3f(1,0,0);
		
		glColor3f(0,1,0);// y
		glVertex3f(0,0,0);
		glVertex3f(0,1,0);
		
		glColor3f(0,0,1);// z
		glVertex3f(0,0,0);
		glVertex3f(0,0,1);
	glEnd();
}



template < typename T >
void DrawRadialControlPoint2D( const RadialControlPoint2D<T>& cp )
{
	glBegin( GL_POINTS );

	glPointSize( 2.5f );
	glVertex3f( cp.Origin().x,  cp.Origin().y );

	glEnd();
}




void ProcessCameraKeys()
{

	if( gKeys[ int('w') ] ){	g_Camera.Transrate(+g_CamSpeed, 0, 0);	}
	if( gKeys[ int('s') ] ){	g_Camera.Transrate(-g_CamSpeed, 0, 0);	}
	if( gKeys[ int('a') ] ){	g_Camera.Transrate(0, +g_CamSpeed, 0);	}
	if( gKeys[ int('d') ] ){	g_Camera.Transrate(0, -g_CamSpeed, 0);	}
	if( gKeys[ int('r') ] ){	g_Camera.Transrate(0, 0, +g_CamSpeed);	}
	if( gKeys[ int('f') ] ){	g_Camera.Transrate(0, 0, -g_CamSpeed);	}
	if( gKeys[ int('q') ] ){	g_Camera.Roll(-g_CamSpeed);				}
	if( gKeys[ int('e') ] ){	g_Camera.Roll(+g_CamSpeed);				}


	//int	i;

	//for(i=0; i<MAX_KEYS; i++)
	//{
	//	if(gKeys[i]==false)	continue;
	//	
	//	switch(i)
	//	{
	//		case 'w' :{	g_Camera.Transrate(+g_CamSpeed, 0, 0);	break; }
	//		case 's' :{	g_Camera.Transrate(-g_CamSpeed, 0, 0);	break; }
	//		case 'a' :{	g_Camera.Transrate(0, +g_CamSpeed, 0);	break; }
	//		case 'd' :{	g_Camera.Transrate(0, -g_CamSpeed, 0);	break; }
	//		case 'r' :{	g_Camera.Transrate(0, 0, +g_CamSpeed);	break; }
	//		case 'f' :{	g_Camera.Transrate(0, 0, -g_CamSpeed);	break; }
	//		case 'q' :{	g_Camera.Roll(-g_CamSpeed);				break; }
	//		case 'e' :{	g_Camera.Roll(+g_CamSpeed);				break; }
	//		default : {	break; }
	//	}// end of switch
	//	
	//}// end of i loop

}



void Initialize()
{
	glClearColor( 0.3f, 0.3f, 0.3f, 1.0f );
	glPointSize(10.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	
	g_Camera.SetViewParameter( {5, 5, 5}, {-1, -1, -1}, {0, 0, 1} );// set position, direction, vertical vector
	//cam = new Camera(5,5,5, -1,-1,-1, 0,0,1);

	MatIdentity( g_MatView );
	MatIdentity( g_MatProj );
	MatIdentity( g_MatViewProj );
	MatIdentity( g_MatInvViewProj );




	TCHAR currdir[ MAX_PATH ];
	GetCurrentDirectory( MAX_PATH, currdir );
	SetCurrentDirectory( _T( "../../../assets/scene/obj" ) );

	g_MeshLoader.Load( _T("viking_room.obj") );

	SetCurrentDirectory( currdir );

	g_MeshLoader.Information();
	//g_MeshLoader.GetGroupInfo(1);

	// Extract polygon vertices/indices from MeshObj.
	g_MeshLoader.GenVertexList( g_Vertices, g_Indices );

}



void display()
{	
	ProcessCameraKeys();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//====================================== Camera setup =======================================//
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	const Vec3f &pos = *g_Camera.GetPosition();
	const Vec3f &dir = *g_Camera.GetForward();
	const Vec3f &up	= *g_Camera.GetVertical();

	gluLookAt(	pos.x, pos.y, pos.z,
				pos.x + dir.x, pos.y + dir.y, pos.z + dir.z,
				up.x, up.y, up.z );
	
glGetFloatv( GL_MODELVIEW_MATRIX, g_MatView.m );// 列優先の行列要素並び順になってる. 
//tcout << g_MatView << tendl;

	//============================== Draw world space axis ================================//
	DRAW_XYZ();
	

	//==================================== Draw OBJLoader ==================================//
	//if( g_bDrawVertexBuffer )
	//{
	//	// Drawing polygon mesh using Veriex/Indices.
	//	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	//	glColor3f( 0.6f, 0.6f, 0.6f );

	//	glEnableClientState(GL_VERTEX_ARRAY);
	//	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);// texture is not supported.

	//	//glTexCoordPointer(2, GL_FLOAT, 0, TexCoordArray);// texture is not supported.
	//	glVertexPointer( 3, GL_FLOAT, sizeof(VertexLayout), g_Vertices.begin() );//glVertexPointer( 3, GL_FLOAT, sizeof(VertexLayout), &(g_Vertices->Position) );

	//	glDrawElements( GL_TRIANGLES, g_Indices.Length(), GL_UNSIGNED_INT, g_Indices.begin() );//glDrawElements( GL_TRIANGLES, g_NumIndices, GL_UNSIGNED_INT, g_Indices );

	//	glDisableClientState(GL_VERTEX_ARRAY);
	//	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);// texture is not supported.
	//}
	//else
	//{
	//	// Put default light
	//	glLightfv(GL_LIGHT0 , GL_POSITION , lightPos);
	//	glLightfv(GL_LIGHT0 , GL_DIFFUSE , lightCol);
	//	glEnable(GL_LIGHTING);
	//	glEnable(GL_LIGHT0);

	//	// Visualize OBJLoader. Using old OpenGL fixed pipeline APIs.
	//	DrawObjMesh( /*g_OBJLoader*/g_MeshLoader );
	//}


	// TestPoint
	glBegin( GL_POINTS );

	glPointSize( 2.5f );
	glColor3f( 1.0f, 0.2f, 0.2f );

	glVertex3fv( g_TestPoint.xyzw );

	glEnd();



	//=================================== Draw 2D layer ==============================================//

	glClear( GL_DEPTH_BUFFER_BIT);

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D( 0, g_ScreenWidth, g_ScreenHeight, 0 );
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();


	glBegin( GL_POINTS );

	glPointSize( 2.5f );
	glColor3f( 0.2f, 1.0f, 0.2f );

	for( auto& cp : g_ControlPoints )
	{
		glVertex3f( cp.Origin().x,  cp.Origin().y, 0.0f );
		//DrawRadialControlPoint2D( cp );
	}

	glEnd();


	glPopMatrix();
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );


	glutSwapBuffers();
}


// キーを押し込んだのを検出する
void KeyboardCallback(unsigned char key, int w, int h)
{
	gKeys[key] = true;
}


// キーを離したのを検出する
void KeyboardUpCallback(unsigned char key, int x, int y)
{
	gKeys[key] = false;

	switch (key)
	{
	case 27: { exit(0);	break; }
	case 'p': { g_bDrawVertexBuffer ^= 1; break; }
	default : { break; }
	}
}



void reshape( int w, int h )
{
	g_ScreenWidth = w;
	g_ScreenHeight = h;

	glViewport( 0, 0, g_ScreenWidth, g_ScreenHeight );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 45, float(g_ScreenWidth) / float(g_ScreenHeight), 0.001, 1000 );

glGetFloatv( GL_PROJECTION_MATRIX, g_MatProj.m );//  列優先の行列要素並び順になってる. 

	glMatrixMode(GL_MODELVIEW);


}



void MouseCallback( int button, int state, int x, int y )
{
	
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)// 左ボタンを押し込んだ状態
	{
		LeftMouseButtonPressed = true;
		g_CursorX = x;
		g_CursorY = y;

		//if( g_InteractionMode == INTERACTION_MODE_ADD_CONTROLLER )
		{

// https://stackoverflow.com/questions/7692988/opengl-math-projecting-screen-space-to-world-space-coords

// g_MatProjもg_MatViewも列優先になってる -> オレオレライブラリは全て行優先 -> 行列かける順番をひっくり返す & 終わったら合成行列を転置する
Multiply( g_MatViewProj, g_MatView, g_MatProj );
MatInverse( g_MatInvViewProj, g_MatViewProj );

//tcout << g_MatInvViewProj << tendl;
auto mattmp = g_MatInvViewProj;
MatTranspose( g_MatInvViewProj, mattmp );

//tcout << g_MatInvViewProj << tendl;

float d = 1.0f;
Vec4f tmp(
	2.0f * float(g_CursorX) / float(g_ScreenWidth) - 1.0f,
	1.0f -( 2.0f * float(g_CursorY) / float(g_ScreenHeight) ),
	0.25f,//2.0f * d - 1.0f,//1.0f,
	1.0f );


Multiply( g_TestPoint, g_MatInvViewProj, tmp );
tcout << tmp << tendl;

g_TestPoint.w = 1.0f / g_TestPoint.w;
g_TestPoint.x *= g_TestPoint.w;
g_TestPoint.y *= g_TestPoint.w;
g_TestPoint.z *= g_TestPoint.w;

//InitZero( g_TestPoint );
tcout << g_TestPoint << tendl;

//g_ControlPoints.AddToTail( RadialControlPoint2D<float>( g_CursorX, g_CursorY, g_CursorX, g_CursorY, 1.0f ) );
		}
	}
	if (state == GLUT_UP)
	{
		LeftMouseButtonPressed = false;

	}

	//cout << "Mouse LB state = " << LeftMouseButtonPressed << endl;
}



void MotionCallback( int x, int y )
{
	g_CursorDX	= x - g_CursorX;
	g_CursorDY	= y - g_CursorY;

	if(LeftMouseButtonPressed ==true)// 左ボタンをクリックしたときだけ回転
	{
		float dx = -float(g_CursorDX) / float(g_ScreenWidth) * 2.0f*M_PI;
		float dy = -float(g_CursorDY) / float(g_ScreenHeight) * 2.0f*M_PI;
		//cout << "dv = (" << dx << ", " << dy << ")" << endl;
		g_Camera.Rotate( dx, dy );
	}

    g_CursorX = x;
    g_CursorY = y;
}


void animate(void)
{
	glutPostRedisplay();
}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize( g_ScreenWidth, g_ScreenHeight );
	glutCreateWindow(argv[0]);
		
	glutDisplayFunc( display );
	glutReshapeFunc( reshape );
	glutIdleFunc( animate );

	glutKeyboardFunc( KeyboardCallback );
	glutKeyboardUpFunc( KeyboardUpCallback );
	glutMouseFunc( MouseCallback );
	glutMotionFunc( MotionCallback );
	MotionCallback(0,0);

	Initialize();
	glutMainLoop();

	return 0;
}
