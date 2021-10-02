#ifndef	LIGHT_SHAFT_SHADER_H
#define	LIGHT_SHAFT_SHADER_H

#include	<graphics/gl4x/shader/IShader.h>
#include	<graphics/gl4x/shader/GLShader.h>
#include	<graphics/gl4x/shader/GLComputeShader.h>



namespace OreOreLib
{

	struct LightShaftBuffer
	{

	};
	

	class LightShaftShader : public IShader
	{
	public:
		
		LightShaftShader();
		LightShaftShader( const TCHAR* filepath, GLSL_VERSION version );
		~LightShaftShader();

		void InitShader( const TCHAR* filepath, GLSL_VERSION version );
		void InitShaderResources( int numsamples, int numslices, int numcascades );
		inline virtual void BindShader( int passid=0 ){}
		void Release();

		void BindFramebuffer( const Texture2D* tex_depth,/* const Texture2D& tex_color, const Texture2D& tex_normal,*/ const Texture2D* tex_pos );
		void UnbindFramebuffer( );
		void BindLightSource( int light_id );
		void UnbindLightSource();

		inline virtual int NumPasses() const						{ return 1; }
		inline virtual GLuint GetPassID( int shader_idx=0 ) const	{ return 0; }

		//inline virtual void BindShader( int shader_idx=0 )	// シェーダーをバインドする
		//{
		//	if( m_pCurrShader == m_pShader )	return;
		//	m_pCurrShader = m_pShader;
		//	m_pCurrShader->Bind();
		//}
		
		virtual void BindBufferObject( const IBufferObject* pbufferobj );
		virtual void UnbindBufferObject();

		void SetInititalSampleStepInSlice( int val )	{}

		void Render( uint32 texid );
		virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat ){}
		void Render( const Vec3f& wsLightPos, int lightID, const ViewTransformMatrix& cam_mat );


		//################## EpipolarSampling用のセットアップ関数試験実装 ###################//
		void InitSliceExitPoints( int dimx, int dimy );// 画面のフレーム上にエピポーラースライスの終点を配置する
		//###################################################################################//
		
		void UpdateEpipolarLightParam( const Vec3f& wsLightPos, int lightID, const ViewTransformMatrix& matCameraTrans );


		// 計算用中間バッファ(テクスチャ)を取得する。可視化デバッグ用
		Texture1D* GetSliceEndPoints() const			{ return (Texture1D*)&m_SliceEndPoints; }
		Texture2D* GetEpipolarCoordinate() const		{ return (Texture2D*)&m_EpipolarCoordinate; }
		Texture2D* GetEpipolarCameraSpaceZ() const		{ return (Texture2D*)&m_EpipolarCameraSpaceZ; }
		Texture2D* GetEpipolarDepthStencil() const		{ return (Texture2D*)&m_EpipolarDepthStencil; }
		Texture2D* GetInterpolationSource() const		{ return (Texture2D*)&m_InterpolationSource; }
		Texture2D* GetSliceUvOriginDirection() const	{ return (Texture2D*)&m_SliceUvOriginDirection; }
		Texture2D* Get1DMinMaxShadowMap() const			{ return (Texture2D*)&m_1DMinMaxShadowMap; }


	private:

		enum PASS
		{
			RENDER_SLICE_ENDPOINTS,
			RENDER_COORDINATE_TEXTURE,
			REFINE_SAMPLE_LOCATIONS,
			RENDER_SLICE_UV_ORIG_DIR,
			COMPUTE_MIN_MAX_MIPMAP,
			NUM_PASS,
		};
		
		enum TEX_UNIT
		{
			TEX_UNIT_SLICE_END_POINTS,	// TODO:調べる. Sampler1Dをテクスチャユニット0に割り当てると、image*Dとの衝突が起こる。2014.10.20 -> 処理終わったあとで、glBindTexture( GL_TEXTURE_1D, 0 )でテクスチャユニット初期化が必要。
			//TEX_UNIT_POSITION,
			TEX_UNIT_DEPTH,
			TEX_UNIT_EPIPOLAR_COORDINATE,
			TEX_UNIT_EPIPOLAR_CAM_SPACE_Z,
			TEX_UNIT_EPIPOLAR_DEPTH_STENCIL,
			TEX_UNIT_SHADOWMAP,
			NUM_TEX_UNIT,
		};
		

		struct EpipolarParam
		{
			int		NumSamplesInSlice;
			int		NumEpipolarSlices;
			Vec2f	ScreenResolution;

			float	RefinementThreshold;
			float	EpipoleSamplingDensityFactor;
		};


		//TODO: 既にあるパラメータを活用する。
		///TODO: 光源のlight_idと、対応するシャドウマップのインデックスshadow_idをシェーダーに転送すればいい.
		// TODO: 現状はとりあえず、下記UBO経由でパラメータを渡す
		struct EpipolarLightParam
		{
			Vec2f	ScreenLightPos;
			//int		NumCascades;		// LightAttribute.glslの g_NumShadows( idx )と同じ
			//Vec2f	ShadowMapTexelSize;	// ShadowMap.glslのg_ShadowMapTexelSizeと同じ
			//Mat4f	MatLight;			// ShadowMap.glslのg_MatShadow(i)と同じ
			int		LightID;
		};


		// ComputerShader - Render Slice EndPoints
		GLComputeShader	*m_pComputeSliceEndPoints;

		// ComputeShader - Render Coordinate Texture
		GLComputeShader	*m_pComputeCoordinateTexture;
		
		// ComputeShader - Refine Sample Locations
		GLComputeShader	*m_pRefineSampleLocations;

		// ComputeShader - Render Slice UV Origin and Direction
		GLComputeShader	*m_pComputeSliceUvOriginDirection;

		// ComputeShader - Compute Min-Max Mipmap
		GLComputeShader	*m_pInitMinMaxShadowMap;
		GLComputeShader	*m_pCompute1DMinMaxMipmap;

		// ComputeShader - Mark Ray Marching Samples
		GLComputeShader	*m_pMarkRayMarchingSamples;

		// ComputeShader - Do Ray Marching
		GLComputeShader	*m_pDoRayMarching;

		// ComputeShader - Interpolate Inscatterting
		GLComputeShader	*m_pInterpolateInscattering;


		// Internal Buffer for Epipolar Sampling
		Texture2D		m_EpipolarCoordinate;		// numSamples * numSlices. RG_FLOAT32
		Texture2D		m_EpipolarCameraSpaceZ;		// numSamples * numSlices. R_FLOAT32
		Texture2D		m_EpipolarDepthStencil;		// numSamples * numSlices. D24_S8 -> R8_UNORMに変更(2014.12.26)
		Texture2D		m_InterpolationSource;		// numSamples * numSlices. RG_UINT16
		Texture2D		m_InitialInscattering;		// numSamples * numSlices. RGBA_FLOAT16
		Texture2D		m_InterpolatedInscattering;	// numSamples * numSlices. RGBA_FLOAT16
		Texture1D		m_SliceEndPoints;			// numSlices. RGBA_FLOAT32. xy: EntryPoint(光源に近い方の点), zw:ExitPoint(光源から遠い方の点)
		Texture2D		m_SliceUvOriginDirection;	// numSlices. * numCascades RGBA_FLOAT32
		Texture2D		m_1DMinMaxShadowMap;			// shadowmapResolution * ( numSlices * numshadowcascades ).

		// Reference Texture from Gbuffer
		const Texture2D	*m_refTexPosition;
		const Texture2D	*m_refTexDepth;

		const IBufferObject *m_refLightAttrib;

		// 1DMinMaxMipmap-specific shader parameters
		GLint				m_ulMinMaxSampleOffset;
		GLint				m_ulMinMaxStoreOffset;
		
		// 色々よく分かってないパラメータ
		int						m_SampleRefinementThreadGroupSize;	// RefineSampleLocationコンピュートシェーダーのスレッドグループの大きさ。必ず32以上にすること
		int						m_InitialSampleStepInSlice;

		int						m_numMipMapLevels;

		static GLuint			m_BindPoints[3];

		Vec2f					*m_SliceExitPoints;
		GLUniformBufferObject	m_uboExitPoints;

		EpipolarParam			m_EpipolarParam;
		GLUniformBufferObject	m_uboEpipolarParam;

		EpipolarLightParam		m_EpipolarLightParam;
		GLUniformBufferObject	m_uboEpipolarLightParam;


		// 処理ステップ
		void ReconstructCameraDepth();	// いらない.はじめから実距離のデプスマップ生成している
		void RenderSliceEndPoints();
		void RenderCoordinateTexture();
		void RefineSampleLocations();
		void RenderSliceUVOriginAndDirection();
		void InitMinMaxShadowMap();
		void ComputeMinMaxMipmap();
		void MarkRayMarchingSamples();
		void DoRayMarching();
		void InterpolateInscattering();

		LightShaftShader( const LightShaftShader& obj ){}

	};




	// LightShaftShaderオブジェクトを生成するファクトリクラス
	class LightShaftShaderFactory : IShaderFactory
	{
	protected:

		IShader* Create( GLSL_VERSION version ){ return new LightShaftShader( _T( "LightShaftShader.glsl" ), version ); }

	public:

		virtual ~LightShaftShaderFactory(){}
		IShader* CreateInstance( GLSL_VERSION version );

	};




}// end of namespace





#endif	// LIGHT_SHAFT_SHADER_H //