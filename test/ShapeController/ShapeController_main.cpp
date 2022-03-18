﻿#include	<GL/freeglut.h>// or glut.h

#include	<oreore/common/TString.h>
#include	<oreore/mathlib/GraphicsMath.h>
#include	<oreore/container/Array.h>

#include	<graphics/gl4x/scene/MeshLoader.h>
#include	<graphics/gl4x/scene/Camera.h>
#include	<graphics/authoring/controller/ControlPoint.h>
#include	"ControlPointExt.h"

using namespace GraphicsLib;
using namespace OreOreLib;


//================ Scene ==================//
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



//================== Edit =================//

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

float	g_ObjectPickRadius = 10.0f;
float	g_RadiusTranslateScale = 2.5f;




// Modes
enum INTERACTION_MODE
{
	INTERACTION_MODE_VIEW,

	INTERACTION_MODE_EDIT_ORIGIN,

	//INTERACTION_MODE_CONROLLER_PLACEMENT,

	INTERACTION_MODE_EDIT_TARGET,
	NUM_INTERACTION_MODES,
};

const char* g_InteractionModeStrings[] =
{
	"View",
	"Controller placement",
	"Controller edit",
};

uint8 g_InteractionMode = INTERACTION_MODE::INTERACTION_MODE_VIEW;


// Color settings
const Vec3f g_UIColors[] =
{
	// None
	{},

	// Origin
	{ 0.1f, 0.1f, 0.1f },// Origin_Locked
	{ 0.6f, 0.3f, 0.3f },// Origin_Editable
	{ 1.0f, 0.5f, 0.5f },// Origin_Selected

	// Target
	{ 0.6f, 0.6f, 0.6f },// Target_Locked
	{ 0.3f, 0.3f, 0.6f },// Target_Editable
	{ 0.5f, 0.5f, 1.0f },// Target_Selected

	// Radius
	{ 0.2f, 0.2f, 0.2f },// Radius_Locked
	{ 0.6f, 0.6f, 0.6f },// Radius_Editable
	{ 0.6f, 0.6f, 0.6f },// Radius_Selected

	// Displacement
	{ 0.3f, 0.3f, 0.3f },// Radius_Locked


	// ViewFrame
	{ 0.5f, 0.3f, 0.3f },

};


uint8	g_SelectionMode = 0;
int64	g_SelectedItemID = -1;


// Control Points
using RadialControlPoint2Df = RadialControlPoint2D<float>;
Array<RadialControlPoint2D<float>>	g_ControlPoints;




//=============== Debug primitive ==============//
Vec4f	g_TestPoint;
Mat4f	g_MatView, g_MatProj, g_MatViewProj, g_MatInvViewProj;







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



void DrawString( float x, float y, void *font, const char* str, const Vec3f& rgb )
{  
//  char *c;

  glColor3f(rgb.r, rgb.g, rgb.b); 
  glRasterPos2f(x, y);

  glutBitmapString(font, (const unsigned char*)str );
}



void DrawCircle( float cx, float cy, float r, int num_segments )
{
    glBegin(GL_LINE_LOOP);
    for( int i=0; i<num_segments; ++i ) 
	{
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);//get the current angle 
        float x = r * cosf(theta);//calculate the x component 
        float y = r * sinf(theta);//calculate the y component 
        glVertex2f(x + cx, y + cy);//output vertex 
    }
    glEnd();
}



template < typename T >
void DrawRadialControlPoint2D( const RadialControlPoint2D<T>& cp, bool selected=false )
{
	uint8 origin_color_id = selected ? 3 :1;
	uint8 target_color_id = selected ? 6 :4;
	uint8 radius_color_id = selected ? 9 :7;

	//glPointSize( 10.0f );

	// Draw Origin and Target
	glBegin( GL_POINTS );
		glPointSize( 10.0f );
		glColor3fv( g_UIColors[ origin_color_id ].rgb );
		glVertex2fv( cp.Origin().xy );

		glPointSize( 7.5f );
		glColor3fv( g_UIColors[ target_color_id ].rgb );
		glVertex2fv( cp.Target().xy );
	glEnd();

	// Draw Displacement
	glBegin( GL_LINES );
		glColor4f( 0.7f, 0.7f, 0.7f, 0.5f );
		glVertex2fv( cp.Origin().xy );
		glVertex2fv( cp.Target().xy );
	glEnd();


	// Draw Radius
	glColor3fv( g_UIColors[ radius_color_id ].rgb );
	DrawCircle( cp.Origin().x, cp.Origin().y, cp.Radius(), 32 );



}


void DrawFixedFrame( int w, int h )
{
	glLineWidth( 15.0f );
	glColor3fv( g_UIColors[ 11 ].rgb );

	glBegin( GL_LINE_LOOP );
		glVertex2f( 0, 0 );
		glVertex2f( w, 0 );
		glVertex2f( w, h );
		glVertex2f( 0, h );
	glEnd();

	// restore line width settings
	glLineWidth( 1.0f );

}




bool GetIntersectedControlPoint( float x, float y, float range, int64& objectid, uint8& type )
{
	Vec2f pos( x, y );

	for( int i=0; i<g_ControlPoints.Length(); ++i )
	{
		// Check origin intersection
		if( Distance( pos, g_ControlPoints[i].Origin() ) <= range )
		{
			objectid = i;
			type = RadialControlPoint2Df::ORIGIN;
			return true;
		}

		// Check target intersection
		else if( Distance( pos, g_ControlPoints[i].Target() ) <= range )
		{
			objectid = i;
			type = RadialControlPoint2Df::TARGET;
			return true;
		}

	}

	objectid = -1;
	return false;
}




void TranslateControlPointOrigin( int64 objectid, float dx, float dy )
{
	if( objectid < 0 || objectid >=g_ControlPoints.Length<int64>() )	return;
	g_ControlPoints[ objectid ].TranslateOrigin( dx, dy );
}



void TranslateControlPointTarget( int64 objectid, float dx, float dy )
{
	if( objectid < 0 || objectid >=g_ControlPoints.Length<int64>() )	return;
	g_ControlPoints[ objectid ].TranslateTarget( dx, dy );
}



void TranslateControlPointRadius( int64 objectid, float radius )
{
	if( objectid < 0 || objectid >=g_ControlPoints.Length<int64>() )	return;
	g_ControlPoints[ objectid ].TranslateRadius( radius );
}



void DeleteSelectedController( int64& objectid )
{
	if( objectid < 0 || objectid >=g_ControlPoints.Length<int64>() )	return;

	g_ControlPoints.Remove( objectid );

	objectid = -1;
}



void ProcessMainMenu( int option )
{
	tcout << "ProcessMainMenu: " << option << tendl;

}


void ProcessSubMenu1( int option )
{
	tcout << "ProcessSubMenu1: " << option << tendl;


}




void ProcessCameraKeys()
{
	if( g_InteractionMode != INTERACTION_MODE::INTERACTION_MODE_VIEW )	return;

	if( gKeys[ int('w') ] ){	g_Camera.Transrate(+g_CamSpeed, 0, 0);	}
	if( gKeys[ int('s') ] ){	g_Camera.Transrate(-g_CamSpeed, 0, 0);	}
	if( gKeys[ int('a') ] ){	g_Camera.Transrate(0, +g_CamSpeed, 0);	}
	if( gKeys[ int('d') ] ){	g_Camera.Transrate(0, -g_CamSpeed, 0);	}
	if( gKeys[ int('r') ] ){	g_Camera.Transrate(0, 0, +g_CamSpeed);	}
	if( gKeys[ int('f') ] ){	g_Camera.Transrate(0, 0, -g_CamSpeed);	}
	if( gKeys[ int('q') ] ){	g_Camera.Roll(-g_CamSpeed);				}
	if( gKeys[ int('e') ] ){	g_Camera.Roll(+g_CamSpeed);				}
}




void Initialize()
{
	glClearColor( 0.3f, 0.3f, 0.3f, 1.0f );
	glPointSize(10.0);
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_POINT_SMOOTH );
	

	// Popup menu setup
	int submenu = glutCreateMenu( ProcessSubMenu1 );
	glutAddMenuEntry( "test0", 0 );
	glutAddMenuEntry( "test1", 1 );
	glutAddMenuEntry( "test2", 2 );


	int mainmenu = glutCreateMenu( ProcessMainMenu );
	glutAddSubMenu( "Other", submenu );
	glutAddMenuEntry( "Red",1 );
	glutAddMenuEntry( "Blue",2 );
	glutAddMenuEntry( "Green",3 );
	glutAddMenuEntry( "Orange",4 );


	glutAttachMenu( GLUT_RIGHT_BUTTON );


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
//	ProcessEditOperations();


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

//############################# スクリーン座標からワールド座標に戻した点の表示 #######################################//
glGetFloatv( GL_MODELVIEW_MATRIX, g_MatView.m );// 列優先の行列要素並び順になってる. 
//tcout << g_MatView << tendl;
//#####################################################################################################################//


	//============================== Draw world space axis ================================//
	DRAW_XYZ();
	

	//==================================== Draw OBJLoader ==================================//
	if( g_bDrawVertexBuffer )
	{
		// Drawing polygon mesh using Veriex/Indices.
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		glColor3f( 0.6f, 0.6f, 0.6f );

		glEnableClientState(GL_VERTEX_ARRAY);
		//glEnableClientState(GL_TEXTURE_COORD_ARRAY);// texture is not supported.

		//glTexCoordPointer(2, GL_FLOAT, 0, TexCoordArray);// texture is not supported.
		glVertexPointer( 3, GL_FLOAT, sizeof(VertexLayout), g_Vertices.begin() );//glVertexPointer( 3, GL_FLOAT, sizeof(VertexLayout), &(g_Vertices->Position) );

		glDrawElements( GL_TRIANGLES, g_Indices.Length(), GL_UNSIGNED_INT, g_Indices.begin() );//glDrawElements( GL_TRIANGLES, g_NumIndices, GL_UNSIGNED_INT, g_Indices );

		glDisableClientState(GL_VERTEX_ARRAY);
		//glDisableClientState(GL_TEXTURE_COORD_ARRAY);// texture is not supported.
	}
	else
	{
		// Put default light
		glLightfv(GL_LIGHT0 , GL_POSITION , lightPos);
		glLightfv(GL_LIGHT0 , GL_DIFFUSE , lightCol);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		// Visualize OBJLoader. Using old OpenGL fixed pipeline APIs.
		DrawObjMesh( /*g_OBJLoader*/g_MeshLoader );

		glDisable( GL_LIGHTING );
		glDisable( GL_LIGHT0 );
	}


//############################# スクリーン座標からワールド座標に戻した点の表示 #######################################//
	//if( g_InteractionMode > INTERACTION_MODE_VIEW )
	{
		glPointSize( 10.0f );
		glColor3f( 1.0f, 0.2f, 0.2f );
		glBegin( GL_POINTS );

		glVertex3fv( g_TestPoint.xyzw );

		glEnd();
	}
//#####################################################################################################################//



	//=================================== Draw 2D layer ==============================================//

	glClear( GL_DEPTH_BUFFER_BIT);

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D( 0, g_ScreenWidth, g_ScreenHeight, 0 );
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();


	// Controllers
	glPointSize( 7.5f );
	glBegin( GL_POINTS );

	glColor3f( 0.2f, 1.0f, 0.2f );

	for( int i=0; i<g_ControlPoints.Length(); ++i )
	{
		//glVertex3f( g_ControlPoints[i].Origin().x, g_ControlPoints[i].Origin().y, 0.0f );
		DrawRadialControlPoint2D( g_ControlPoints[i], i==g_SelectedItemID );
	}

	glEnd();


	// Draw FixedFrame if interaction mode is placement/edit mode.
	if( g_InteractionMode > INTERACTION_MODE_VIEW )
	{
		DrawFixedFrame( g_ScreenWidth, g_ScreenHeight );
		DrawString( float(g_ScreenWidth-100), float(g_ScreenHeight-10), GLUT_BITMAP_HELVETICA_18, "View Fixed.", {0.5f, 0.3f, 0.3f} );
	}


	// Caption
	std::string str = std::string() + "Mode: " + g_InteractionModeStrings[ g_InteractionMode ];
	DrawString( 10.0f, 20.0f, GLUT_BITMAP_HELVETICA_18,  str.c_str(), {0.8f, 0.8f, 0.8f} );
	


	glPopMatrix();
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );


	glutSwapBuffers();
}



// キーを押し込んだのを検出する
void KeyboardCallback( unsigned char key, int w, int h )
{
	gKeys[key] = true;
}



// キーを離したのを検出する
void KeyboardUpCallback( unsigned char key, int x, int y )
{
	gKeys[key] = false;

	switch (key)
	{
	case 27:// escape( exit application )
	{
		exit(0);
		break;
	}
	case 'p':// change render mode
	{
		g_bDrawVertexBuffer ^= 1;
		break;
	}
	case 0x09:// tab( change edit mode )
	{
		g_InteractionMode = ( g_InteractionMode + 1 ) % 2;// switch between
		break;
	}
	case 127:// delete( delete selected controller )
	{
		if( g_InteractionMode > INTERACTION_MODE_VIEW )
			DeleteSelectedController( g_SelectedItemID );
		break;
	}

	default :
	{
		break;
	}
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

//################################# スクリーンピクセル座標からワールド座標に戻すコード ################################//

glGetFloatv( GL_PROJECTION_MATRIX, g_MatProj.m );//  列優先の行列要素並び順になってる.

//#####################################################################################################################//

	glMatrixMode(GL_MODELVIEW);


}



void MouseCallback( int button, int state, int x, int y )
{
	
	if( button==GLUT_LEFT_BUTTON && state==GLUT_DOWN )// 左ボタンを押し込んだ状態
	{
		LeftMouseButtonPressed = true;
		g_CursorX = x;
		g_CursorY = y;


		if( g_InteractionMode == INTERACTION_MODE_EDIT_ORIGIN )
		{
			// Check if controller already exists
			uint8 attrib = -1;
			int64 objectid = -1;
			GetIntersectedControlPoint( g_CursorX, g_CursorY, g_ObjectPickRadius, g_SelectedItemID, attrib );

			if( g_SelectedItemID == -1 && (glutGetModifiers() & GLUT_ACTIVE_ALT)  )// If nothing is picked, put origin at current position, then transfer to target edit mode
			{
				g_ControlPoints.AddToTail( RadialControlPoint2Df( g_CursorX, g_CursorY, g_CursorX, g_CursorY, 20.0f ) );
				g_SelectedItemID = g_ControlPoints.Length<int64>() - 1;
				g_InteractionMode = INTERACTION_MODE_EDIT_TARGET;
			}
			else if( attrib == RadialControlPoint2Df::TARGET )//targetid != -1 )// If Target is picked, transfer to target edit mode
			{
				//g_SelectedItemID = objectid;
				g_InteractionMode = INTERACTION_MODE_EDIT_TARGET;
			}
			else if( attrib == RadialControlPoint2Df::ORIGIN )// If Origin is picked, transfer to origin edit mode
			{
				//g_SelectedItemID = objectid;
				g_InteractionMode = INTERACTION_MODE_EDIT_ORIGIN;
			}
			
		}


	}

	else if( state == GLUT_UP )
	{
		if( g_InteractionMode == INTERACTION_MODE_EDIT_TARGET )
			g_InteractionMode = INTERACTION_MODE_EDIT_ORIGIN;

		LeftMouseButtonPressed = false;
	}

	//cout << "Mouse LB state = " << LeftMouseButtonPressed << endl;
}



void MotionCallback( int x, int y )
{
	g_CursorDX	= x - g_CursorX;
	g_CursorDY	= y - g_CursorY;

	if( LeftMouseButtonPressed ==true )
	{
		float dx = -float(g_CursorDX) / float(g_ScreenWidth) * 2.0f*M_PI;
		float dy = -float(g_CursorDY) / float(g_ScreenHeight) * 2.0f*M_PI;
		//cout << "dv = (" << dx << ", " << dy << ")" << endl;

		if( g_InteractionMode == INTERACTION_MODE_VIEW )// Mouse drag with view mode
		{
			g_Camera.Rotate( dx, dy );
		}
		else if( g_InteractionMode == INTERACTION_MODE_EDIT_ORIGIN )
		{
			//tcout << "Delta = (" << g_CursorDX << ", " << g_CursorDY << ")" << tendl;
			TranslateControlPointOrigin( g_SelectedItemID, float(g_CursorDX), float(g_CursorDY) );
		}
		else if( g_InteractionMode == INTERACTION_MODE_EDIT_TARGET )
		{
			TranslateControlPointTarget( g_SelectedItemID, float(g_CursorDX), float(g_CursorDY) );
		}


//#################################### スクリーンピクセル座標からワールド座標に戻すコード ###################################//


		if( g_InteractionMode > INTERACTION_MODE_VIEW )
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
				0.999f,//2.0f * d - 1.0f,//1.0f,
				1.0f );


			Multiply( g_TestPoint, g_MatInvViewProj, tmp );
			tcout << tmp << tendl;

			g_TestPoint.w = 1.0f / g_TestPoint.w;
			g_TestPoint.x *= g_TestPoint.w;
			g_TestPoint.y *= g_TestPoint.w;
			g_TestPoint.z *= g_TestPoint.w;

			tcout << g_TestPoint << tendl;
		}
//#############################################################################################################################//




	}

    g_CursorX = x;
    g_CursorY = y;
}



void WheelCallback( int button, int dir, int x, int y )
{
	if( g_InteractionMode >= INTERACTION_MODE_EDIT_ORIGIN )
	{
		//tcout << dir<< tendl;
		TranslateControlPointRadius( g_SelectedItemID, float(dir) * g_RadiusTranslateScale );
	}


	return;
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
	glutMotionFunc( MotionCallback );//	MotionCallback(0,0);
	glutMouseWheelFunc( WheelCallback );

	Initialize();
	glutMainLoop();

	return 0;
}
