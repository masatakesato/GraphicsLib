#include	"GLVertexArrayObject.h"

#include	"../common/GLHelperFunctions.h"
#include	"../common/GLDataFormat.h"



namespace OreOreLib
{


	GLVertexArrayObject::GLVertexArrayObject()
	{
		//#ifdef _DEBUG
		//	tcout << "GLVertexArrayObject::Release()..." << tendl;
		//#endif // _DEBUG

		m_PrimitiveType	= GL_TRIANGLES;

		m_VAO			= 0;
		m_VBO			= 0;

		m_numDescs		= 0;
		m_VBDescs		= NULL;

		m_IndexType		= 0;
		m_numIndices	= 0;
		m_IBO			= 0;
	}



	GLVertexArrayObject::~GLVertexArrayObject()
	{
		Release();
	}



	void GLVertexArrayObject::Init()
	{
		Release();
		// VertexArrayObject生成
		GL_SAFE_CALL( glGenVertexArrays( 1, &m_VAO ) );
		// VertexBufferObject生成
		GL_SAFE_CALL( glGenBuffers( 1, &m_VBO ) );
	}



	// 頂点データフォーマットの登録
	bool GLVertexArrayObject::SetVertexAttributes( const InputElementDesc* pInputElementDescs, int numDescs, PRIMITIVE_TYPE primitive_type )
	{
		if( !pInputElementDescs )	return false;


		SetPrimitiveType( primitive_type );

		//=================== m_VBOsの初期化 ===================//
		SafeDeleteArray( m_VBDescs );

		m_numDescs	= numDescs;
		m_VBDescs	= new GLAttribDesc[m_numDescs];
		int dummy_iformat;
		uint32 dummy_format;

		// 頂点フォーマットのデータ格納
		for( int i=0; i < m_numDescs; ++i )
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



	bool GLVertexArrayObject::SetPrimitiveType( PRIMITIVE_TYPE primitive_type )
	{
		switch( primitive_type )
		{
		case PRIMITIVE_TYPE::PRIM_POINT:
			m_PrimitiveType	= GL_POINTS;
			return true;

		case PRIMITIVE_TYPE::PRIM_LINE:
			m_PrimitiveType	= GL_LINES;
			return true;

		case PRIMITIVE_TYPE::PRIM_TRIANGLE:
			m_PrimitiveType	= GL_TRIANGLES;
			return true;

			//case PRIMITIVE_TYPE::PRIM_TRIANGLE_PATCH:
			//	break;

			//case PRIMITIVE_TYPE::PRIM_QUAD_PATCH:
			//	break;

		default:
			m_PrimitiveType	= GL_TRIANGLES;
			return false;
		}
	}



	// 配列データを使う
	bool GLVertexArrayObject::SetVertexBuffers( const void *pData, int data_size, GLenum target, GLenum usage )
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
	bool GLVertexArrayObject::SetVertexBuffers( const Buffer *pBuffer, GLenum target, GLenum usage )
	{
		if( !pBuffer->pData || !m_VBO )	return false;

		m_BufferDesc.buffer_size	= static_cast<GLsizei>( pBuffer->elmSize * pBuffer->numElms );
		m_BufferDesc.target			= target;
		m_BufferDesc.usage			= usage;

		GL_SAFE_CALL( glBindBuffer( m_BufferDesc.target, m_VBO ) );
		GL_SAFE_CALL( glBufferData( m_BufferDesc.target, m_BufferDesc.buffer_size, pBuffer->pData, m_BufferDesc.usage ) );// 第二引数は、転送するデータのバイトサイズを指定
		GL_SAFE_CALL( glBindBuffer( m_BufferDesc.target, 0 ) );
		
		return true;
	}



	bool GLVertexArrayObject::SetIndexBuffer( const void *pData, int numelms, DATA_FORMAT format, GLenum usage )
	{
		m_numIndices	= numelms;

		m_IndexDesc.buffer_size	= numelms * SizeOfFormatElement( format );
		//m_IndexDesc.target		= GL_ELEMENT_ARRAY_BUFFER;
		m_IndexDesc.usage		= usage;

		int dummy_iformat;
		uint32 dummy_format;
		uint8 dummy_channels;

		GetGLFormat( format, dummy_iformat, dummy_format, m_IndexType, dummy_channels );

		// バッファ生成
		glGenBuffers( 1, &m_IBO );

		// インデックスのセット
		GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IBO ) );
		GL_SAFE_CALL( glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_IndexDesc.buffer_size, pData, m_IndexDesc.usage ) );// 第二引数は、転送するデータのバイトサイズを指定
		GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) );

		return true;
	}



	bool GLVertexArrayObject::SetIndexBuffer( const Buffer *pBuffer, DATA_FORMAT format, GLenum usage )
	{
		m_numIndices	= pBuffer->numElms;

		m_IndexDesc.buffer_size	= static_cast<GLsizei>( m_numIndices * pBuffer->elmSize );
		//m_IndexDesc.target		= GL_ELEMENT_ARRAY_BUFFER;
		m_IndexDesc.usage		= usage;

		int dummy_iformat;
		uint32 dummy_format;
		uint8 dummy_channels;

		GetGLFormat( format, dummy_iformat, dummy_format, m_IndexType, dummy_channels );

		// バッファ生成
		GL_SAFE_CALL( glGenBuffers( 1, &m_IBO ) );

		// インデックスのセット
		GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IBO ) );
		GL_SAFE_CALL( glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_IndexDesc.buffer_size, pBuffer->pData, m_IndexDesc.usage ) );// 第二引数は、転送するデータのバイトサイズを指定
		GL_SAFE_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) );

		return true;
	}



	bool GLVertexArrayObject::Update()
	{
		//==================== m_VAOの初期化 ===================//

		// Bind VBO
		GL_SAFE_CALL( glBindBuffer( m_BufferDesc.target, m_VBO ) );

		// VAOに描画コールを登録する
		GL_SAFE_CALL( glBindVertexArray( m_VAO ) );	// VAOをバインド
		{
			for( int i=0; i < m_numDescs; ++i )
			{
				GLAttribDesc *pformat	= &m_VBDescs[i];
				GL_SAFE_CALL( glVertexAttribPointer( pformat->attribLocation, pformat->channels, pformat->type, pformat->bNormalized, pformat->stride, (GLvoid*)pformat->offset ) );
				GL_SAFE_CALL( glEnableVertexAttribArray( pformat->attribLocation ) );
			}
		}
		GL_SAFE_CALL( glBindVertexArray( 0 ) );	// VAOをアンバインド

		// Unbind VBO
		GL_SAFE_CALL( glBindBuffer( m_BufferDesc.target, 0 ) );
		
		return true;
	}


	bool GLVertexArrayObject::Release()
	{
		//#ifdef _DEBUG
		//	tcout << "GLVertexArrayObject::Release()..." << tendl;
		//#endif // _DEBUG

		if( m_VBO )
		{
			GL_SAFE_CALL( glDeleteBuffers( 1, &m_VBO ) );
			m_VBO	= 0;
		}

		if( m_VBDescs )
		{
			SafeDeleteArray( m_VBDescs );
			m_numDescs	= 0;
		}

		if( m_IBO )
		{
			GL_SAFE_CALL( glDeleteBuffers( 1, &m_IBO ) );
			m_IBO	= 0;
			m_numIndices = 0;
			m_IndexType = 0;
		}

		if( m_VAO )
		{
			GL_SAFE_CALL( glDeleteVertexArrays( 1, &m_VAO ) );
			m_VAO	= 0;
		}

		return true;
	}

}