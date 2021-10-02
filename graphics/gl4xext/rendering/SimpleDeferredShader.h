#ifndef	SIMPLE_DEFERRED_LIGHTING_SHADER_H
#define	SIMPLE_DEFERRED_LIGHTING_SHADER_H

#include	<graphics/gl4x/shader/IShader.h>
#include	<graphics/gl4x/shader/GLShader.h>



namespace OreOreLib
{

	class SimpleDeferredShader : public IShader
	{	
	public:
		
		SimpleDeferredShader();
		SimpleDeferredShader( const TCHAR* filepath, GLSL_VERSION version );
		~SimpleDeferredShader();

		void InitShader( const TCHAR* filepath, GLSL_VERSION version );
		void Release();

		void SetIrrasianceMapMode( bool useIrrad )	{ m_bUseIrradianceMap = useIrrad; }
		void Render( const Texture2D *texDiff, const Texture2D *texPos, const Texture2D *texDepth, const Texture2D *texNormal, const Texture2D *texIrrad );
		
		// Override Virtual Functions
		inline virtual int NumPasses() const						{ return 1; }
		inline virtual GLuint GetPassID( int shader_idx=0 ) const	{ return m_pShader->ID(); }


		inline virtual void BindShader( int passid=0 )
		{
			if( m_pCurrShader == m_pShader )	return;
			m_pCurrShader = m_pShader;
			m_pCurrShader->Bind();
		}

		virtual void BindBufferObject( const IBufferObject* pbufferobj );
		virtual void UnbindBufferObject(){}

		virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat ){}


	private:

		// shader
		GLShader	*m_pShader;	// デフォルトのシェーダー

		// uniform location
		GLint		m_ulTexDiffuse;	// Diffuse Gbuffer
		GLint		m_ulTexPosition;// Position Gbuffer
		GLint		m_ulTexDepth;	// Depth Gbuffer
		GLint		m_ulTexNormal;	// Normal Gbuffer
		GLint		m_ulTexIrradiance;	//
		GLint		m_ulShadowMapArray;

		GLint		m_ulUseIrradianceMap;

		bool		m_bUseIrradianceMap;

	};



	// SimpleDeferredShaderオブジェクトを生成するファクトリクラス
	class SimpleDeferredShaderFactory : IShaderFactory
	{
	protected:

		IShader* Create( GLSL_VERSION version ){ return new SimpleDeferredShader( _T( "SimpleDeferredShader.glsl" ), version ); }

	public:

		virtual ~SimpleDeferredShaderFactory(){}
		IShader* CreateInstance( GLSL_VERSION version );
	};



}// end of namespace




#endif	// SIMPLE_DEFERRED_LIGHTING_SHADER_H //