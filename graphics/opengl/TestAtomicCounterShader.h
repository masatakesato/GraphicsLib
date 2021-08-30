#ifndef	TEST_ATOMIC_COUNTER_SHADER_H
#define	TEST_ATOMIC_COUNTER_SHADER_H


#include	"IRenderer.h"
#include	"GLShader.h"
#include	"GLAtomicCounterBufferObject.h"



namespace OreOreLib
{
	// アトミックカウンター可視化シェーダー
	class TestAtomicCounterShader : public IRenderer
	{	
	public:

		TestAtomicCounterShader( const char* filepath=NULL );
		~TestAtomicCounterShader();

		void Release();

		void InitShader( const char* filepath );

		inline virtual void BindShader( int passid=0 )
		{
			if( CurrentShader == m_pShader )	return;

			CurrentShader = m_pShader;
			CurrentShader->Bind();
		}

		void Render();
		virtual void Render( MovableObject *pObj, Frustum *pfrustum ){}


	private:

		// shader
		GLShader	*m_pShader;	// デフォルトのシェーダー

		// uniform location
	

		GLAtomicCounterBufferObject	m_AtomicCounter;

	};



	// TestAtomicCounterShaderオブジェクトを生成するファクトリクラス
	class TestAtomicCounterShaderFactory : IRendererFactory
	{
	private:

		IRenderer* CreateInstance(){ return new TestAtomicCounterShader( "TestAtomicCounterShader.glsl" ); }

	public:

		TestAtomicCounterShaderFactory(){}
		//~TestAtomicCounterShaderFactory(){}
	};


}// end of namespace




#endif	// TEST_ATOMIC_COUNTER_SHADER_H //