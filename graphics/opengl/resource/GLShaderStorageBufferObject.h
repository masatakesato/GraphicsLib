#ifndef GL_SHADER_STORAGE_OBJECT_H
#define GL_SHADER_STORAGE_OBJECT_H


#include	"./common/TString.h"
#include	"Buffer.h"
#include	"IGLBufferObject.h"



namespace OreOreLib
{
	
	class GLShaderStorageBufferObject : public IGLBufferObject
	{

	public:

		GLShaderStorageBufferObject();
		~GLShaderStorageBufferObject();

		void Init( uint32 size, GLenum usage, GLuint bindpoint );
		void Release();
		

		void SetBindPoint( GLuint bindpoint );
		void SetUsage( GLenum usage )	{ m_Usage = usage; }
		void BindBuffer()	{ GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, m_BufferID ) ); }
		void UnbindBuffer() { GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 ) ); }
		void BufferData( const void* pData, GLsizei size );
		void BufferData( const Buffer *pBuffer );
		void BufferSubData( const void *pData, uint32 offset, int size );

		void UpdateSubresource( const void* pData, GLsizei size );
		void UpdateSubresource( const Buffer *pBuffer );

		void UpdateSubresource( const void *pData, uint32 offset, int size );
		void UpdateSubresource( const Buffer *pBuffer, uint32 offset );

		void Readback2CPU( void *pdest, int size );

		
		//void BindToShaderByName( GLuint program_id, const tstring& ssbname );
		void BindToShaderByName( GLuint program_id, const char* ssbname );

		inline void BindToShaderByIndex( GLuint program_id, GLuint shader_storage_block_index )
		{
			GL_SAFE_CALL( glShaderStorageBlockBinding( program_id, shader_storage_block_index, m_BindPoint ) );
		
			GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, m_BufferID ) );
			GL_SAFE_CALL( glBindBufferBase( GL_SHADER_STORAGE_BUFFER, m_BindPoint, m_BufferID ) );
			GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 ) );
		
		}// ShaderStorageBlockに対してバインドポイントを割り当てる. テクスチャのテクスチャユニット割り当てglUniform1i( texloc, texunit )に相当

		inline virtual void Bind() const { GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, m_BufferID ) ); GL_SAFE_CALL( glBindBufferBase( GL_SHADER_STORAGE_BUFFER, m_BindPoint, m_BufferID ) ); }
		inline virtual void Bind( GLuint bindpoint ) const { GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, m_BufferID ) ); GL_SAFE_CALL( glBindBufferBase( GL_SHADER_STORAGE_BUFFER, bindpoint, m_BufferID ) ); }
		inline virtual void Unbind() const { GL_SAFE_CALL( glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 ) ); }


	private:

		GLuint	m_BufferID;
		GLuint	m_BindPoint;
		GLsizei	m_Size;
		GLenum	m_Usage;

	};


}// end of namespace


#endif	// GL_SHADER_STORAGE_OBJECT_H //