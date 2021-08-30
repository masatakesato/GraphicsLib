#ifndef GL_INDEX_BUFFER_OBJECT_H
#define	GL_INDEX_BUFFER_OBJECT_H

#include	"../common/GLDataFormat.h"
#include	"IGLBufferObject.h"



namespace OreOreLib
{


	class GLIndexBufferObject : public IGLBufferObject
	{
	public:

		GLIndexBufferObject();
		~GLIndexBufferObject();

		void Init();
		bool SetIndexBuffer( const void *pData, int numelms, DATA_FORMAT format, GLenum usage );
		bool SetIndexBuffer( const Buffer *pBuffer, DATA_FORMAT format, GLenum usage );
		
		void Bind() { GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IBO ) ); }
		void Unbind() { GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) ); }

		void Release();


		inline void Draw()
		{
			GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IBO ) );
			GL_SAFE_CALL( glDrawElements( m_PrimitiveType, m_numIndices, m_IndexType, 0 ) );
			GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) );
		}


		inline void Draw( unsigned int numelms, GLvoid* startIndex )
		{
			GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IBO ) );
			GL_SAFE_CALL( glDrawElements( m_PrimitiveType, numelms, m_IndexType, (GLvoid *)startIndex ) );
			GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) );
		}



		inline void DrawElements()
		{
			GL_SAFE_CALL( glDrawElements( m_PrimitiveType, m_numIndices, m_IndexType, 0 ) );
		}



		inline void DrawElements( unsigned int numelms, GLvoid* startIndex )
		{
			GL_SAFE_CALL( glDrawElements( m_PrimitiveType, numelms, m_IndexType, startIndex ) );
		}


	private:
		
		GLenum			m_PrimitiveType;
		GLenum			m_IndexType;
		GLBufferDesc	m_IndexDesc;
		int				m_numIndices;
		GLuint			m_IBO;


	};





}// end of namespace


#endif // !GL_INDEX_BUFFER_OBJECT_H
