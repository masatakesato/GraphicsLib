#ifndef	SCENE_GRAPH_H
#define	SCENE_GRAPH_H


#include	<list>
#include	"SceneNode.h"
#include	"MeshObject.h"
#include	"CameraObject.h"
#include	"LightObject.h"


namespace OreOreLib
{

	class SceneGraph
	{

		friend class RenderScene;
		//friend class SceneGraphManager;

	private:

		// Buffer
		std::list<VertexBuffer>		m_VertexBuffers;	// vertex buffer list
		std::list<IndexBuffer>		m_IndexBuffers;		// index buffer list
		std::list<Buffer>			m_Materials;		// material list
		std::list<TextureBuffer>	m_Textures;			// texture list
		std::list<FaceGroupBuffer>	m_FaceGroupBuffers;	// face group list

		std::list<Frustum>			m_Frustums;	// shadow casters

		// MovableObject
		std::list<MeshObject>		m_MeshObjects;
		std::list<CameraObject>		m_CameraObjects;
		std::list<LightObject>		m_LightObjects;

		// Node Graph
		SceneNode					*m_pMeshGroupTree;	// メッシュ形状の階層構造
		SceneNode					*m_pCameraTree;		// 
		SceneNode					*m_pLightTree;		// 光源ノード
		
		// グラフノードへのポインタ
		SceneNode					*m_pCurrentMeshNode;
		SceneNode					*m_pCurrentCameraNode;
		SceneNode					*m_pCurrentLightNode;


		// トラバース処理
		void Traverse_rec( Node *pNode );			// シーングラフのノードのトラバース
		void UpdateNodeTree_rec( Node *pNode );	// Nodeのツリー構造をトラバースして更新処理を実行する


	public:

		SceneGraph();
		~SceneGraph();
		

		// オブジェクトの追加. TODO: 後で整理する2013.08.05
		void AddMesh(MeshAttribute *attrib, VertexBuffer *vb, IndexBuffer *ib, Buffer *mb, TextureBuffer *tb, FaceGroupBuffer *fgb, const Vec3f& pos, const Vec3f& scale, const Quatf& rot);
		void AddChildMesh(MeshAttribute *attrib, VertexBuffer *vb, IndexBuffer *ib, Buffer *mb, TextureBuffer *tb, FaceGroupBuffer *fgb, const Vec3f& pos, const Vec3f& scale, const Quatf& rot);
		void AddCamera( FrustumAttribute *attrib );
		void AddLight( LightAttribute *lgt_attrib );

		SceneNode *AddMeshNode(SceneNode* pNode, MeshAttribute *attrib, VertexBuffer *vb, IndexBuffer *ib, Buffer *mb, TextureBuffer *tb, FaceGroupBuffer *fgb, const Vec3f& pos, const Vec3f& scale, const Quatf& rot);// pNodeの下に新しいメッシュオブジェクトを追加する
		SceneNode *AddMeshInstanceNode(SceneNode* pNode, const MeshObject*pSrc, MeshAttribute *attrib, Buffer *mb, TextureBuffer *tb, const Vec3f& pos, const Vec3f& scale, const Quatf& rot);// pNodeの下に新しいメッシュインスタンスを追加する


		// オブジェクトの削除
		void RemoveCurrentMesh();
		void RemoveCurrentCamera();

		// ノードの取得
		SceneNode*	GetCurrentMeshNode() const			{ return m_pCurrentMeshNode; };
		SceneNode*	GetCameraNode() const				{ return m_pCurrentCameraNode; };
		SceneNode*	GetCurrentLightNode() const			{ return m_pCurrentLightNode; };

		SceneNode*	GetRootMeshNode() const				{ return m_pMeshGroupTree; };
		SceneNode*	GetRootCameraNode() const			{ return m_pCameraTree; };
		SceneNode*	GetRootLightNode() const			{ return m_pLightTree; };


		// オブジェクトの取得
		MeshObject* GetCurrentMeshObject() const		{ return (MeshObject *)m_pCurrentMeshNode->m_pObject; };		// カレントメッシュノードの取得
		CameraObject* GetCurrentCameraObject() const	{ return (CameraObject *)m_pCurrentCameraNode->m_pObject; };	// カメラの取得
		LightObject* GetCurrentLightObject()	const	{ return (LightObject *)m_pCurrentLightNode->m_pObject; };	// カメラの取得


		// シーングラフのアップデート
		void Update();


		void MoveCurrentNode_Next()
		{
			if( m_pCurrentMeshNode->next )		m_pCurrentMeshNode = (SceneNode *)m_pCurrentMeshNode->next;
		}
		
		void MoveCurrentNode_Prev()
		{
			if( m_pCurrentMeshNode->prev )		m_pCurrentMeshNode = (SceneNode *)m_pCurrentMeshNode->prev;
		}
		
		void MoveCurrentNode_Child()
		{
			if( m_pCurrentMeshNode->children )		m_pCurrentMeshNode = (SceneNode *)m_pCurrentMeshNode->children;
		}

		void MoveCurrentNode_Parent()
		{
			if( m_pCurrentMeshNode->parent )		m_pCurrentMeshNode = (SceneNode *)m_pCurrentMeshNode->parent;
		}


		void Info();

		
		//friend class FbxLoader;
	};

}// end of namespace


#endif	// SCENE_GRAPH_H //