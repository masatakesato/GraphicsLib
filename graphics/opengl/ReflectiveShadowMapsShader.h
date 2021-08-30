#ifndef	REFLECTIVE_SHADOWMAPS_SHADER_H
#define	REFLECTIVE_SHADOWMAPS_SHADER_H



#include	"IShader.h"
#include	"ViewFrustum.h"
#include	"ShadowCaster.h"
#include	"GLUniformBufferObject.h"



namespace OreOreLib
{

	// ReflectiveShadowMapsShader描画クラス
	class ReflectiveShadowMapsShader : public IShader
	{
	public:

		ReflectiveShadowMapsShader();
		ReflectiveShadowMapsShader( const TCHAR* filepath, GLSL_VERSION version );
		~ReflectiveShadowMapsShader();

		void InitShader( const TCHAR* filepath, GLSL_VERSION version );

		void SetLightID( int light_id );
		void SetRsmSize( int width, int height )				{ InitVec( m_RsmSize, width, height ); }
		
		//void BindFramebufferObject( GLFrameBufferObject* pfbo )	{ m_refFbo = pfbo; InitVec( m_RsmSize, m_refFbo->Width(), m_refFbo->Height() ); }
		//void UnbindFramebufferObject()							{ m_refFbo = NULL; InitZero( m_RsmSize ); }
		
		void Render( const GeometryBuffer *pGeom, Frustum *pfrustum );


		// Override Virtual Functions
		virtual int NumPasses() const							{ return NUM_RENDER_PASS; }
		virtual GLuint GetPassID( int shader_idx=0 ) const		{ return (m_Pass + shader_idx)->ID(); }

		inline virtual void BindShader( int passid=0 )
		{
			GLShader *psahder	= m_Pass + m_ProjectionType;
			//if( CurrentShader == psahder )	return;
			m_pCurrShader = psahder;
			m_pCurrShader->Bind();
		}
		
		inline virtual void UnbindShader()
		{
			if( m_pCurrShader ) m_pCurrShader->Unbind();
			m_pCurrShader = NULL;
		}

		virtual void BindBufferObject( const IBufferObject* pbufferobj );
		virtual void UnbindBufferObject(){}

		virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat ){}


	private:

		// TODO: UniformBufferObject本格運用までのつなぎ
		struct LightParams
		{
			GLint	ulMatWorld;
			GLint	ulMatLight;
			GLint	ulRsmSize;
			GLint	ulLightID;
			GLint	ulTexelCoeff;

			//GLint	ulViewPos;
			//GLint	ulLightIntensity;	// TODO: LightAttributeBufferバインドして廃止
			//GLint	ulLightPos;			// TODO: LightAttributeBufferバインドして廃止

			GLint	ulDiffuse;			// diffuse material TODO: Support all material attributes
		};

		enum RENDER_PASS{ RENDER_PASS_PERSPECTIVE, RENDER_PASS_ORTHOGRAPHIC, NUM_RENDER_PASS };// 一応Perspective/Orthographicの両方に対応
		enum FRAG_LOCATION{ FRAG_LOCATION_POSITION, FRAG_LOCATION_NORMAL, FRAG_LOCATION_FLUX, FRAG_LOCATION_DEPTH, NUM_FRAG_LOCATION };// RSMの出力チャンネル

		
		// Shader
		int						m_ProjectionType;	// 光源タイプ
		GLShader				m_Pass[ NUM_RENDER_PASS ];		// シェーダー(今のところ2種類)

		// Uniform Locations
		LightParams				m_Param[ NUM_RENDER_PASS ];

		//GLFrameBufferObject		*m_refFbo;
		
		Vec2i					m_RsmSize;

	};






	// ReflectiveShadowMapsShaderオブジェクトを生成するファクトリクラス
	class ReflectiveShadowMapsShaderFactory : IShaderFactory
	{
	protected:

		IShader* Create( GLSL_VERSION version ){ return new  ReflectiveShadowMapsShader( _T( "ReflectiveShadowMapsShader.glsl" ), version ); }

	public:

		virtual ~ReflectiveShadowMapsShaderFactory(){}
		IShader* CreateInstance( GLSL_VERSION version );

	};





}// end of namespace




#endif	// REFLECTIVE_SHADOWMAPS_SHADER_H //