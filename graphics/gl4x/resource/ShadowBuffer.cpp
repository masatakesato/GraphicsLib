#include	"ShadowBuffer.h"


#include	"GLHelperFunctions.h"
#include	"GLBindPointManager.h"


namespace OreOreLib
{

	GLuint	ShadowBuffer::m_BindPoints[3]	= { 0, 0, 0 };



	ShadowBuffer::ShadowBuffer()
	{
		m_MatShadows			= NULL;
		m_SplitDepths			= NULL;
//		m_refShadowAttachments	= NULL;
	}
	
	
	
	ShadowBuffer::~ShadowBuffer()
	{
		Release();
	}


	
	void ShadowBuffer::Init( int texwidth, int texheight, int num_maxshadows )
	{
		m_CacheManager.Init( num_maxshadows );

		SafeDeleteArray( m_MatShadows );
		m_MatShadows	= new Mat4f[ num_maxshadows ];

		SafeDeleteArray( m_SplitDepths );
		m_SplitDepths	= new float[ num_maxshadows ];

		// Generate Texture2DArray for shadowmaps
		m_ShadowMapArray.Allocate( texwidth, texheight, 0, num_maxshadows, DATA_FORMAT::FORMAT_R32_FLOAT );//DATA_FORMAT::FORMAT_R16_FLOAT );//
		m_ShadowMapArray.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );
		m_ShadowMapArray.GenHardwareTexture();
		
		// Init FrameBufferObject
		m_FboShadowMap.Init( texwidth, texheight, true );
		
		// Allocate BindPoints
		for( int i=0; i<3; ++i )	if( m_BindPoints[i]==0 )	m_BindPoints[i]	= GLBindPointManager::Reserve( GL_UNIFORM_BUFFER );
//#ifdef	_DEBUG
//GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//#endif
		// Init UniformBufferObject for shadow matrices
		m_uboMatShadows.Init( GL_DYNAMIC_DRAW, m_BindPoints[0] );//TODO: staticメンバ変数にバインドポイントを格納する。バインドポイントはゼロで初期化しておく。ゼロの場合だけ新規取得、既に値がある場合はそれをつかう

		// Init UniformBufferObject for SplitDepths
		m_uboSplitDepths.Init( GL_DYNAMIC_DRAW, m_BindPoints[1] );
		
		// Init UniformBufferObject for ShadowID linked list
		m_uboShadowIDLinkedList.Init( GL_DYNAMIC_DRAW, m_BindPoints[2] );

	}
		
	
	
	void ShadowBuffer::Release()
	{
		m_CacheManager.Release();

		SafeDeleteArray( m_MatShadows );
		SafeDeleteArray( m_SplitDepths );

		m_ShadowMapArray.Release();
		m_FboShadowMap.Release();

		m_uboMatShadows.Release();
		m_uboSplitDepths.Release();
		m_uboShadowIDLinkedList.Release();

//		m_refShadowAttachments	= NULL;
	}

	

	void ShadowBuffer::BindToShader( GLuint program_id )
	{
		m_uboMatShadows.BindToShaderByName( program_id, "ubMatShadows" );
		m_uboSplitDepths.BindToShaderByName( program_id, "ubSplitDepths" );
		m_uboShadowIDLinkedList.BindToShaderByName( program_id, "ubShadowIDLinkedList" );
	}



	void ShadowBuffer::UpdateSubresource()
	{
		int num_activeslots = m_CacheManager.numMaxSlots();//m_CacheManager.numReservedSlots();
		if( num_activeslots == 0 )	return;

		m_uboMatShadows.UpdateSubresource( m_MatShadows, sizeof(Mat4f) * num_activeslots );
		m_uboSplitDepths.UpdateSubresource( m_SplitDepths, sizeof(float) * num_activeslots );
		m_uboShadowIDLinkedList.UpdateSubresource( m_CacheManager.GetLinkBuffer(), sizeof(int) * num_activeslots );
	}



}// end of namespace