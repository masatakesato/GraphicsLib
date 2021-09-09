#include	"PlanetRenderingEngine.h"

#include	<math.h>

#include	<graphics/gl4x/common/GLHelperFunctions.h>
#include	<graphics/gl4x/resource/Texture.h>
using namespace OreOreLib;



//================== フレームレート計測 =========================//
float	g_FrameRate		= 0.0f;			// フレームレート
float	g_DeltaT		= 1.0f / 60.0f;	// 1フレームあたりの描画時間(初期値)


//========================== カメラ =============================//
float	g_CamRotSpeed = 5.0f;	// カメラの回転速度
float	g_CamMovSpeed = 5.0f;	// カメラの移動速度


//======================== 地表マテリアル =======================//
char*	g_MatName[]			= { "Grass",				"Forest",					"Snow" };				// マテリアル名称
int		g_numMaterials			= sizeof(g_MatName) / sizeof(g_MatName[0]);										// マテリアル数
Vec3f	g_Ambient[]				= { {0.0f, 0.0f, 0.0f},		{0.0f, 0.0f, 0.0f},			{0.0f, 0.0f, 0.0f} };	// 環境光(RGB)
Vec3f	g_Diffuse[]				= { {0.117f, 0.136f, 0.0f},	{0.001f, 0.02f, 0.008f},	{0.78f, 0.78f, 0.8f} };	// 拡散反射率(RGB)
Vec3f	g_Specular[]			= { {0.0f, 0.0f, 0.0f},		{0.0f, 0.0f, 0.0f},			{0.01f, 0.01f, 0.01f} };// 鏡面反射率(RGB)
float	g_SpecularIntensity[]	= { 0.0f,					0.0f,						0.0f };					// 鏡面反射の強さ
float	g_SpecularPower[]		= {	0.0f,					0.0f,						0.2f };					// 光沢度合い
Vec3f	g_Distribution[]		= { {1.7f, 2.0f, 0.4f},	{1.8f, 1.4f, 0.57f},		{7.0f, 3.5f, 0.07f} };	// マテリアル分布パラメータ（高度,　幅, 減衰領域）
float	g_Slope[]				= { 1.5f,					35.0f,						25.0f };				// 勾配パラメータ


//============================= 太陽 ============================//
Vec3f	g_LightIntensity = {100.0f, 100.0f, 100.0f};// 明るさ
Vec3f	g_LightDirection = {1.0f, 1.0f, 1.0f};		// 向き


//================= GUIを利用した制御パラメータ =================//
// GUI表示の切り替えフラグ
bool		g_ShowGui			= false;// GUIから入力した状態
bool		g_CurrGui			= false;// 現在の状態



// 大気散乱効果のOnOff
bool		g_ShowAtmosphere	= true;

// ワイヤーフレーム表示切り替え
bool		g_ShowWireFrame		= false;
GLenum		g_PolygonMode		= GL_FILL;

// G-Bufferの表示切替え
bool		g_ShowGbuffer		= false;

// VirtualTexture
bool		g_VirtualTexture	= false;

// ビューフラスタム固定On/Offの切り替え
bool		g_FixFrustum		= false;


// HDRレンダリングの露光感度調整
float		g_Exposure			= 0.36f;

// HDRレンダリングのブラー効果適用度合い
float		g_BloomWeight		= 1.0f;



// 地形テッセレーションの再分割レベル調整
float		g_numSubdivision	= 13;

// 地形テッセレーションの感度調整
float		g_Rho				= 1.3f;



// ハイトマップのスケール
const float	g_HeightRange		= 10.5f;		// 最大高度(固定値)
float		g_HorizontalScale	= 6.2f;	// 凹凸スケールの大きさ
float		g_VerticalScale		= 3.0f;		// 起伏の変化度合い

// ハイトマップの解像度選択
bool		g_showListBox_vt	= false;
char		*g_TexSizes[]		= {" 32 x 32 ", " 64 x 64 ", " 128 x 128 ", " 256 x 256 ", " 512 x 512 ", " 1024 x 1024 "};	// 解像度
int			g_numTexSizes		= sizeof(g_TexSizes) / sizeof(g_TexSizes[0]);	// 選択できる解像度の種類数
int			g_CurrentTexSize	= 3;											// 現在使用している解像度(g_TexSizes[]の要素インデックス)
int			g_SelectedTexSize	= g_CurrentTexSize;								// GUIから新たに選択した解像度（初期値はg_CurrentTexSize）

// ハイトマップ更新ボタン
bool		g_UpdateHeightmap = false;// クリック判定フラグ



// 地表マテリアルの選択
bool		g_showListBox_mat	= false;
int			g_CurrentMatID		= 0;

// 分布高度
float		g_CurrentHeight;
float		g_SelectedHeight;

// 分布幅
float		g_CurrentHeightRange;
float		g_SelectedHeightRange;

// マテリアル境界付近の分布量
float		g_CurrentBoundary;
float		g_SelectedBoundary;

// スロープ
float		g_CurrentSlope;
float		g_SelectedSlope;




PlanetRenderingEngine::PlanetRenderingEngine(int width, int height)
{
	m_Device_Main	= NULL;
	m_Device_Sub	= NULL;
	m_GUI			= NULL;

	m_Camera			= NULL;
	m_VirtualTexture	= NULL;

	m_TerrainEngine		= NULL;
	m_AtmosphericShader	= NULL;
	m_HDRShader			= NULL;
	m_Gbuffer			= NULL;
//	m_FramebufferObject	= NULL;
	
	m_ScreenWidth		= width;
	m_ScreenHeight		= height;

	m_LeftMouseButtonPressed	= false;
	m_RightMouseButtonPressed	= false;
	m_MouseX					= 0;
	m_MouseY					= 0;
	dx							= 0;
	dy							= 0;

	for(int i=0; i<MAX_KEYS; i++)	m_Keys[i] = false;

	m_hEvent		= CreateEvent(NULL, TRUE, TRUE, _T("ThreadExecution") );	// シグナル状態のイベントを生成する
	m_hMutex		= CreateMutex(NULL, FALSE, _T("ThreadPause") );			// 非シグナル状態のセマフォを作る
	m_hThread		= NULL;// スレッド
	m_ThreadId		= NULL;// スレッドＩＤ
	m_ThreadPool	= new ThreadPool(1);

	framecounter	= 0;

	Normalize(g_LightDirection);
}

// デストラクタ
PlanetRenderingEngine::~PlanetRenderingEngine()
{
	if(m_ThreadPool){		delete	m_ThreadPool; m_ThreadPool = NULL; }
	if(m_hThread)			CloseHandle( m_hThread );
	if(m_hEvent)			CloseHandle( m_hEvent );


	//if(m_GUI){ delete m_GUI; m_GUI = NULL; }
	if(m_Camera){ delete m_Camera; m_Camera = NULL; }
	//if(m_VirtualTexture){ delete m_VirtualTexture; m_VirtualTexture = NULL; }
	
	//if(m_TerrainEngine){ delete m_TerrainEngine; m_TerrainEngine = NULL; }
	//if(m_Gbuffer){ delete m_Gbuffer; m_Gbuffer = NULL; }
	//if(m_AtmosphericShader){ delete m_AtmosphericShader; m_AtmosphericShader = NULL; }
	//if(m_HDRShader){ delete m_HDRShader; m_HDRShader = NULL; }

	//if(m_FramebufferObject){ delete m_FramebufferObject; m_FramebufferObject = NULL; }
	
	//if(m_FinalImage){ glDeleteTextures(1, &m_FinalImage); m_FinalImage = NULL; }


	if(m_Device_Sub)
	{
		m_Device_Sub->Begin();
		{
			if(m_VirtualTexture){ delete m_VirtualTexture; m_VirtualTexture = NULL; }
			if(m_TerrainEngine){ delete m_TerrainEngine; m_TerrainEngine = NULL; }
		}
		m_Device_Sub->End();
		
		m_Device_Sub->CloseContext( m_hWnd );
		delete m_Device_Sub;
		m_Device_Sub = NULL;
	}



	if(m_Device_Main)
	{
		m_Device_Main->Begin();
		{
			if(m_GUI){ delete m_GUI; m_GUI = NULL; }
			if(m_Gbuffer){ delete m_Gbuffer; m_Gbuffer = NULL; }
			m_RenderTarget.Release();//if(m_FramebufferObject){ delete m_FramebufferObject; m_FramebufferObject = NULL; }
			if(m_FinalImage){ glDeleteTextures(1, &m_FinalImage); m_FinalImage = NULL; }
			if(m_HDRShader){ delete m_HDRShader; m_HDRShader = NULL; }
			if(m_AtmosphericShader){ delete m_AtmosphericShader; m_AtmosphericShader = NULL; }
		}
		m_Device_Main->End();

		m_Device_Main->CloseContext( m_hWnd );
		delete	m_Device_Main;
		m_Device_Main = NULL;
	}
}



void PlanetRenderingEngine::Create()
{

tcout << _T("PlanetRenderingEngine::Create()...") << tendl;

	Vec3f pos = {0,0,0};

	//=============== グラフィックスデバイスの生成 ===============//
	m_Device_Sub = new GraphicsDevice();
	m_Device_Sub->CreateContext(m_hWnd, 24, 24);
	
	m_Device_Main = new GraphicsDevice();
	m_Device_Main->CreateContext(m_hWnd, 24, 24);
	
	// グラフィックスデバイスの共有
	m_Device_Main->ShareRenderingContext( m_Device_Sub->GetRenderingContext() );
	
	//================= GUIインスタンス生成 =============//
	m_GUI = new MyGUI();
	m_GUI->SetScreenResolution(m_ScreenWidth, m_ScreenHeight);
	m_Device_Main->Begin();
	m_GUI->InitFontDisplayList(GetDC(m_hWnd));//ビットマップフォントの初期化
	m_Device_Main->End();
	

	//======================= カメラインスタンス作成 =========================//
	m_Camera		= new Camera(-3800.0f,3800.0f,3800.0f, 1.0f,-1.0f,-1.0f, 0.0f,0.0f,1.0f);
	m_Camera->SetProjectionParameter((float)m_ScreenWidth/(float)m_ScreenHeight, M_PI/4.0f, 1.0e-4f, 100000.0f);
	m_Camera->Update();
	
	int mapsize = (int)pow(2.0f, 5+g_CurrentTexSize);// ハイトマップの解像度を取得する
	
	//====================== VirtualTextureインスタンス生成 ======================//
	m_VirtualTexture = new VirtualTexture();
	m_VirtualTexture->AttachGraphicsDevice(m_Device_Sub);
	m_VirtualTexture->InitTileCache(16, 16, mapsize);// 256枚（16x16）のハイトマップを格納できるタイルキャッシュを確保する
	m_VirtualTexture->AttachThreadPool( m_ThreadPool );

	//================== 地形レンダリングエンジン生成 ==========================//
	m_TerrainEngine = new TerrainEngine();
	m_TerrainEngine->AttachGraphicsDevice(m_Device_Sub);
	m_TerrainEngine->Init(pos, 6360.0f, g_HeightRange, g_numSubdivision, g_Rho);
	m_TerrainEngine->InitHeightmapGenerator(mapsize, 2.5f, g_HeightRange, g_HorizontalScale, g_VerticalScale);// ハイトマップ生成オブジェクトを初期化する
	m_TerrainEngine->InitMeshObject(65, 3);
	m_TerrainEngine->InitSurfaceProperties(	g_numMaterials, (const char **)&(*g_MatName),
											g_Ambient, g_Diffuse, g_Specular,
											g_SpecularIntensity, g_SpecularPower,
											g_Distribution, g_Slope);

	m_TerrainEngine->AttachVirtualTexture( m_VirtualTexture );
	
	
	m_Device_Main->Begin();
	{		
		//======================== G-Bufferの初期化 =============================//
		m_Gbuffer			= new G_Buffer(m_ScreenWidth, m_ScreenHeight);
		m_FinalImage		= AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_ScreenWidth, m_ScreenHeight, GL_RGBA16F, GL_RGBA, GL_FLOAT);

		//==================フレームバッファオブジェクト初期化 ==================//
		const GLuint texids[] =	{ m_Gbuffer->Position(), m_Gbuffer->Normal(), m_Gbuffer->Diffuse(), m_Gbuffer->Specular() };
//m_Attachment.Init( ArraySize( texids ), g_DefaultColorAttachments, texids );

//m_FramebufferObject = new GLFrameBufferObject();
//m_FramebufferObject->Init( m_ScreenWidth, m_ScreenHeight, true );

m_RenderTarget.Init( m_ScreenWidth, m_ScreenHeight, true ); 
m_RenderTarget.BindTextures( ArraySize( texids ), g_DefaultColorAttachments, texids );

		//m_FramebufferObject->AllocateBuffer(4);
		//m_FramebufferObject->AttachRenderBuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, m_ScreenWidth, m_ScreenHeight);
		
		// G-Buffer割り当て
//m_FramebufferObject->BindAttachment( &m_Attachment );
		//m_FramebufferObject->AttachTexture(0, GL_TEXTURE_2D, m_Gbuffer->Position(),	GL_COLOR_ATTACHMENT0);
		//m_FramebufferObject->AttachTexture(1, GL_TEXTURE_2D, m_Gbuffer->Normal(),	GL_COLOR_ATTACHMENT1);
		//m_FramebufferObject->AttachTexture(2, GL_TEXTURE_2D, m_Gbuffer->Diffuse(),	GL_COLOR_ATTACHMENT2);
		//m_FramebufferObject->AttachTexture(3, GL_TEXTURE_2D, m_Gbuffer->Specular(),	GL_COLOR_ATTACHMENT3);
		
		//===================== 大気散乱シェーダ作成 ============================//
		m_AtmosphericShader = new AtmosphericShader(m_ScreenWidth, m_ScreenHeight);
		m_AtmosphericShader->AttachGbuffer(m_Gbuffer);
		m_HDRShader			= new HDRShader(m_ScreenWidth, m_ScreenHeight);


		//========================== OpenGLステートの初期化 ==========================//
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_DEPTH_TEST);
			
		//============================= ビューポート設定 ============================//
		glViewport(0, 0, m_ScreenWidth, m_ScreenHeight);
	}
	m_Device_Main->End();
	

	//======== スレッド作成 ========//
	if(!m_hThread)
	m_hThread = (HANDLE)_beginthreadex(0, 0, ThreadExecute, this, 0, &m_ThreadId);	// スレッドを作成する
	
}


// スレッド開始
void PlanetRenderingEngine::Start()
{
	if(!m_hThread)
		m_hThread = (HANDLE)_beginthreadex(0, 0, ThreadExecute, this, 0, &m_ThreadId);// スレッドを作成する
	else
		ResetEvent(m_hEvent);//イベントを 非シグナル状態にする
}


// スレッド中断
void PlanetRenderingEngine::Stop()
{
	if(m_hThread)
	{
		SetEvent(m_hEvent);// メインスレッド停止信号を出す（イベントをシグナル状態にする）
		WaitForSingleObject(m_hThread, INFINITE);// メインスレッドの停止を待つ
	}
}


// スレッド呼び出し
unsigned _stdcall PlanetRenderingEngine::ThreadExecute(void *Param)
{
	((PlanetRenderingEngine *)Param)->RunThread();
	return 0;
}

unsigned int bufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

// スレッド呼び出し関数の本体
void PlanetRenderingEngine::RunThread()
{
	DWORD Time_origin, Time_next;

	int numframe = 0;
	Time_origin = timeGetTime(); // CPU時間をミリ秒単位で取得する
	Time_next = Time_origin;


	//=============================== 描画ループ ===============================//
	while(WaitForSingleObject(m_hEvent,0) != WAIT_OBJECT_0)// イベント0が非シグナル状態の間描画し続ける
	{
		ProcessCameraKeys();

		WaitForSingleObject(m_hMutex, INFINITE);// ミューテックスでスレッドを一時中断する

		m_Device_Main->Begin();
		{

			GLenum errCode;

			if((errCode=glGetError()) != GL_NO_ERROR)
			{
				tcout << gluErrorString(errCode) << tendl;
				//return;
			}


			//===================== シーンをレンダリングする ======================//
			// フレームバッファ上にシーンをレンダリングする
			m_RenderTarget.Bind();
//m_FramebufferObject->Bind();
			{
				// G-bufferへの地形情報の書き出し(GL_COLOR_ATTACHMENT0～GL_COLOR_ATTACHMENT3)
				glDrawBuffers(4, bufs);
				m_TerrainEngine->DrawGbuffer(*m_Camera, g_PolygonMode);

				// G-Bufferを使ったレンダリング（ライティングおよび大気散乱効果の付与）
				m_AtmosphericShader->Draw(m_Camera, g_LightDirection, g_LightIntensity, m_FinalImage, g_ShowAtmosphere);
			}
//m_FramebufferObject->Unbind();
			m_RenderTarget.Unbind();


			//=================== レンダリング結果を画面に表示する =================//
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// GUIの表示
			DoGUI();

			// G-Bufferの表示
			if(g_ShowGbuffer==true)
			{
				int width = m_ScreenWidth;///3;
				int height = m_ScreenHeight;///3;

				m_GUI->Begin();
				m_GUI->DoText(10,			10,				"Diffuse");	// bottom left 
				m_GUI->DoText(10+width/2,	10,				"Specular");// bottom right
				m_GUI->DoText(10+width/2,	10+height/2,	"Normal");	// top right
				m_GUI->DoText(10,			10+height/2,	"Position (WorldSpace)");// top left
				m_GUI->End();

				m_Gbuffer->Draw( width, height );
			}
			else
			{
				if( g_VirtualTexture==true )
				{
					//m_Gbuffer->DrawTexture_RGB(m_FinalImage, g_Exposure);
					m_Gbuffer->DrawTexture_RGB( m_VirtualTexture->GetTileCache_normal(), 384, 384, 1.0f );//g_Exposure );
					//m_Gbuffer->ShowHeightMap(  m_ScreenWidth, m_ScreenHeight, 0.15f);
				}

			// レンダリングしたシーンの表示（HDRレンダリング）
			m_HDRShader->DoHDRLighting(m_FinalImage, g_DeltaT, g_Exposure, g_BloomWeight, m_ScreenWidth, m_ScreenHeight );
			}


			m_Device_Main->swapBuffers();

		}
		m_Device_Main->End();

		ReleaseMutex(m_hMutex);

		//=================== フレーム数をカウントする ========================//
		Time_next = timeGetTime();
		numframe++;

		if(Time_next - Time_origin >= 100)// 100[ms]毎に実施
		{
			g_FrameRate = (float)numframe / (Time_next - Time_origin) * 1000.0f;
			Time_origin = Time_next;
			numframe = 0;
			g_DeltaT = 0.5f * (g_DeltaT + (1.0f / g_FrameRate));
		}

		//============== VirtualTextureの未使用データを削除する ===============//
		if(framecounter == 50)// 50フレーム毎に実施
		{
			m_VirtualTexture->TileCache_PageOut();
			framecounter = 0;
		}

		framecounter++;

	}
}



void PlanetRenderingEngine::ProcessCameraKeys()
{
	int	i;

	for(i=0; i<MAX_KEYS; i++)
	{
		if(m_Keys[i]==false) continue;

		switch(i)
		{
			case 'W' :{	m_Camera->Transrate( ( exp(g_CamMovSpeed) - 1.0f ) * g_DeltaT, 0, 0 );	break; }
			case 'S' :{	m_Camera->Transrate( -( exp(g_CamMovSpeed) - 1.0f ) * g_DeltaT, 0, 0 );	break; }
			case 'A' :{	m_Camera->Transrate( 0, +( exp(g_CamMovSpeed) - 1.0f ) * g_DeltaT, 0 );	break; }
			case 'D' :{	m_Camera->Transrate( 0, -( exp(g_CamMovSpeed) - 1.0f ) * g_DeltaT, 0 );	break; }
			case 'R' :{	m_Camera->Transrate( 0, 0, +( exp(g_CamMovSpeed) - 1.0f ) * g_DeltaT );	break; }
			case 'F' :{	m_Camera->Transrate( 0, 0, -( exp(g_CamMovSpeed) - 1.0f ) * g_DeltaT );	break; }
			case 'Q' :{	m_Camera->Roll(-g_CamRotSpeed * g_DeltaT);			break; }
			case 'E' :{	m_Camera->Roll(+g_CamRotSpeed * g_DeltaT);			break; }
			default : {	break; }
		}// end of switch
		
	}// end of i loop

}


void PlanetRenderingEngine::DoGUI()
{
	m_GUI->Begin();
	
	auto uiPosy = m_ScreenHeight;
	//===================== GUI表示のOn/Off ====================//
	m_GUI->DoButton(m_ScreenWidth-42, uiPosy-=22, 40, 20, " GUI", g_ShowGui);
	g_CurrGui ^= g_ShowGui;
	if(g_CurrGui==false)	goto end_gui;


	//===================== レンダリングの設定 =================//
	// 大気散乱効果のOn/Off
	m_GUI->DoCheckBox(m_ScreenWidth-150, uiPosy-=MyGUI::CheckBoxHeight, "Show Atmosphere", g_ShowAtmosphere);

	//ワイヤフレーム表示On/Off
	m_GUI->DoCheckBox(m_ScreenWidth-150, uiPosy-=MyGUI::CheckBoxHeight, "Show WireFrame", g_ShowWireFrame);
	g_PolygonMode = g_ShowWireFrame==true ? GL_LINE : GL_FILL;

	// G-Buffer表示On/Off
	m_GUI->DoCheckBox(m_ScreenWidth-150, uiPosy-=MyGUI::CheckBoxHeight, "Show G-Buffer", g_ShowGbuffer);

// Virtual Texture表示On/Off
m_GUI->DoCheckBox(m_ScreenWidth-150, uiPosy-=MyGUI::CheckBoxHeight, "Show VirtualTexture", g_VirtualTexture );

	// ビューフラスタム固定On/Off
	m_GUI->DoCheckBox(m_ScreenWidth-150, uiPosy-=MyGUI::CheckBoxHeight, "Fix ViewFrustum", g_FixFrustum);
	m_TerrainEngine->FixFrustum(g_FixFrustum);
	
	uiPosy -= MyGUI::VerticalSpacing;


	//===================== HDRシェーダの設定 =================//
	// HDRレンダリングの明るさ調整
	m_GUI->DoHorizontalSlider(m_ScreenWidth-150, uiPosy-=MyGUI::HorizontalSliderHeight, 100, "Exposure:", 0.0f, 1.0f, g_Exposure);
	
	// HDRレンダリングのブラー適用度合い調整
	m_GUI->DoHorizontalSlider(m_ScreenWidth-150, uiPosy-=MyGUI::HorizontalSliderHeight, 100, "BloomWeight:", 0.0f, 2.0f, g_BloomWeight);


	//===================== Tessellation ===================//
	// 地面LODの分割レベル数調整
	m_GUI->DoHorizontalSlider(m_ScreenWidth-150, uiPosy-=MyGUI::HorizontalSliderHeight, 100, "LOD Lv:", 0.0f, 14.0f, g_numSubdivision, 14);
	m_TerrainEngine->SetMaximumLevel((int)g_numSubdivision);

	// 地面LOD分割の感度調整
	m_GUI->DoHorizontalSlider(m_ScreenWidth-150, uiPosy-=MyGUI::HorizontalSliderHeight, 100, "LOD Sensitivity:", 0.0, 10.0, g_Rho, 100);
	m_TerrainEngine->SetRho(g_Rho);

	uiPosy -= MyGUI::VerticalSpacing;


	//===================== Heightmap settings =========================//
	// horizontal/vertical heghtmap scale sliders
	m_GUI->DoHorizontalSlider(m_ScreenWidth-150, uiPosy-=MyGUI::HorizontalSliderHeight, 100, "HorizontalScale:", 0.0, 10.0, g_HorizontalScale, 100);// 凹凸の大きさ
	m_GUI->DoHorizontalSlider(m_ScreenWidth-150, uiPosy-=MyGUI::HorizontalSliderHeight, 100, "VerticalScale:", 0.0, 10.0, g_VerticalScale, 100);// 凹凸の高さ
	

	// update heightmap button
	m_GUI->DoButton(m_ScreenWidth-120, uiPosy-=MyGUI::HorizontalSliderHeight, 85, 20, "UpdateHeight", g_UpdateHeightmap);
	if(g_UpdateHeightmap)
	{
		m_VirtualTexture->TileCache_Clear();
		m_TerrainEngine->SetHeightmapScale(g_HorizontalScale, g_VerticalScale);
	}
	
	m_GUI->DoComboBox(m_ScreenWidth-270, uiPosy, 80, g_numTexSizes, g_TexSizes, g_SelectedTexSize, g_showListBox_vt, "HeightmapSize:");

	if(g_SelectedTexSize != g_CurrentTexSize)
	{
		g_CurrentTexSize = g_SelectedTexSize;
		int tex_size = (int)pow(2.0f, 5+g_CurrentTexSize);
		ChangeVirtualTextureParam(tex_size);
	}

	uiPosy -= MyGUI::VerticalSpacing;


	//=================== マテリアル関連の設定 =================//
	// マテリアル選択コンボボックス
	m_GUI->DoComboBox(m_ScreenWidth-270, uiPosy-=MyGUI::HorizontalSliderHeight, 80, g_numMaterials, g_MatName, g_CurrentMatID, g_showListBox_mat, "Materials:");

	g_CurrentHeight = g_Distribution[g_CurrentMatID].x;
	g_SelectedHeight = g_CurrentHeight;

	g_CurrentHeightRange = g_Distribution[g_CurrentMatID].y;
	g_SelectedHeightRange = g_CurrentHeightRange;

	g_CurrentBoundary = g_Distribution[g_CurrentMatID].z;
	g_SelectedBoundary = g_CurrentBoundary;

	g_CurrentSlope	= g_Slope[g_CurrentMatID];
	g_SelectedSlope = g_CurrentSlope;
		
	// 分布高度下限調整スライダ
	m_GUI->DoHorizontalSlider(m_ScreenWidth-150, uiPosy, 100, "Height:", 0.0f, 10.0f, g_SelectedHeight, 100);
	if(g_SelectedHeight != g_CurrentHeight)
	{
		g_Distribution[g_CurrentMatID].x = g_SelectedHeight;
		m_TerrainEngine->SetDistribution(g_CurrentMatID, g_Distribution[g_CurrentMatID]);// 分布高度を変更する
	}

	// 分布高度の上限調整スライダ
	m_GUI->DoHorizontalSlider(m_ScreenWidth-150, uiPosy-=MyGUI::HorizontalSliderHeight, 100, "Range:", 0.0f, 10.0f, g_SelectedHeightRange, 100);
	if(g_SelectedHeightRange != g_CurrentHeightRange)
	{
		g_Distribution[g_CurrentMatID].y = g_SelectedHeightRange;
		m_TerrainEngine->SetDistribution(g_CurrentMatID, g_Distribution[g_CurrentMatID]);// 分布高度を変更する
	}

	// 高度境界付近のマテリアル適用度
	m_GUI->DoHorizontalSlider(m_ScreenWidth-150, uiPosy-=MyGUI::HorizontalSliderHeight, 100, "BounbdarySmooth:", 0.0f, 1.0f, g_SelectedBoundary, 100);
	if(g_SelectedBoundary != g_CurrentBoundary)
	{
		g_Distribution[g_CurrentMatID].z = g_SelectedBoundary;
		m_TerrainEngine->SetDistribution(g_CurrentMatID, g_Distribution[g_CurrentMatID]);// 分布高度を変更する
	}

	// 勾配調整スライダ
	m_GUI->DoHorizontalSlider(m_ScreenWidth-150, uiPosy-=MyGUI::HorizontalSliderHeight, 100, "Slope:", 0.0f, 50.0f, g_SelectedSlope, 100);
	if(g_SelectedSlope != g_CurrentSlope)
	{
		g_Slope[g_CurrentMatID] = g_SelectedSlope;
		m_TerrainEngine->SetSlope(g_CurrentMatID, g_Slope[g_CurrentMatID]);
	}

	uiPosy -= MyGUI::VerticalSpacing;


	//=================== カメラ移動速度の設定 =================//
	m_GUI->DoHorizontalSlider(m_ScreenWidth-150, uiPosy-=MyGUI::HorizontalSliderHeight, 100, "CameraSpeed:", 0.0f, 10.0f, g_CamMovSpeed, 10000);

end_gui:
	
	//==================== 文字列表示 ===================//
	char str[256];
	Vec3f	camPos = *m_Camera->GetPosition();
	// フレーム数
	sprintf_s(str, 16, "FPS: %.2f", g_FrameRate);
	m_GUI->DoText(20, m_ScreenHeight-20, str );
	// カメラ座標
	sprintf_s(str, 64, "CameraPosition[km]: %.2f, %.2f, %.2f", camPos.x, camPos.y, camPos.z );
	m_GUI->DoText(20, m_ScreenHeight-40, str);
	// カメラ高度
	sprintf_s(str, 64, "Altitude[km]: %.2f", (Length(camPos) - 6360.0f) );
	m_GUI->DoText(20, m_ScreenHeight-60, str);
	
	m_GUI->End();
}



unsigned PlanetRenderingEngine::keyDown(WPARAM wParam)
{
	m_Keys[int(wParam)] = true;
	return 0;
}


unsigned PlanetRenderingEngine::keyUp(WPARAM wParam)
{
	m_Keys[int(wParam)] = false;
	return 0;
}


unsigned PlanetRenderingEngine::lButtonDown(LPARAM lParam)
{
	SetCapture(m_hWnd);
	
	if(m_GUI->IsActive()) return 1;

	m_LeftMouseButtonPressed = true;
	m_MouseX = LOWORD(lParam);
	m_MouseY = HIWORD(lParam);

	m_GUI->lButtonDown(m_MouseX, m_MouseY);

	return 0;
}


unsigned PlanetRenderingEngine::lButtonUp(LPARAM lParam)
{
	ReleaseCapture();
	
	m_LeftMouseButtonPressed = false;
	m_MouseX = LOWORD(lParam);
	m_MouseY = HIWORD(lParam);

	m_GUI->lButtonUp(m_MouseX, m_MouseY);

	return 0;
}


unsigned PlanetRenderingEngine::rButtonDown(LPARAM lParam)
{
	SetCapture(m_hWnd);
	m_RightMouseButtonPressed = true;
	m_MouseX = LOWORD(lParam);
	m_MouseY = HIWORD(lParam);
	
	return 0;
}


unsigned PlanetRenderingEngine::rButtonUp(LPARAM lParam)
{
	ReleaseCapture();
	m_RightMouseButtonPressed = false;
	m_MouseX = LOWORD(lParam);
	m_MouseY = HIWORD(lParam);

	return 0;
}


unsigned PlanetRenderingEngine::mouseMove(LPARAM lParam)
{
	WaitForSingleObject(m_hMutex, INFINITE);// ミューテックスでスレッドを一時中断する

	int x, y;

	x = LOWORD(lParam) > m_ScreenWidth ? m_MouseX : LOWORD(lParam);
	y = HIWORD(lParam) > m_ScreenHeight ? m_MouseY : HIWORD(lParam);

	m_GUI->mouseMove(x,y);

	if(m_GUI->IsActive())
	{
		ReleaseMutex(m_hMutex);
		return 1;
	}
	
	if(m_LeftMouseButtonPressed==true)
	{
		dx = float(m_MouseX - x) / 100.0f;
		dy = float(m_MouseY - y) / 100.0f;

		m_Camera->Rotate(dx, dy);

		m_MouseX = x;
		m_MouseY = y;	
	}

	//############################# 試験実装．光源をマウスドラッグで移動させる ###########################//
	else if(m_RightMouseButtonPressed==true)
	{
		dx = float(m_MouseX - x) / 1000.0f;
		dy = float(m_MouseY - y) / 1000.0f;

		Vec3f	vert_scaled, hol_scaled;
		Vec3f	rot_axis;
		float	angle;

		if(dx != 0.0f || dy != 0.0f)
		{
			//=============== 回転軸と角度を決める ===============//
			hol_scaled = *m_Camera->GetHorizontal();
			Scale(hol_scaled, dy);// 画面横方向の動きでカメラ垂直軸の回転を制御する
			vert_scaled = *m_Camera->GetVertical();
			Scale(vert_scaled, dx);// 画面たて方向の動きでカメラ水平軸の回転を制御する

			Subtract(rot_axis, vert_scaled, hol_scaled);
			angle = Length(rot_axis);	
			Scale(rot_axis, 1.0f/angle);

			//================== 軸周りに回転 ====================//
			Quatf quat;
			InitQuat( quat, angle, rot_axis );
			Rotate( g_LightDirection, quat );
			//RotateVector(angle, rot_axis.x, rot_axis.y, rot_axis.z, g_LightDirection.x, g_LightDirection.y, g_LightDirection.z );
		}

		m_MouseX = x;
		m_MouseY = y;

	}

//tcout << _T("Mouse Cursor = (") << m_MouseX << _T(", ") << m_MouseY << _T(")") << tendl;
	
	ReleaseMutex(m_hMutex);

	return 0;
}

// スクリーンサイズを変更する
void PlanetRenderingEngine::SetScreenSize(int width, int height)
{

	if(m_ScreenWidth==width && m_ScreenHeight==height) return;

	m_ScreenWidth = width;
	m_ScreenHeight = height;
	m_GUI->SetScreenResolution(width, height);
	
	WaitForSingleObject(m_hMutex, INFINITE);

	m_Device_Main->Begin();
	{
		//============= テクスチャ/Renderbufferを一旦削除する ==============//
//m_Attachment.Clear();
//m_FramebufferObject->Release();


		//============= テクスチャ/Renderbufferを再度生成する ============//
		m_Gbuffer->SetSize( m_ScreenWidth, m_ScreenHeight);
		ReallocateTexture2D(m_FinalImage, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_ScreenWidth, m_ScreenHeight, GL_RGBA16F, GL_RGBA, GL_FLOAT);
		
		//============================= FBOに割り当てる ===================//
		const GLuint texids[] =	{ m_Gbuffer->Position(), m_Gbuffer->Normal(), m_Gbuffer->Diffuse(), m_Gbuffer->Specular() };
//m_Attachment.Init( ArraySize( texids ), texids, g_DefaultColorAttachments );
m_RenderTarget.Init( m_ScreenWidth, m_ScreenHeight, true );
m_RenderTarget.BindTextures( ArraySize( texids ), g_DefaultColorAttachments, texids );

//m_FramebufferObject->Init( m_ScreenWidth, m_ScreenHeight );
//m_FramebufferObject->BindAttachment( &m_Attachment );
		//m_FramebufferObject->AllocateBuffer(4);
		//m_FramebufferObject->AttachRenderBuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, m_ScreenWidth, m_ScreenHeight);
		//m_FramebufferObject->AttachTexture(0, GL_TEXTURE_2D, m_Gbuffer->Position(),	GL_COLOR_ATTACHMENT0);
		//m_FramebufferObject->AttachTexture(1, GL_TEXTURE_2D, m_Gbuffer->Normal(),	GL_COLOR_ATTACHMENT1);
		//m_FramebufferObject->AttachTexture(2, GL_TEXTURE_2D, m_Gbuffer->Diffuse(),	GL_COLOR_ATTACHMENT2);
		//m_FramebufferObject->AttachTexture(3, GL_TEXTURE_2D, m_Gbuffer->Specular(),	GL_COLOR_ATTACHMENT3);
		



		//===================== 大気散乱シェーダ/ブルームシェーダにテクスチャ解像度変更を反映する
		m_AtmosphericShader->SetScreenSize(m_ScreenWidth, m_ScreenHeight);
		m_AtmosphericShader->AttachGbuffer(m_Gbuffer);
		m_HDRShader->SetScreenSize(m_ScreenWidth, m_ScreenHeight);
		
		//==================== ビューポートを再設定する ==================//
		glViewport(0, 0, m_ScreenWidth, m_ScreenHeight);
		m_Camera->SetProjectionParameter((float)m_ScreenWidth/(float)m_ScreenHeight, M_PI/4.0f, 0.01f, 100000.0f);
		m_Camera->Update();
	}
	m_Device_Main->End();
	
	ReleaseMutex(m_hMutex);
}


// VirtualTextureの解像度を変更する
void PlanetRenderingEngine::ChangeVirtualTextureParam(int tex_size)
{
	WaitForSingleObject(m_hMutex, INFINITE);

tcout << _T("PlanetRenderingEngine::ChangeVirtualTextureParam()...") << tendl;

	m_VirtualTexture->TileCache_Clear();
	m_VirtualTexture->InitTileCache(16,16,tex_size);

	m_TerrainEngine->UpdateTextureParameter();
	
	ReleaseMutex(m_hMutex);
}