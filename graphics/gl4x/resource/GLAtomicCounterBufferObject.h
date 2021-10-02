#ifndef GL_ATOMIC_COUNTER_BUFFER_OBJECT_H
#define GL_ATOMIC_COUNTER_BUFFER_OBJECT_H

#include <GL/glew.h>
#pragma comment( lib, "glew32.lib" )

#include	<oreore/container/RingQueue.h>

#include	<graphics/gl4x/resource/IGLBufferObject.h>



namespace OreOreLib
{

	// Currently supports unsigned int only. 2015.02.25
	class GLAtomicCounterBufferObject : public IGLBufferObject
	{

	public:

		GLAtomicCounterBufferObject();
		~GLAtomicCounterBufferObject();

		void Init();
		void Clear( const void *val=0 );
		void Release();

		GLuint GetBufferID();
		unsigned int GetCount() const	{ return m_Count; }

		GLenum GetTargetType();
		void SetTargetType(GLenum target);

		GLsizei GetBufferSize();
		void SetBufferSize(GLsizei size);

		virtual inline void Bind() const	{ GL_SAFE_CALL( glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, _bufferID ) ); }
		virtual inline void Unbind() const	{ GL_SAFE_CALL( glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, 0 ) ); }
		
		//void BindBufferBase() const;
		void BindBufferBase( GLuint bindpoint ) const;
		
		void* MapBuffer( GLenum accessFlag );
		bool UnMapBuffer();

		void Readback2CPU();


	private:

		GLuint	_bufferID;
		//GLuint	m_BindPoint;
		GLsizei	_size;

		unsigned int	m_Count;
		
		//internalFormat	= GL_R32UI;
		//format			= GL_RED_INTEGER;
		//type			= GL_UNSIGNED_INT;

		
	};


}// end of namespace


#endif	// GL_ATOMIC_COUNTER_BUFFER_OBJECT_H //