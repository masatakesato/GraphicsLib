#ifndef	Z_PREPASS_SHADER_H
#define	Z_PREPASS_SHADER_H



#include	"IShader.h"


namespace OreOreLib
{

	class ZPrepassShader : public IShader
	{
	public:

		ZPrepassShader();
		ZPrepassShader( const TCHAR *filepath, GLSL_VERSION version );
		~ZPrepassShader();

		bool InitShader( const TCHAR *filepath, GLSL_VERSION version );

		inline void Bind()	{ m_Shader.Bind();   };
		inline void Unbind(){ m_Shader.Unbind(); };

		virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat );
		
		inline virtual int NumPasses() const						{ return 1; }
		inline virtual GLuint GetPassID( int shader_idx=0 ) const{ return m_Shader.ID(); }

		inline virtual void BindShader( int shader_idx=0 )	// シェーダーをバインドする
		{
			if( m_pCurrShader == &m_Shader )	return;
			m_pCurrShader = &m_Shader;
			m_pCurrShader->Bind();
		}

		virtual void BindBufferObject( const IBufferObject* pbufferobj );
		virtual void UnbindBufferObject(){}
		

	private:

		//=================== シェーダ ==================//
		GLShader	m_Shader;


		//================= シェーダ変数 ================//
		// attribute location
		enum	m_AttribLocation{ ATTRIB_LOCATION_POSITION, ATTRIB_LOCATION_NORMAL, ATTRIB_LOCATION_TEXCOORD, ATTRIB_LOCATION_COLOR };

		// uniform location
		GLint	m_ulMatWorld;		// wordl matrix

	};

	

	class ZPrepassShaderFactory : IShaderFactory
	{
	protected:

		IShader* Create( GLSL_VERSION version ){ return new ZPrepassShader( _T( "ZPrepass.glsl" ), version ); };

	public:

		virtual ~ZPrepassShaderFactory(){};
		IShader* CreateInstance( GLSL_VERSION version );

	};



}


#endif	// Z_PREPASS_SHADER_H //