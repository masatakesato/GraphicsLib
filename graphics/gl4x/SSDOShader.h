#ifndef	SSDO_SHADER_H
#define	SSDO_SHADER_H


#include	"IShader.h"
#include	"GLRenderTarget.h"

#include	"SSDOBuffer.h"



namespace OreOreLib
{
	
	class SSDOShader : public IShader
	{
	public:

		SSDOShader();
		SSDOShader( const TCHAR *filepath, GLSL_VERSION version );
		~SSDOShader();

		void InitShader( const TCHAR *filepath, GLSL_VERSION version );
		void Release();

		void BindFramebuffer( const Texture2D& tex_pos, const Texture2D& tex_normal, const Texture2D& tex_diffuse );
		void UnbindFramebuffer();
		void BindSSDOBuffer( SSDOBuffer* buffer );
		void UnbindSSDOBuffer();

		void Render( uint32 texid );

		// Override Virtual Functions
		inline virtual int NumPasses() const	{ return NUM_SHADER_PASS; }
		inline virtual GLuint GetPassID( int shader_idx=0 ) const{ return m_pShader ? m_pShader->ID() : 0; }

		inline virtual void BindShader( int shader_idx=0 )	// シェーダーをバインドする
		{
			if( m_pCurrShader == m_pShader )	return;
			m_pCurrShader = m_pShader;
			m_pCurrShader->Bind();
		}
		
		virtual void BindBufferObject( const IBufferObject* pbufferobj );
		virtual void UnbindBufferObject(){}
		
		void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat ){}


	private:

		enum SHADER_PASS{ SHADER_PASS_OCCLUSION_SH, SHADER_PASS_BLUR, NUM_SHADER_PASS };


		GLShader				*m_pShader;
		GLShader				m_Pass[ NUM_SHADER_PASS ];

		GLUniformBufferObject	m_uboParams;
		static GLuint			m_BindPoint;
		Texture2D				m_TexNoise;

		GLRenderTarget			m_RenderTarget;

		GLint					m_ulBlurDir;
		GLint					m_ulTexPosition;
		GLint					m_ulTexNormal;
		GLint					m_ulTexNoise;
		GLint					m_ulTexOcclusion;
		GLint					m_ulTexBlur;
		GLint					m_ulTexFinal;


		Texture2D				*m_refTexPosition;
		Texture2D				*m_refTexNormal;
		SSDOBuffer				*m_refSSDOBuffer;
	




		enum TEX_SLOT		{ TEX_SLOT_POSITION, TEX_SLOT_NORMAL, TEX_SLOT_NOISE, TEX_SLOT_OCCLUSION_SH, TEX_SLOT_BLUR, TEX_SLOT_FINAL, NUM_TEX_SLOT };
		enum FRAG_LOCATION	{ FRAG_LOCATION_COLOR, NUM_FRAG_LOCATION };
		enum FBO_SLOT		{ FBO_SLOT_OCCLUSION_SH, FBO_SLOT_BLUR, FBO_SLOT_FINAL };


		void InitNoiseTexture( int dimx, int dimy );
	};





	class SSDOShaderFactory : IShaderFactory
	{
	protected:

		IShader* Create( GLSL_VERSION version ){ return new SSDOShader( _T( "SSDOShader.glsl" ), version ); }

	public:

		virtual ~SSDOShaderFactory(){};
		IShader* CreateInstance( GLSL_VERSION version );

	};








}// end of namespace


#endif	// SSDO_SHADER_H //