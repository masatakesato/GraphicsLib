#include	"MeshObject.h"


namespace OreOreLib
{

	MeshObject::MeshObject()
	{
#ifdef _DEBUG
		tcout << _T("MeshObject::MeshObject()") << tendl;
#endif
		//m_NodeType		= OBJ_MESH;

		m_pVertexBuffer				= NULL;
		m_pIndexBuffer				= NULL;
		m_pMaterial					= NULL;
		m_pTexture					= NULL;
		m_pFaceGroupBuffer			= NULL;

		m_MeshAttrib.PrimitiveType	= PRIMITIVE_TYPE::PRIM_UNKNOWN;
		m_MeshAttrib.ShaderType		= SHADER_SIMPLEMESH;
		m_MeshAttrib.Instance		= false;

		m_RenderStats.Visible		= true;
		m_RenderStats.CastShadow	= false;
		m_RenderStats.ReceiveShadow	= false;
	}


	MeshObject::MeshObject( MeshAttribute *attrib, VertexBuffer *vb, IndexBuffer *ib, Buffer *mb, TextureBuffer *tb, FaceGroupBuffer* fgb )
	{
		//tcout << "MeshObject::MeshObject( VertexBuffer *vb, IndexBuffer *ib, SceneNode *tn, MaterialBuffer *mb, TextureBuffer *tb, FaceGroupBuffer* fgb  )" << endl;
		//m_NodeType	= OBJ_MESH;
		AttachBuffer( attrib, vb, ib, mb, tb, fgb );
	}


	MeshObject::~MeshObject()
	{
#ifdef _DEBUG
		tcout << _T("MeshObject::~MeshObject()") << tendl;
#endif
		DetachBuffer();
	}


	void MeshObject::AttachBuffer( MeshAttribute *attrib, VertexBuffer *vb, IndexBuffer *ib, Buffer *mb, TextureBuffer *tb, FaceGroupBuffer* fgb )
	{
		if( attrib )	m_MeshAttrib	= *attrib;

		m_pVertexBuffer		= vb;
		m_pIndexBuffer		= ib;
		m_pMaterial			= mb;
		m_pTexture			= tb;
		m_pFaceGroupBuffer	= fgb;
	}


	void MeshObject::DetachBuffer()
	{
		m_pVertexBuffer		= NULL;
		m_pIndexBuffer		= NULL;
		m_pMaterial			= NULL;
		m_pTexture			= NULL;
		m_pFaceGroupBuffer	= NULL;
	}


	/*
	void MeshObject::Info()
	{


	}
	*/


}// end of namespace