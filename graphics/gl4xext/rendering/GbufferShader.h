#ifndef	GBUFFER_SHADER_H
#define	GBUFFER_SHADER_H



#include	"IShader.h"
#include	"GLFrameBufferObject.h"


namespace OreOreLib
{


	class GbufferShader : public IShader
	{
	public:

		GbufferShader();
		GbufferShader( const TCHAR *filepath, GLSL_VERSION version );
		~GbufferShader();

		bool InitShader( const TCHAR *filepath, GLSL_VERSION version );
		
		inline void Bind(){ m_Shader.Bind(); };
		inline void Unbind(){ m_Shader.Unbind(); };

		// Override Virtual Functions
		inline virtual int NumPasses() const	{ return 1; }
		inline virtual GLuint GetPassID( int shader_idx=0 ) const{ return m_Shader.ID(); }

		virtual void BindShader( int shader_idx=0 )	// シェーダーをバインドする
		{
			if( m_pCurrShader == &m_Shader )	return;
			m_pCurrShader = &m_Shader;
			m_pCurrShader->Bind();
		}


		inline virtual void UnbindShader()
		{
			if( !m_pCurrShader )	return;
			m_pCurrShader->Unbind();
			m_pCurrShader	= NULL;
		}

		virtual void BindBufferObject( const IBufferObject* pbufferobj );
		virtual void UnbindBufferObject(){}
		virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat );


	private:

		GLShader	m_Shader;


		// Uniform Location
		GLint	m_ulMatWorld;			// world matrix
		GLint	m_ulDiffuse;			// diffuse

		//GLFrameBufferObject	*m_refFBO;//FramebufferObject	*m_refFBO;	// 参照

		enum FRAG_LOCATION
		{
			FRAG_POSITION,
			FRAG_NORMAL,
			FRAG_DEPTH,
			FRAG_COLOR,
			NUM_FRAG
		};


	};





	class GbufferShaderFactory : IShaderFactory
	{
	protected:

		IShader* Create( GLSL_VERSION version ){ return new GbufferShader( _T( "GbufferShader.glsl" ), version ); }

	public:

		virtual ~GbufferShaderFactory(){}
		IShader* CreateInstance( GLSL_VERSION version );

	};



}// end of namespace


#endif	// GBUFFER_SHADER_H //