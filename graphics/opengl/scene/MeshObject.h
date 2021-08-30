#ifndef MESH_OBJECT_H
#define	MESH_OBJECT_H


#include	"MovableObject.h"



namespace OreOreLib
{


	//######################################################################//
	// MeshObject															//
	//######################################################################//
	struct MeshObject : MovableObject
	{
		MeshAttribute	m_MeshAttrib;
		RenderStats		m_RenderStats;

		VertexBuffer	*m_pVertexBuffer;	// pointer to vertex buffer
		IndexBuffer		*m_pIndexBuffer;	// pointer to index buffer
		Buffer			*m_pMaterial;		// pointer to material param
		TextureBuffer	*m_pTexture;		// pointer to texture param
FaceGroupBuffer* m_pFaceGroupBuffer;	// pointer to facegroup buffer
		AABB			m_ObjectSpaceBoundingBox;// Local Space Bounding Box

		MeshObject();
		MeshObject( MeshAttribute *attrib, VertexBuffer *vb, IndexBuffer *ib, Buffer *mb, TextureBuffer *tb, FaceGroupBuffer* fgb );
		~MeshObject();

		void AttachBuffer( MeshAttribute *attrib, VertexBuffer *vb, IndexBuffer *ib, Buffer *mb, TextureBuffer *tb, FaceGroupBuffer* fgb );
		void DetachBuffer();


		PRIMITIVE_TYPE GetPrimitiveType() const		{ return m_MeshAttrib.PrimitiveType; }

		void SetVisible( bool visible )				{ m_RenderStats.Visible = visible; }
		void SetCastShadow( bool castshadow )		{ m_RenderStats.CastShadow = castshadow; }
		void SetRecieveShadow( bool recieveshadow )	{ m_RenderStats.RecieveShadow = recieveshadow; }

		//inline void SetScale( const Vec3f &scale )		{ m_pParent->SetScale( scale );		}
		//inline void SetRotation( const Mat4f &rot )		{ m_pParent->SetRotation( rot );	}
		//inline void SetQuaternion( const Quatf &quat)	{ m_pParent->SetQuaternion( quat ); }
		//inline void SetTranslation( const Vec3f& trans ){ m_pParent->SetTranslation( trans ); }

		//virtual void Info();

		//virtual void Update()
		//{
		//};// MeshObjectに関しては余計な時にUpdate()入ってほしくない

	};


}// end of namespace



#endif // !MESH_OBJECT_H
