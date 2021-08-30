#ifndef	LOCAL_REFLECTION_SHADER_H
#define	LOCAL_REFLECTION_SHADER_H


#include	"IShader.h"
#include	"GLShader.h"



namespace OreOreLib
{

	// ScreenSpace Local Reflection Shader
	class LocalReflectionShader : public IShader
	{	
	public:

		LocalReflectionShader();
		LocalReflectionShader( const TCHAR* filepath, GLSL_VERSION version );
		~LocalReflectionShader();

		void InitShader( const TCHAR* filepath, GLSL_VERSION version );
		void Release();

		void Render( uint32 tex_pos, uint32 tex_depth, uint32 tex_normal, uint32 tex_color/*, Frustum *pcamera*/ );

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
		GLShader	*m_pShader;

		// uniform location	
		GLint		m_ulTexPosition;// position texture
		GLint		m_ulTexDepth;	// depth texture
		GLint		m_ulTexNormal;	// normal texture
		GLint		m_ulTexColor;	// color texture
		

		enum TEX_IN
		{
			TEX_POSITION,
			TEX_DEPTH,
			TEX_NORMAL,
			TEX_COLOR
		};

		enum TEX_SLOT
		{
			TEX_SLOT_POSITION	= GL_TEXTURE0,
			TEX_SLOT_DEPTH		= GL_TEXTURE1,
			TEX_SLOT_NORMAL		= GL_TEXTURE2,
			TEX_SLOT_COLOR		= GL_TEXTURE3,
		};

	};


	

	// LocalReflectionShaderオブジェクトを生成するファクトリクラス
	class LocalReflectionShaderFactory : IShaderFactory
	{
	protected:

		IShader* Create( GLSL_VERSION version ){ return new LocalReflectionShader( _T("LocalReflectionShader.glsl"  ), version ); }

	public:

		virtual ~LocalReflectionShaderFactory(){}
		IShader* CreateInstance( GLSL_VERSION version );
	};



}// end of namespace




#endif	// LOCAL_REFLECTION_SHADER_H //