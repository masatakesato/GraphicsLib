#ifndef	GL_TEXTURE_BUFFER_OBJECT_H
#define	GL_TEXTURE_BUFFER_OBJECT_H

#include	"./common/TString.h"
#include	"./common/Utility.h"

#include	<GL/glew.h>
#pragma comment( lib, "glew32.lib" )


#include	"BufferLayout.h"
#include	"IGLBufferObject.h"
#include	"./container/RingQueue.h"


namespace OreOreLib
{

	class GLTextureBufferObject : public IGLBufferObject
	{
	public:

		GLTextureBufferObject();
		~GLTextureBufferObject();

		void Init( DATA_FORMAT dataFormat, int numElms, GLenum usage, const void *pData );
		void Clear( const void *val );
		void Release();

		//void SetBindPoint( GLuint bindpoint );

		inline virtual void Bind() const { GL_SAFE_CALL( glBindBuffer( GL_TEXTURE_BUFFER, m_TboID ) ); }
		inline virtual void Unbind() const { GL_SAFE_CALL( glBindBuffer( GL_TEXTURE_BUFFER, 0 ) ); }

		void BindImageTexture( GLuint unit, GLint level, GLboolean layered, GLint layer, GLenum access )
		{
			glBindImageTexture( unit, m_TexID, level, layered, layer, access, m_Desc.InternalFormat );
		}

		void BufferData( const void *pData, GLsizei size, GLenum usage );
		void BufferSubData( const void *pData, uint32 offset, int size );

		void UpdateSubresource( const void *pData, GLsizei size );
		void UpdateSubresource( const void *pData, uint32 offset, int size );

		//void BindToShader( GLuint program_id, const TCHAR* paramname );
		//void BindToShader( GLuint program_id, GLuint uniform_block_index );


	private:

		GLuint		m_TboID;	// TBOのID.
		GLuint		m_TexID;		// TextureオブジェクトのID
		GLenum		m_Usage;

		
		// opengl texture buffer descriptor
		struct TextureBufferDesc
		{
			// OreOre Format
			DATA_FORMAT	DataFormat;
			uint8		Channels;
			int			NumElms;
			int			ByteWidth;
			
			// GL format
			int			InternalFormat;		// 内部フォーマット. GL_RGBA32F, etc...
			uint32		Format;				// フォーマット. GL_RGBA, etc...
			uint32		Type;				// GL_FLOAT, etc...
		};

		TextureBufferDesc	m_Desc;
	};



}// end of namespace






#endif	// GL_TEXTURE_BUFFER_OBJECT_H //