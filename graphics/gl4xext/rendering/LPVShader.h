#ifndef	LIGHT_PROPAGATION_VOLUMES_H
#define	LIGHT_PROPAGATION_VOLUMES_H

#include	<vector>

#include	<graphics/gl4x/shader/IShader.h>
#include	<graphics/gl4x/scene/Frustum.h>

#include	"LPVIrradianceVolume.h"



namespace OreOreLib
{


	// LPV Shader
	class LPVShader : public IShader
	{
	public:

		LPVShader();
		LPVShader( const TCHAR *filepath, GLSL_VERSION version );
		~LPVShader();

		//bool Init( int num_cascades, /*const Vec3i &lpv_size,*/ /*const Vec2i &rsm_size,*/ const Vec2i &vpl_size );
		bool InitShader( GLSL_VERSION version );		// シェーダの初期化
		bool InitRSMTexture( int vpl_width, int vpl_height );	// テクスチャの初期化
		bool InitVPLSprites();	// 仮想点光源スプライトの初期化
		bool InitLPVSlices();	// LPVセル中心点スプライトの初期化
		
		void Update( const Frustum& lightFrustum );
		void SetNumIteration(int numiter){ m_numIter = numiter; };


		//	void ViewSH(const Camera &camera/*, GLuint texSH*/);

		void BindDrawSurface();
		void UnbindDrawSurface();

		void SetMode(){ m_Mode = (m_Mode+1)%NUM_MODES; };

		void BindLPV( LPVIrradianceVolume *lpv_tex );	// LPVテクスチャをバインドする
		void UnbindLPV()	{ m_refLPVTex	= NULL; }

		void BindRSMBuffer( Texture2D* rsm_pos, Texture2D* rsm_normal, Texture2D* rsm_flux, Texture2D* rsm_depth );
		void UnbindRSMBuffer(){}

		void Render( const GeometryBuffer *pGeom, Frustum *pfrustum, const Frustum &lightFrustum, const Vec3f &lightIntensity );
		void Render( const GeometryBuffer *pGeom, Frustum *pfrustum );


		// Override Virtual Functions
		inline virtual int NumPasses() const						{ return 1; }
		inline virtual GLuint GetPassID( int shader_idx=0 ) const	{ return (m_Pass + shader_idx)->ID(); }

		virtual void BindShader( int passid=0 ){}

		virtual void BindBufferObject(const IBufferObject* pbufferobj){}
		virtual void UnbindBufferObject(){}

		virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat ){}
		

	private:

		struct stLPVSlice
		{
			Vec3f Position;
			Vec3f TexCoord;
		};

		//================== 各種パラメータ =============//
		int		m_numIter;	// 伝播計算の反復回数
		//int		m_numCascades;	// LPVの多重解像度レベル数
		//Vec3i	m_LpvSize;	// LPV(LighhtPropagationVolumes)のピクセル解像度
		Vec2i	m_RsmSize;	// RSM(Reflective Shadow Map)のピクセル解像度
		Vec2i	m_VplSize;	// 仮想点光源の解像度


		// レンダリングモード
		short	m_Mode;
		enum	m_RenderMode{ NORMAL, DIRECT_ONLY, INDIRECT_ONLY, NUM_MODES };



		//=================== シェーダ ==================//
		enum		m_PassIndex{ LPV_INJECTION, GV_INJECTION, PROPAGATION, SURFACE_SHADER, NUM_PASS };// レンダリングパスのインデックス
		GLShader	m_Pass[ NUM_PASS ];// レンダリングパスのシェーダ


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
		GLint	m_ulLpvCellSize;	// セルの各辺の要素数

		//GLint	m_ulMatLPVScreen;	// LPVスクリーン座標への変換マトリクス
		GLint	m_ulVPLScale;		// RSMのテクセルに対する仮想点光源の面積の比率

		// GVのInjectionシェーダー用変数
		GLint	m_ulRsmSizeInj;		// RSMの解像度
		GLint	m_ulLpvCellDim2;		// LPVのセルの辺の長さ
		GLint	m_ulLpvCellSize2;
		//GLint	m_ulMatLPVScreen2;

		// Propagationシェーダー用変数
		//GLint	m_ulIter;			// 反復計算の回数
		GLint	m_ulLpvCellSize_prop; // LPVセルの大きさ情報(xyz方向の要素数, 要素数の逆数)
		

		GLuint	matUboID[1];// マトリクスのUBO
		GLuint	matBlockIndex;
		GLint	matBlockSize;
		enum{UBP_MATRIX = 0};


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



		// LightPrpagationVolumeテクスチャへのポインタ
		LPVIrradianceVolume	*m_refLPVTex;

		// RSMテクスチャへのポインタ
		Texture2D		*m_refRsmPositionBuffer;
		Texture2D		*m_refRsmNormalBuffer;
		Texture2D		*m_refRsmFluxBuffer;
		Texture2D		*m_refRsmDepthBuffer;

		//======== フレームバッファオブジェクト =========//
		// Injection用フレームバッファ
		GLuint	m_Fbo_lpv;
		
		//================ ジオメトリ ===================//
		// 形状データ
		std::vector <Vec2f>			m_VPLSprites;	// 仮想点光源(2次元)スプライト(0.0, 0.0) - (1.0, 1.0)
		std::vector <stLPVSlice>	m_LPVSlices;	// LPVスライス面
		std::vector <GLuint>		m_LPVIndices;	// LPVスライス面のインデックス
		// OpenGLのバッファ
		GLuint	m_VAO[2];	// Vertex Array Object. 0: m_VBO[0]をバインド / 1: m_VBO[1]をバインド
		GLuint	m_VBO[2];	// Vertex Buffer Object. 0:Injection用の仮想点光源ジオメトリ / 1:Propagation用のセル中心ジオメトリ
		GLuint	eao;


		//=============== プライベート関数 ==============//
		void DrawRSM( const Vec3f& lightIntensity, const Vec3f& lightPos, const ViewTransformMatrix& light_mat );//Buffer *light_param, CameraMatrix *light_mat/*const CameraNode &camera*/ );	// 光源からRSMを描画する
		//void DownSample();				// RSMの解像度を下げる(16pixelを1pixelに集約).余裕があれば実装する
		void LPVInjection(const Vec3f &lightpos);				// 仮想点光源（RSMの各ピクセル）をLPVに書き込む
		void GVInjection();					
		void Propagation();					// LPV上で光の伝播計算を行う




		//======================= サーフェスシェーダの試験実装 TODO: 将来的には本クラスと分離する==================//

		GLShader	m_SurfaceShader;// サーフェスレンダリング用シェーダ

		GLint	m_ulMatWorld;
		GLint	m_ulMatViewProj;
		GLint	m_ulViewPos;	// カメラ位置
		GLint	m_ulLightPos2;	// 光源位置
		GLint	m_SHR, m_SHG, m_SHB, m_ShadowMap;
		GLint	m_ulLpvCellSize_;
		GLint	m_ulLpvCellDim_;
		GLint	m_ulMatTexCoord;

		GLint		m_ulTexture;			// texture
		GLint		m_ulDiffuse;			// diffuse material TODO: Support all material attributes
		GLint		m_ulSpecular;			// specular material TODO: Support all material attributes
		GLint		m_ulShininess;			// shininess material


		GLint	m_ulMode;	// レンダリングモード


	};





	class LPVShaderFactory : IShaderFactory
	{
	protected:

		IShader* Create( GLSL_VERSION version ){ return new LPVShader( _T( "" ), version ); };

	public:

		virtual ~LPVShaderFactory(){};
		IShader* CreateInstance( GLSL_VERSION version );

	};






}


#endif	// LIGHT_PROPAGATION_VOLUMES_H //