#ifndef GL_RENDER_TARGET_H
#define	GL_RENDER_TARGET_H

#include	"Texture.h"
#include	"GLFrameBufferObject.h"



namespace OreOreLib
{

	class GLRenderTarget
	{
	public:

		GLRenderTarget();
		GLRenderTarget( int width, int height, bool enableDepth );
		~GLRenderTarget();
		
		void Init( int width, int height, bool enableDepth );
		void Release();

		int Width() const	{ return m_Fbo.Width(); }
		int Height() const	{ return m_Fbo.Height(); }

		void BindTextures( int numAttachments, const uint32 attachmentPoints[], const uint32 texids[], const int levels[]=NULL, const int layers[]=NULL );
		void UnbindTextures();

		inline void Bind() const							{ m_Fbo.Bind(); m_Attachment.DrawBuffers(); }
		inline void Bind( int idx ) const					{ m_Fbo.Bind(); m_Attachment.DrawBuffer( idx ); }
		inline void Bind( int numtex, int start_idx ) const	{ m_Fbo.Bind(); m_Attachment.DrawBuffers( numtex, start_idx ); }
		inline void Bind( const FrameBufferAttachment& attachment );

		inline void Unbind() const							{ m_Fbo.Unbind(); }

		



		//void BindAttachment( FrameBufferAttachment *attachment );
		//void BindAttachmentLayer( FrameBufferAttachment *attachment );
		//void UnbindAttachment();
		//void UnbindAttachmentLayer();
		

	private:

		FrameBufferAttachment	m_Attachment;
		GLFrameBufferObject		m_Fbo;

	};


	
}// end of namespace



#endif // !GL_RENDER_TARGET_H
