#include	"LightShaftShader.h"


#include	"GLBindPointManager.h"
#include	"ViewTransformBuffer.h"
#include	"LightAttributeBuffer.h"
#include	"ShadowBuffer.h"



namespace OreOreLib
{

#define	NUM_EPIPOLAR_SLICES_X	128
#define	NUM_EPIPOLAR_SLICES_Y	128
//#define	NUM_EPIPOLAR_SLICES		512

#define	MAX_SAMPLES_IN_SLICE	512


#define INITIAL_SAMPLE_STEP		128

#define	SAMPLE_REFINEMENT_MINIMUM_THREAD_GROUP_SIZE	128

	

	GLuint	LightShaftShader::m_BindPoints[3]	= { 0, 0, 0 };


	//######################################################################################################//
	//							LightShaftShader class public member functions								//
	//######################################################################################################//
	
	// Default Constructor
	LightShaftShader::LightShaftShader()
	{
#ifdef _DEBUG
		tcout << "LightShaftShader()..." << tendl;
#endif // _DEBUG

		m_pComputeSliceEndPoints			= NULL;
		m_pComputeCoordinateTexture			= NULL;
		m_pRefineSampleLocations			= NULL;
		m_pComputeSliceUvOriginDirection	= NULL;
		m_pInitMinMaxShadowMap				= NULL;
		m_pCompute1DMinMaxMipmap			= NULL;
		m_pMarkRayMarchingSamples			= NULL;
		m_pDoRayMarching					= NULL;
		m_pInterpolateInscattering			= NULL;

		m_numMipMapLevels					= 4;
		m_SliceExitPoints					= NULL;


		m_EpipolarParam.NumEpipolarSlices	= 0;
		m_EpipolarParam.NumSamplesInSlice	= 0;
		InitVec( m_EpipolarParam.ScreenResolution, 0.0f, 0.0f );
		m_EpipolarParam.RefinementThreshold				= 0.03f;
		m_EpipolarParam.EpipoleSamplingDensityFactor	= 2.0f;


		m_SampleRefinementThreadGroupSize	= 0;
		m_InitialSampleStepInSlice			= INITIAL_SAMPLE_STEP;

		InitVec( m_EpipolarLightParam.ScreenLightPos, -999.9f, -999.9f );

		m_refTexPosition					= NULL;
		m_refTexDepth						= NULL;

		m_refLightAttrib					= NULL;
	}



	// Constructor
	LightShaftShader::LightShaftShader( const TCHAR *filepath, GLSL_VERSION version )
	{
#ifdef _DEBUG
		tcout << "LightShaftShader( const TCHAR *filepath, GLSL_VERSION version )..." << tendl;
#endif // _DEBUG

		m_pComputeSliceEndPoints			= NULL;
		m_pComputeCoordinateTexture			= NULL;
		m_pRefineSampleLocations			= NULL;
		m_pComputeSliceUvOriginDirection	= NULL;
		m_pInitMinMaxShadowMap				= NULL;
		m_pCompute1DMinMaxMipmap			= NULL;
		m_pMarkRayMarchingSamples			= NULL;
		m_pDoRayMarching					= NULL;
		m_pInterpolateInscattering			= NULL;

		m_numMipMapLevels					= 4;
		m_SliceExitPoints					= NULL;
		

		m_EpipolarParam.NumEpipolarSlices	= 0;
		m_EpipolarParam.NumSamplesInSlice	= 0;
		InitVec( m_EpipolarParam.ScreenResolution, 0.0f, 0.0f );
		m_EpipolarParam.RefinementThreshold				= 0.03f;
		m_EpipolarParam.EpipoleSamplingDensityFactor	= 2.0f;


		m_SampleRefinementThreadGroupSize	= 0;
		m_InitialSampleStepInSlice			= INITIAL_SAMPLE_STEP;

		InitVec( m_EpipolarLightParam.ScreenLightPos, -999.9f, -999.9f );

		m_refTexPosition					= NULL;
		m_refTexDepth						= NULL;

		m_refLightAttrib					= NULL;


		if( filepath )
			InitShader( filepath, version );
	}



	// Destructor
	LightShaftShader::~LightShaftShader()
	{
#ifdef _DEBUG
		tcout << "~LightShaftShader()..." << tendl;
#endif // _DEBUG
		
		Release();
	}



	// init shader
	void LightShaftShader::InitShader( const TCHAR *filepath, GLSL_VERSION version )
	{
		GLuint	program_id;

		//======================= Init ComputeShader ============================//
		// ComputeSliceEndPoints shader
		SafeDelete( m_pComputeSliceEndPoints );
		m_pComputeSliceEndPoints	= new GLComputeShader();
		m_pComputeSliceEndPoints->Init( _T( "LightShaftShader/ComputeSliceEndPoints.glsl" ), version );
		program_id					= m_pComputeSliceEndPoints->ID();


		// ComputeCoordinateTexture shader
		SafeDelete( m_pComputeCoordinateTexture );
		m_pComputeCoordinateTexture	= new GLComputeShader();
		m_pComputeCoordinateTexture->Init( _T( "LightShaftShader/ComputeCoordinateTexture.glsl" ), version );
		program_id					= m_pComputeCoordinateTexture->ID();

		//m_ulSliceEndPoints			= glGetUniformLocation( program_id, "g_SliceEndPoints" );
		//m_ulTexPosition				= glGetUniformLocation( program_id, "g_TexPosition" );
		//m_ulTexDepth				= glGetUniformLocation( program_id, "g_TexDepth" );

		m_pComputeCoordinateTexture->Link();

		// テクスチャをシェーダーにバインドする
		m_pComputeCoordinateTexture->Bind();
		{
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( m_pComputeCoordinateTexture->ID(), "g_SliceEndPoints" ), TEX_UNIT_SLICE_END_POINTS ) );// SliceEndPoints
			//GL_SAFE_CALL( glUniform1i( glGetUniformLocation( m_pComputeCoordinateTexture->ID(), "g_TexPosition" ), TEX_UNIT_POSITION ) );			// Position Texture
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( m_pComputeCoordinateTexture->ID(), "g_TexDepth" ), TEX_UNIT_DEPTH ) );					// Depth Texture
		}
		m_pComputeCoordinateTexture->Unbind();


		// RefineSampleLocations shader
		SafeDelete( m_pRefineSampleLocations );
		m_pRefineSampleLocations	= new GLComputeShader();
		m_pRefineSampleLocations->Init( _T( "LightShaftShader/RefineSampleLocations.glsl" ), version );


		m_pRefineSampleLocations->Link();


		// Compute Slice UV Origin and Direction shader
		m_pComputeSliceUvOriginDirection	= new GLComputeShader();
		m_pComputeSliceUvOriginDirection->Init( _T( "LightShaftShader/ComputeSliceUVOriginAndDirection.glsl" ), version );

		m_pComputeSliceUvOriginDirection->Link();


		// Init MinMaxShadowmap shader
		m_pInitMinMaxShadowMap	= new GLComputeShader();
		m_pInitMinMaxShadowMap->Init( _T( "LightShaftShader/InitMinMaxShadowMap.glsl" ), version );

		m_pInitMinMaxShadowMap->Link();


		// Compute 1D MinMax Mipmap shader
		m_pCompute1DMinMaxMipmap	= new GLComputeShader();
		m_pCompute1DMinMaxMipmap->Init( _T( "LightShaftShader/Compute1DMinMaxMipmap.glsl" ), version );
		program_id					= m_pCompute1DMinMaxMipmap->ID();
		
		m_ulMinMaxSampleOffset		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_MinMaxSampleOffset" ) );
		m_ulMinMaxStoreOffset		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_MinMaxStoreOffset" ) );

		m_pCompute1DMinMaxMipmap->Link();


		// Mark Ray Marching Samples
		m_pMarkRayMarchingSamples	= new GLComputeShader();
		m_pMarkRayMarchingSamples->Init( _T( "LightShaftShader/MarkRayMarchingSamples.glsl" ), version );

		m_pMarkRayMarchingSamples->Link();


		// Do Ray Marching
		m_pDoRayMarching			= new GLComputeShader();
		m_pDoRayMarching->Init( _T( "LightShaftShader/DoRayMarching.glsl" ), version );

		m_pDoRayMarching->Link();


		// Interpolate Inscattering
		m_pInterpolateInscattering	= new GLComputeShader();
		m_pInterpolateInscattering->Init( _T( "LightShaftShader/InterpolateInscattering.glsl" ), version );

		m_pInterpolateInscattering->Link();



		//======================= Init inermediate buffers ======================//
		// 画面フレーム上のスライス終点(m_SliceExitPoints)を生成する
		InitSliceExitPoints( NUM_EPIPOLAR_SLICES_X, NUM_EPIPOLAR_SLICES_Y );
		m_EpipolarParam.NumSamplesInSlice	= MAX_SAMPLES_IN_SLICE;

        // RefineSampleLocationsコンピュートシェーダーのスレッドグループ数を設定する
		// スレッドグループ数は、初期サンプルステップ数以上になること
        m_SampleRefinementThreadGroupSize	= Max( SAMPLE_REFINEMENT_MINIMUM_THREAD_GROUP_SIZE, m_InitialSampleStepInSlice );
        // スレッドグループ数はスライスの最大サンプル数以下になること　Thread group size cannot be larger than the total number of samples in slice
		m_SampleRefinementThreadGroupSize	= Min( m_SampleRefinementThreadGroupSize, m_EpipolarParam.NumSamplesInSlice );


		// 計算途中データを格納するためのテクスチャを確保する
		InitShaderResources( m_EpipolarParam.NumSamplesInSlice, m_EpipolarParam.NumEpipolarSlices, 1 );//MAX_SHADOW_CASCADES );//


		// UniformBufferobjectを生成して、m_SliceExitPointsをGPUに転送する
		for( int i=0; i<3; ++i )	if( m_BindPoints[i]==0 )	m_BindPoints[i] = GLBindPointManager::Reserve( GL_UNIFORM_BUFFER );

		m_uboExitPoints.Init( GL_STATIC_DRAW, m_BindPoints[0] );
		m_uboExitPoints.BindToShaderByName( m_pComputeSliceEndPoints->ID(), "ubEndPoints" );
		m_uboExitPoints.UpdateSubresource( m_SliceExitPoints, sizeof(Vec2f)* m_EpipolarParam.NumEpipolarSlices );

		m_uboEpipolarParam.Init( GL_STATIC_DRAW, m_BindPoints[1] );
		m_uboEpipolarParam.BindToShaderByName( m_pComputeSliceEndPoints->ID(), "ubEpipolarParam" );
		m_uboEpipolarParam.BindToShaderByName( m_pComputeCoordinateTexture->ID(), "ubEpipolarParam" );
		m_uboEpipolarParam.BindToShaderByName( m_pRefineSampleLocations->ID(), "ubEpipolarParam" );
		m_uboEpipolarParam.BindToShaderByName( m_pComputeSliceUvOriginDirection->ID(), "ubEpipolarParam" );
		m_uboEpipolarParam.BindToShaderByName( m_pInitMinMaxShadowMap->ID(), "ubEpipolarParam" );

		m_uboEpipolarParam.UpdateSubresource( &m_EpipolarParam, sizeof( EpipolarParam ) );


		m_uboEpipolarLightParam.Init( GL_DYNAMIC_DRAW, m_BindPoints[2] );
		m_uboEpipolarLightParam.BindToShaderByName( m_pComputeSliceEndPoints->ID(), "ubEpipolarLightParam" );
		m_uboEpipolarLightParam.BindToShaderByName( m_pRefineSampleLocations->ID(), "ubEpipolarLightParam" );
		m_uboEpipolarLightParam.BindToShaderByName( m_pInitMinMaxShadowMap->ID(), "ubEpipolarLightParam" );

		m_uboEpipolarLightParam.UpdateSubresource( &m_EpipolarLightParam, sizeof( EpipolarLightParam ) );
	}



	void LightShaftShader::InitShaderResources( int numsamples, int numslices, int numcascades )
	{
		m_EpipolarCoordinate.Allocate( numsamples, numslices, 0, 0, DATA_FORMAT::FORMAT_R32G32_FLOAT );			// numSamples * numSlices. RG_FLOAT32
		m_EpipolarCoordinate.GenHardwareTexture();

		m_EpipolarCameraSpaceZ.Allocate( numsamples, numslices, 0, 0, DATA_FORMAT::FORMAT_R32_FLOAT );			// numSamples * numSlices. R_FLOAT32
		m_EpipolarCameraSpaceZ.GenHardwareTexture();

		m_EpipolarDepthStencil.Allocate( numsamples, numslices, 0, 0, DATA_FORMAT::FORMAT_R8_UNORM );		// numSamples * numSlices. D24_S8 -> R8_UNORM
		m_EpipolarDepthStencil.GenHardwareTexture();

		m_InterpolationSource.Allocate( numsamples, numslices, 0, 0, DATA_FORMAT::FORMAT_R16G16_UINT );			// numSamples * numSlices. RG_UINT16
		m_InterpolationSource.GenHardwareTexture();

		m_InitialInscattering.Allocate( numsamples, numslices, 0, 0, DATA_FORMAT::FORMAT_R16G16B16A16_FLOAT );		// numSamples * numSlices. RGBA_FLOAT16
		m_InitialInscattering.GenHardwareTexture();

		m_InterpolatedInscattering.Allocate( numsamples, numslices, 0, 0, DATA_FORMAT::FORMAT_R16G16B16A16_FLOAT );	// numSamples * numSlices. RGBA_FLOAT16
		m_InterpolatedInscattering.GenHardwareTexture();

		m_SliceEndPoints.Allocate( numslices, 0, 0, DATA_FORMAT::FORMAT_R32G32B32A32_FLOAT );					// numSlices. RGBA_FLOAT32
		m_SliceEndPoints.GenHardwareTexture();

		m_SliceUvOriginDirection.Allocate( numslices, numcascades, 0, 0, DATA_FORMAT::FORMAT_R32G32B32A32_FLOAT );			// numSlices * numCascades. RGBA_FLOAT32
		m_SliceUvOriginDirection.GenHardwareTexture();
	}



	void LightShaftShader::Release()
	{
		
		SafeDelete( m_pComputeSliceEndPoints );
		SafeDelete( m_pComputeCoordinateTexture );
		SafeDelete( m_pRefineSampleLocations );
		SafeDelete( m_pComputeSliceUvOriginDirection );
		SafeDelete( m_pInitMinMaxShadowMap );
		SafeDelete( m_pCompute1DMinMaxMipmap );
		SafeDelete( m_pMarkRayMarchingSamples );
		SafeDelete( m_pDoRayMarching );
		SafeDelete( m_pInterpolateInscattering );

		SafeDeleteArray( m_SliceExitPoints );
		m_uboExitPoints.Release();

		m_EpipolarParam.NumEpipolarSlices	= 0;
		m_EpipolarParam.NumSamplesInSlice	= 0;
		InitVec( m_EpipolarParam.ScreenResolution, 0.0f, 0.0f );
		m_uboEpipolarParam.Release();

		InitVec( m_EpipolarLightParam.ScreenLightPos, -999.9f, -999.9f );
		m_uboEpipolarLightParam.Release();

		m_EpipolarCoordinate.Release();
		m_EpipolarDepthStencil.Release();
		m_EpipolarCameraSpaceZ.Release();
		m_InterpolationSource.Release();
		m_InitialInscattering.Release();
		m_InterpolatedInscattering.Release();
		m_SliceEndPoints.Release();
		m_SliceUvOriginDirection.Release();
		m_1DMinMaxShadowMap.Release();

		UnbindFramebuffer( );

		m_refLightAttrib	= NULL;
	}



	void LightShaftShader::BindFramebuffer( const Texture2D* tex_depth,/* const Texture2D* tex_color, const Texture2D* tex_normal,*/ const Texture2D* tex_pos )
	{
		m_refTexPosition	= tex_pos;
		m_refTexDepth		= tex_depth;

		InitVec( m_EpipolarParam.ScreenResolution, float(m_refTexDepth->Width()), float(m_refTexDepth->Height()) );
		m_uboEpipolarParam.UpdateSubresource( &m_EpipolarParam, sizeof( EpipolarParam ) );

	}
	


	void LightShaftShader::UnbindFramebuffer( )
	{
		m_refTexPosition	= NULL;
		m_refTexDepth		= NULL;
	
	}



	void LightShaftShader::BindLightSource( int light_id )
	{
	
	
	}



	void LightShaftShader::UnbindLightSource()
	{
	
	
	}



	void LightShaftShader::BindBufferObject( const IBufferObject* pbufferobj )
	{
		const type_info &buffer_type	= typeid(*pbufferobj);
		//tcout << "class name: " << id.name() << tendl;// .name()使うと、コンパイラが間違えてメモリリーク報告してくる. 2013.08.05

		if( buffer_type == typeid(ViewTransformBuffer) )
		{
			( (ViewTransformBuffer *)pbufferobj )->BindToShader( m_pComputeSliceUvOriginDirection->ID() );
		}
		else if( buffer_type == typeid(LightAttributeBuffer) )// 光源だった場合
		{
			m_refLightAttrib = pbufferobj;
			( (LightAttributeBuffer *)pbufferobj )->BindToShader( m_pComputeSliceUvOriginDirection->ID() );
			( (LightAttributeBuffer *)pbufferobj )->BindToShader( m_pInitMinMaxShadowMap->ID() );
		}
		else if( buffer_type == typeid(ShadowBuffer) )
		{
			ShadowBuffer *pshadowbuf = (ShadowBuffer *)pbufferobj;

			// Allocate 1D MinMaxShadowmap (カスケードシャドウ毎に、EpipolarSlice1セット分のMinMaxを保持する)
			m_1DMinMaxShadowMap.Allocate( Max( pshadowbuf->Width(), pshadowbuf->Height() ), m_EpipolarParam.NumEpipolarSlices * m_SliceUvOriginDirection.Height(), 0, 0, DATA_FORMAT::FORMAT_R16G16_FLOAT );
			m_1DMinMaxShadowMap.GenHardwareTexture();

			// Bind UniformBuffer to Shader
			pshadowbuf->BindToShader( m_pComputeSliceUvOriginDirection->ID() );
			pshadowbuf->BindToShader( m_pInitMinMaxShadowMap->ID() );

			// Bind Shadowmap texture to ComputeCoordinateTex Shader
			m_pComputeSliceUvOriginDirection->Bind();
			{
				glActiveTexture( g_DefaultTextureUnits[TEX_UNIT_SHADOWMAP] );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D_ARRAY, pshadowbuf->GetTexture()->texID ) );
				GL_SAFE_CALL( glUniform1i( glGetUniformLocation( m_pComputeCoordinateTexture->ID(), "g_ShadowMapArray" ), TEX_UNIT_SHADOWMAP ) );
			}
			m_pComputeSliceUvOriginDirection->Unbind();


			// Bind Shadowmap texture to InitMinMaxShadowMap Shader
			m_pInitMinMaxShadowMap->Bind();
			{
				glActiveTexture( g_DefaultTextureUnits[TEX_UNIT_SHADOWMAP] );
				GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D_ARRAY, pshadowbuf->GetTexture()->texID ) );
				GL_SAFE_CALL( glUniform1i( glGetUniformLocation( m_pComputeCoordinateTexture->ID(), "g_ShadowMapArray" ), TEX_UNIT_SHADOWMAP ));
			}
			m_pInitMinMaxShadowMap->Unbind();

		}

	}



	void LightShaftShader::UnbindBufferObject()
	{
		m_refLightAttrib = NULL;	
	}
	


	// メッシュデータをレンダリングする(シェーダーはバインド済みと想定)
	void LightShaftShader::Render( const Vec3f& wsLightPos, int lightID, const ViewTransformMatrix& matCameraTrans )
	{
		if ( m_refTexPosition == NULL || m_refTexDepth == NULL || m_refLightAttrib==NULL )
			return;

		
		UpdateEpipolarLightParam( wsLightPos, lightID, matCameraTrans );

		RenderSliceEndPoints();

		RenderCoordinateTexture();

		RefineSampleLocations();

		RenderSliceUVOriginAndDirection();

		InitMinMaxShadowMap();

		ComputeMinMaxMipmap();

		MarkRayMarchingSamples();

		DoRayMarching();
	}



	void LightShaftShader::InitSliceExitPoints( int dimx, int dimy )
	{

		//======================= 終点座標の計算 ========================//
		SafeDeleteArray( m_SliceExitPoints );

		m_EpipolarParam.NumEpipolarSlices	= 2 * ( dimx + dimy ) - 4;
		m_SliceExitPoints	= new Vec2f[ m_EpipolarParam.NumEpipolarSlices ];
		
		// (-1.0, -1.0) -> (+1.0, -1.0) -> (+1.0, +1.0) -> (-1.0, +1.0)の四つ角を設定する.
		const int	c0	= 0,
			c1	= dimx-1,
			c2	= c1 + dimy-1,
			c3	= c2 + dimx-1;
		InitVec( m_SliceExitPoints[ c0 ], -1.0f, -1.0f );
		InitVec( m_SliceExitPoints[ c1 ], +1.0f, -1.0f );
		InitVec( m_SliceExitPoints[ c2 ], +1.0f, +1.0f );
		InitVec( m_SliceExitPoints[ c3 ], -1.0f, +1.0f );

		// 4つ角の間のEndPoint(底辺と上辺)を設定する
		for( int i=1; i<dimx-1; ++i )
		{
			float percentage = float(i) / float(dimx-1);
			Lerp( m_SliceExitPoints[c0+i], m_SliceExitPoints[c0], m_SliceExitPoints[c1], percentage );// 底辺
			Lerp( m_SliceExitPoints[c2+i], m_SliceExitPoints[c2], m_SliceExitPoints[c3], percentage );// 上辺
		}

		// 4つ角の間のEndPoint(左右側辺)を設定する
		for( int i=1; i<dimy-1; ++i )
		{
			float percentage = float(i) / float(dimy-1);
			Lerp( m_SliceExitPoints[c1+i], m_SliceExitPoints[c1], m_SliceExitPoints[c2], percentage );// 左辺
			Lerp( m_SliceExitPoints[c3+i], m_SliceExitPoints[c3], m_SliceExitPoints[c0], percentage );// 右辺	
		}

	}



	void LightShaftShader::UpdateEpipolarLightParam( const Vec3f& wsLightPos, int lightID, const ViewTransformMatrix& matCameraTrans )
	{
		LightAttributeBuffer *refAttrib = (LightAttributeBuffer *)m_refLightAttrib;

		// 光源インデックスをセットする
		m_EpipolarLightParam.LightID = lightID;

		int numshadows = 0;
		int shadowIDList[MAX_SHADOW_CASCADES];
		refAttrib->GetShadowCasterIndexList( lightID, numshadows, shadowIDList );

		const Vec3f& wsLightPos_	= *refAttrib->GetPosition( lightID );


		//========================== スクリーン上の光源座標を計算する[-1,1] ==========================//
		// 光源がカメラの後方にあるかどうか調べる
		Vec4f vsLightPos;
		Multiply( vsLightPos, matCameraTrans.matView, /*wsLightPos*/wsLightPos_ );//光源座標をカメラ空間に変換する

		if( vsLightPos.z > 0.0 )// z値が正の場合は光源後方にある.->カメラ空間上の座標を正負反転する
		{
			vsLightPos.x *= -1.0f;
			vsLightPos.y *= -1.0f;
			vsLightPos.z *= -1.0f;
		}

		// スクリーン空間上の光源位置を計算する
		Vec4f ssLightPos;
		Multiply( ssLightPos, matCameraTrans.matProjection, vsLightPos );// カメラ空間上の光源座標に射影変換行列をかける

		// カメラ後方にある場合を加味して、光源位置を設定する
		InitVec( m_EpipolarLightParam.ScreenLightPos, ssLightPos.x / ssLightPos.w, ssLightPos.y / ssLightPos.w );
	


		// GPUにパラメータ情報を転送する
		m_uboEpipolarLightParam.UpdateSubresource( &m_EpipolarLightParam, sizeof( EpipolarLightParam ) );
	}



	//######################################################################################################//
	//							LightShaftShader class private member functions								//
	//######################################################################################################//

	void LightShaftShader::ReconstructCameraDepth()
	{


	}



	// epilolar line segmentの終端点(スライド31ページ)を計算する
	void LightShaftShader::RenderSliceEndPoints()
	{
		glBindImageTexture( 0, m_SliceEndPoints.texID, 0, GL_FALSE, 0, GL_WRITE_ONLY, m_SliceEndPoints.GLInternalFormat() );

		m_pComputeSliceEndPoints->Bind();
		{			
			glDispatchCompute( DivUp( m_EpipolarParam.NumEpipolarSlices, 16 ), 1, 1 ); // 512^2 threads in blocks of 16^2
			//checkErrors("Dispatch compute shader");

			glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );
		}
		m_pComputeSliceEndPoints->Unbind();
	}



	void LightShaftShader::RenderCoordinateTexture()
	{
		// Bind Output Image
		glBindImageTexture( TEX_UNIT_EPIPOLAR_COORDINATE, m_EpipolarCoordinate.texID, 0, GL_FALSE, 0, GL_WRITE_ONLY, m_EpipolarCoordinate.GLInternalFormat() );			//
		glBindImageTexture( TEX_UNIT_EPIPOLAR_CAM_SPACE_Z, m_EpipolarCameraSpaceZ.texID, 0, GL_FALSE, 0, GL_WRITE_ONLY, m_EpipolarCameraSpaceZ.GLInternalFormat() );	// 
		glBindImageTexture( TEX_UNIT_EPIPOLAR_DEPTH_STENCIL, m_EpipolarDepthStencil.texID, 0, GL_FALSE, 0, GL_WRITE_ONLY, m_EpipolarDepthStencil.GLInternalFormat() );	// Depth Stencil. 普通のシェーダー向き??
		
		// Execute Compute Shader
		m_pComputeCoordinateTexture->Bind();
		{
			// SliceEndPoints
			//glUniform1i( m_ulSliceEndPoints, TEX_UNIT_SLICE_END_POINTS );
			glActiveTexture( g_DefaultTextureUnits[ TEX_UNIT_SLICE_END_POINTS ] );
			glBindTexture( GL_TEXTURE_1D, m_SliceEndPoints.texID );
			
			//// Position Texture
			//glUniform1i( m_ulTexPosition, TEX_UNIT_POSITION );
			//glActiveTexture( g_DefaultTextureUnits[ TEX_UNIT_POSITION ] );
			//glBindTexture( GL_TEXTURE_2D, m_refTexPosition->texID );

			// Depth Texture
			//glUniform1i( m_ulTexDepth, TEX_UNIT_DEPTH );
			glActiveTexture( g_DefaultTextureUnits[ TEX_UNIT_DEPTH ] );
			glBindTexture( GL_TEXTURE_2D, m_refTexDepth->texID );

			// カーネル実行 ( num_groups_X: numSamplesInSlice dimension, num_groups_Y: numEpipolarSlices dimension )
			glDispatchCompute( DivUp( m_EpipolarParam.NumSamplesInSlice, 16 ), DivUp( m_EpipolarParam.NumEpipolarSlices, 16 ), 1 );


			glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );


			// SliceEndPoints
			glActiveTexture( g_DefaultTextureUnits[ TEX_UNIT_SLICE_END_POINTS ] );
			glBindTexture( GL_TEXTURE_1D, 0 );
			
			//// Position Texture
			//glActiveTexture( g_DefaultTextureUnits[ TEX_UNIT_POSITION ] );
			//glBindTexture( GL_TEXTURE_2D, m_refTexPosition->texID );

			// Depth Texture
			glActiveTexture( g_DefaultTextureUnits[ TEX_UNIT_DEPTH ] );
			glBindTexture( GL_TEXTURE_2D, 0 );

		}
		m_pComputeCoordinateTexture->Unbind();

	}


	// TODO: デバッグ. どうやって確認する? 2014.10.23
	void LightShaftShader::RefineSampleLocations()
	{
		// Bind Input Image
		glBindImageTexture( TEX_UNIT_EPIPOLAR_COORDINATE, m_EpipolarCoordinate.texID, 0, GL_FALSE, 0, GL_READ_ONLY, m_EpipolarCoordinate.GLInternalFormat() );
		glBindImageTexture( TEX_UNIT_EPIPOLAR_CAM_SPACE_Z, m_EpipolarCameraSpaceZ.texID, 0, GL_FALSE, 0, GL_READ_ONLY, m_EpipolarCameraSpaceZ.GLInternalFormat() );

		// Bind Output Image
		glBindImageTexture( 0, m_InterpolationSource.texID, 0, GL_FALSE, 0, GL_WRITE_ONLY, m_InterpolationSource.GLInternalFormat() );

		
		m_pRefineSampleLocations->Bind();
		{			
			// カーネル実行 ( num_groups_X: numSamplesInSlice dimension, num_groups_Y: numEpipolarSlices dimension )
			glDispatchCompute( DivUp( m_EpipolarParam.NumSamplesInSlice, m_SampleRefinementThreadGroupSize ), m_EpipolarParam.NumEpipolarSlices, 1 );
			glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );
		}
		m_pRefineSampleLocations->Unbind();
		
	}



	void LightShaftShader::RenderSliceUVOriginAndDirection()
	{
		// Bind Input Image
		glBindImageTexture( TEX_UNIT_SLICE_END_POINTS, m_SliceEndPoints.texID, 0, GL_FALSE, 0, GL_READ_ONLY, m_SliceEndPoints.GLInternalFormat() );
		//glBindImageTexture( TEX_UNIT_EPIPOLAR_COORDINATE, m_EpipolarCoordinate.texID, 0, GL_FALSE, 0, GL_READ_ONLY, m_EpipolarCoordinate.GLInternalFormat() );

		// Bind Output Image
		glBindImageTexture( 3, m_SliceUvOriginDirection.texID, 0, GL_FALSE, 0, GL_WRITE_ONLY, m_SliceUvOriginDirection.GLInternalFormat() );
		
		// Execute ComputeShader
		m_pComputeSliceUvOriginDirection->Bind();
		{
			// ( num_groups_X: numEpipolarSlices dimension, num_groups_Y: numCascades dimension )
			glDispatchCompute( DivUp( m_SliceUvOriginDirection.Width(), 16 ), DivUp( m_SliceUvOriginDirection.Height(), 16 ), 1 );
			glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );
		}
		m_pComputeSliceUvOriginDirection->Unbind();

	}



	void LightShaftShader::InitMinMaxShadowMap()
	{
		// Bind Input Image
		glBindImageTexture( 5, m_SliceUvOriginDirection.texID, 0, GL_FALSE, 0, GL_READ_ONLY, m_SliceUvOriginDirection.GLInternalFormat() );

		// Bind Output Image
		glBindImageTexture( 1, m_1DMinMaxShadowMap.texID, 0, GL_FALSE, 0, GL_WRITE_ONLY, m_1DMinMaxShadowMap.GLInternalFormat() );

		// Execute Compute Shader
		m_pInitMinMaxShadowMap->Bind();
		{
			// ( num_groups_X: shadowmapresolution, num_groups_Y: numEpipolarSlices * numcascades )
			glDispatchCompute( DivUp( m_1DMinMaxShadowMap.Width(), 32 ), DivUp( m_1DMinMaxShadowMap.Height(), 32 ), 1 );
			//glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );
		}
		m_pInitMinMaxShadowMap->Unbind();
	
	}



	void LightShaftShader::ComputeMinMaxMipmap()
	{
		// Bind Input/Output Image
		glBindImageTexture( 0, m_1DMinMaxShadowMap.texID, 0, GL_FALSE, 0, GL_READ_WRITE, m_1DMinMaxShadowMap.GLInternalFormat() );


		// Execute Compute Shader
		m_pCompute1DMinMaxMipmap->Bind();
		{
			int sampleOffset	= 0;							// ダウンサンプリングに使う画素の開始位置
			int sampleRange		= m_1DMinMaxShadowMap.Width() / 2;// ダウンサンプリングに使う画素データの範囲
			int storeOffset		= sampleRange;				// 書き込み開始位置
			int storeRange		= storeOffset / 2;
	
//=============== 1DMinMaxShadowMapのレンジとオフセット ==============//
//																	//
//	sampleOffset		            storeOffset						//
//		----*---------------------------x-------------------+		//
//			|  <---- sampleRange ---->  | <- storeRange ->	|		//
//			|							|					|		//
//			|							|					|		//
//			|							|					|		//
//		----+---------------------------+-------------------+		//
//																	//
//==================================================================//

			for( int i=0; i < m_numMipMapLevels; ++i )
			{
				glUniform1i( m_ulMinMaxSampleOffset, sampleOffset );
				glUniform1i( m_ulMinMaxStoreOffset, storeOffset );
				
				// ( num_groups_X: shadowmapresolution, num_groups_Y: numEpipolarSlices * numcascades )
				glDispatchCompute( DivUp( storeRange, 16 ), DivUp( m_1DMinMaxShadowMap.Height(), 16 ), 1 );

				sampleOffset	+= sampleRange;
				sampleRange		/= 2;
				storeOffset		+= storeRange;
				storeRange		/= 2;
			}
		}
		m_pCompute1DMinMaxMipmap->Unbind();


		// Execute Compute Shader (Alternative code. calculate all mip levels at once. )
		//m_pCompute1DMinMaxMipmap->Bind();
		//{
		//	int levelOffset		= m_MinMaxShadowMap.Width() / 2;// 書き込み開始位置
		//	
		//	for( int i=0; i < 1; ++i )
		//	{
		//		glUniform1i( m_ulMinMaxLevelOffset, levelOffset );//m_MinMaxShadowMap.Width() / 2 );

		//		// ( num_groups_X: shadowmapresolution, num_groups_Y: numEpipolarSlices )
		//		glDispatchCompute( DivUp( levelOffset, 16 ), DivUp( m_MinMaxShadowMap.Height(), 16 ), 1 );
		//	}
		//}
		//m_pCompute1DMinMaxMipmap->Unbind();
		
	}



	void LightShaftShader::MarkRayMarchingSamples()
	{
		// Bind Input Image
		glBindImageTexture( 0, m_InterpolationSource.texID, 0, GL_FALSE, 0, GL_READ_ONLY, m_InterpolationSource.GLInternalFormat() );

		// Bind Output Image
		glBindImageTexture( 1, m_EpipolarDepthStencil.texID, 0, GL_FALSE, 0, GL_READ_WRITE, m_EpipolarDepthStencil.GLInternalFormat() );

		// Execute Compute Shader
		m_pMarkRayMarchingSamples->Bind();
		{
			// カーネル実行 ( num_groups_X: numSamplesInSlice dimension, num_groups_Y: numEpipolarSlices dimension )
			glDispatchCompute( DivUp( m_EpipolarDepthStencil.Width(), 16 ), DivUp( m_EpipolarDepthStencil.Height(), 16 ), 1 );
		
		}
		m_pMarkRayMarchingSamples->Unbind();

	}


	void LightShaftShader::DoRayMarching()
	{
		// Bind Input Image
		glBindImageTexture( 0, m_EpipolarCoordinate.texID, 0, GL_FALSE, 0, GL_READ_ONLY, m_EpipolarCoordinate.GLInternalFormat() );
		glBindImageTexture( 1, m_EpipolarCameraSpaceZ.texID, 0, GL_FALSE, 0, GL_READ_ONLY, m_EpipolarCameraSpaceZ.GLInternalFormat() );
		glBindImageTexture( 2, m_SliceUvOriginDirection.texID, 0, GL_FALSE, 0, GL_READ_ONLY, m_SliceUvOriginDirection.GLInternalFormat() );
		glBindImageTexture( 3, m_1DMinMaxShadowMap.texID, 0, GL_FALSE, 0, GL_READ_ONLY, m_1DMinMaxShadowMap.GLInternalFormat() );
		glBindImageTexture( 4, m_EpipolarDepthStencil.texID, 0, GL_FALSE, 0, GL_READ_ONLY, m_EpipolarDepthStencil.GLInternalFormat() );

		// Bind Output Image
		glBindImageTexture( 5, m_InitialInscattering.texID, 0, GL_FALSE, 0, GL_WRITE_ONLY, m_InitialInscattering.GLInternalFormat() );

		// Execute Compute Shader
		m_pDoRayMarching->Bind();
		{
			// カーネル実行 ( num_groups_X: numSamplesInSlice dimension, num_groups_Y: numEpipolarSlices dimension )
			glDispatchCompute( DivUp( m_InitialInscattering.Width(), 16 ), DivUp( m_InitialInscattering.Height(), 16 ), 1 );
		}
		m_pDoRayMarching->Unbind();
	
	}



	void LightShaftShader::InterpolateInscattering()
	{
		// Bind Input Image
		glBindImageTexture( 0, m_InterpolationSource.texID, 0, GL_FALSE, 0, GL_READ_ONLY, m_InterpolationSource.GLInternalFormat() );
		glBindImageTexture( 1, m_InitialInscattering.texID, 0, GL_FALSE, 0, GL_READ_ONLY, m_InitialInscattering.GLInternalFormat() );

		// Bind Output Image
		glBindImageTexture( 2, m_InterpolatedInscattering.texID, 0, GL_FALSE, 0, GL_WRITE_ONLY, m_InterpolatedInscattering.GLInternalFormat() );

		// Execute Compute Shader
		m_pDoRayMarching->Bind();
		{
			// カーネル実行 ( num_groups_X: numSamplesInSlice dimension, num_groups_Y: numEpipolarSlices dimension )
			glDispatchCompute( DivUp( m_InterpolatedInscattering.Width(), 16 ), DivUp( m_InterpolatedInscattering.Height(), 16 ), 1 );
		}
		m_pDoRayMarching->Unbind();

	}





}// end of namespace