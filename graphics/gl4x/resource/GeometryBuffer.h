#ifndef	GEOMETRY_BUFFER_H
#define	GEOMETRY_BUFFER_H


#include	"IBufferObject.h"
#include	"GLUniformBufferObject.h"
#include	"GLVertexArrayObject.h"



namespace OreOreLib
{


	class GeometryBuffer : public IBufferObject
	{
	public:

		GeometryBuffer();
		~GeometryBuffer();

		void Init( PRIMITIVE_TYPE primitive_type, const Buffer* pverts, const Buffer* pindices, TransformMatrices* pObjTrans, Buffer* pMat, TextureBuffer* pTex, FaceGroupBuffer* pFaceGroup, GLenum vertusage = GL_STATIC_DRAW, GLenum idxusage = GL_STATIC_DRAW );
		void Release();

		int NumFaceGroups() const							{ return m_refFaceGroup ? m_refFaceGroup->numElms : 1; }
		TransformMatrices *GetTransform() const				{ return m_refObjectTransform; }
		Buffer *GetMaterial() const							{ return m_refMaterial; }
		TextureBuffer *GetTexure() const					{ return m_refTexture;  }
		FaceGroupBuffer* GetFaceGroup() const				{ return m_refFaceGroup; }
		GLVertexArrayObject *GetVertexArrayObject() const	{ return (GLVertexArrayObject *)&m_VAO; }

		inline void Bind() const							{ m_VAO.Bind(); }
		inline void Unbind() const							{ m_VAO.Unbind(); }

		inline void Draw() const
		{
			m_VAO.Draw();
		}
		
		inline void DrawGroup( int grpidx ) const
		{
			if(!m_refFaceGroup)
				return;

			const Vec2ui *pFaceGroup = m_refFaceGroup->FaceGroup( grpidx );
			m_VAO.Draw( (unsigned int *)( pFaceGroup->x ), pFaceGroup->y );
		}

		inline void DrawGroups() const
		{
			if(!m_refFaceGroup)
				return;

			for( int i = 0; i < m_refFaceGroup->numElms; ++i )
			{
				const Vec2ui *pFaceGroup = m_refFaceGroup->FaceGroup( i );
				m_VAO.Draw( (unsigned int *)( pFaceGroup->x ), pFaceGroup->y );
			}
		}
		


	private:

		TransformMatrices			*m_refObjectTransform;
		Buffer					*m_refMaterial;
		TextureBuffer			*m_refTexture;
		FaceGroupBuffer			*m_refFaceGroup;
		GLVertexArrayObject		m_VAO;


	};




}// end of namespace


#endif	// GEOMETRY_BUFFER_H //