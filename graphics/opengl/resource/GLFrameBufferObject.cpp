#include	"GLFrameBufferObject.h"




#include	"GLHelperFunctions.h"



namespace OreOreLib
{


	//##########################################################################################//
	//								FrameBufferAttachment class									//
	//##########################################################################################//

	FrameBufferAttachment::FrameBufferAttachment()
	{
		Clear();
	}



	FrameBufferAttachment::FrameBufferAttachment( int numattachments, const uint32 channels[], const uint32 texids[], const int levels[], const int layers[] )
	{
		Init( numattachments, channels, texids, levels, layers );
	}



	void FrameBufferAttachment::Init( int numattachments, const uint32 channels[], const uint32 texids[], const int levels[], const int layers[] )
	{
		m_numAttachPoints	= Min( numattachments, 16 );

		if( channels )	memcpy( m_Attachments, channels, sizeof( uint32 ) * m_numAttachPoints );
		else			memcpy( m_Attachments, g_DefaultColorAttachments, sizeof( m_Attachments ) );

		if( texids )	memcpy( m_texIDs, texids, sizeof(uint32) * m_numAttachPoints );
		else			memset( m_texIDs, 0, sizeof(m_texIDs) );
		
		if( levels )	memcpy( m_Levels, levels, sizeof(int) * m_numAttachPoints );
		else			memset( m_Levels, 0, sizeof(m_Levels) );

		if( layers )	memcpy( m_Layers, layers, sizeof(int) * m_numAttachPoints );
		else			memset( m_Layers, 0, sizeof(m_Layers) );

	}



	void FrameBufferAttachment::Clear()
	{
		m_numAttachPoints	= 0;
		memset( m_texIDs, 0, sizeof(m_texIDs) );
		memset( m_Attachments, 0, sizeof(m_Attachments) );
		memset( m_Levels, 0, sizeof(m_Levels) );
		memset( m_Layers, 0, sizeof(m_Layers) );
	}



	bool FrameBufferAttachment::SetNumAttachPoints( int numattachments )
	{
		m_numAttachPoints	= Min( numattachments, 16 );
		return true;
	}



	bool FrameBufferAttachment::SetTexID( uint32 texid )
	{
		for( int i=0; i<m_numAttachPoints; ++i )
			m_texIDs[ i ]	= texid;
		return true;
	}



	bool FrameBufferAttachment::SetTexID( int idx, uint32 texid )
	{
		if( idx >= m_numAttachPoints )	return false;
		m_texIDs[ idx ]	= texid;
		return true;
	}
	


	bool FrameBufferAttachment::SetTexIDs( int numattachments, const uint32 texids[] )
	{
		if( m_numAttachPoints == 0 )	return false;
		memcpy( m_texIDs, texids, sizeof(uint32) * Min( numattachments, m_numAttachPoints ) );
		return true;
	}



	bool FrameBufferAttachment::SetAttachment( int idx, uint32 attachment )
	{
		if( idx >= m_numAttachPoints )	return false;
		m_Attachments[ idx ]	= attachment;
		return true;
	}
		


	bool FrameBufferAttachment::SetAttachments( int numattachments, const uint32 attachments[] )
	{
		if( m_numAttachPoints == 0 )	return false;
		memcpy( m_Attachments, attachments, sizeof(uint32) * Min( numattachments, m_numAttachPoints ) );
		return true;
		
	}

	

	bool FrameBufferAttachment::SetLevel( int idx, int level )
	{
		if( idx >= m_numAttachPoints )	return false;
		m_Levels[ idx ]	= level;
		return true;
	}
	

	
	bool FrameBufferAttachment::SetLevels( int numattachments, const int levels[] )
	{
		if( m_numAttachPoints == 0 )	return false;
		memcpy( m_Levels, levels, sizeof(int) * Min( numattachments, m_numAttachPoints ) );
		return true;
	}
	


	bool FrameBufferAttachment::SetLayer( int idx, int layer )
	{
		if( idx >= m_numAttachPoints )	return false;
		m_Layers[ idx ]	= layer;
		return true;
	}


	bool FrameBufferAttachment::SetLayers( int numattachments, const int layers[] )
	{
		if( m_numAttachPoints == 0 )	return false;
		memcpy( m_Layers, layers, sizeof(int) * Min( numattachments, m_numAttachPoints ) );
		return true;
	}




	//##########################################################################################//
	//								FramebufferObject class										//
	//##########################################################################################//

	GLFrameBufferObject::GLFrameBufferObject()
	{
		m_Width			= 0;
		m_Height		= 0;
		m_Framebuffer	= 0;
		m_Renderbuffer	= 0;
	}



	GLFrameBufferObject::~GLFrameBufferObject()
	{
		//tcout << "GLFrameBufferObject::~GLFrameBufferObject()..." << endl;
		Release();
	}



	void GLFrameBufferObject::Init( int width, int height, bool enableDepth )
	{
		Release();

		m_Width		= width;
		m_Height	= height;

		//================= Create Framebuffer ================//
		GL_SAFE_CALL( glGenFramebuffers( 1, &m_Framebuffer ) );

		if( m_Width == 0 && m_Height == 0 ) return;
		if( enableDepth == false )			return;

		//==== Create Renderbuffer and Bind to Framebuffer ====//

		// Create RenderBuffer as Depth component
		GL_SAFE_CALL( glGenRenderbuffers( 1, &m_Renderbuffer ) );
		GL_SAFE_CALL( glBindRenderbuffer( GL_RENDERBUFFER, m_Renderbuffer ) );
		{
			GL_SAFE_CALL( glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Width, m_Height ) );
		}
		GL_SAFE_CALL( glBindRenderbuffer( GL_RENDERBUFFER, 0 ) );

		// Bind to Framebuffer
		GL_SAFE_CALL( glBindFramebuffer( GL_FRAMEBUFFER, m_Framebuffer ) );
		{
			GL_SAFE_CALL( glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_Renderbuffer ) );
		}
		GL_SAFE_CALL( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );

#ifdef _DEBUG
		CheckStatus();
#endif
	}



	void GLFrameBufferObject::Release()
	{
		if( m_Framebuffer )
		{
			GL_SAFE_CALL( glBindFramebuffer( GL_FRAMEBUFFER, m_Framebuffer ) );

			// RenderBufferをデタッチする
			GL_SAFE_CALL( glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, NULL ) );
			// テクスチャをデタッチする
			//if( m_refAttachment )	m_refAttachment->UnbindAttachment();

			GL_SAFE_CALL( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );

			// フレームバッファオブジェクトを削除する
			GL_SAFE_CALL( glDeleteFramebuffers( 1, &m_Framebuffer ) );
			m_Framebuffer = 0;
		}

		//============================== m_Renderbufferを削除する ================================//
		if( m_Renderbuffer )
		{
			GL_SAFE_CALL( glDeleteRenderbuffers(1, &m_Renderbuffer) );
			m_Renderbuffer = 0;
		}

		//=================== テクスチャ情報を保持するバッファを削除する =======================//
		m_Width		= 0;
		m_Height	= 0;
	}



	void GLFrameBufferObject::CheckStatus()
	{
		GL_SAFE_CALL( glBindFramebuffer( GL_FRAMEBUFFER, m_Framebuffer ) );

		GLenum fboStatus	= GL_SAFE_CALL( glCheckFramebufferStatus( GL_FRAMEBUFFER ) );

		switch( fboStatus )
		{
		case GL_FRAMEBUFFER_COMPLETE:
			tcout << _T( "FBO generation succeeded...FRAMEBUFFER_COMPLETE" ) << tendl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			tcout << _T( "FBO generation failed...INCOMPLETE_ATTACHMENT" ) << tendl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			tcout << _T( "FBO generation failed...INCOMPLETE_MISSING_ATTACHMENT" ) << tendl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			tcout << _T( "FBO generation failed...INCOMPLETE_DRAW_BUFFER" ) << tendl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			tcout << _T( "FBO generation failed...INCOMPLETE_READ_BUFFER" ) << tendl;
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			tcout << _T( "FBO generation failed...UNSUPPORTED" ) << tendl;
			break;
		default:
			tcout << _T( "FBO generation failed...unknown error" ) << tendl;
			break;

		}

		GL_SAFE_CALL( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );
	}



}// end of namespace