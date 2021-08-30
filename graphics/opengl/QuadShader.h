#ifndef	QUAD_SHADER_H
#define	QUAD_SHADER_H


#include	"IShader.h"
#include	"GLVertexArrayObject.h"


namespace OreOreLib
{


	class QuadShader : public IShader
	{
	public:

		QuadShader();
		QuadShader( const TCHAR *filepath, GLSL_VERSION version );
		~QuadShader();

		void InitShader( const TCHAR *filepath, GLSL_VERSION version );
		void Release();


		void Render( uint32 texid, float scale=1.0f );
		void Render( Texture1D *tex1d, float scale=1.0f );
		void Render( Texture2D *tex2d, float scale=1.0f );

		// Override Virtual Functions
		inline virtual int NumPasses() const	{ return 1; }
		inline virtual GLuint GetPassID( int shader_idx=0 ) const{ return m_pShader->ID(); }

		inline virtual void BindShader( int shader_idx=0 )	// シェーダーをバインドする
		{
			if( m_pCurrShader == m_pShader )	return;
			m_pCurrShader = m_pShader;
			m_pCurrShader->Bind();
		}

		virtual void BindBufferObject(const IBufferObject* pbufferobj){}
		virtual void UnbindBufferObject(){}

		virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat ){}


	private:

		GLShader	*m_pShader;
		GLint		m_ulTexture1D;	// 1DTexture
		GLint		m_ulTexture2D;	// 2DTexture
		GLint		m_ulScale;		// scale;
		GLint		m_ulMode;		// Texture1D/2D

	};


	class QuadShaderFactory : IShaderFactory
	{
	protected:

		IShader* Create( GLSL_VERSION version ){ return new QuadShader( _T( "QuadShader.glsl" ), version ); }

	public:

		virtual ~QuadShaderFactory(){}
		IShader* CreateInstance( GLSL_VERSION version );

	};


}// end of namespace


#endif	// QUAD_SHADER_H //