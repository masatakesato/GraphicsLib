#ifndef	DOF_SHADER_H
#define	DOF_SHADER_H


#include	"IShader.h"
#include	"GLShader.h"


namespace OreOreLib
{


	class DOFShader : public IShader
	{	
	public:

		DOFShader();
		DOFShader( const TCHAR* filepath, GLSL_VERSION version );
		~DOFShader();

		void InitShader( const TCHAR* filepath, GLSL_VERSION version );
		void BindInputTexture( Texture2D* texrendered, Texture2D* texdepth );
		void UnbindInputTexture();
		void Release();
		
		void Render();

		// Override Virtual Functions
		inline virtual int NumPasses() const						{ return 1; }
		inline virtual GLuint GetPassID( int shader_idx=0 ) const	{ return m_pShader->ID(); }

		inline virtual void BindShader( int passid=0 )
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
		GLShader	*m_pShader;	// デフォルトのシェーダー

		// uniform location	
		GLint		m_ulTexRendered;// Depth Texture
		GLint		m_ulTexDepth;	// Rendered Image
		
		// reference textures
		Texture2D	*m_refTexRendered;
		Texture2D	*m_refTexDepth;

		enum TEX_SLOT{ TEX_SLOT_RENDERED, TEX_SLOT_DEPTH, NUM_TEX_SLOT };

	};



	// DOFShaderオブジェクトを生成するファクトリクラス
	class DOFShaderFactory : IShaderFactory
	{
	protected:

		IShader* Create( GLSL_VERSION version ){ return new DOFShader( _T( "BokehShader.glsl" ), version ); }

	public:

		virtual ~DOFShaderFactory(){}
		IShader* CreateInstance( GLSL_VERSION version );

	};



}// end of namesapce



#endif	// DOF_SHADER_H //