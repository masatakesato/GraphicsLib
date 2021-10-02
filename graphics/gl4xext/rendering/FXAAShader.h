#ifndef	FXAA_SHADER_H
#define	FXAA_SHADER_H

#include	<graphics/gl4x/shader/IShader.h>



namespace OreOreLib
{


	class FXAAShader : public IShader
	{
	private:

		GLShader	*m_pShader;

		GLint		m_ulTexSize;
		GLint		m_ulTexture;


	public:

		FXAAShader();
		FXAAShader( const TCHAR *filepath, GLSL_VERSION version );
		~FXAAShader();

		void InitShader( const TCHAR *filepath, GLSL_VERSION version );

		void Render( const Texture2D *tex );


		// Override Virtual Functions
		inline virtual int NumPasses() const	{ return 1; }
		inline virtual GLuint GetPassID( int shader_idx=0 ) const{ return m_pShader->ID(); }

		inline virtual void BindShader( int shader_idx=0 )
		{
			if( m_pCurrShader == m_pShader )	return;
			m_pCurrShader = m_pShader;
			m_pCurrShader->Bind();
		}

		virtual void BindBufferObject( const IBufferObject* pbufferobj ){}
		virtual void UnbindBufferObject(){}

		virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat ){}

	};





	class FXAAShaderFactory : IShaderFactory
	{
	protected:

		IShader* Create( GLSL_VERSION version ){ return new FXAAShader( _T( "FXAAShader.glsl" ), version ); }

	public:

		virtual ~FXAAShaderFactory(){}
		IShader* CreateInstance( GLSL_VERSION version );

	};








}// end of namespace


#endif	// FXAA_SHADER_H //