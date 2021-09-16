#ifndef	SMAA_SHADER_H
#define	SMAA_SHADER_H


#include	"IShader.h"
#include	"GLShader.h"
#include	"GLRenderTarget.h"


namespace OreOreLib
{


	// SMAA Shader
	class SMAAShader : public IShader
	{
	public:

		SMAAShader();
		SMAAShader( const TCHAR* filepath, GLSL_VERSION version );
		~SMAAShader();

		void InitShader( const TCHAR* filepath, GLSL_VERSION version );
		void Release();
		
		void BindInputTexture( Texture2D* reftex );
		void UnbindInputTexture();

		void Render( const Texture2D *tex, float scale=1.0f );

		const Texture2D* const GetAreaTexture() const	{ return (Texture2D *)&m_TexSmaaArea; }
		const Texture2D* const GetSearchTexture() const	{ return (Texture2D *)&m_TexSmaaSearch; }
		const Texture2D* const GetTexAA() const			{ return (Texture2D *)&m_TexAA; }
		const Texture2D* const GetTexSmaaWk() const		{ return (Texture2D *)&m_TexSmaaWk; }


		// Override Virtual Functions
		inline virtual int NumPasses() const	{ return 1; }
		inline virtual GLuint GetPassID( int shader_idx=0 ) const{ return m_pShader->ID(); }

		inline virtual void BindShader( int shader_idx=0 )	// シェーダーをバインドする
		{
			if( m_pCurrShader == m_pShader )	return;
			m_pCurrShader = m_pShader;
			m_pCurrShader->Bind();
		}

		virtual void BindBufferObject( const IBufferObject* pbufferobj ){}
		virtual void UnbindBufferObject(){}

		virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat ){}
		

	private:

		// shader
		GLShader				*m_pShader;	// デフォルトのシェーダー

		// uniform location	
		GLint					m_ulPass;
		GLint					m_ulInpTex;
		GLint					m_ulInpTex2;
		GLint					m_ulInpTex3;

		// sampler state object
//		GLuint					m_Sampler;

		// FramebufferObject
		GLRenderTarget			m_RenderTarget;
		//FrameBufferAttachment	m_Attachment;
		//GLFrameBufferObject		m_Fbo;


		// Non Anti-Aliased Input Image 
		Texture2D				*m_refTexInput;

		// Internal Buffer for SMAA Calculation
		Texture2D				m_TexSmaaArea;
		Texture2D				m_TexSmaaSearch;

		Texture2D				m_TexAA;
		Texture2D				m_TexSmaaWk;

	};

	

	// SMAAShaderオブジェクトを生成するファクトリクラス
	class SMAAShaderFactory : IShaderFactory
	{
	protected:

		IShader* Create( GLSL_VERSION version ){ return new SMAAShader( _T( "SMAAShader.glsl" ), version ); }

	public:

		virtual ~SMAAShaderFactory(){}
		IShader* CreateInstance( GLSL_VERSION version );

	};



}// end of namespace




#endif	// SMAA_SHADER_H //