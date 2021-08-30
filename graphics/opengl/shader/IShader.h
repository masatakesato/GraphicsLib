#ifndef	I_SHADER_H
#define	I_SHADER_H


#include	"Defines.h"
#include	"GLShader.h"
#include	"GLComputeShader.h"
#include	"Texture.h"
#include	"GLVertexArrayObject.h"
#include	"GeometryBuffer.h"


namespace OreOreLib
{

	class IShader
	{

	public:

		IShader(){};
		virtual ~IShader(){};


		static void Init();
		static void Release();

		static void BindScreenSpaceQuad( GLVertexArrayObject* ssquad )
		{
			m_refScreenSpaceQuad = ssquad;
		}

		static void UnbindScreenSpaceQuad()
		{
			m_refScreenSpaceQuad = NULL;
		}


		static void BindUnitCube( GLVertexArrayObject* cube )
		{
			m_refUnitCube = cube;
		}

		static void UnbindUnitCube()
		{
			m_refUnitCube = NULL;
		}



		virtual int NumPasses() const = 0;
		virtual GLuint GetPassID( int shader_idx=0 ) const	= 0;

		virtual void BindShader( int shader_idx=0 ) = 0;	// Bind Current Shader
		inline virtual void UnbindShader(){ if( m_pCurrShader ) m_pCurrShader->Unbind(); m_pCurrShader= NULL; }// Unbind Current Shader

		virtual void BindBufferObject( const IBufferObject* pbufferobj )=0;
		virtual void UnbindBufferObject()=0;

		virtual void Render(){}
		virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat ) = 0;


	protected:

		static GLShader				*m_pCurrShader;		// Current Shader

		static GLVertexArrayObject	*m_refScreenSpaceQuad;
		static GLVertexArrayObject	*m_refUnitCube;

	};




	class IShaderFactory
	{
	protected:

		virtual IShader* Create( GLSL_VERSION version )=0;

	public:

		virtual ~IShaderFactory(){};

		// インスタンスを生成する
		IShader* CreateInstance( GLSL_VERSION version ){ return Create( version ); };

	};




}// end of namespace



#endif	// IRENDERER_H //