#ifndef	CONSTANT_SHADER_H
#define	CONSTANT_SHADER_H

#include	<graphics/gl4x/shader/IShader.h>



namespace OreOreLib
{

	class ConstantShader : public IShader
	{
	public:
		
		ConstantShader();
		ConstantShader( const TCHAR *filepath, GLSL_VERSION version );
		~ConstantShader();

		bool InitShader( const TCHAR *filepath, GLSL_VERSION version );
		void Release();

		inline void Bind()	{ m_Shader.Bind(); };
		inline void Unbind(){ m_Shader.Unbind(); };

		virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat );
		void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat, const Vec4f *color );

		inline virtual int NumPasses() const						{ return 1; }
		inline virtual GLuint GetPassID( int shader_idx=0 ) const	{ return m_Shader.ID(); }

		inline virtual void BindShader( int shader_idx=0 )	// シェーダーをバインドする
		{
			if( m_pCurrShader == &m_Shader )	return;
			m_pCurrShader = &m_Shader;
			m_pCurrShader->Bind();
		}

		virtual void BindBufferObject( const IBufferObject* pbufferobj );
		virtual void UnbindBufferObject(){}


	private:

		enum COLOR_MODE{ COLOR_MODE_VERTEX, COLOR_MODE_INPUT };

		//=================== シェーダ ==================//
		GLShader		m_Shader;

		//================= シェーダ変数 ================//
		// uniform location
		GLint			m_ulMatWorld;		// wordl matrix
		GLint			m_ulColor;
		GLint			m_ulUseVertexColor;
	};





	class ConstantShaderFactory : IShaderFactory
	{
	protected:

		IShader* Create( GLSL_VERSION version ){ return new ConstantShader( _T( "ConstantShader.glsl" ), version ); }

	public:

		virtual ~ConstantShaderFactory(){}
		IShader* CreateInstance( GLSL_VERSION version );

	};


}// end of namespace


#endif	// CONSTANT_SHADER_H //