#ifndef SCENE_NODE_H
#define	SCENE_NODE_H

#include	<vector>
#include	<iostream>
using namespace std;

#include	<oreore/mathlib/GraphicsMath.h>

#include	"../resource/BufferLayout.h"
#include	"../resource/IBufferObject.h"
#include	"MovableObject.h"



namespace OreOreLib
{
	// ノードの定義
//	struct	Node;
//	struct	SceneNode;
//	struct	CameraNode;


	// ノードのタイプ
	enum NODE_TYPE
	{
		NODE_UNKNOWN	= -1,
		NODE_GROUP		= 0,
		NODE_TRANSFORM	= 1,
		NODE_MESH		= 2,
		NODE_CAMERA		= 3,
		NODE_LIGHT		= 4,
		NODE_MATERIAL	= 5,
		NUM_NODE_TYPE,
	};

	



	//######################################################################//
	// SceneNode (groupNode)												//
	//######################################################################//
	struct SceneNode : Node
	{
		//bool UpdateNode;
		MovableObject		*m_pObject;

		bool				m_bInheritScale;			// flag whether or not this node inherits parent's scale
		bool				m_bInheritOrientation;	// flag whether or not this node inherits parent's orientation
		//TransformMatrices		m_TransformMatrix;		// transformation matrices (local and world)

		SceneNode();
		~SceneNode();

		void AttachObject( MovableObject *obj )		{ m_pObject = obj; m_pObject->AttachNode(this); }
		void DetachObject()							{ m_pObject->DetachNode(); m_pObject = NULL; }
		MovableObject* GetAttachedObject()			{ return m_pObject; }
//		TransformMatrices* GetTransformMatrix() const	{ return (TransformMatrices *)&m_TransformMatrix; }

		Node* AddChild();
		Node* AddSibling();

		virtual void Update();
		virtual void Info();

		void SetInheritScale( bool flag );		// set scale inheritance flag
		void SetInheritOrientation( bool flag );// set orientation inheritance flag 
		void InheritTransform();

	};



}// end of namespace



#endif	// SCENE_NODE_H //