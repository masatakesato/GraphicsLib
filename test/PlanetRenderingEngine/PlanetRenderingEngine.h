#ifndef PLANET_RENDERING_ENGINE_H
#define PLANET_RENDERING_ENGINE_H


#include    <windows.h>
#include	<process.h>
#include	<iostream>
using namespace std;

// Model
#include	<oreore/mathlib/GraphicsMath.h>
#include	<oreore/thread/ThreadPool.h>

#include	<graphics/gl4x/resource/GLRenderTarget.h>
#include	<graphics/gl4x/resource/TileCacheTexture.h>
#include	<graphics/gl4x/scene/Camera.h>
#include	<graphics/gl4x/app/MyGUI.h>
using namespace OreOreLib;


#include	"SeamlessPatches.h"
#include	"VirtualTexture.h"
#include	"TerrainEngine.h"
#include	"G_Buffer.h"
#include	"AtmosphericShader.h"
#include	"HDRShader.h"

// View
#include	"GraphicsDevice.h"

// Controller
#include	"Controller_base.h"






class PlanetRenderingEngine : public Controller_base
{
private:
	
	//============================ メンバ変数 ==========================//
	GraphicsDevice		*m_Device_Main;		// 地形描画用レンダリングコンテキスト
	GraphicsDevice		*m_Device_Sub;		// テクスチャ生成用レンダリングコンテキスト
	MyGUI				*m_GUI;				// GUI
	
	Camera				*m_Camera;			// カメラ
	VirtualTexture		*m_VirtualTexture;	// 仮想テクスチャ（ハイトマップを格納する）
	
	// 惑星
	TerrainEngine		*m_TerrainEngine;		// 地形レンダリングエンジン
	AtmosphericShader	*m_AtmosphericShader;	// 大気散乱シェーダー
	HDRShader			*m_HDRShader;			// ブルームシェーダー

	// G-buffer
	G_Buffer			*m_Gbuffer;
	
	// フレームバッファ
	int					m_ScreenWidth;		// スクリーン幅
	int					m_ScreenHeight;		// スクリーン高さ
	//FrameBufferAttachment	m_Attachment;
	//GLFrameBufferObject	*m_FramebufferObject;
	GLRenderTarget		m_RenderTarget;

	unsigned int		m_FinalImage;
	
	// スレッド
	HANDLE				m_hEvent;			// イベントハンドル(開始/終了)
	HANDLE				m_hMutex;			// スレッド排他制御ハンドル(一時停止)
	HANDLE				m_hThread;			// スレッド
	unsigned int		m_ThreadId;			// スレッドID
	ThreadPool			*m_ThreadPool;		// スレッドプール
	
	// その他
	int framecounter;						// 描画フレーム数カウンタ（VirtualTextureのキャッシュデータ更新に利用）
	
	//===================== プライベートメンバ関数 =====================//
	static unsigned _stdcall ThreadExecute(void *Param);	// スレッド関数
	void RunThread();										// スレッドで処理する中身
	
	PlanetRenderingEngine(){};	// デフォルトコンストラクタ


public:
	
	// 初期化
	PlanetRenderingEngine(int width, int height);	// コンストラクタ
	~PlanetRenderingEngine();						// デストラクタ

	// ゲームループの制御
	void Create();	// メインスレッドの作成 + その他色々セットアップ
	void Start();	// ループ開始
	void Stop();	// ループ中断
	
	// 入力
	void ProcessCameraKeys();						// カメラ操作
	void DoGUI();									// GUI描画処理
	unsigned virtual keyDown(WPARAM wParam);		// キー押し込み
	unsigned virtual keyUp(WPARAM wParam);			// キー解放

	unsigned virtual lButtonDown(LPARAM lParam);	// マウス左ボタン押し込み
	unsigned virtual lButtonUp(LPARAM lParam);		// マウス左ボタン解放
	unsigned virtual rButtonDown(LPARAM lParam);	// マウス右ボタン押し込み
	unsigned virtual rButtonUp(LPARAM lParam);		// マウス右ボタン解放
	unsigned virtual mouseMove(LPARAM lParam);		// カーソル移動

	void SetScreenSize(int width, int height);		// 描画領域の大きさをセットする
	void ChangeVirtualTextureParam(int tex_size);	// VirtualTextureの解像度を変更する
};





#endif // PLANET_RENDERING_ENGINE_H //