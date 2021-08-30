#ifndef	VOLUME_RAY_CASTING_SHADER_H
#define	VOLUME_RAY_CASTING_SHADER_H



#include	"IShader.h"


namespace OreOreLib
{

	class VolumeRayCastingShader : public IShader
	{
	public:

		VolumeRayCastingShader();
		VolumeRayCastingShader( const TCHAR *filepath, GLSL_VERSION version );
		~VolumeRayCastingShader();

		void InitShader( const TCHAR *filepath, GLSL_VERSION version );
		void Release();

		inline void Bind()	{ m_Shader.Bind(); };
		inline void Unbind(){ m_Shader.Unbind(); };

		void Render( Texture3D *pTexVolume, const Mat4f* pMatVoxlSpace, const Mat4f* pMatVoxlToWorldSpace );


		// Override Virtual Functions
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

		virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat ){}


	private:

		//=================== シェーダ ==================//
		GLShader		m_Shader;

		//================= シェーダ変数 ================//
		// uniform location
		GLint			m_ulMatVoxelToWorld;
		GLint			m_ulMatVoxelSpace;
		GLint			m_ulShadowMapArray;
		GLint			m_ulMatLight;
	};





	class VolumeRayCastingShaderFactory : IShaderFactory
	{
	protected:

		IShader* Create( GLSL_VERSION version ){ return new VolumeRayCastingShader( _T( "VolumeRayCastingShader.glsl" ), version ); }

	public:

		virtual ~VolumeRayCastingShaderFactory(){}
		IShader* CreateInstance( GLSL_VERSION version );

	};


}// end of namespace


#endif	// VOLUME_RAY_CASTING_SHADER_H //