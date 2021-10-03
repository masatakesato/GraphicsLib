#ifndef	SILHOUETTE_SHADER_H
#define	SILHOUETTE_SHADER_H

#include	<graphics/gl4x/shader/IShader.h>
#include	<graphics/gl4x/shader/GLShader.h>



namespace OreOreLib
{

// 輪郭線描画シェーダー
	class SilhouetteShader : public IShader
	{
	private:

		// shader
		GLShader	*m_pShader;

		// uniform location
		GLint		m_ulMatWorld;			// world matrix
		GLint		m_ulMatViewProj;		// view projection matrix

		GLint		m_ulTexture;			// texture
		GLint		m_ulDepthMap;
		GLint		m_ulNormalMap;
		GLint		m_ulDiffuse;			// diffuse material TODO: Add entire material





		GLint		m_ulLineWidth;
		GLint		m_ulOverhangLength;

		GLint		m_ulNormalMatrix;
		GLint		m_ulProjection;
		GLint		m_ulSize;
		GLint		m_ulDiffuseMaterial;
		GLint		m_ulSpecularMaterial;
		GLint		m_ulShininess;
		GLint		m_ulAmbientMaterial;
		GLint		m_ulLightPosition;



	public:

		SilhouetteShader();
		SilhouetteShader( const TCHAR* filepath, GLSL_VERSION version );
		~SilhouetteShader();

		void InitShader( const TCHAR* filepath, GLSL_VERSION version );


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

		virtual void Render(){}
		virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat );

	};




	// SilhouetteShaderオブジェクトを生成するファクトリクラス
	//class SilhouetteShaderFactory : IRendererFactory
	//{
	//private:

	//	IRenderer* CreateInstance()
	//	{
	//		return new SilhouetteShader( "labworks/SilhouetteShader.glsl" );
	//	}

	//public:

	//	SilhouetteShaderFactory(){};
	//	//~SilhouetteShaderFactory(){};
	//};




}// end of namespace


#endif	// SILHOUETTE_SHADER_H //