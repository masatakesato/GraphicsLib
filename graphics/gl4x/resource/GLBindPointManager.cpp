#include	"GLBindPointManager.h"





namespace OreOreLib
{

	RingQueue<GLuint>	GLBindPointManager::m_UniformBufferBindPoints;
	//bool				*GLBindPointManager::m_bUniformBufferBindPointReserved	= NULL;

	RingQueue<GLuint>	GLBindPointManager::m_ShaderStorageBufferBindPoints;
	//bool				*GLBindPointManager::m_bShaderStorageBufferBindPointReserved	= NULL;


	void GLBindPointManager::InitBindPoints()
	{
		InitBindPoints( &m_UniformBufferBindPoints, /*&m_bUniformBufferBindPointReserved,*/ GL_MAX_UNIFORM_BUFFER_BINDINGS );
		InitBindPoints( &m_ShaderStorageBufferBindPoints, /*&m_bShaderStorageBufferBindPointReserved,*/ GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS );
	}



	void GLBindPointManager::ReleaseBindPoints()
	{
		if( !m_UniformBufferBindPoints.IsEmpty() )	m_UniformBufferBindPoints.Release();
		//SafeDeleteArray( m_bUniformBufferBindPointReserved );

		if( !m_ShaderStorageBufferBindPoints.IsEmpty() )	m_ShaderStorageBufferBindPoints.Release();
		//SafeDeleteArray( m_bShaderStorageBufferBindPointReserved );
	}



	GLuint GLBindPointManager::Reserve( GLenum bufferType )
	{
		switch( bufferType )
		{
		case GL_UNIFORM_BUFFER:
			return m_UniformBufferBindPoints.Dequeue();

		case GL_SHADER_STORAGE_BUFFER:
			return m_ShaderStorageBufferBindPoints.Dequeue();

		default:
			return 0;
		}
		
	}
	


	void GLBindPointManager::Free( GLuint bindpoint, GLenum bufferType )
	{
		m_UniformBufferBindPoints.Enqueue( bindpoint );
	}



	void GLBindPointManager::InitBindPoints( RingQueue<GLuint> *queue, /*bool** breserved,*/ GLenum bufferType )
	{
		//============= バインドポイント最大数を取得する ============//
		int max_bind_points = 0;
		glGetIntegerv( bufferType, &max_bind_points );

		//======= 使用可能なバインドポイントのリストを初期化する ==========//
		queue->Init( max_bind_points );
		//(*breserved)	= new bool[max_bind_points];
		for( int i=1; i<=max_bind_points; ++i )
		{
			queue->Enqueue( GLint( i ) );
			//(*breserved)[i-1] = false;
		}

	}







}// end of namespace