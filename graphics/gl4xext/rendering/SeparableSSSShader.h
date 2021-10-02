#ifndef	SEPARABLE_SSS_SHADER_H
#define	SEPARABLE_SSS_SHADER_H

#include	<graphics/gl4x/shader/IShader.h>
#include	<graphics/gl4x/resource/GLRenderTarget.h>



namespace OreOreLib
{


	// SeparableSubsurfaceScatteringShader
	class SeparableSSSShader : public IShader
	{
	public:

		SeparableSSSShader();
		SeparableSSSShader( const TCHAR* filepath, GLSL_VERSION version );
		~SeparableSSSShader();

		void InitShader( const TCHAR* filepath, GLSL_VERSION version );
		void BindTexture( const Texture2D& tex_depth, const Texture2D& tex_color, const Texture2D& tex_normal, const Texture2D& tex_pos );
		void UnbindTexture();

		void SetScale( float val )			{ m_Scale	= val; }
		void SetTranslucency( float val )	{ m_Translucency = val; }

		void Render();

		// Override Virtual Functions
		inline virtual int NumPasses() const	{ return 1; }
		inline virtual GLuint GetPassID( int shader_idx=0 ) const{ return m_pShader->ID(); }

		inline virtual void BindShader( int shader_idx=0 )	// シェーダーをバインドする
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
		GLint		m_ulBlurDir;	// blur direction
		//GLint		m_ulScreenSize;
		GLint		m_ulScale;
		GLint		m_ulTranslucency;

		GLint		m_ulTexDepth;	// depth texture
		GLint		m_ulTexColor;	// color texture
		GLint		m_ulTexPosition;
		GLint		m_ulTexNormal;
		GLint		m_ulShadowMapArray;


		// GL Objects
		Texture2D	*m_refTexDepth;
		Texture2D	*m_refTexColor;
		Texture2D	*m_refTexNormal;
		Texture2D	*m_refTexPosition;

		GLRenderTarget	m_RenderTarget;
		Texture2D		m_IntermediateBuffer;	// 中間バッファ

		enum TEX_INPUT_SLOT		{ TEX_SLOT_DEPTH, TEX_SLOT_COLOR, TEX_SLOT_NORMAL, TEX_SLOT_POSITION, TEX_SLOT_SHADOWMAP, NUM_TEX, };
		enum FRAG_DATA_LOCATION	{ FRAG_COLOR, NUM_FRAG, };

		// ShaderParams
		float	m_AspectRatio;	// スクリーン高さを1.0とした時の横方向の拡大率
		Vec2f	m_hBlurDir;
		Vec2f	m_vBlurDir;
		float	m_Scale;
		float	m_Translucency;
	};

	

	// SeparableSSSShaderオブジェクトを生成するファクトリクラス
	class SeparableSSSShaderFactory : IShaderFactory
	{
	protected:

		IShader* Create( GLSL_VERSION version ){ return new SeparableSSSShader( _T( "SeparableSSSShader.glsl" ), version ); }

	public:

		virtual ~SeparableSSSShaderFactory(){}
		IShader* CreateInstance( GLSL_VERSION version );

	};




}// end of namespace




#endif	// SIMPLE_MESH_RENDERER_H //