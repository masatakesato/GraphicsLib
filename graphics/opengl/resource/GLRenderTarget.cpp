#include	"GLRenderTarget.h"



namespace OreOreLib
{

	// Default Constructor
	GLRenderTarget::GLRenderTarget()
	{
		
	}



	// Constructor
	GLRenderTarget::GLRenderTarget( int width, int height, bool enableDepth )
	{
		Init( width, height, enableDepth );
	}



	// Destructor
	GLRenderTarget::~GLRenderTarget()
	{
		Release();
	}



	void GLRenderTarget::Init( int width, int height, bool enableDepth )
	{
		Release();
		m_Fbo.Init( width, height, enableDepth );
	}



	void GLRenderTarget::Release()
	{
		if( m_Fbo.IsValid() )
		{
			m_Fbo.Bind();
			{
				m_Attachment.UnbindAttachments();
			}
			m_Fbo.Unbind();

			m_Fbo.Release();
		}

		m_Attachment.Clear();
	}



	void GLRenderTarget::BindTextures( int numAttachments, const uint32 attachmentPoints[], const uint32 texids[], const int levels[], const int layers[] )
	{
		if( !m_Fbo.IsValid() )
			return;

		m_Attachment.Init( numAttachments, attachmentPoints, texids, levels, layers );

		m_Fbo.Bind();
		{
			m_Attachment.BindAttachments();
		}
		m_Fbo.Unbind();

	}
	
	
	
	void GLRenderTarget::UnbindTextures()
	{
		if( !m_Fbo.IsValid() )
			return;

		m_Fbo.Bind();
		{
			m_Attachment.UnbindAttachments();
		}
		m_Fbo.Unbind();

		m_Attachment.Clear();
	}





	//void GLRenderTarget::BindAttachment( FrameBufferAttachment *attachment )
	//{
	//	//if( !m_Fbo.isvalid )	return;

	//	//m_Attachment = attachment;

	//	m_Fbo.Bind();
	//	
	//	attachment->BindAttachment();

	//	m_Fbo.Unbind();
	//}


	//void GLRenderTarget::BindAttachmentLayer( FrameBufferAttachment *attachment )
	//{
	//	//if( !m_Framebuffer )	return;

	//	//m_refAttachment = attachment;

	//	m_Fbo.Bind();

	//	attachment->BindAttachmentLayer();

	//	m_Fbo.Unbind();
	//	
	//}


	//void GLRenderTarget::UnbindAttachment()
	//{
	//	//if( m_refAttachment==NULL || m_Framebuffer==0 )	return;

	//	m_Fbo.Bind();

	//	m_Attachment.UnbindAttachment();

	//	m_Fbo.Unbind();
	//}



	//void GLRenderTarget::UnbindAttachmentLayer()
	//{
	//	//if( !m_refAttachment )	return;

	//	m_Fbo.Bind();

	//	m_Attachment.UnbindAttachmentLayer();
	//
	//	m_Fbo.Unbind();
	//}







}// end of namespace