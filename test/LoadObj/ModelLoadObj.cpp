#include	"ModelLoadObj.h"

#include	<oreore/images/Bitmap.h>

#include	<graphics/gl4x/resource/GLTexture.h>
#include	<graphics/gl4x/resource/BufferLayout.h>

using namespace OreOreLib;




int numVertAttrs = 0;
VertexLayout *vertexlist = NULL;
int numIndices = 0;
int *Indices = 0;



//--------------- 光線ベクトル ------------//
static float	LightDir[] = {1, 1, 1};




// 頂点配列を使って接空間ベクトル付き立方体を描画する
static void DrawCube_with_tangent(	float dim,
									GLint vertLoc,
									GLint normLoc,
									GLint texcLoc,
									GLint tangLoc,						
									GLint bnormLoc,
									Vec3f pos
									)
{
	VertexLayout_Ext	vertices[] =
	{
		{ {-dim+pos.x, -dim+pos.y, -dim+pos.z}, {0,0,0}, {0,0,-1}, {1,0,0}, {0,1,0} },
		{ {+dim+pos.x, -dim+pos.y, -dim+pos.z}, {1,0,0}, {0,0,-1}, {1,0,0}, {0,1,0} },
		{ {+dim+pos.x, +dim+pos.y, -dim+pos.z}, {1,1,0}, {0,0,-1}, {1,0,0}, {0,1,0} },
		{ {-dim+pos.x, +dim+pos.y, -dim+pos.z}, {0,1,0}, {0,0,-1}, {1,0,0}, {0,1,0} },
		
		{ {+dim+pos.x, -dim+pos.y, -dim+pos.z}, {0,0,0}, {1,0,0}, {0,0,1}, {0,1,0} },
		{ {+dim+pos.x, -dim+pos.y, +dim+pos.z}, {1,0,0}, {1,0,0}, {0,0,1}, {0,1,0} },
		{ {+dim+pos.x, +dim+pos.y, +dim+pos.z}, {1,1,0}, {1,0,0}, {0,0,1}, {0,1,0} },
		{ {+dim+pos.x, +dim+pos.y, -dim+pos.z}, {0,1,0}, {1,0,0}, {0,0,1}, {0,1,0} },
		
		{ {+dim+pos.x, -dim+pos.y, +dim+pos.z}, {0,0,0}, {0,0,1}, {-1,0,0}, {0,1,0} },
		{ {-dim+pos.x, -dim+pos.y, +dim+pos.z}, {1,0,0}, {0,0,1}, {-1,0,0}, {0,1,0} },
		{ {-dim+pos.x, +dim+pos.y, +dim+pos.z}, {1,1,0}, {0,0,1}, {-1,0,0}, {0,1,0} },
		{ {+dim+pos.x, +dim+pos.y, +dim+pos.z}, {0,1,0}, {0,0,1}, {-1,0,0}, {0,1,0} },
	
		{ {-dim+pos.x, -dim+pos.y, +dim+pos.z}, {0,0,0}, {-1,0,0}, {0,0,-1}, {0,1,0} },
		{ {-dim+pos.x, -dim+pos.y, -dim+pos.z}, {1,0,0}, {-1,0,0}, {0,0,-1}, {0,1,0} },
		{ {-dim+pos.x, +dim+pos.y, -dim+pos.z}, {1,1,0}, {-1,0,0}, {0,0,-1}, {0,1,0} },
		{ {-dim+pos.x, +dim+pos.y, +dim+pos.z}, {0,1,0}, {-1,0,0}, {0,0,-1}, {0,1,0} },

		{ {-dim+pos.x, +dim+pos.y, -dim+pos.z}, {0,0,0}, {0,1,0}, {1,0,0}, {0,0,1} },
		{ {+dim+pos.x, +dim+pos.y, -dim+pos.z}, {1,0,0}, {0,1,0}, {1,0,0}, {0,0,1} },
		{ {+dim+pos.x, +dim+pos.y, +dim+pos.z}, {1,1,0}, {0,1,0}, {1,0,0}, {0,0,1} },
		{ {-dim+pos.x, +dim+pos.y, +dim+pos.z}, {0,1,0}, {0,1,0}, {1,0,0}, {0,0,1} },

		{ {-dim+pos.x, -dim+pos.y, -dim+pos.z}, {0,0,0}, {0,-1,0}, {0,0,1}, {1,0,0} },
		{ {-dim+pos.x, -dim+pos.y, +dim+pos.z}, {1,0,0}, {0,-1,0}, {0,0,1}, {1,0,0} },
		{ {+dim+pos.x, -dim+pos.y, +dim+pos.z}, {1,1,0}, {0,-1,0}, {0,0,1}, {1,0,0} },
		{ {+dim+pos.x, -dim+pos.y, -dim+pos.z}, {0,1,0}, {0,-1,0}, {0,0,1}, {1,0,0} },
	};

	GLint	index[] =
	{
		0,1,2, 3,0,2,
		4,5,6, 7,4,6,
		8,9,10, 11,8,10,
		12,13,14, 15,12,14,
		16,17,18, 19,16,18,
		20,21,22, 23,20,22,
	};


	glEnableVertexAttribArray(vertLoc);
	glEnableVertexAttribArray(normLoc);
	glEnableVertexAttribArray(texcLoc);
//	glEnableVertexAttribArray(tangLoc);
//	glEnableVertexAttribArray(bnormLoc);
	

	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLayout_Ext), &(vertices->Position) );
	glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLayout_Ext), &(vertices->Normal) );
	glVertexAttribPointer(texcLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexLayout_Ext), &(vertices->TexCoord) );
//	glVertexAttribPointer(tangLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLayout_Ext), &(vertices->Tangent) );
//	glVertexAttribPointer(bnormLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLayout_Ext), &(vertices->Binormal) );

	
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, index);

	glDisableVertexAttribArray(vertLoc);
	glDisableVertexAttribArray(normLoc);
	glDisableVertexAttribArray(texcLoc);
//	glDisableVertexAttribArray(tangLoc);
//	glDisableVertexAttribArray(bnormLoc);

}




ModelLoadObj::ModelLoadObj()
{
std::cout << "ModelLoadObj::ModelLoadObj()..." << std::endl;

	m_Camera = new Camera();
	m_Shader = new GLShader();
}


ModelLoadObj::~ModelLoadObj()
{
std::cout << "ModelLoadObj:~:ModelLoadObj()..." << std::endl;
	delete	m_Shader;
	delete	m_Camera;
/*
	glDeleteTextures(1, &m_HeightTexture);
	glDeleteTextures(1, &m_NormalTexture);
	glDeleteTextures(1, &m_DiffuseTexture);
	*/
}


void ModelLoadObj::Init()
{
	TCHAR currdir[ MAX_PATH ];
	GetCurrentDirectory( MAX_PATH, currdir );
	SetCurrentDirectory( _T( "../../../assets/scene/obj" ) );

	m_mesh.Load(//"a building.obj"
				//"30x36_bc_m.obj"
				//"mjc00.obj"
				//"sir00obj.obj"
				//"SponzaMoreMeshes.obj"
				//"test.obj"
				//"dosei.obj"
				//"Sponza.obj"
				//"new_csie_b1.obj"
				//"mba1.obj"
				//"mba2.obj"
				//"chinese.obj"
				//"ateneam.obj"
				//"elepham.obj"
				//"kiy00g.obj"
				"testscene.obj"
				);

	SetCurrentDirectory( currdir );

	m_mesh.Information();

	m_mesh.GetGroupInfo(1);


	m_mesh.GenVertexList(numVertAttrs, &vertexlist, numIndices, &Indices);
//	m_mesh.GenerateMeshObject();
}



void ModelLoadObj::LoadShader(const char *vsFile, const char *gsFile, const char *fsFile)
{
	//m_Shader->Init(vsFile, fsFile);
	m_Shader->Init( _T("../../../test/LoadObj/Shader/Shader.glsl"), GLSL_430 );

	vertLoc = 0;
	normLoc = 1;
	texcLoc = 2;
	//m_Tangent = 3;
	//m_Binormal = 4;
	glBindAttribLocation(m_Shader->ID(), vertLoc, "POSITION");
	glBindAttribLocation(m_Shader->ID(), normLoc, "NORMAL");
	glBindAttribLocation(m_Shader->ID(), texcLoc, "TEXCOORD0");
	glBindAttribLocation(m_Shader->ID(), m_Tangent, "TANGENT");
	glBindAttribLocation(m_Shader->ID(), m_Binormal, "BINORMAL");

	mvpLoc			= glGetUniformLocation(m_Shader->ID(), "g_matMVP");
	g_epLightDir	= glGetUniformLocation(m_Shader->ID(), "g_LightDir");
	g_epEyePos		= glGetUniformLocation(m_Shader->ID(), "g_EyePos");

	glLinkProgram(m_Shader->ID());
}


void ModelLoadObj::InitCamera(const Vec3f &pos, const Vec3f &dir, const Vec3f &up, float aspect_ratio, float fovy, float znear, float zfar)
{
	m_Camera->SetViewParameter(pos, dir, up);
	m_Camera->SetProjectionParameter(aspect_ratio, fovy, znear, zfar);
	m_Camera->Update();
}


void ModelLoadObj::SetCameraView(const Vec3f &pos, const Vec3f &dir, const Vec3f &up)
{
	m_Camera->SetViewParameter(pos, dir, up);
	m_Camera->Update();
}


void ModelLoadObj::SetCameraProjection(float aspect_ratio, float fovy, float znear, float zfar)
{
	m_Camera->SetProjectionParameter(aspect_ratio, fovy, znear, zfar);
	m_Camera->Update();
}


void ModelLoadObj::Draw()
{
	m_Shader->Bind();
	
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, m_Camera->GetViewProjectionMatrix()->m );	
	glUniform3fv(g_epLightDir, 1, LightDir);
	glUniform3fv(g_epEyePos, 1, m_Camera->GetPosition()->xyz);
/*	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_NormalTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_HeightTexture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_DiffuseTexture);
*/
	
	//for(int i=0; i<10; ++i)
	//	for(int j=0; j<10; ++j)
	//		for(int k=0; k<10; ++k)
	//		{
	//			Vec3f Pos = {float(i), float(j), float(k)};
	//DrawCube_with_tangent(0.4f, vertLoc, normLoc, texcLoc, m_Tangent, m_Binormal, Pos); 
	//		}



	
	glEnableVertexAttribArray(vertLoc);
	glEnableVertexAttribArray(texcLoc);
	glEnableVertexAttribArray(normLoc);
	
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLayout), &(vertexlist->Position) );
	glVertexAttribPointer(texcLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLayout), &(vertexlist->TexCoord) );
	glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLayout), &(vertexlist->Normal) );
	
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, Indices);
	
	glDisableVertexAttribArray(vertLoc);
	glDisableVertexAttribArray(texcLoc);
	glDisableVertexAttribArray(normLoc);


	m_Shader->Unbind();

}