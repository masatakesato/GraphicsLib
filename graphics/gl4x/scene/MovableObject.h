#ifndef	I_OBJECT_H
#define	I_OBJECT_H


#include	<oreore/mathlib/GraphicsMath.h>

#include	"../resource/BufferLayout.h"
#include	"../scene/Node.h"
#include	"../scene/BoundingVolume.h"
#include	"../scene/Frustum.h"
#include	"../resource/IBufferObject.h"
#include	"../resource/IGLBufferObject.h"
#include	"../scene/TransformAttribute.h"



namespace OreOreLib
{

	// オブジェクトのタイプ
	enum OBJECT_TYPE
	{
		OBJ_UNKNOWN	= -1,
		OBJ_GROUP		= 0,
		OBJ_TRANSFORM	= 1,
		OBJ_MESH		= 2,
		OBJ_CAMERA		= 3,
		OBJ_LIGHT		= 4,
		OBJ_MATERIAL	= 5,
		NUM_OBJ_TYPE,
	};


	


	//######################################################################//
	// MovableObject														//
	//######################################################################//
	class MovableObject
	{
	public:

		MovableObject()
		{
			m_pParent = NULL;
			m_pBuffer = NULL;
			m_BufferID = -1;

			////MatIdentity( m_TransformAttrib.Rotation );
			//QuatIdentity( m_TransformAttrib.Quaternion );

			//m_DerivedTransform	= m_TransformAttrib;

			//m_DerivedSpace	= m_LocalSpace;

			MatIdentity( m_TransformMatrix.matLocal );
			MatIdentity( m_TransformMatrix.matWorld );
		}

		virtual ~MovableObject()
		{
			m_pParent = NULL;
			m_pBuffer = NULL;
			m_BufferID = -1;
		}

		void AttachNode( Node *pnode )								{ m_pParent = pnode; }
		void DetachNode()											{ m_pParent = NULL; }
		void AttachBufferObject( IBufferObject *pbuffer )			{ m_pBuffer = pbuffer; }
		void DetachBufferObject()									{ m_pBuffer = NULL; }

		void SetBufferID( int id )									{ m_BufferID = id; }
		int GetBufferID() const										{ return m_BufferID; }

		template< typename T >
		inline T* GetBuffer() const									{ return reinterpret_cast<T*>(m_pBuffer); }
		inline TransformAttribute* GetTransformAttribute() const	{ return (TransformAttribute *)&m_LocalSpace; }
		inline TransformAttribute* GetDerivedTransform() const		{ return (TransformAttribute *)&m_DerivedSpace; }
		inline TransformMatrices* GetTransformMatrix() const		{ return (TransformMatrices *)&m_TransformMatrix;  }

		inline AABB* WorldSpaceAABB() const							{ return (AxisAlignedBoundingBox *)&m_WorldSpaceAABB; }


		inline void SetTransformAttribute( const Vec3f& pos, const Vec3f& scale, const Quatf& rot )
		{	
			m_LocalSpace.SetOrigin( pos );
			m_LocalSpace.SetScale( scale );
			m_LocalSpace.SetOrientation( rot );
		}


		inline void SetScale( const Vec3f &scale )
		{
			m_LocalSpace.SetScale( scale );
		}

		inline void SetScale( float x, float y, float z )
		{
			m_LocalSpace.SetScale( x, y, z );
		}

		//inline void SetRotation( const Mat4f &rot )
		//{
		//	m_TransformAttrib.Rotation	= rot;
		//}

		inline void SetQuaternion( const Quatf &quat)
		{
			m_LocalSpace.SetOrientation( quat );
		}

		inline void SetTranslation( const Vec3f& trans )
		{
			m_LocalSpace.SetOrigin( trans );
		}

		inline void SetTranslation( float x, float y, float z )
		{
			m_LocalSpace.SetOrigin( x, y, z );
		}

		//inline void  SetBoundary( const Vec3f& bbmin, const Vec3f& bbmax )
		//{
		//	m_ObjectSpaceBoundingBox.SetMinMax( bbmin, bbmax );
		//}

		virtual void Update(){};


	protected:

		Node				*m_pParent;// 本オブジェクトが所属するNodeへのポインタ
		IBufferObject		*m_pBuffer;	// ハードウェアバッファ
		int					m_BufferID;	// ハードウェアバッファのID
		
		TransformAttribute	m_LocalSpace;		// シーンオブジェクト自体のローカル座標空間
		TransformAttribute	m_DerivedSpace;		// 親オブジェクトから継承した座標空間
		TransformMatrices	m_TransformMatrix;	// transformation matrices (local and world)
		AABB				m_WorldSpaceAABB;

		friend struct SceneNode;
	};


}// end of namespace



#endif	// MOVABLE_OBJECT_H //