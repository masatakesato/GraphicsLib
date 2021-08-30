#ifndef GL_VERTEX_BUFFER_OBJECT_H
#define	GL_VERTEX_BUFFER_OBJECT_H


#include	"../common/GLDataFormat.h"
#include	"IGLBufferObject.h"


namespace OreOreLib
{

	class GLVertexBufferObject : public IGLBufferObject
	{
	public:

		GLVertexBufferObject();
		~GLVertexBufferObject();

		void Init();
		bool SetVertexAttributes( const InputElementDesc* pInputElementDescs, int numDescs );
		bool SetVertexBuffers( const void *pData, int data_size, GLenum target, GLenum usage );
		bool SetVertexBuffers( const Buffer *pBuffer, GLenum target, GLenum usage );

		void Release();


	private:

		// Vertex Buffer Object
		GLuint			m_VBO;
		GLBufferDesc	m_BufferDesc;

		int				m_numDescs;
		GLAttribDesc	*m_VBDescs;

	};



}// end of namespace




#endif // !GL_VERTEX_BUFFER_OBJECT_H
