#include	"GeometryBuffer.h"



namespace OreOreLib
{

	GeometryBuffer::GeometryBuffer()
	{
		m_refObjectTransform	= NULL;
		m_refMaterial			= NULL;
		m_refTexture			= NULL;
		m_refFaceGroup			= NULL;
	}



	GeometryBuffer::~GeometryBuffer()
	{
		Release();
	}



	void GeometryBuffer::Init( PRIMITIVE_TYPE primitive_type, const Buffer* pverts, const Buffer* pindices, TransformMatrices* pObjTrans, Buffer* pMat, TextureBuffer* pTex, FaceGroupBuffer* pFaceGroup, GLenum vertusage, GLenum idxusage )
	{
		const InputElementDesc layout[] =
		{
			{ ATTRIB_LOCATION_POSITION, FORMAT_R32G32B32_FLOAT, GL_FALSE, sizeof(VertexLayout), 0 },	// POSIAION
			{ ATTRIB_LOCATION_NORMAL, FORMAT_R32G32B32_FLOAT, GL_FALSE, sizeof(VertexLayout), 12 },	// NORMAL
			{ ATTRIB_LOCATION_TEXCOORD, FORMAT_R32G32_FLOAT, GL_FALSE, sizeof(VertexLayout), 24 },		// TEXCOORD
			{ ATTRIB_LOCATION_COLOR, FORMAT_R32G32B32_FLOAT, GL_FALSE, sizeof(VertexLayout), 32 },	// COLOR
		};
			
		m_VAO.Init();
		m_VAO.SetVertexAttributes( layout, ArraySize<int>( layout ), primitive_type );
		m_VAO.SetVertexBuffers( pverts, GL_ARRAY_BUFFER, vertusage );
		m_VAO.SetIndexBuffer( pindices, DATA_FORMAT::FORMAT_R32_UINT, idxusage );
		m_VAO.Update();

		m_refObjectTransform	= pObjTrans;
		m_refMaterial			= pMat;
		m_refTexture			= pTex;
		m_refFaceGroup			= pFaceGroup;
	}


	void GeometryBuffer::Release()
	{
		m_VAO.Release();
	
		m_refObjectTransform	= NULL;
		m_refMaterial			= NULL;
		m_refTexture			= NULL;
		m_refFaceGroup			= NULL;
	}







}// end of namespace