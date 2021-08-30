#ifndef	RENDER_SHADOW_H
#define	RENDER_SHADOW_H



#include	"IShader.h"
#include	"ShadowCaster.h"


namespace OreOreLib
{

	// TODO: 射影変換の種類でシェーダーを切り替えるようにする
	class RenderShadow : public IShader
	{

	public:

		RenderShadow();
		RenderShadow( const TCHAR *filepath, GLSL_VERSION version );
		~RenderShadow();

		bool InitShader( const TCHAR *filepath, GLSL_VERSION version );

		void SetProjectionType( LIGHT_TYPE ltype )	{ m_ProjectionType	= ltype==LIGHT_TYPE_DIRECTIONAL ? PROJECION_TYPE_ORTHOGRAPHIC : PROJECION_TYPE_PERSPECTIVE; }
		void SetMatShadowIndex( int i )				{ glUniform1i( m_ulParam[ m_ProjectionType ].ulShadowIndex, i ); }//m_ulShadowIndex, i ); }
		void BindShadowCaster( Frustum *pshadowcaster );
		
		inline void Bind()	{ (m_Pass + m_ProjectionType)->Bind(); }//m_Shader.Bind();   };
		inline void Unbind(){ (m_Pass + m_ProjectionType)->Unbind(); }//m_Shader.Unbind(); };

		// Override Virtual Functions
		inline virtual int NumPasses() const	{ return 1; }
		inline virtual GLuint GetPassID( int shader_idx=0 ) const{ return (m_Pass + shader_idx)->ID(); }//m_Shader.ID(); }
		
		inline virtual void BindShader( int passid=0 )
		{
			GLShader *psahder	= m_Pass + m_ProjectionType;

			if( m_pCurrShader == psahder )	return;

			m_pCurrShader = psahder;
			m_pCurrShader->Bind();
		}
		
		virtual void BindBufferObject( const IBufferObject* pbufferobj );
		virtual void UnbindBufferObject(){}

		virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat );
		

	private:

		struct ulParam
		{
			GLint	ulMatWorld;	// world matrix
			GLint	ulMatLight;	// view projection matrix of lightsource
			GLint	ulShadowIndex;
		};


		// GLShader
		GLShader		m_Pass[ 2 ];// rendershadow passes for perspective and orthographic projections

		// Uniform Locations
		ulParam			m_ulParam[ 2 ];// uniform locations for each m_Pass

		// Projection type
		PROJECTION_TYPE	m_ProjectionType;

		// Fragment Output  Location
		enum FRAG_LOCATION{ FRAG_DEPTH, FRAG_POSITION, FRAG_NORMAL, FRAG_FLUX };
	};





	class RenderShadowFactory : IShaderFactory
	{
	protected:

		IShader* Create( GLSL_VERSION version ){ return new RenderShadow( _T( "RenderShadowPerspective.glsl" ), version ); };

	public:

		virtual ~RenderShadowFactory(){};
		IShader* CreateInstance( GLSL_VERSION version );

	};






}


#endif	// RENDER_SHADOW_H //