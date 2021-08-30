#include	"GLTextureBufferObject.h"

#include	"GLDataFormat.h"



namespace OreOreLib
{

	GLTextureBufferObject::GLTextureBufferObject()
	{
		m_TboID	= 0;
		m_TexID	= 0;
		m_Usage	= GL_STATIC_DRAW;
	}



	GLTextureBufferObject::~GLTextureBufferObject()
	{
		Release();
	}



	void GLTextureBufferObject::Init( DATA_FORMAT dataFormat, int numElms, GLenum usage, const void *pData )
	{
		Release();

		m_Usage	= usage;

		//====================	Init Desc	===================//
		m_Desc.DataFormat	= dataFormat;
		m_Desc.NumElms		= numElms;
		m_Desc.ByteWidth	= SizeOfFormatElement(dataFormat) * m_Desc.NumElms;
		GetGLFormat( m_Desc.DataFormat, m_Desc.InternalFormat, m_Desc.Format, m_Desc.Type, m_Desc.Channels );


		//===================	Init TBO	===================//
		GL_SAFE_CALL( glGenBuffers( 1, &m_TboID ) );
		
		GL_SAFE_CALL( glBindBuffer( GL_TEXTURE_BUFFER, m_TboID ) );
		GL_SAFE_CALL( glBufferData( GL_TEXTURE_BUFFER, m_Desc.ByteWidth, pData, m_Usage ) );
		GL_SAFE_CALL( glBindBuffer( GL_TEXTURE_BUFFER, 0 ) );
		

		//=================== Init Texture	===================//
		GL_SAFE_CALL( glGenTextures( 1, &m_TexID ) );

		GL_SAFE_CALL( glBindTexture( GL_TEXTURE_BUFFER, m_TexID ) );
		GL_SAFE_CALL( glTexBuffer( GL_TEXTURE_BUFFER, m_Desc.InternalFormat, m_TboID ) );
		GL_SAFE_CALL( glBindTexture( GL_TEXTURE_BUFFER, 0 ) );
	}



	void GLTextureBufferObject::Clear( const void *val )
	{
		//GL_SAFE_CALL( glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, _bufferID ) );
		//GLuint* ptr	= (GLuint*)GL_SAFE_CALL( glMapBufferRange( GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint),
		//										GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT ) );
		//ptr[0]	= value;
		//GL_SAFE_CALL( glUnmapBuffer( GL_ATOMIC_COUNTER_BUFFER ) );
		//GL_SAFE_CALL( glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, 0 ) );

		GL_SAFE_CALL( glBindBuffer( GL_TEXTURE_BUFFER, m_TboID ) );
		GL_SAFE_CALL( glClearBufferData( GL_TEXTURE_BUFFER, m_Desc.InternalFormat, m_Desc.Format, m_Desc.Type, val ) );// 最後の引数は要素1個分だけでいいか
		GL_SAFE_CALL( glBindBuffer( GL_TEXTURE_BUFFER, 0 ) );
	}



	void GLTextureBufferObject::Release()
	{
		if( m_TboID )
		{
			GL_SAFE_CALL( glDeleteBuffers( 1, &m_TboID ) );
			m_TboID	= 0;
		}

		if( m_TexID )
		{
			GL_SAFE_CALL( glDeleteTextures( 1, &m_TexID ) );
			m_TexID	= 0;
		}
		
		m_Usage		= GL_STATIC_DRAW;
	}



	//void GLUniformBufferObject::SetBindPoint( GLuint bindpoint )
	//{
	//	m_BindPoint	= bindpoint;
	//}



	void GLTextureBufferObject::BufferData( const void *pData, GLsizei size, GLenum usage )
	{
		GL_SAFE_CALL( glBufferData( GL_TEXTURE_BUFFER, size, pData, usage ) );
	}



	void GLTextureBufferObject::BufferSubData( const void *pData, uint32 offset, int size )
	{
		GL_SAFE_CALL( glBufferSubData( GL_TEXTURE_BUFFER, offset, size, pData ) );
	}



	void GLTextureBufferObject::UpdateSubresource( const void *pData, GLsizei size )
	{
		GL_SAFE_CALL( glBindBuffer( GL_TEXTURE_BUFFER, m_TboID ) );
		{
			GL_SAFE_CALL( glBufferData( GL_TEXTURE_BUFFER, size, pData, m_Usage ) );
		}
		GL_SAFE_CALL( glBindBuffer( GL_TEXTURE_BUFFER, 0 ) );
	}



	void GLTextureBufferObject::UpdateSubresource( const void *pData, uint32 offset, int size )
	{
		GL_SAFE_CALL( glBindBuffer( GL_TEXTURE_BUFFER, m_TboID ) );
		{
			GL_SAFE_CALL( glBufferSubData( GL_TEXTURE_BUFFER, offset, size, pData ) );
		}
		GL_SAFE_CALL( glBindBuffer( GL_TEXTURE_BUFFER, 0 ) );
	}


}// end of namespace