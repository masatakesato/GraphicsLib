#ifndef GL_VERTEX_ARRAY_OBJECT_H
#define	GL_VERTEX_ARRAY_OBJECT_H



//#include	"BufferLayout.h"
//#include	"IGLBufferObject.h"
#include	"../common/GLHelperFunctions.h"
#include	"GLVertexBufferObject.h"
#include	"GLIndexBufferObject.h"




namespace OreOreLib
{

	// Input Element Description
	//struct InputElementDesc
	//{
	//	GLuint		AttribLocation;	// IRender::POSIATION, etc...
	//	DATA_FORMAT	Format;			// DATA_FORMAT::FORMAT_R32G32B32A32_FLOAT, etc...
	//	bool		bNormalized;	// normalize flag
	//	int			Stride;			// stride
	//	int			Offset;
	//};



	class GLVertexArrayObject : public IGLBufferObject
	{
	public:

		GLVertexArrayObject();
		~GLVertexArrayObject();

		void Init();
		bool SetVertexAttributes( const InputElementDesc* pInputElementDescs, int numDescs, PRIMITIVE_TYPE primitive_type );
		bool SetPrimitiveType( PRIMITIVE_TYPE primitive_type );
		bool SetVertexBuffers( const void *pData, int data_size, GLenum target, GLenum usage );
		bool SetVertexBuffers( const Buffer *pBuffer, GLenum target, GLenum usage );
		bool SetIndexBuffer( const void *pData, int numelms, DATA_FORMAT format, GLenum usage );
		bool SetIndexBuffer( const Buffer *pBuffer, DATA_FORMAT format, GLenum usage );
		bool Update();	// Update VAO draw commands using latest VBOs and VAOs

		bool Release();

		inline virtual void Bind() const	{ GL_SAFE_CALL( glBindVertexArray( m_VAO ) ); if( m_IBO )	GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IBO ) ); }
		inline virtual void Unbind() const { GL_SAFE_CALL( glBindVertexArray( 0 ) ); GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) ); }

		inline virtual void BindVertexBuffer() const	{ GL_SAFE_CALL( glBindVertexArray( m_VAO ) ); }
		inline virtual void UnbindVertexBuffer() const { GL_SAFE_CALL( glBindVertexArray( 0 ) ); }

		inline virtual void BindIndexBuffer() const		{ GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IBO ) ); }
		inline virtual void UnbindIndexBuffer() const { GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) ); }


		inline void Draw( GLenum mode = GL_TRIANGLES ) const
		{
			if(!m_IBO)	return;

			GL_SAFE_CALL( glBindVertexArray( m_VAO ) );
			GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IBO ) );
			GL_SAFE_CALL( glDrawElements( mode, m_numIndices, m_IndexType, 0 ) );
			GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) );
			GL_SAFE_CALL( glBindVertexArray( 0 ) );
		}


		inline void Draw( GLvoid* startIndex, unsigned int numelms, GLenum mode = GL_TRIANGLES ) const
		{
			if(!m_IBO)	return;

//			glBindVertexArray( m_VAO );
//			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IBO );
			GL_SAFE_CALL( glDrawElements( mode, numelms, m_IndexType, startIndex ) );
//			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
//			glBindVertexArray( 0 );
		}


		inline void Draw( GLIndexBufferObject *pIBO ) const
		{
			GL_SAFE_CALL( glBindVertexArray( m_VAO ) );
			pIBO->Draw();
			GL_SAFE_CALL( glBindVertexArray( 0 ) );
		}


		inline void Draw( GLIndexBufferObject *pIBO, unsigned int numelms, GLvoid* startIndex ) const
		{
			GL_SAFE_CALL( glBindVertexArray( m_VAO ) );
			pIBO->Draw( numelms, startIndex );
			GL_SAFE_CALL( glBindVertexArray( 0 ) );
		}


		inline void DrawInstanced( unsigned int princount, GLenum mode = GL_TRIANGLES )
		{
			if(!m_IBO)	return;

			GL_SAFE_CALL( glBindVertexArray( m_VAO ) );
			GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IBO ) );
			GL_SAFE_CALL( glDrawElementsInstanced( mode, m_numIndices, m_IndexType, 0, princount ) );
			GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) );
			GL_SAFE_CALL( glBindVertexArray( 0 ) );
		}


		inline void DrawArrays( unsigned int startIndex, unsigned int numelms, GLenum mode = GL_TRIANGLES )
		{
			if(!m_VAO)	return;

			GL_SAFE_CALL( glBindVertexArray( m_VAO ) );
			GL_SAFE_CALL( glDrawArrays( mode, startIndex, numelms ) );
			GL_SAFE_CALL( glBindVertexArray( 0 ) );
		}



	private:

		struct GLAttribDesc
		{
			GLuint	attribLocation;
			uint32	type;
			uint8	channels;
			GLsizei	element_size;
			bool	bNormalized;
			int		stride;
			int		offset;
		};

		struct GLBufferDesc
		{
			GLuint	target;
			GLsizei	buffer_size;
			GLenum	usage;
		};

		// Primitive Type
		GLenum			m_PrimitiveType;

		// Vertex Array Object
		GLuint			m_VAO;

		// Vertex Buffer Object
		GLuint			m_VBO;
		GLBufferDesc	m_BufferDesc;

		int				m_numDescs;
		GLAttribDesc	*m_VBDescs;

		// Index Buffer Object
		GLenum			m_IndexType;
		GLBufferDesc	m_IndexDesc;
		int				m_numIndices;
		GLuint			m_IBO;
	};



}// end of namespace


#endif // !GL_VERTEX_ARRAY_OBJECT



// 頂点バッファをセット
// インデックスバッファをセット
// VertexArrayObjectの生成

// VertexArrayObjectへの描画コールの登録