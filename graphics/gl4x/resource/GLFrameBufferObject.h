// OpenGL GLFrameBufferObjectクラス
#ifndef GL_FRAME_BUFFER_OBJECT_H
#define GL_FRAME_BUFFER_OBJECT_H

#include	<oreore/common/Utility.h>

#include	"../common/GLHelperFunctions.h"
#include	"IGLBufferObject.h"



namespace OreOreLib
{

	class FrameBufferAttachment
	{
	public:

		FrameBufferAttachment();
		FrameBufferAttachment( int numattachments, const uint32 attachments[], const uint32 texids[]=NULL, const int levels[]=NULL, const int layers[]=NULL );
		~FrameBufferAttachment(){}

		void Init( int numattachments, const uint32 attachments[], const uint32 texids[]=NULL, const int levels[]=NULL, const int layers[]=NULL );
		void Clear();

		int	NumAttachPoints() const	{ return m_numAttachPoints; }
		
		uint32* GetTexID( int idx ) const			{ return (uint32 *)( m_texIDs + idx ); }
		uint32* GetTexIDs() const					{ return (uint32 *)m_texIDs; }
		uint32* GetAttachment( int idx ) const	{ return (uint32 *)( m_Attachments + idx ); }
		uint32* GetAttachments() const			{ return (uint32 *)m_Attachments; }
		int* GetLevel( int idx ) const			{ return (int *)( m_Levels + idx ); }
		int* GetLevels() const					{ return (int *)m_Levels; }
		int* GetLayer( int idx ) const			{ return (int *)(m_Layers + idx); }
		int* GetLayers() const					{ return (int *)m_Layers; }


		bool SetNumAttachPoints( int numattachments );
		bool SetTexID( uint32 texid );
		bool SetTexID( int idx, uint32 texid );
		bool SetTexIDs( int numattachments, const uint32 texids[] );
		bool SetAttachment( int idx, uint32 attachment );
		bool SetAttachments( int numattachments, const uint32 attachments[] );
		bool SetLevel( int idx, int level );
		bool SetLevels( int numattachments, const int levels[] );
		bool SetLayer( int idx, int layer );
		bool SetLayers( int numattachments, const int layers[] );

		
		inline void DrawBuffer( int idx ) const						{ GL_SAFE_CALL( glDrawBuffer( m_Attachments[idx] ) ); }
		inline void DrawBuffers( int numtex, int start_idx ) const	{ GL_SAFE_CALL( glDrawBuffers( numtex, &m_Attachments[start_idx] ) ); }
		inline void DrawBuffers() const								{ GL_SAFE_CALL( glDrawBuffers( m_numAttachPoints, &m_Attachments[0] ) ); }

	
		inline void BindAttachment() const		{ for( int i=0; i<m_numAttachPoints; ++i ) GL_SAFE_CALL( glFramebufferTexture( GL_FRAMEBUFFER, m_Attachments[i], m_texIDs[i], m_Levels[i] ) ); }
		inline void UnbindAttachment() const	{ for( int i=0; i<m_numAttachPoints; ++i ) GL_SAFE_CALL( glFramebufferTexture( GL_FRAMEBUFFER, m_Attachments[i], 0, 0 ) ); }

		inline void BindAttachment( int numtex, int start_idx ) const	{ for( int i=start_idx; i<numtex; ++i )	GL_SAFE_CALL( glFramebufferTexture( GL_FRAMEBUFFER, m_Attachments[i], m_texIDs[i], m_Levels[i] ) ); }
		inline void UnbindAttachment( int numtex, int start_idx ) const { for( int i=start_idx; i<numtex; ++i ) GL_SAFE_CALL( glFramebufferTexture( GL_FRAMEBUFFER, m_Attachments[i], 0, 0 ) ); }
		
		inline void BindAttachmentLayer() const			{ for( int i=0; i<m_numAttachPoints; ++i ) GL_SAFE_CALL( glFramebufferTextureLayer( GL_FRAMEBUFFER, m_Attachments[i], m_texIDs[i], m_Levels[i], m_Layers[i] ) ); }
		inline void BindAttachmentLayer( int i ) const	{ GL_SAFE_CALL( glFramebufferTextureLayer( GL_FRAMEBUFFER, m_Attachments[i], m_texIDs[i], m_Levels[i], m_Layers[i] ) ); }
		inline void UnbindAttachmentLayer() const		{ for( int i=0; i<m_numAttachPoints; ++i ) GL_SAFE_CALL( glFramebufferTextureLayer( GL_FRAMEBUFFER, m_Attachments[i], 0, 0, 0 ) ); }
		

		inline void BindAttachments() const
		{
			for( int i=0; i<m_numAttachPoints; ++i )
				GL_SAFE_CALL( m_Layers[i] ? glFramebufferTextureLayer( GL_FRAMEBUFFER, m_Attachments[i], m_texIDs[i], m_Levels[i], m_Layers[i] ) : glFramebufferTexture( GL_FRAMEBUFFER, m_Attachments[i], m_texIDs[i], m_Levels[i] ) );
		}

		inline void UnbindAttachments() const
		{
			for( int i=0; i<m_numAttachPoints; ++i )
				GL_SAFE_CALL( m_Layers[i] ? glFramebufferTextureLayer( GL_FRAMEBUFFER, m_Attachments[i], 0, 0, 0 ) : glFramebufferTexture( GL_FRAMEBUFFER, m_Attachments[i], 0, 0 ) );
		}



	private:

		int		m_numAttachPoints;
		uint32	m_texIDs[16];
		uint32	m_Attachments[16];
		int		m_Levels[16];
		int		m_Layers[16];

		friend class GLFrameBufferObject;
	};



	
	class GLFrameBufferObject : public IGLBufferObject
	{
	public:

		GLFrameBufferObject();
		~GLFrameBufferObject();

		void Init( int width, int height, bool enableDepth );
		void Release();// 全テクスチャを解除

		void CheckStatus();
		bool IsValid() const	{ return m_Framebuffer !=0; }

		inline void Bind() const { GL_SAFE_CALL( glBindFramebuffer( GL_FRAMEBUFFER, m_Framebuffer ) ); }
		inline void Unbind() const { GL_SAFE_CALL( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) ); }

		int Width() const	{ return m_Width; }
		int Height() const	{ return m_Height; }
		//int Depth() const	{ return m_Depth; }


	private:

		int		m_Width;		//フレームバッファ幅
		int		m_Height;		//フレームバッファ高さ
		//int		m_Depth;	// フレームバッファ奥行き

		uint32	m_Framebuffer;	// Framebuffer
		uint32	m_Renderbuffer;	// RenderBuffer

		GLFrameBufferObject( const GLFrameBufferObject &obj ){}// コピーコンストラクタの使用禁止
		GLFrameBufferObject& operator =( const GLFrameBufferObject &obj ){}// 代入演算子も使わない

	};



}// end of namespace






#endif	// GL_FRAME_BUFFER_OBJECT_H //