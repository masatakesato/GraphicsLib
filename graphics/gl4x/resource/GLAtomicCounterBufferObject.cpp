#include "GLAtomicCounterBufferObject.h"


#include	<assert.h>

namespace OreOreLib
{

	GLAtomicCounterBufferObject::GLAtomicCounterBufferObject()
	{
		_bufferID	= 0;
		//m_BindPoint	= GL_INVALID_INDEX;
		_size		= 0;

		m_Count		= 0;
	}



	GLAtomicCounterBufferObject::~GLAtomicCounterBufferObject()
	{
		Release();
	}



	void GLAtomicCounterBufferObject::Init()
	{
		_size	= sizeof(GLuint);
		GL_SAFE_CALL( glGenBuffers( 1, &_bufferID ) );

		GL_SAFE_CALL( glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, _bufferID ) );
		GL_SAFE_CALL( glBufferData( GL_ATOMIC_COUNTER_BUFFER, _size, 0, GL_DYNAMIC_DRAW ) );
		GL_SAFE_CALL( glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, 0 ) );
	}



	void GLAtomicCounterBufferObject::Clear( const void *val )
	{
		// データフォーマット指定なしでクリアするなら、Map/Unmapを使う
		//GL_SAFE_CALL( glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, _bufferID ) );
		//GLuint* ptr	= (GLuint*)glMapBufferRange( GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint),
		//										GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT );
		//ptr[0]	= val;
		//GL_SAFE_CALL( glUnmapBuffer( GL_ATOMIC_COUNTER_BUFFER ) );
		//GL_SAFE_CALL( glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, 0 ) );

		// データフォーマットを指定してクリアする場合はglClearBufferDataを使う
		GL_SAFE_CALL( glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, _bufferID ) );
		GL_SAFE_CALL( glClearBufferData( GL_ATOMIC_COUNTER_BUFFER, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, val ) );
		GL_SAFE_CALL( glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, 0 ) );
		
		m_Count	= val ? *(unsigned int *)val : 0;
	}



	void GLAtomicCounterBufferObject::Release()
	{
		if( _bufferID )
		{
			GL_SAFE_CALL( glDeleteBuffers( 1, &_bufferID ) );
			_bufferID = 0;
			_size	= 0;
		}

		m_Count	= 0;

		//m_BindPoint	= GL_INVALID_INDEX;
	}



	GLenum  GLAtomicCounterBufferObject::GetBufferID()
	{
		return _bufferID;
	}



	void GLAtomicCounterBufferObject::BindBufferBase( GLuint bindpoint ) const
	{
		GL_SAFE_CALL( glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, _bufferID ) );
		GL_SAFE_CALL( glBindBufferBase( GL_ATOMIC_COUNTER_BUFFER, bindpoint, _bufferID ) );
	}



	void* GLAtomicCounterBufferObject::MapBuffer(GLenum accessFlag)
	{
		void* vptr = GL_SAFE_CALL( glMapBuffer( GL_ATOMIC_COUNTER_BUFFER, accessFlag ) );
		assert(vptr);
		return vptr;
	}



	//GLboolean is unsigned char type
	bool GLAtomicCounterBufferObject::UnMapBuffer()
	{
		if( glUnmapBuffer( GL_ATOMIC_COUNTER_BUFFER ) != GL_TRUE )
			return false;
		else
			return true;
	}



	void GLAtomicCounterBufferObject::Readback2CPU()
	{
		//=============			glMapBuffer() approach			=============//
		GL_SAFE_CALL( glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, _bufferID ) );
		{
			GLuint* p = (GLuint*)GL_SAFE_CALL( glMapBuffer( GL_ATOMIC_COUNTER_BUFFER, GL_READ_ONLY ) );
			m_Count	= *p;//memcpy( pdest, p, size );
			GL_SAFE_CALL( glUnmapBuffer( GL_ATOMIC_COUNTER_BUFFER ) );
		}
		GL_SAFE_CALL( glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, 0 ) );


		//============= we can also use glGetBufferSubData()...	=============//
		//GL_SAFE_CALL( glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, _bufferID ) );
		//GL_SAFE_CALL( glGetBufferSubData( GL_ATOMIC_COUNTER_BUFFER, 0, sizeof( GLuint ), &m_Count ) );
		//GL_SAFE_CALL( glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, 0 ) );

	}





}// end of namespace