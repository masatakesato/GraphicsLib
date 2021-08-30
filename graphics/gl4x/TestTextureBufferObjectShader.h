#ifndef	TEST_TEXTURE_BUFFER_OBJECT_SHADER_H
#define	TEST_TEXTURE_BUFFER_OBJECT_SHADER_H


#include	"IRenderer.h"
#include	"GLShader.h"
#include	"GLComputeShader.h"
#include	"GLAtomicCounterBufferObject.h"
#include	"GLTextureBufferObject.h"



namespace OreOreLib
{
	
	class TestTextureBufferObjectShader : public IRenderer
	{	
	public:

		TestTextureBufferObjectShader( const char* filepath=NULL );
		~TestTextureBufferObjectShader();

		void Release();

		void InitShader( const char* filepath );

		inline virtual void BindShader( int passid=0 )
		{
			if( CurrentShader == &m_Pass[passid] )	return;

			CurrentShader = &m_Pass[passid];
			CurrentShader->Bind();
		}

		void Render();
		virtual void Render( MovableObject *pObj, Frustum *pfrustum ){}


	private:

		enum SHADER_PASS{ PASS_COUNTUP_PIXELS, PASS_PREVIEW_TBO, NUM_PASS };

		// Shaders
		GLShader				m_Pass[ NUM_PASS ];
//		GLComputeShader			m_WriteToTBO;

		// Uniform Locations
//		GLint					m_ulTBOResolution;

		GLAtomicCounterBufferObject	m_AtomicCounter;
		GLTextureBufferObject	m_TBO;
		GLVertexArrayObject		m_OverlappedQuad;
	};



	// TestTextureBufferObjectShaderオブジェクトを生成するファクトリクラス
	class TestTextureBufferObjectShaderFactory : IRendererFactory
	{
	private:

		IRenderer* CreateInstance(){ return new TestTextureBufferObjectShader( "TestTextureBufferObjectShader.glsl" ); }

	public:

		TestTextureBufferObjectShaderFactory(){}
		//~TestTextureBufferObjectShaderFactory(){}
	};


}// end of namespace




#endif	// TEST_ATOMIC_COUNTER_SHADER_H //