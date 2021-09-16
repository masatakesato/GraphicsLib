#ifndef	GENERAL_PROCEDURAL_SHADER_H
#define	GENERAL_PROCEDURAL_SHADER_H

#include	<graphics/gl4x/resource/Texture.h>
#include	<graphics/gl4x/resource/GLFramebufferObject.h>
#include	<graphics/gl4x/shader/IShader.h>

#include	"BlendShader.h"
#include	"../procedural/SimplexNoiseGPU.h"
#include	"../procedural/WorleyNoiseGPU.h"



//#include	"OutputShader.h"	// ポストエフェクト系のプロセス



namespace OreOreLib
{


	// メッシュオブジェクト描画クラス
	class EdgeDetectionShader : public IShader
	{
	public:

		EdgeDetectionShader();
		EdgeDetectionShader( const TCHAR* filepath, GLSL_VERSION version );
		~EdgeDetectionShader();

		void Release();

		void InitShader( const TCHAR *filepath, GLSL_VERSION version );

		void SetFilterRadius( int radius );
		void BindInputTexture( Texture2D *ptex )	{ m_refTexture = ptex; }


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

		virtual void Render();
		virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat ){}


	private:

		GLShader	*m_pShader;

		// パラメータ
		GLint		m_ulRadius;
		GLint		m_ulTexture;
		//GLUniformBufferObject	m_uboAttribute;


		// Texture reference
		Texture2D	*m_refTexture;

		int			m_Radius;
	};


	/*
	// EdgeDetectionShaderオブジェクトを生成するファクトリクラス
	class EdgeDetectionShaderactory : IRendererFactory
	{
	private:

		IRenderer* CreateInstance(){ return new EdgeDetectionShader( "EdgeDetectionShader.glsl" ); }

	public:

		EdgeDetectionShaderFactory(){};
		//~EdgeDetectionShaderFactory(){};
	};
	*/





}// end of namespace


#endif	// GENERAL_PROCEDURAL_SHADER_H //