#include	"MatrixBuffer.h"

#include	"GLBindPointManager.h"


namespace OreOreLib
{

	GLuint	MatrixBuffer::m_BindPoint	= 0;


	MatrixBuffer::MatrixBuffer()
	{
#ifdef	_DEBUG
		tcout << "MatrixBuffer::MatrixBuffer()..." << tendl;
#endif

		m_Matrices	= NULL;
	}



	MatrixBuffer::~MatrixBuffer()
	{
		Release();
	}



	MatrixBuffer::MatrixBuffer( const MatrixBuffer& obj )
	{



	}



	void MatrixBuffer::Init( int num_maxelms )
	{
		m_CacheManager.Init( num_maxelms );
		SafeDeleteArray( m_Matrices );
		m_Matrices	= new Mat4f[ num_maxelms ];

		if( m_BindPoint==0 )	m_BindPoint = GLBindPointManager::Reserve( GL_UNIFORM_BUFFER );
		m_uboMatrices.Init( GL_DYNAMIC_DRAW, m_BindPoint );
	}



	void MatrixBuffer::Release()
	{
		m_CacheManager.Release();
		SafeDeleteArray( m_Matrices );
		m_uboMatrices.Release();
	}


}