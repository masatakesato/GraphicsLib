#ifndef	LIGHT_PROPAGATION_VOLUMES_H
#define	LIGHT_PROPAGATION_VOLUMES_H


#include	<vector>


#include	<oreore/MathLib.h>
#include	<oreore/GLShader.h>
#include	<oreore/Camera.h>
using namespace OreOreLib;


typedef Vec3ui SH_LPV;



// LPV Texture Data
struct LPVTexture
{
	// Light Propagation Volumesの情報
	int		m_numCascades;	// LPVの多重解像度レベル数
	Vec3i	m_LpvSize;	// LPV(LighhtPropagationVolumes)のピクセル解像度
	float	*m_LpvCellDim;	// カスケード毎のLPVセルの1辺の長さ
	Vec3f	*m_LpvOrigin;	// カスケード毎のLPVの中心座標


	// LightPrpagationVolumeテクスチャ
	SH_LPV	**m_SH_LPVs;// LPVのSH.2次元配列(1次元目:FrontBack, 2次元目:Lv)
	SH_LPV	*m_SHaccum;// TODO: メモリ解放処理を書くこと
	GLuint	*m_SH_GV;	// GeometryVolumesのSH
	
	LPVTexture();
	~LPVTexture();

	void Init(int num_lpvs, const Vec3i &lpv_size/*, const Vec2i &rsm_size, const Vec2i &vpl_size*/);	// LPVテクスチャを生成する
	void Release();	// LPVテクスチャを削除する

};








// LPV Shader
class LightPropagationVolumes
{
private:
	
	struct stLPVSlice
	{
		Vec3f Position;
		Vec3f TexCoord;
	};

	//================== 各種パラメータ =============//
	int		m_numIter;	// 伝播計算の反復回数
//int		m_numLpvLevels;	// LPVの多重解像度レベル数
//Vec3i	m_LpvSize;	// LPV(LighhtPropagationVolumes)のピクセル解像度
	Vec2i	m_RsmSize;	// RSM(Reflective Shadow Map)のピクセル解像度
	Vec2i	m_VplSize;	// 仮想点光源の解像度


	// レンダリングモード
	short	m_Mode;
	enum	m_RenderMode{ NORMAL, DIRECT_ONLY, INDIRECT_ONLY, NUM_MODES };



	//=================== シェーダ ==================//
	enum	m_PassIndex{ DRAW_RSM, LPV_INJECTION, GV_INJECTION, PROPAGATION, SURFACE_SHADER, NUM_PASS };// レンダリングパスのインデックス
	GLShader	m_Pass[NUM_PASS];// レンダリングパスのシェーダ


	//================= シェーダ変数 ================//
	// 共通
	enum	m_AttribLocation
	{
		m_alPosition	= 0,
		m_alNormal		= 1,
		m_alTexCoord	= 2,
		m_alColor		= 3
	};
	GLint	m_ulMatMVP;			// model view projection matrix

	// Reflective Shadow Maps.シェーダー
	GLint	m_ulLightIntensity;	// 光源の明るさ(Flux)
	GLint	m_ulLightPos;		// 光源の位置
	GLint	m_ulLightDir;		// 光源へのベクトル
	GLint	m_ulRsmSize;		// RSMの解像度
	GLint	m_ulLightPos_;

		
	// LPVのInjectionシェーダー用変数
	GLint	m_ulLpvCellDim;		// LPVのセルの辺の長さ
	//GLint	m_ulMatLPVScreen;	// LPVスクリーン座標への変換マトリクス
	GLint	m_ulVPLScale;		// RSMのテクセルに対する仮想点光源の面積の比率
	
	// GVのInjectionシェーダー用変数
	GLint	m_ulRsmSizeInj;		// RSMの解像度
	GLint	m_ulLpvCellDim2;		// LPVのセルの辺の長さ
	//GLint	m_ulMatLPVScreen2;

	// Propagationシェーダー用変数
	GLint	m_ulIter;			// 反復計算の回数
	GLint	m_ulLpvCellSize; // LPVセルの大きさ情報(xyz方向の要素数, 要素数の逆数)



GLuint	matUboID[1];// マトリクスのUBO
GLuint	matBlockIndex;
GLint	matBlockSize;
enum{UBP_MATRIX = 0};


// RSMのUniformBufferObject
GLuint	rsmUboID[1];
GLuint	rsmBlockIndex;
GLint	rsmBlockSize;
enum{UBP_RSM = 1};


	//============ テクスチャオブジェクト ===========//
	// テクスチャのアタッチポイント(glActiveTexture()，glFtamebufferTexture()で使用する)
	enum	m_Attachment_RSM
	{
		RsmPosition	= 0,
		RsmNormal	= 1,
		RsmFlux		= 2,
		RsmDepth	= 3
	};// RSMの出力チャンネル

	enum	m_Attachment_SH
	{
		SH_R		= 0,
		SH_G		= 1,
		SH_B		= 2,
		SHaccum_R	= 3,
		SHaccum_G	= 4,
		SHaccum_B	= 5,
		SH_GV		= 6
	};// SHの出力チャンネル


	// Refrective Shadow Mapテクスチャ
	GLuint		*m_RSM;

	// LightPrpagationVolumeテクスチャへのポインタ
	LPVTexture	*m_pLPVTex;

	unsigned char Src, Dst;


	//======== フレームバッファオブジェクト =========//
	// RSM用フレームバッファとレンダーバッファ
	GLuint	m_Fbo_rsm;
	GLuint	m_Rb_rsm;
	// Injection用フレームバッファ
	GLuint	m_Fbo_lpv;



	//================ ジオメトリ ===================//
	// 形状データ
	std::vector <Vec2f>		m_VPLSprites;	// 仮想点光源(2次元)スプライト
	std::vector <stLPVSlice>	m_LPVSlices;	// LPVスライス面
	std::vector <GLuint>		m_LPVIndices;	// LPVスライス面のインデックス
	// OpenGLのバッファ
	GLuint	m_VAO[2];	// Vertex Array Object. 0: m_VBO[0]をバインド / 1: m_VBO[1]をバインド
	GLuint	m_VBO[2];	// Vertex Buffer Object. 0:Injection用の仮想点光源ジオメトリ / 1:Propagation用のセル中心ジオメトリ
	GLuint	eao;

	//=============== プライベート関数 ==============//
	bool InitShader();		// シェーダの初期化
	bool InitTexture();		// テクスチャの初期化
	bool InitVPLSprites();	// 仮想点光源スプライトの初期化
	bool InitLPVSlices();	// LPVセル中心点スプライトの初期化

	void DrawRSM(const Camera &camera);	// 光源からRSMを描画する
	//void DownSample();				// RSMの解像度を下げる(16pixelを1pixelに集約).余裕があれば実装する
	void LPVInjection(const Vec3f &lightpos);				// 仮想点光源（RSMの各ピクセル）をLPVに書き込む
	void GVInjection();					
	void Propagation();					// LPV上で光の伝播計算を行う


//======================= サーフェスシェーダの試験実装 TODO: 将来的には本クラスと分離する==================//

	GLShader	m_SurfaceShader;// サーフェスレンダリング用シェーダ

	GLint	m_ulMatMVP2;	// モデルビュー変換行列
	GLint	m_ulEyePos2;	// カメラ位置
GLint	m_ulLightPos2;	// 光源位置
GLint	m_SHR, m_SHG, m_SHB, m_ShadowMap;

GLint	m_ulMode;	// レンダリングモード





public:

	LightPropagationVolumes();
	~LightPropagationVolumes();

	bool Init(int num_lpvs, const Vec3i &lpv_size, const Vec2i &rsm_size, const Vec2i &pvl_size);

	void Draw(const Camera &lightcamera);
	void SetNumIteration(int numiter){ m_numIter = numiter; };


//	void ViewSH(const Camera &camera/*, GLuint texSH*/);

	void DrawSurface(const Camera &camera, const Camera &lightcam);

	void SetMode(){ m_Mode = (m_Mode+1)%NUM_MODES; };


	void BindLPV(LPVTexture *lpv_tex)	{ m_pLPVTex = lpv_tex; };	// LPVテクスチャをバインドする
	void UnbindLPV()					{ m_pLPVTex	= NULL; };

};





#endif	// LIGHT_PROPAGATION_VOLUMES_H //