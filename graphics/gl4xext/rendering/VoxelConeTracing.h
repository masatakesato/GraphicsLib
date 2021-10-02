#ifndef	VOXEL_CONE_TRACING_H
#define	VOXEL_CONE_TRACING_H


#include	<graphics/gl4x/shader/IShader.h>
#include	<graphics/gl4x/shader/GLComputeShader.h>
#include	<graphics/gl4x/resource/GLFrameBufferObject.h>
#include	<graphics/gl4x/scene/MovableObject.h>

#include	"VCTVoxelData.h"



namespace OreOreLib
{

	// Voxel Cone Tracing
	class VoxelConeTracing : public IShader
	{
	public:

		VoxelConeTracing();
		VoxelConeTracing( const TCHAR* filepath, GLSL_VERSION version );
		~VoxelConeTracing();

		void InitShader( const TCHAR* filepath, GLSL_VERSION version );
		bool InitVPLSprites( int vpl_width, int vpl_height );	// 仮想点光源スプライトの初期化
		void Release();
		void ClearVoxels();

		// ボクセルデータのバインド/アンバインド
		void BindVCTVoxelData( VCTVoxelData *vct_voxel );
		void UnbindVCTVoxelData();

		// ReflectiveShadowMapsのバインド/アンバインド
		void BindRSMBuffer( Texture2D* rsm_pos, Texture2D* rsm_normal, Texture2D* rsm_flux, Texture2D* rsm_depth );
		void UnbindRSMBuffer();
		
		void Update( int numObjects, const MovableObject *renderQueue[] );
		//void PreviewVoxel( const Texture2D* pTexWorldPos, const Texture2D* pTexWorldNormal );
		void TraceVoxel( const Texture2D* pTexWorldPos, const Texture2D* pTexWorldNormal, Texture2D* pOutIrradiance=NULL );
		

		// Override Virtual Functions
		inline virtual int NumPasses() const	{ return 1; }
		inline virtual GLuint GetPassID( int shader_idx=0 ) const{ return GL_INVALID_INDEX; }

		inline virtual void BindShader( int passid=0 )
		{
			if( m_pCurrShader == &m_Pass[passid] )	return;

			m_pCurrShader = &m_Pass[passid];
			m_pCurrShader->Bind();
		}

		virtual void BindBufferObject( const IBufferObject* pbufferobj );
		virtual void UnbindBufferObject(){}

		virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat ){};


	private:

		enum PASS { VOXELIZATION, INJECTION, TRACE_VOXEL/*, PREVIEW_VOXEL*/, NUM_PASS };

		// Shader
		GLShader			m_Pass[ NUM_PASS ];
GLComputeShader		m_CreateSurfaceVoxelList;
GLComputeShader		m_PerVoxelLightInjection;
GLComputeShader		m_SetupAnisotropicVoxels;


		GLComputeShader		m_ComputeDownSample;
		GLComputeShader		m_ComputeDownSampleAniso[6];


		// Uniform Buffer Objects
		GLUniformBufferObject	m_uboVoxelAttribute;
		GLUniformBufferObject	m_uboVoxelTransform;

		// Bind Points
		static GLuint			m_BindPoints[2];


		// Uniform Locations for Voxelization Pass
		GLint		m_ulMatWorld;// world matrix
		GLint		m_ulViewportResolution;
		GLint		m_ulDiffuse;	// Diffuse Color
		GLint		m_ulSpecular;	// Specular Color
		GLint		m_ulShininess;	// Shininess
		GLint		m_ulOpacity;	// Opacity

		// Uniform Locations for Injection Pass
		GLint		m_ulVPLAreaScale;// RSMテクスチャ1テクセルの面積に対する、VPL面積の相対比(例：RSM:256^2, VPL:128^2なら，4.0)
	
// Uniform Locations for m_CreateSurfaceVoxelList Shader
GLuint						m_biSurfaceVoxelsOutput;	// ssbo block index for surface voxel index array

// Uniform Locations for m_PerVoxelLightInjection Shader
GLuint						m_biSurfaceVoxelsInput;		// ssbo block index for surface voxel index array


		// Framebuffer Objects
		GLFrameBufferObject		m_Fbo;
		FrameBufferAttachment	m_FboAttachment;
		FrameBufferAttachment	m_FboAttachment2;


		enum INPUT_TEXTURE
		{
			TEX_POSITION,
			TEX_NORMAL,
			TEX_COLOR,
			TEX_IRRADIANCE,
			NUM_INPUT_TEXTURE,
		};

		enum TEXUNIT_RSM
		{
			RSM_POSITION	= 0,
			RSM_NORMAL		= 1,
			RSM_FLUX		= 2,
			RSM_DEPTH		= 3
		};// RSMの出力チャンネル


		enum TEXUNIT_TRACE
		{
			TEX_TRACE_POSITION,
			TEX_TRACE_NORMAL,
			TEX_TRACE_VOXEL,
		};


		// Reference to Voxel Data
		VCTVoxelData			*m_refVCTVoxel;


		// Reference to Reflective Shadow Maps
		Texture2D				*m_refRsmPositionBuffer;
		Texture2D				*m_refRsmNormalBuffer;
		Texture2D				*m_refRsmFluxBuffer;
		Texture2D				*m_refRsmDepthBuffer;


		// Virtual Point Ligths for Reflective Shadow Map Projection
		Vec2i					m_VplSize;	// 仮想点光源の解像度
		std::vector <Vec2f>		m_VPLSprites;	// 仮想点光源(2次元)スプライト(0.0, 0.0) - (1.0, 1.0)
		GLVertexArrayObject		m_vaoVPLSprites;

		void VoxelizeScene( int numObjects, const MovableObject *renderQueue[] );
		void InjectRSM();
		void DownSampleAniso();//void DownSample();

		void CreateSurfaceVoxelList();// シェーディング対象ボクセルを抽出する
		void PerVoxelLightInjection();
		void SetupAnisotropicVoxels();


	};




	// VoxelConeTracingオブジェクトを生成するファクトリクラス
	class VoxelConeTracingFactory : IShaderFactory
	{
	protected:

		IShader* Create( GLSL_VERSION version ){ return new VoxelConeTracing( _T( "" ), version ); }

	public:

		virtual ~VoxelConeTracingFactory(){}
		IShader* CreateInstance( GLSL_VERSION version );
	};



}// end of namespace



#endif	// VOXEL_CONE_TRACING_H //