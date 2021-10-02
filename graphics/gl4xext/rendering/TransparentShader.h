#ifndef	TRANSPARENT_SHADER_H
#define	TRANSPARENT_SHADER_H

#include	<graphics/gl4x/shader/IShader.h>
#include	<graphics/gl4x/shader/GLShader.h>



namespace OreOreLib
{

	// 半透明描画シェーダー
	class TransparentShader : public IShader
	{
	public:

		TransparentShader();
		TransparentShader( const TCHAR* filepath, GLSL_VERSION version );
		~TransparentShader();

		void InitShader( const TCHAR* filepath, GLSL_VERSION version );
		void Release();
		
		void Render( const Texture2D* pTexAccum, const Texture2D* pTexRevealage, const Vec3f* bgColor );

		// Override Virtual Functions
		inline virtual int NumPasses() const	{ return 1; }
		inline virtual GLuint GetPassID( int shader_idx=0 ) const{ return m_Pass[shader_idx].ID(); }

		inline virtual void BindShader( int passid=0 )
		{
			if( m_pCurrShader == &m_Pass[passid] )	return;
			m_pCurrShader = &m_Pass[passid];
			m_pCurrShader->Bind();
		}

		virtual void BindBufferObject( const IBufferObject* pbufferobj );
		virtual void UnbindBufferObject(){}

		virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat );


	private:

		enum SHADER_PASS{ PASS_ACCUM, PASS_COMPOSITE, NUM_PASS };
		// shader
		GLShader	m_Pass[ NUM_PASS ];//*m_pShader;	// デフォルトのシェーダー

		// Uniform Locations
		GLint		m_ulMatWorld;			// world matrix

		GLint		m_ulTexture;			// texture
		GLint		m_ulDiffuse;			// diffuse material TODO: Support all material attributes
		GLint		m_ulSpecular;			// specular material TODO: Support all material attributes
		GLint		m_ulShininess;			// shininess material
		GLint		m_ulTransparency;

		GLint		m_ulShadowMapArray;


		GLint		m_ulBackgroundColor;


		enum TEX_INPUT
		{
			TEX_ACCUM,
			TEX_REVEALAGE,
			NUM_TEX_INPUT,
		};

	};




	// TransparentShaderオブジェクトを生成するファクトリクラス
	class TransparentShaderFactory : IShaderFactory
	{
	protected:

		IShader* Create( GLSL_VERSION version ){ return new TransparentShader( _T( "TransparentShader.glsl" ), version ); }

	public:

		virtual ~TransparentShaderFactory(){}
		IShader* CreateInstance( GLSL_VERSION version );

	};




}// end of namespace


#endif	// TRANSPARENT_SHADER_H //