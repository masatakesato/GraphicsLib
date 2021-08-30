#ifndef	RAMP_SHADER_H
#define	RAMP_SHADER_H


#include	"IShader.h"
#include	"GLVertexArrayObject.h"


namespace OreOreLib
{


	class RampShader : public IShader
	{
	public:

		RampShader();
		RampShader( const TCHAR *filepath, GLSL_VERSION version );
		~RampShader();

		void InitShader( const TCHAR *filepath, GLSL_VERSION version );
		void Release();


		void Render( Texture1D *texRamp, Texture2D *texScalar, float scale=1.0f );


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
		GLint		m_ulTexRamp;	// Ramp Texture
		GLint		m_ulTexScalar;	// Scalar Texture
		GLint		m_ulScale;		// scale;

	};


	class RampShaderFactory : IShaderFactory
	{
	protected:

		IShader* Create( GLSL_VERSION version ){ return new RampShader( _T( "RampShader.glsl" ), version ); }

	public:

		virtual ~RampShaderFactory(){}
		IShader* CreateInstance( GLSL_VERSION version );

	};


}// end of namespace


#endif	// RAMP_SHADER_H //