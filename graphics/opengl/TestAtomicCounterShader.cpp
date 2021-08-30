#include	"TestAtomicCounterShader.h"


namespace OreOreLib
{
	// constructor
	TestAtomicCounterShader::TestAtomicCounterShader( const char *filepath )
	{
#ifdef	_DEBUG
		const type_info& id = typeid(*this);
		tcout << "AbstractClass-typeid: " << id.name() << tendl;
#endif

		m_pShader		= NULL;
		m_AtomicCounter.Release();

		if( filepath )
		{
			m_AtomicCounter.Init();
			InitShader( filepath );
		}
	}



	// destructor
	TestAtomicCounterShader::~TestAtomicCounterShader()
	{
		Release();
		/*SafeDelete( m_pShader );*/

	}



	void TestAtomicCounterShader::Release()
	{
		SafeDelete( m_pShader );
		m_AtomicCounter.Release();
	}



	// init shader
	void TestAtomicCounterShader::InitShader( const char *filepath )
	{
		// create shader
		m_pShader	= new GLShader();
		m_pShader->Init( filepath );

		// init attribute location
		glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_POSITION, "POSITION" );
		glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" );

		// init uniform location	


		// link shader
		m_pShader->Link();

		// その他
		// glBindFragDataLocation( program_id, channel, "FragColor0" );
		// サンプル
	}



	// メッシュデータをレンダリングする(シェーダーはバインド済みと想定)
	void TestAtomicCounterShader::Render()
	{
		if( !m_refScreenSpaceQuad )	return;
		
		m_AtomicCounter.Clear();

		m_AtomicCounter.BindBufferBase(0);

		m_pShader->Bind();
		
		m_refScreenSpaceQuad->Draw();

		m_pShader->Unbind();

		m_AtomicCounter.Unbind();
	}


}// end of namespace