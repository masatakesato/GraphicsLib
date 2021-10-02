#include	"GLUniformBufferObject.h"

#include	"../common/GLHelperFunctions.h"

#include	"GLBindPointManager.h"



namespace OreOreLib
{

	GLUniformBufferObject::GLUniformBufferObject()
	{
		m_UBO		= 0;
		m_BindPoint	= 0;
		m_Usage		= GL_DYNAMIC_DRAW;
	}



	GLUniformBufferObject::~GLUniformBufferObject()
	{
		Release();
	}



	void GLUniformBufferObject::Init( GLenum usage, GLuint bindpoint )
	{
		Release();

		GL_SAFE_CALL( glGenBuffers( 1, &m_UBO ) );// buffer作成
		m_BindPoint	= bindpoint;
		m_Usage		= usage;

		GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, m_UBO ) );
		GL_SAFE_CALL( glBindBufferBase( GL_UNIFORM_BUFFER, m_BindPoint, m_UBO ) );// テクスチャのglActiveTexture(), glBindTexture()に相当
		GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, 0 ) );

	}



	void GLUniformBufferObject::Release()
	{
		if( m_UBO )
		{
			GL_SAFE_CALL( glDeleteBuffers( 1, &m_UBO ) );
			m_UBO	= 0;
		}
		
		m_BindPoint	= 0;

		m_Usage		= GL_DYNAMIC_DRAW;
	}



	void GLUniformBufferObject::SetBindPoint( GLuint bindpoint )
	{
		m_BindPoint	= bindpoint;

		GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, m_UBO ) );
		{
			GL_SAFE_CALL( glBindBufferBase( GL_UNIFORM_BUFFER, m_BindPoint, m_UBO ) );// テクスチャのglActiveTexture(), glBindTexture()に相当
		}
		GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, 0 ) );
	}



	void GLUniformBufferObject::BufferData( const void* pData, GLsizei size/*, GLenum usage*/ )
	{
		//m_Size	= size;
		GL_SAFE_CALL( glBufferData( GL_UNIFORM_BUFFER, /*m_Size*/size, pData, m_Usage/*usage*/ ) );// UBO領域を確保する
		//glBindBufferBase( GL_UNIFORM_BUFFER, m_BindPoint, m_UBO );// UniformBufferObjectを、BindingPoint"UBP_LIGHTPARAM"にバインドする
	}



	void GLUniformBufferObject::BufferData( const Buffer *pBuffer/*, GLenum usage*/ )
	{
		//m_Size	= pBuffer->elmSize * pBuffer->numElms;
		GL_SAFE_CALL( glBufferData( GL_UNIFORM_BUFFER, /*m_Size*/pBuffer->elmSize * pBuffer->numElms, pBuffer->pData, m_Usage/*usage*/ ) );// UBO領域を確保する
		//glBindBufferBase( GL_UNIFORM_BUFFER, m_BindPoint, m_UBO );// UniformBufferObjectを、BindingPoint"UBP_LIGHTPARAM"にバインドする
	}



	void GLUniformBufferObject::BufferSubData( const void *pData, uint32 offset, int size )
	{
		GL_SAFE_CALL( glBufferSubData( GL_UNIFORM_BUFFER, offset, size, pData ) );
	}



	void GLUniformBufferObject::UpdateSubresource( const void* pData, GLsizei size )
	{
		GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, m_UBO ) );
		{
			//GL_SAFE_CALL( glBindBufferBase( GL_UNIFORM_BUFFER, m_BindPoint, m_UBO ) );
			GL_SAFE_CALL( glBufferData( GL_UNIFORM_BUFFER, size, pData, m_Usage ) );
		}
		GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, 0 ) );
	} 



	void GLUniformBufferObject::UpdateSubresource( const Buffer *pBuffer )
	{
		GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, m_UBO ) );
		{
			//GL_SAFE_CALL( glBindBufferBase( GL_UNIFORM_BUFFER, m_BindPoint, m_UBO ) );
			GL_SAFE_CALL( glBufferData( GL_UNIFORM_BUFFER, pBuffer->elmSize * pBuffer->numElms, pBuffer->pData, m_Usage ) );
		}
		GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, 0 ) );
	}



	void GLUniformBufferObject::UpdateSubresource( const void *pData, uint32 offset, int size )
	{
		GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, m_UBO ) );
		{
			//GL_SAFE_CALL( glBindBufferBase( GL_UNIFORM_BUFFER, m_BindPoint, m_UBO ) );
			GL_SAFE_CALL( glBufferSubData( GL_UNIFORM_BUFFER, offset, size, pData ) );
		}
		GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, 0 ) );
	} 



	void GLUniformBufferObject::UpdateSubresource( const Buffer *pBuffer, uint32 offset )
	{
		GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, m_UBO ) );
		{
			//GL_SAFE_CALL( glBindBufferBase( GL_UNIFORM_BUFFER, m_BindPoint, m_UBO ) );
			GL_SAFE_CALL( glBufferSubData( GL_UNIFORM_BUFFER, offset, pBuffer->elmSize * pBuffer->numElms, pBuffer->pData ) );
		}
		GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, 0 ) );
	}



	void GLUniformBufferObject::Readback2CPU( void *pdest, int size )
	{
		if( !pdest )
			return;

		//=============			glMapBuffer() approach			=============//
		//GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, m_UBO ) );
		//{
		//	GLvoid* p = glMapBuffer( GL_UNIFORM_BUFFER, GL_READ_ONLY ) );
		//	memcpy( pdest, p, size );
		//	GL_SAFE_CALL( glUnmapBuffer( GL_UNIFORM_BUFFER ) );
		//}
		//GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, 0 ) );


		//============= we can also use glGetBufferSubData()...	=============//
		GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, m_UBO ) );
		GL_SAFE_CALL( glGetBufferSubData( GL_UNIFORM_BUFFER, 0, size, pdest ) );
		GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, 0 ) );

	}



//	void GLUniformBufferObject::BindToShaderByName( GLuint program_id, const tstring& ubname )
//	{
//#ifdef UNICODE
//		size_t wLen = 0;
//		errno_t err = 0;
//		char	*wStrC = new char[ubname.length( )];
//		setlocale( LC_ALL, "japanese" );	// Set Locate
//		err = wcstombs_s( &wLen, wStrC, 20, ubname.c_str( ), _TRUNCATE );	// Convert from w_char_t to char
//		
//		GLuint ubindex = GL_SAFE_CALL( glGetProgramResourceIndex( program_id, GL_UNIFORM_BLOCK, wStrC ) );
//		SafeDeleteArray( wStrC );
//#else
//		
//		GLuint ubindex = GL_SAFE_CALL( glGetProgramResourceIndex( program_id, GL_UNIFORM_BLOCK, ubname.c_str( ) ) );
//
//#endif // UNICODE
//		
//		GL_SAFE_CALL( glUniformBlockBinding( program_id, ubindex, m_BindPoint ) );// UniformBlockに対してBindingPointを割り当てる. テクスチャのglUniform1i( texloc, texunit )に相当
//
//	}


	void GLUniformBufferObject::BindToShaderByName( GLuint program_id, const char* ubname )
	{
		GLuint ubindex = GL_SAFE_CALL( glGetProgramResourceIndex( program_id, GL_UNIFORM_BLOCK, ubname ) );
		GL_SAFE_CALL( glUniformBlockBinding( program_id, ubindex, m_BindPoint ) );// UniformBlockに対してBindingPointを割り当てる. テクスチャのglUniform1i( texloc, texunit )に相当
	}




};// end of namespace