#include	"ModelGL.h"

#include	<oreore/Bitmap.h>
#include	<oreore/GLTexture.h>

#include	"LightPropagationVolumes.h"





ModelGL::ModelGL()
{
std::cout << "ModelGL::ModelGL()..." << std::endl;

	m_Camera = new Camera();
	m_LightCam = new Camera();
	m_LpvTex	= NULL;
}


ModelGL::~ModelGL()
{
std::cout << "ModelGL:~:ModelGL()..." << std::endl;
if(m_Camera){	delete	m_Camera; m_Camera = NULL; }
if(m_LightCam){	delete	m_LightCam; m_LightCam = NULL; }

m_LPV.UnbindLPV();

if(m_LpvTex){ delete m_LpvTex;	m_LpvTex = NULL; }
}


void ModelGL::Init()
{

}



void ModelGL::LoadShader(const char *vsFile, const char *gsFile, const char *fsFile)
{
	Vec3i lpv_size = {32, 32, 32};	// Light Propagation Volumesの解像度
	Vec2i rsm_size = {2048, 2048};	// Reflective Shadow Mapの解像度
	Vec2i vpl_size = {256, 256};	// Virtual Point Lightの配置グリッドの解像度

	cout << "ModelGL::LoadShader..." << endl;
	// LPVテクスチャを初期化する
	m_LpvTex	= new LPVTexture();
	m_LpvTex->Init(1, lpv_size/*, rsm_size, vpl_size*/);
	//
	m_LPV.BindLPV(m_LpvTex);
	m_LPV.Init(1, lpv_size, rsm_size, vpl_size);
}


void ModelGL::InitCamera(const Vec3f &pos, const Vec3f &dir, const Vec3f &up, float aspect_ratio, float fovy, float znear, float zfar)
{
	m_Camera->SetViewParameter(pos, dir, up);
	m_Camera->SetProjectionParameter(aspect_ratio, fovy, znear, zfar);
	m_Camera->Update();


	// 光源カメラの初期化. TODO: シーングラフ情報として光源を登録する
	Vec3f	posl = {5,5,5},	dirl = {-1,-1,-1};
	m_LightCam->SetViewParameter(posl, dirl, up);
	m_LightCam->SetProjectionParameter(1.0f, fovy, /*znear, zfar*/1.0, 1000.0);
	m_LightCam->Update();
}


void ModelGL::SetCameraView(const Vec3f &pos, const Vec3f &dir, const Vec3f &up)
{
	m_Camera->SetViewParameter(pos, dir, up);
	m_Camera->Update();
}


void ModelGL::SetCameraProjection(float aspect_ratio, float fovy, float znear, float zfar)
{
	m_Camera->SetProjectionParameter(aspect_ratio, fovy, znear, zfar);
	m_Camera->Update();
}


void ModelGL::Draw()
{	
	// TODO: 光源カメラを注視点周りに回転させるパラメータ
	static float phi  = 0.25f * 3.141592f;

	Vec3f pos = {cos(phi)*7.07106781, sin(phi)*7.07106781, 5.0};
	Vec3f prp = {0,0,0};
	Vec3f dir;
	Subtract(dir, prp, pos);
	Normalize(dir);
	Vec3f up = {0, 0, 1};

	m_LightCam->SetViewParameter(pos, dir, up);
	m_LightCam->Update();

	static short counter = 50;
//	glClearColor(0.0, 0.0, 0.0, 0.0);
//	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	// 一定フレーム毎にLPVテクスチャを更新する	
	//m_LPV.Draw(*m_LightCam);
	//phi += 0.002;
	
	if(counter>=5)
	{
		m_LPV.Draw(*m_LightCam);
		counter=0;
		phi += 0.001;
	}
	counter++;
	
//	m_LPV.ViewSH(*m_Camera);
// TODO: 画面の解像度取得方法を考えて作り直す
	glClearColor(0.2, 0.2, 0.2, 0.0);
	glViewport(0, 0, 1280, 720);
	m_LPV.DrawSurface(*m_Camera, *m_LightCam);
	//m_LPV.ViewSH(*m_Camera);

	
	
}