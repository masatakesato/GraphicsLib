#include	<GL/freeglut.h>// or glut.h

#include	<oreore/common/TString.h>
#include	<oreore/mathlib/GraphicsMath.h>
#include	<oreore/container/Array.h>

#include	<graphics/gl4x/scene/Camera.h>
#include	<graphics/gl4x/scene/OBJLoader.h>
using namespace OreOreLib;



// Lightsource
const GLfloat lightPos[] = { 10 , 10 , 10 , 0 };
const GLfloat lightCol[] = { 1 , 1 , 1 , 1 };


// Camera
Camera	g_Camera;


// Obj Mesh 
OBJLoader g_OBJLoader;

/*
// Vertex buffer ( for glsl rendering pipeline )
int g_NumVertices = 0;
int g_NumIndices = 0;
VertexLayout* g_Vertices = NULL;// vertices
int *g_Indices = 0;// triangle indices
*/

Array<VertexLayout> g_Vertices;
Array<uint32>	g_Indices;



#define MAX_KEYS 256
bool	gKeys[ MAX_KEYS ];// pushed key flags
int		mx = 0;// cursor screen position x
int		my = 0;// cursor screen position y
bool	LeftMouseButtonPressed = false;
float	g_CamSpeed = 0.1f;
bool	g_bDrawVertexBuffer = false;





void DrawObjMesh( OBJLoader& mesh )
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

			if( VertAttr_Iter.x < 0 || VertAttr_Iter.x > vertices.Length()-1 )
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



void ProcessCameraKeys()
{
	int	i;

	for(i=0; i<MAX_KEYS; i++)
	{
		if(gKeys[i]==false)	continue;
		
		switch(i)
		{
			case 'w' :{	g_Camera.Transrate(+g_CamSpeed, 0, 0);	break; }
			case 's' :{	g_Camera.Transrate(-g_CamSpeed, 0, 0);	break; }
			case 'a' :{	g_Camera.Transrate(0, +g_CamSpeed, 0);	break; }
			case 'd' :{	g_Camera.Transrate(0, -g_CamSpeed, 0);	break; }
			case 'r' :{	g_Camera.Transrate(0, 0, +g_CamSpeed);	break; }
			case 'f' :{	g_Camera.Transrate(0, 0, -g_CamSpeed);	break; }
			case 'q' :{	g_Camera.Roll(-g_CamSpeed);				break; }
			case 'e' :{	g_Camera.Roll(+g_CamSpeed);				break; }
			default : {	break; }
		}// end of switch
		
	}// end of i loop

}



void Initialize()
{
	glClearColor(0,0,0,1);
	glPointSize(10.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	
	g_Camera.SetViewParameter( {5, 5, 5}, {-1, -1, -1}, {0, 0, 1} );// set position, direction, vertical vector
	//cam = new Camera(5,5,5, -1,-1,-1, 0,0,1);



	TCHAR currdir[ MAX_PATH ];
	GetCurrentDirectory( MAX_PATH, currdir );
	SetCurrentDirectory( _T( "../../../assets/scene/obj" ) );

	g_OBJLoader.Load( "viking_room.obj" );

	SetCurrentDirectory( currdir );

	g_OBJLoader.Information();
	g_OBJLoader.GetGroupInfo(1);

	// Extract polygon vertices/indices from MeshObj.
	//g_OBJLoader.GenVertexList( g_NumVertices, &g_Vertices, g_NumIndices, &g_Indices );
	g_OBJLoader.GenVertexList( g_Vertices, g_Indices );
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

	gluLookAt( pos.x, pos.y, pos.z, pos.x + dir.x, pos.y + dir.y, pos.z + dir.z, up.x, up.y, up.z );
	

	//============================== Draw world space axis ================================//
	//DRAW_XYZ();
	


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
		DrawObjMesh( g_OBJLoader );
	}

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



void reshape(int w, int h)
{
	if(w >=h) {glViewport(0, 0, (int)(h*4/3), h);
	printf("w =%d\n", (int)(h*4/3) );
	printf("h =%d\n", h);}
	else glViewport(0, 0, (int)(w*4/3), w);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1.25, 0.001, 10000);
	glMatrixMode(GL_MODELVIEW);
}


// マウス状態の監視
void MouseCallback(int button, int state, int x, int y)
{
	mx = x;
	my = y;
	
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)// 左ボタンを押し込んだ状態
	{
		LeftMouseButtonPressed = true;
		//cout << "Mouse Position = (" << x << ", " << y << ")" << endl;
	}
	if (state == GLUT_UP)
	{
		LeftMouseButtonPressed = false;
	}

	//cout << "Mouse LB state = " << LeftMouseButtonPressed << endl;
}


// マウス動作の監視 
void MotionCallback(int x, int y)
{
	float dx = float(mx - x) * 2.0f*M_PI / 800;
	float dy = float(my - y) * 2.0f*M_PI / 600;
	//cout << "dv = (" << dx << ", " << dy << ")" << endl;

	if(LeftMouseButtonPressed ==true)// 左ボタンをクリックしたときだけ回転
	{
		g_Camera.Rotate(dx, dy);
	}

    mx = x;
    my = y;
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
	glutInitWindowSize(800, 600);
	glutCreateWindow(argv[0]);
		
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(animate);

	glutKeyboardFunc(KeyboardCallback);
	glutKeyboardUpFunc(KeyboardUpCallback);
	glutMouseFunc(MouseCallback);
	glutMotionFunc(MotionCallback);
	MotionCallback(0,0);

	Initialize();
	glutMainLoop();

	return 0;
}
