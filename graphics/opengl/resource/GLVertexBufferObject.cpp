#include	"GLVertexBufferObject.h"


#include	"GLHelperFunctions.h"
#include	"GLDataFormat.h"


namespace OreOreLib
{

	GLVertexBufferObject::GLVertexBufferObject()
	{
		m_VBO			= 0;
		m_numDescs		= 0;
		m_VBDescs		= NULL;
	}



	GLVertexBufferObject::~GLVertexBufferObject()
	{



	}



	void GLVertexBufferObject::Init()
	{
		Release();

		GL_SAFE_CALL( glGenBuffers( 1, &m_VBO ) );
	}


	// 頂点データフォーマットの登録
	bool GLVertexBufferObject::SetVertexAttributes( const InputElementDesc* pInputElementDescs, int numDescs )
	{
		if( !pInputElementDescs )	return false;

		//=================== m_VBOsの初期化 ===================//
		SafeDeleteArray( m_VBDescs );

		m_numDescs	= numDescs;
		m_VBDescs	= new GLAttribDesc[m_numDescs];
		int dummy_iformat;
		uint32 dummy_format;

		// 頂点フォーマットのデータ格納
		for( int i=0; i<m_numDescs; ++i )
		{
			const InputElementDesc *pDesc	= &pInputElementDescs[i];

			// フォーマット取得
			GLAttribDesc *pformat	= &m_VBDescs[i];
			pformat->attribLocation	= pDesc->AttribLocation;
			GetGLFormat( pDesc->Format, dummy_iformat, dummy_format, pformat->type, pformat->channels );
			pformat->element_size	= SizeOfFormatElement( pDesc->Format );
			pformat->bNormalized	= pDesc->bNormalized;
			pformat->stride			= pDesc->Stride;
			pformat->offset			= pDesc->Offset;
		}// end of i loop

		return true;
	}



	// 配列データを使う
	bool GLVertexBufferObject::SetVertexBuffers( const void *pData, int data_size, GLenum target, GLenum usage )
	{
		if( !pData || !m_VBO )	return false;

		m_BufferDesc.buffer_size	= data_size;
		m_BufferDesc.target			= target;
		m_BufferDesc.usage			= usage;

		GL_SAFE_CALL( glBindBuffer( m_BufferDesc.target, m_VBO ) );
		GL_SAFE_CALL( glBufferData( m_BufferDesc.target, m_BufferDesc.buffer_size, pData, m_BufferDesc.usage ) );// 第二引数は、転送するデータのバイトサイズを指定
		GL_SAFE_CALL( glBindBuffer( m_BufferDesc.target, 0 ) );
		
		return true;
	}



	// Buffer構造体を使う
	bool GLVertexBufferObject::SetVertexBuffers( const Buffer *pBuffer, GLenum target, GLenum usage )
	{
		if( !pBuffer->pData || !m_VBO )	return false;

		m_BufferDesc.buffer_size	= pBuffer->elmSize * pBuffer->numElms;
		m_BufferDesc.target			= target;
		m_BufferDesc.usage			= usage;

		GL_SAFE_CALL( glBindBuffer( m_BufferDesc.target, m_VBO ) );
		GL_SAFE_CALL( glBufferData( m_BufferDesc.target, m_BufferDesc.buffer_size, pBuffer->pData, m_BufferDesc.usage ) );// 第二引数は、転送するデータのバイトサイズを指定
		GL_SAFE_CALL( glBindBuffer( m_BufferDesc.target, 0 ) );

		return true;
	}











	void GLVertexBufferObject::Release()
	{
		if( m_VBO )
		{
			GL_SAFE_CALL( glDeleteBuffers( 1, &m_VBO ) );
			m_VBO	= 0;
		}
	
	
	}





	

}// end of namespace

