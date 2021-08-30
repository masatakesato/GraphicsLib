#ifndef	GL_UNIFORM_BUFFER_OBJECT_H
#define	GL_UNIFORM_BUFFER_OBJECT_H

#include	<oreore/common/TString.h>
#include	<oreore/common/Utility.h>
#include	<oreore/container/RingQueue.h>

#include	"Buffer.h"
#include	"IGLBufferObject.h"



namespace OreOreLib
{


	class GLUniformBufferObject : public IGLBufferObject
	{
	public:

		GLUniformBufferObject();
		~GLUniformBufferObject();

		void Init( GLenum usage, GLuint bindpoint );
		void Release();


		void SetBindPoint( GLuint bindpoint );
		void SetUsage( GLenum usage )	{ m_Usage = usage; }
		void BindBuffer() { GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, m_UBO ) ); }
		void UnbindBuffer() { GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, 0 ) ); }
		void BufferData( const void* pData, GLsizei size );
		void BufferData( const Buffer *pBuffer );
		void BufferSubData( const void *pData, uint32 offset, int size );

		void UpdateSubresource( const void* pData, GLsizei size );
		void UpdateSubresource( const Buffer *pBuffer );

		void UpdateSubresource( const void *pData, uint32 offset, int size );
		void UpdateSubresource( const Buffer *pBuffer, uint32 offset );

		void Readback2CPU( void *pdest, int size );

		void BindToShaderByIndex( GLuint program_id, GLuint uniform_block_index ) { GL_SAFE_CALL( glUniformBlockBinding( program_id, uniform_block_index, m_BindPoint ) ); }// UniformBlockに対してBindingPointを割り当てる. テクスチャのテクスチャユニット割り当てglUniform1i( texloc, texunit )に相当
		//void BindToShaderByName( GLuint program_id, const tstring& ubname );
		void BindToShaderByName( GLuint program_id, const char* ubname );

		inline virtual void Bind() const { GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, m_UBO ) ); GL_SAFE_CALL( glBindBufferBase( GL_UNIFORM_BUFFER, m_BindPoint, m_UBO ) ); }	// 挙動と関数名から連想する挙動とちがう
		inline virtual void Bind( GLuint bindpoint ) const { GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, m_UBO ) ); GL_SAFE_CALL( glBindBufferBase( GL_UNIFORM_BUFFER, bindpoint, m_UBO ) ); }
		inline virtual void Unbind() const { GL_SAFE_CALL( glBindBuffer( GL_UNIFORM_BUFFER, 0 ) ); }


	private:

		GLuint	m_UBO;
		GLuint	m_BindPoint;
		GLenum	m_Usage;

	};



}// end of namespace


#endif	// GL_UNIFORM_BUFFER_OBJECT_H //