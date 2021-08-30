#include	"SceneGraph.h"

//#include	"Defines.h"

#include	<iostream>
using namespace std;


namespace OreOreLib
{

	SceneGraph::SceneGraph()
	{
#ifdef _DEBUG
tcout << _T("SceneGraph()") << tendl;
#endif // _DEBUG

		//============ Initialize Root MeshNode ==============//
		m_pMeshGroupTree	= new SceneNode();//m_pMeshGroupTree	= new MeshNode();
		m_pCurrentMeshNode		= m_pMeshGroupTree;
		
		//============== Initialize CameraList ===============//
		//m_ViewMatrices.push_back( ViewTransformMatrix() );
		m_pCameraTree		= new SceneNode();	
		m_pCurrentCameraNode	= m_pCameraTree;

		//============== Initialize LightList ================//
		m_pLightTree		= new SceneNode();	// create root node
		m_pCurrentLightNode		= m_pLightTree;


		//============== Attach Default Material =============//
		m_Materials.push_back( Buffer() );

		MaterialBuffer *pMat	= (MaterialBuffer *)&m_Materials.back();
		AllocateMaterialBuffer( pMat, 1, MATERIAL_TYPE_OPAQUE );
		
		InitVec( *pMat->Ambient(), 0.0f, 0.0f, 0.0f );
		InitVec( *pMat->Diffuse(), 0.5f, 0.5f, 0.5f );
		InitVec( *pMat->Specular(), 0.2f, 0.2f, 0.2f );
		*pMat->RefractiveIndex()	= 0.0f;
		*pMat->Shininess()			= 50.0f;
		*pMat->Opacity()			= 1.0f;
		InitVec( *pMat->TransparentColor(), 0.0f, 0.0f, 0.0f );

	}



	SceneGraph::~SceneGraph()
	{
#ifdef _DEBUG
tcout << _T("~SceneGraph()") << tendl;
#endif // _DEBUG

		//================= 参照の解除 ==================//
		m_pCurrentMeshNode		= NULL;
		m_pCurrentCameraNode	= NULL;
		m_pCurrentLightNode		= NULL;

		//============== 階層グラフを削除 ===============//
		SafeDelete( m_pMeshGroupTree );
		SafeDelete( m_pCameraTree );
		SafeDelete( m_pLightTree );

		//============= MovableObjectを削除 =============//
		m_MeshObjects.clear();
		m_CameraObjects.clear();
		m_LightObjects.clear();

		//================ データを削除 =================//
		// Delete Vertex Buffer
		list<VertexBuffer>::iterator itr_vb	= m_VertexBuffers.begin();
		while( itr_vb != m_VertexBuffers.end())
		{
			ReleaseVertexBuffer( &(*itr_vb) );
			++itr_vb;
		}
		m_VertexBuffers.clear();
		
		// Delete Index Buffer
		list<IndexBuffer>::iterator itr_ib	= m_IndexBuffers.begin();
		while( itr_ib != m_IndexBuffers.end())
		{
			ReleaseIndexBuffer( &(*itr_ib) );
			++itr_ib;
		}
		m_IndexBuffers.clear();


		// Delete Material Buffer
		list<Buffer>::iterator itr_mb	= m_Materials.begin();
		while( itr_mb != m_Materials.end() )
		{
			ReleaseBuffer( &(*itr_mb) );
			++itr_mb;
		}
		m_Materials.clear();

		// Detele Texture Buffer
		list<TextureBuffer>::iterator itr_tb	= m_Textures.begin();
		while( itr_tb != m_Textures.end())
		{
			ReleaseTextureBuffer( &(*itr_tb) );
			++itr_tb;
		}
		m_Textures.clear();
		
		// Delete FaceGroup Buffers
		list<FaceGroupBuffer>::iterator it_fgb = m_FaceGroupBuffers.begin();
		while (it_fgb != m_FaceGroupBuffers.end())
		{
			ReleaseFaceGroupBuffer(&(*it_fgb));
			++it_fgb;
		}
		m_FaceGroupBuffers.clear();


	}


	
	void SceneGraph::AddMesh( MeshAttribute *attrib,
		VertexBuffer *vb, IndexBuffer *ib, Buffer *mb, TextureBuffer *tb, FaceGroupBuffer *fgb,
		const Vec3f& pos, const Vec3f& scale, const Quatf& rot )
	{
		//========================= データをコピー ==============================//
		// copy vertex buffer
		if( vb )
		{
			m_VertexBuffers.push_back( VertexBuffer() );
			CopyVertexBuffer( &m_VertexBuffers.back(), vb );
		}

		// copy index buffer
		if( ib )
		{
			m_IndexBuffers.push_back( IndexBuffer() );
			CopyIndexBuffer( &m_IndexBuffers.back(), ib );
		}

		// copy material buffer
		if( mb )
		{
			m_Materials.push_back( MaterialBuffer() );
			CopyBuffer( &m_Materials.back(), mb );//CopyMaterialBuffer( &m_Materials.back(), mb );
		}

		// copy texture buffer
		if( tb )
		{
			m_Textures.push_back( TextureBuffer() );
			CopyTextureBuffer( &m_Textures.back(), tb );
		}

		// copy facegroup buffer
		if( fgb )
		{
			m_FaceGroupBuffers.push_back( FaceGroupBuffer() );
			CopyFaceGroupBuffer( &m_FaceGroupBuffers.back(), fgb );
		}
		

		m_pCurrentMeshNode	= (SceneNode *)m_pMeshGroupTree->AddChild( /*trans_attr*/ );


		m_MeshObjects.push_back( MeshObject() );
		MeshObject *pMesh = &m_MeshObjects.back();
		//pMesh->SetTransformAttribute( trans_attr );
		pMesh->SetTranslation( pos );
		pMesh->SetScale( scale );
		pMesh->SetQuaternion( rot );
		pMesh->AttachBuffer( attrib,
							(vb) ? &m_VertexBuffers.back() : NULL,
							(ib) ? &m_IndexBuffers.back() : NULL,
							(mb) ? &m_Materials.back() : &m_Materials.front(),
							(tb) ? &m_Textures.back() : NULL,
							(fgb) ? &m_FaceGroupBuffers.back() : NULL
							);

		m_pCurrentMeshNode->AttachObject( pMesh );

	//MaterialBuffer *pmat = (MaterialBuffer *)&m_Materials.front();
	//tcout << *pmat->Shininess() << endl;
	}


	SceneNode* SceneGraph::AddMeshNode( SceneNode* pNode, MeshAttribute *attrib,
		VertexBuffer *vb, IndexBuffer *ib, Buffer *mb, TextureBuffer *tb, FaceGroupBuffer *fgb,
		const Vec3f& pos, const Vec3f& scale, const Quatf& rot )// pNodeの下に新しいメッシュオブジェクトを追加する
	{
		SceneNode *pChild	= NULL;

		//========================= データをコピー ==============================//
		// copy vertex buffer
		m_VertexBuffers.push_back( VertexBuffer() );
		m_VertexBuffers.back().pData	= NULL;

		// copy index buffer
		m_IndexBuffers.push_back( IndexBuffer() );
		m_IndexBuffers.back().pData	= NULL;

		// copy material buffer
		m_Materials.push_back( MaterialBuffer() );
		m_Materials.back().pData	= NULL;

		// copy texture buffer
		m_Textures.push_back( TextureBuffer() );
		m_Textures.back().pData	= NULL;

		// copy facegroup buffer
		m_FaceGroupBuffers.push_back(FaceGroupBuffer());
		CopyFaceGroupBuffer( &m_FaceGroupBuffers.back(), fgb );


		if( vb )	CopyVertexBuffer( &m_VertexBuffers.back(), vb );
		if( ib )	CopyIndexBuffer( &m_IndexBuffers.back(), ib );
		if( mb )	CopyBuffer( &m_Materials.back(), mb );
		if( tb )	CopyTextureBuffer( &m_Textures.back(), tb );
		if (fgb)	CopyFaceGroupBuffer( &m_FaceGroupBuffers.back(), fgb );
	

		//======================= メッシュ用ノードの作成 ======================//
		pChild	= (SceneNode *)pNode->AddChild( /*trans_attr*/ );
		m_pCurrentMeshNode	= pChild;//(SceneNode *)m_pCurrentMeshNode->AddChild( trans_attr );

		cout << " New Mesh: " << m_pCurrentMeshNode << endl;

		//====================== メッシュオブジェクトの作成 ==================//
		m_MeshObjects.push_back( MeshObject() );
		MeshObject *pMesh = &m_MeshObjects.back();
		//pMesh->SetTransformAttribute( trans_attr );
		pMesh->SetTranslation( pos );
		pMesh->SetScale( scale );
		pMesh->SetQuaternion( rot );
		pMesh->AttachBuffer( attrib, &m_VertexBuffers.back(), &m_IndexBuffers.back(), &m_Materials.back(), &m_Textures.back(), &m_FaceGroupBuffers.back() );


		//================= ノードにメッシュオブジェクトをアタッチ ===========//
		pChild->AttachObject( pMesh );
		//m_pCurrentMeshNode->AttachObject( pMesh );

		// 親子の相対スケーリングを実施
		pChild->InheritTransform();
		//m_pCurrentMeshNode->InheritTransform();

		return pChild;
	}



	SceneNode* SceneGraph::AddMeshInstanceNode( SceneNode* pNode, const MeshObject*pSrc, MeshAttribute *attrib, Buffer *mb, TextureBuffer *tb,
		const Vec3f& pos, const Vec3f& scale, const Quatf& rot )// pNodeの下に新しいメッシュオブジェクトを追加する
	{
		SceneNode *pChild	= NULL;

		//========================= データをコピー ==============================//
		VertexBuffer *pVB		= NULL;
		IndexBuffer *pIB		= NULL;
		Buffer *pMB				= NULL;
		TextureBuffer *pTB		= NULL;
		FaceGroupBuffer *pFGB	= NULL;
		
		// 頂点バッファはpSrcを流用
		pVB		= pSrc->m_pVertexBuffer;

		// インデックスバッファもpSrcを流用
		pIB		= pSrc->m_pIndexBuffer;

		// フェースグループバッファもpSrcを流用
		pFGB	= pSrc->m_pFaceGroupBuffer;

		// copy material buffer
		if( mb )// 引数でマテリアルを指定した場合は、新しく確保する
		{
			m_Materials.push_back( MaterialBuffer() );
			pMB	= &m_Materials.back();
			CopyBuffer( pMB, mb );
		}
		else
		{
			pMB	= pSrc->m_pMaterial;
		}

		// copy texture buffer
		if( tb )// 引数でテクスチャを指定した場合は、新しく確保する
		{
			m_Textures.push_back( TextureBuffer() );
			pTB	= &m_Textures.back();
			CopyTextureBuffer( pTB, tb );
		}
		else
		{
			pTB	= pSrc->m_pTexture;
		}


		//======================= メッシュ用ノードの作成 ======================//
		pChild	= (SceneNode *)pNode->AddChild( /*trans_attr*/ );
		m_pCurrentMeshNode	= pChild;//(SceneNode *)m_pCurrentMeshNode->AddChild( trans_attr );

		cout << " New Mesh: " << m_pCurrentMeshNode << endl;

		//====================== メッシュオブジェクトの作成 ==================//
		m_MeshObjects.push_back( MeshObject() );
		MeshObject *pMesh = &m_MeshObjects.back();
		//pMesh->SetTransformAttribute( trans_attr );
		pMesh->SetTranslation( pos );
		pMesh->SetScale( scale );
		pMesh->SetQuaternion( rot );
		pMesh->AttachBuffer( attrib, pVB, pIB, pMB, pTB, pFGB );


		//================= ノードにメッシュオブジェクトをアタッチ ===========//
		pChild->AttachObject( pMesh );
		//m_pCurrentMeshNode->AttachObject( pMesh );

		// 親子の相対スケーリングを実施
		pChild->InheritTransform();
		//m_pCurrentMeshNode->InheritTransform();

		return pChild;
	}



	void SceneGraph::AddChildMesh(MeshAttribute *attrib, VertexBuffer *vb, IndexBuffer *ib, Buffer *mb, TextureBuffer *tb, FaceGroupBuffer *fgb,
		const Vec3f& pos, const Vec3f& scale, const Quatf& rot )
	{
		//========================= データをコピー ==============================//
		// copy vertex buffer
		if( vb )
		{
			m_VertexBuffers.push_back( VertexBuffer() );
			CopyVertexBuffer( &m_VertexBuffers.back(), vb );
		}

		// copy index buffer
		if( ib )
		{
			m_IndexBuffers.push_back( IndexBuffer() );
			CopyIndexBuffer( &m_IndexBuffers.back(), ib );
		}

		// copy material buffer
		if( mb )
		{
			m_Materials.push_back( MaterialBuffer() );
			CopyBuffer( &m_Materials.back(), mb );
		}

		// copy texture buffer
		if( tb )
		{
			m_Textures.push_back( TextureBuffer() );
			CopyTextureBuffer( &m_Textures.back(), tb );
		}
		
		// copy facegroup buffer
		if(fgb)
		{
			m_FaceGroupBuffers.push_back( FaceGroupBuffer() );
			CopyFaceGroupBuffer( &m_FaceGroupBuffers.back(), fgb );
		}


		//======================= メッシュ用ノードの作成 ======================//
		m_pCurrentMeshNode	= (SceneNode *)m_pCurrentMeshNode->AddChild();

		cout << " New Mesh: " << m_pCurrentMeshNode << endl;

		//====================== メッシュオブジェクトの作成 ==================//
		m_MeshObjects.push_back( MeshObject() );
		MeshObject *pMesh = &m_MeshObjects.back();
		//pMesh->SetTransformAttribute( trans_attr );
		pMesh->SetTranslation( pos );
		pMesh->SetScale( scale );
		pMesh->SetQuaternion( rot );
		pMesh->AttachBuffer( attrib,
							(vb) ? &m_VertexBuffers.back() : NULL,
							(ib) ? &m_IndexBuffers.back() : NULL,
							(mb) ? &m_Materials.back() : &m_Materials.front(),
							(tb) ? &m_Textures.back() : NULL,
							(fgb) ? &m_FaceGroupBuffers.back() : NULL
							);

		//================= ノードにメッシュオブジェクトをアタッチ ===========//
		m_pCurrentMeshNode->AttachObject( pMesh );

// 親子の相対スケーリングを実施
m_pCurrentMeshNode->InheritTransform();

	}



	void SceneGraph::AddCamera( FrustumAttribute *attrib )
	{
		// Allocate Frustum for Camera
		m_Frustums.push_back( Frustum() );

		// Allocate CameraObject
		m_CameraObjects.push_back( CameraObject( m_Frustums.back() ) );
		CameraObject *pCamera	= &m_CameraObjects.back();

		if( attrib )	pCamera->SetAttribute( *attrib );

		// Allocate Node for CameraObject
		m_pCurrentCameraNode	= (SceneNode *)m_pCameraTree->AddChild();
		m_pCurrentCameraNode->AttachObject( pCamera );

	}



	void SceneGraph::AddLight( LightAttribute *lgt_attrib )
	{
		// Acclloate Frustum for Light
		m_Frustums.push_back( Frustum() );

		// Allocate LightObject		
		m_LightObjects.push_back( LightObject( lgt_attrib ) );
		LightObject *pLight = &m_LightObjects.back();
		pLight->BindFrustum( &m_Frustums.back() );	// bind frustum

		// Allocate Node for LightObject
		m_pCurrentLightNode	= (SceneNode *)m_pLightTree->AddChild();
		m_pCurrentLightNode->AttachObject( pLight );
	}



	void SceneGraph::RemoveCurrentMesh()
	{
		Node *tmp;

		//============ m_pCurrentMeshNode以下の階層のMeshNodeを全て削除 =================//
		tmp	= m_pCurrentMeshNode;
		
		if(m_pCurrentMeshNode->next)		m_pCurrentMeshNode	= (SceneNode *)m_pCurrentMeshNode->next;
		else if(m_pCurrentMeshNode->prev)	m_pCurrentMeshNode	= (SceneNode *)m_pCurrentMeshNode->prev;
		else							m_pCurrentMeshNode	= (SceneNode *)m_pCurrentMeshNode->parent;

		if( tmp != tmp->parent )	SafeDelete( tmp );
	}
	


	void SceneGraph::Update()
	{
		//=========== Update SceneNode ===============//
		UpdateNodeTree_rec( m_pMeshGroupTree );//		UpdateNodeTree_rec( m_pTransformTree );//UpdateTransformTree_rec( m_pTransformTree );		

		//=============== Update Current Camera ==============//
		m_pCurrentCameraNode->Update();// UpdateNodeTree_rec( m_pCameraTree );


		//============== Update LightNode ================//
		UpdateNodeTree_rec( m_pLightTree );

		//=========== Update Camera Matrix ==============//
		//UpdateNodeTree_rec( m_pCameraTree );
//		((CameraObject *)m_pCurrentCameraNode->m_pObject)->Update();
		
		//((CameraNode *)m_pCurrentCameraNode)->Update();
		
		
		//Node *pcamera = m_pCameraTree;
		//while( pcamera != NULL )
		//{
		//	((CameraNode *)pcamera)->Update();
		//	pcamera	= pcamera->next;
		//}

	}



	void SceneGraph::Traverse_rec( Node *pNode )
	{
		if(pNode == NULL)	return;
	
		pNode->Info();

		Node *curr = pNode;

		Traverse_rec(curr->children);
		Traverse_rec(curr->next);
	}
	
	
	void SceneGraph::UpdateNodeTree_rec( Node *pNode )
	{
		if(pNode == NULL)	return;
	
		//SceneNode *curr		= (SceneNode *)pNode;
		
		if( !pNode->IsRoot() )	pNode->Update();
		
		UpdateNodeTree_rec( pNode->children );
		UpdateNodeTree_rec( pNode->next );
	}












}// end of namespace