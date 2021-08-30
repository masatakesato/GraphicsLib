#include "GLShaderStorageBufferObject.h"


#include	<assert.h>

namespace OreOreLib
{

	GLShaderStorageBufferObject::GLShaderStorageBufferObject()
	{
		m_BufferID	= 0;
		m_BindPoint	= 0;
		m_Size		= 0;
		m_Usage		= GL_DYNAMIC_COPY;
	}



	GLShaderStorageBufferObject::~GLShaderStorageBufferObject()
	{
		Release();
	}



	void GLShaderStorageBufferObject::Init( uint32 size, GLenum usage, GLuint bindpoint )
	{
		Release();

		GL_SAFE_CALL( glGenBuffers( 1, &m_BufferID ) );
		m_BindPoint	= bindpoint;
		m_Size		= size;
		m_Usage		= usage;

		GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, m_BufferID ) );
		{
			GL_SAFE_CALL( glBindBufferBase( GL_SHADER_STORAGE_BUFFER, m_BindPoint, m_BufferID ) );
			GL_SAFE_CALL( glBufferData( GL_SHADER_STORAGE_BUFFER, size, 0, m_Usage ) );
		}
		GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 ) );

	}



	void GLShaderStorageBufferObject::Release()
	{
		if( m_BufferID )
		{
			GL_SAFE_CALL( glDeleteBuffers( 1, &m_BufferID ) );
			m_BufferID	= 0;
			m_Size		= 0;
		}
		
		m_BindPoint	= 0;
		m_Usage		= GL_DYNAMIC_COPY;
	}
	


	void GLShaderStorageBufferObject::SetBindPoint( GLuint bindpoint )
	{
		m_BindPoint	= bindpoint;

		GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, m_BufferID ) );
		{
			GL_SAFE_CALL( glBindBufferBase( GL_SHADER_STORAGE_BUFFER, m_BindPoint, m_BufferID ) );
		}
		GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 ) );
	}


	void GLShaderStorageBufferObject::BufferData( const void* pData, GLsizei size )
	{
		//m_Size	= size;
		GL_SAFE_CALL( glBufferData( GL_SHADER_STORAGE_BUFFER, /*m_Size*/size, pData, m_Usage/*usage*/ ) );// UBO領域を確保する
		//glBindBufferBase( GL_SHADER_STORAGE_BUFFER, m_BindPoint, m_BufferID );// UniformBufferObjectを、BindingPoint"UBP_LIGHTPARAM"にバインドする
	}



	void GLShaderStorageBufferObject::BufferData( const Buffer *pBuffer )
	{
		//m_Size	= pBuffer->elmSize * pBuffer->numElms;
		GL_SAFE_CALL( glBufferData( GL_SHADER_STORAGE_BUFFER, /*m_Size*/pBuffer->elmSize * pBuffer->numElms, pBuffer->pData, m_Usage/*usage*/ ) );// UBO領域を確保する
		//glBindBufferBase( GL_SHADER_STORAGE_BUFFER, m_BindPoint, m_BufferID );// UniformBufferObjectを、BindingPoint"UBP_LIGHTPARAM"にバインドする
	}



	void GLShaderStorageBufferObject::BufferSubData( const void *pData, uint32 offset, int size )
	{
		GL_SAFE_CALL( glBufferSubData( GL_SHADER_STORAGE_BUFFER, offset, size, pData ) );
	}



	void GLShaderStorageBufferObject::UpdateSubresource( const void* pData, GLsizei size )
	{
		GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, m_BufferID ) );
		{
			//glBindBufferBase( GL_SHADER_STORAGE_BUFFER, m_BindPoint, m_BufferID );
			GL_SAFE_CALL( glBufferData( GL_SHADER_STORAGE_BUFFER, size, pData, m_Usage ) );
		}
		GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 ) );


		//GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, m_BufferID ) );
		//{
		//	GLvoid* p = GL_SAFE_CALL( glMapBuffer( GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY ) );
		//	memcpy( p, &pData, size );
		//	GL_SAFE_CALL( glUnmapBuffer( GL_SHADER_STORAGE_BUFFER ) );
		//}
		//GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 ) );
	}



	void GLShaderStorageBufferObject::UpdateSubresource( const Buffer *pBuffer )
	{
		GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, m_BufferID ) );
		{
			//GL_SAFE_CALL( glBindBufferBase( GL_SHADER_STORAGE_BUFFER, m_BindPoint, m_BufferID ) );
			GL_SAFE_CALL( glBufferData( GL_SHADER_STORAGE_BUFFER, pBuffer->elmSize * pBuffer->numElms, pBuffer->pData, m_Usage ) );
		}
		GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 ) );


		//GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, m_BufferID ) );
		//{
		//	GLvoid* p = GL_SAFE_CALL( glMapBuffer( GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY ) );
		//	memcpy( p, &pBuffer->pData, pBuffer->elmSize * pBuffer->elmSize );
		//	GL_SAFE_CALL( glUnmapBuffer( GL_SHADER_STORAGE_BUFFER ) );
		//}
		//GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 ) );
	}



	void GLShaderStorageBufferObject::UpdateSubresource( const void *pData, uint32 offset, int size )
	{
		GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, m_BufferID ) );
		{
			//glBindBufferBase( GL_SHADER_STORAGE_BUFFER, m_BindPoint, m_BufferID );
			GL_SAFE_CALL( glBufferSubData( GL_SHADER_STORAGE_BUFFER, offset, size, pData ) );
		}
		GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 ) );
	}



	void GLShaderStorageBufferObject::UpdateSubresource( const Buffer *pBuffer, uint32 offset )
	{
		GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, m_BufferID ) );
		{
			//glBindBufferBase( GL_SHADER_STORAGE_BUFFER, m_BindPoint, m_BufferID );
			GL_SAFE_CALL( glBufferSubData( GL_SHADER_STORAGE_BUFFER, offset, pBuffer->elmSize * pBuffer->numElms, pBuffer->pData ) );
		}
		GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 ) );

	}



	void GLShaderStorageBufferObject::Readback2CPU( void *pdest, int size )
	{
		if( !pdest )
			return;

		//=============			glMapBuffer() approach			=============//
		//GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, m_BufferID ) );
		//{
		//	GLvoid* p = GL_SAFE_CALL( glMapBuffer( GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY ) );
		//	memcpy( pdest, p, size );
		//	GL_SAFE_CALL( glUnmapBuffer( GL_SHADER_STORAGE_BUFFER ) );
		//}
		//GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 ) );


		//============= we can also use glGetBufferSubData()...	=============//
		GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, m_BufferID ) );
		GL_SAFE_CALL( glGetBufferSubData( GL_SHADER_STORAGE_BUFFER, 0, size, pdest ) );
		GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 ) );

	}



//	void GLShaderStorageBufferObject::BindToShaderByName( GLuint program_id, const tstring& ssbname )
//	{
//#ifdef UNICODE
//		size_t outputSize = ssbname.length() +1;
//		char *ssbName = new char[outputSize];
//		size_t wLen = 0;
//		errno_t err = 0;
//		err = wcstombs_s( &wLen, ssbName, outputSize, ssbname.c_str(), ssbname.length() );	// Convert from w_char_t to char
//
//		GLuint ubindex = glGetProgramResourceIndex( program_id, GL_SHADER_STORAGE_BLOCK, ssbName );
//		SafeDeleteArray( ssbName );
//#else
//
//		GLuint ubindex = glGetProgramResourceIndex( program_id, GL_SHADER_STORAGE_BLOCK, ssbname.c_str() );
//
//#endif // UNICODE
//
//		glShaderStorageBlockBinding( program_id, ubindex, m_BindPoint );// ShaderStorageBlockに対してBindingPointを割り当てる
//
//
//		GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, m_BufferID ) );
//		GL_SAFE_CALL( glBindBufferBase( GL_SHADER_STORAGE_BUFFER, m_BindPoint, m_BufferID ) );
//		GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 ) );
//	}
	


	void GLShaderStorageBufferObject::BindToShaderByName( GLuint program_id, const char* ssbname )
	{
		GLuint ubindex = GL_SAFE_CALL( glGetProgramResourceIndex( program_id, GL_SHADER_STORAGE_BLOCK, ssbname ) );

		glShaderStorageBlockBinding( program_id, ubindex, m_BindPoint );// ShaderStorageBlockに対してBindingPointを割り当てる

		GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, m_BufferID ) );
		GL_SAFE_CALL( glBindBufferBase( GL_SHADER_STORAGE_BUFFER, m_BindPoint, m_BufferID ) );
		GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 ) );
	}


}// end of namespace