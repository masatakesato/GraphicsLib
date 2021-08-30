#ifndef GL_BUFFER_OBJECT_H
#define GL_BUFFER_OBJECT_H



#include	"IGLBufferObject.h"


namespace OreOreLib
{

	class GLBufferObject : public IGLBufferObject
	{
	public:

		//create buffer in the constructor
		GLBufferObject();
		GLBufferObject( GLenum target, GLsizei size=0 );
		~GLBufferObject();

		void Init( GLenum target, GLsizei size );
		void Release();

		GLuint GetBufferID();

		GLenum GetTargetType();
		void SetTargetType( GLenum target );

		GLsizei GetBufferSize();
		void SetBufferSize( GLsizei size );
		/*
		GL_PIXEL_UNPACK_BUFFER, GL_PIXEL_PACK_BUFFER
		GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER
		*/
		inline virtual void Bind() const	{ GL_SAFE_CALL( glBindBuffer( _target, _bufferID ) ); }
		inline virtual void Unbind() const	{ GL_SAFE_CALL( glBindBuffer( _target, 0 ) ); }
		/*
		GL_STATIC_DRAW_ARB      GL_STATIC_READ_ARB      GL_STATIC_COPY_ARB
		GL_DYNAMIC_DRAW_ARB     GL_DYNAMIC_READ_ARB     GL_DYNAMIC_COPY_ARB
		GL_STREAM_DRAW_ARB              GL_STREAM_READ_ARB      GL_STREAM_COPY_ARB
		*/
		void BufferData( const void* data, GLenum usage );
		void BufferDataStreamDraw( const void* data, GLsizei size=0 );//drawing constantly changing buffer
		void BufferDataStreamRead( const void* data, GLsizei size=0 );
		void BufferDataStaticDraw( const void* data, GLsizei size=0 );

		void BufferSubData( const void* data, GLsizei size, GLint offset=0 );

		/*
		GL_READ_ONLY_ARB
		GL_WRITE_ONLY_ARB
		GL_READ_WRITE_ARB
		*/
		void* MapBuffer( GLenum accessFlag );
		bool UnMapBuffer();

		//Functions
		//void CopyToTexture2D(GLTexture *texObject);
		//void CopyToTexture2D(GLuint tex);


	private:

		GLsizei _size;
		GLuint _target;
		GLuint _bufferID;

	};

}// end of namespace


#endif	// GL_BUFFER_OBJECT_H //