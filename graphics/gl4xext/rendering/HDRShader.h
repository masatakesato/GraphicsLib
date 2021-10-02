#ifndef HDR_SHADER_H
#define HDR_SHADER_H

#include	<graphics/gl4x/resource/GLFrameBufferObject.h>
#include	<graphics/gl4x/shader/IShader.h>
#include	<graphics/gl4x/shader/GLShaderParameter.h>



namespace OreOreLib
{

	class HDRShader : public IShader
	{
	public:

		HDRShader( const TCHAR *filepath, GLSL_VERSION version );
		~HDRShader();

		void InitShader( const TCHAR *filepath, GLSL_VERSION version );
		void Release();

		void BindInputTexture( Texture2D *tex );
		void UnbindInputTexture();

		const Texture2D *const GetScaledImage() const					{ return (Texture2D*)&m_ScaledImage; }
		const Texture2D *const GetAverageLuminance( int idx=0 ) const	{ return (Texture2D*)&m_AverageLuminance[idx]; }
		const Texture2D *const GetAdaptedLuminance() const				{ return (Texture2D*)&m_AdaptedLuminance[m_CurrAdaptedLuminance ]; }
		const Texture2D *const GetBrightPass()const						{ return (Texture2D*)&m_BrightPass; }
		const Texture2D *const GetBrightPass2x2() const					{ return (Texture2D*)&m_BrightPass2x2; }
		const Texture2D *const GetBlurredImage() const					{ return (Texture2D*)&m_BlurredImage; }
		const Texture2D *const GetBlurredImage2() const					{ return (Texture2D*)&m_BlurredImage2; }
		
		void DoHDRLighting( float timestep, float key, float bloomweight );

		// Override Virtual Functions
		inline virtual int NumPasses() const						{ return NUM_PASS; }
		inline virtual GLuint GetPassID( int shader_idx=0 ) const	{ return m_Pass[shader_idx].ID(); }

		inline virtual void BindShader( int shader_idx=0 )	// シェーダーをバインドする
		{
			if( m_pCurrShader == &m_Pass[ shader_idx ] )	return;
			m_pCurrShader = &m_Pass[ shader_idx ];
			m_pCurrShader->Bind();
		}

		virtual void BindBufferObject(const IBufferObject* pbufferobj){}
		virtual void UnbindBufferObject(){}

		virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat ){}


	private:

		int	m_Width4x4;
		int	m_Height4x4;

		Texture2D	*m_refTexture;

		//===== テクスチャ（作業用バッファ）=====//
		Texture2D	m_ScaledImage;// 1/4に縮小したテクスチャ
		Texture2D	m_AverageLuminance[4];// 現在の画面の輝度テクスチャ(64x64, 16x16, 4x4, 1x1)
		
		Texture2D	m_AdaptedLuminance[2];	
		uint8		m_PrevAdaptedLuminance, m_CurrAdaptedLuminance;

		Texture2D	m_BrightPass, m_BrightPass2x2;
		Texture2D	m_BlurredImage;	// ぼかしテクスチャ
		Texture2D	m_BlurredImage2;	// ぼかしテクスチャ

		// フレームバッファオブジェクト
		GLFrameBufferObject	m_Fbo;


		enum EFFECT_PASS
		{
			DOWNSAMPLE_2X2,
			DOWNSAMPLE_4X4,
			CALC_LOG_LUMINANCE,
			AVERAGE_LOG_LUMINANCE,
			CALC_ADAPTIVE_LUMINANCE,
			BRIGHT_PASS_FILTER,
			GAUSSIAN_BLUR,
			COMPOSITE,
			NUM_PASS
		};

		GLShader	m_Pass[NUM_PASS];

		GLint		m_ulTimeStep;		// 明順応のタイムステップ
		GLint		m_ulKey, m_ulKey2;	// 明るさパラメータ
		GLint		m_ulBloomWeight;	// ブルーム効果の重み係数
		GLint		m_ulCalcExponential;// expを計算するか否かのフラグ(平均輝度テクスチャ作成時に使用)
		GLint 		m_ulBlurDirection;	// ブラー方向


		GLUniform	m_ulViewportResolution;


		//======= シェーダグローバル変数 ========//
		struct HDRParam
		{
			Vec2f	texOffset;
			GLint	timeStep;
			GLint	key;
			GLint	bloomWeight;
		};

		HDRParam	m_HDRParam;
		GLUniformBufferObject	m_uboHDRParam;
		

		void GenScaledImage();// 画像INの縮小イメージを作成する
		void GenAverageLuminance();			// 縮小イメージから平均輝度を計算する
		void AdaptLuminance();				// 明順応
		void FilterBlightPass();			// 縮小イメージの明るい領域だけ取り出す
		void BlurImage();					// 画像をぼかす
		void RenderFinalImage();

	};




	class HDRShaderFactory : IShaderFactory
	{
	protected:

		IShader* Create( GLSL_VERSION version ){ return new HDRShader( _T( "HDRShader.glsl" ), version ); };

	public:

		virtual ~HDRShaderFactory(){};
		IShader* CreateInstance( GLSL_VERSION version );

	};




}// end of namespace



#endif // HDR_SHADER_H //
