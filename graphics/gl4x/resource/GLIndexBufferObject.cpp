#include	"GLIndexBufferObject.h"


//#include	"GLHelperFunctions.h"
#include	"GLDataFormat.h"



namespace OreOreLib
{

	GLIndexBufferObject::GLIndexBufferObject()
	{
		m_PrimitiveType	= GL_TRIANGLES;
		m_IndexType		= 0;
		m_numIndices	= 0;
		m_IBO			= 0;
	}



	GLIndexBufferObject::~GLIndexBufferObject()
	{
		Release();
	}



	void GLIndexBufferObject::Init()
	{
		Release();
		
		glGenBuffers( 1, &m_IBO );
	}



	bool GLIndexBufferObject::SetIndexBuffer( const void *pData, int numelms, DATA_FORMAT format, GLenum usage )
	{
		if( !m_IBO )
			return false;

		m_numIndices			= numelms;

		m_IndexDesc.buffer_size	= numelms * SizeOfFormatElement( format );
		m_IndexDesc.target		= GL_ELEMENT_ARRAY_BUFFER;
		m_IndexDesc.usage		= usage;

		int dummy_iformat;
		uint32 dummy_format;
		uint8 dummy_channels;

		GetGLFormat( format, dummy_iformat, dummy_format, m_IndexType, dummy_channels );

		// インデックスのセット
		GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IBO ) );
		GL_SAFE_CALL( glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_IndexDesc.buffer_size, pData, m_IndexDesc.usage ) );// 第二引数は、転送するデータのバイトサイズを指定
		GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) );

		return true;
	}



	bool GLIndexBufferObject::SetIndexBuffer( const Buffer *pBuffer, DATA_FORMAT format, GLenum usage )
	{
		if( !m_IBO )
			return false;

		m_numIndices	= pBuffer->numElms;

		m_IndexDesc.buffer_size	= m_numIndices * pBuffer->elmSize;
		//m_IndexDesc.target		= GL_ELEMENT_ARRAY_BUFFER;
		m_IndexDesc.usage		= usage;

		int dummy_iformat;
		uint32 dummy_format;
		uint8 dummy_channels;

		GetGLFormat( format, dummy_iformat, dummy_format, m_IndexType, dummy_channels );

		// インデックスのセット
		GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IBO ) );
		GL_SAFE_CALL( glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_IndexDesc.buffer_size, pBuffer->pData, m_IndexDesc.usage ) );// 第二引数は、転送するデータのバイトサイズを指定
		GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) );

		return true;
	}



	void GLIndexBufferObject::Release()
	{
		if( m_IBO )
		{
			GL_SAFE_CALL( glDeleteBuffers( 1, &m_IBO ) );
			m_IBO	= 0;
			m_numIndices = 0;
			m_IndexType = 0;
		}
	}

}// end of namespace