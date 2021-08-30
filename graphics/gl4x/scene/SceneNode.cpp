#include	"SceneNode.h"



namespace OreOreLib
{


	//######################################################################//
	// SceneNode (groupNode)												//
	//######################################################################//
	SceneNode::SceneNode()
	{
		m_NodeType				= NODE_TRANSFORM;
		m_bInheritScale			= true;
		m_bInheritOrientation	= true;
		//UpdateNode	= true;
		m_pObject				= NULL;
	}



	SceneNode::~SceneNode()
	{
//cout << "SceneNode::~SceneNode()" << endl;
		m_pObject = NULL;
	}



	Node* SceneNode::AddChild()
	{
		if(children==NULL)	// まだ子ノードがいない場合
		{
			children			= new SceneNode();
			children->m_ID		= 0;
			children->parent	= this;

			m_numChildren++;

			return children;
		}
		else				// 既に子ノードがいる場合は最後尾に追加
		{
			Node	*psibling	= children;

			while(psibling->next != NULL)
				psibling = psibling->next;

			psibling->next			= new SceneNode();
			psibling->next->m_ID	= psibling->m_ID+1;
			psibling->next->prev	= psibling;
			psibling->next->parent	= psibling->parent;

			m_numChildren++;
			
			return psibling->next;
		}
		
	}


	Node* SceneNode::AddSibling()
	{
		Node *psibling	= this;
			
		// go to the last node
		while( psibling->next != NULL )
			psibling = psibling->next;

		// initialize node
		psibling->next			= new SceneNode();
		psibling->next->prev	= this;
		psibling->next->parent	= parent;

		// change parent node
		if(parent)	parent->m_numChildren++;

		return psibling->next;
	}

		

	void SceneNode::Update()
	{
		if( !m_pObject )	return;

		TransformAttribute	*pLocalSpace		= &m_pObject->m_LocalSpace;
		TransformAttribute	*pDerivedSpace		= &m_pObject->m_DerivedSpace;
		TransformMatrices *pMatTrans			= &m_pObject->m_TransformMatrix;

		TransformMatrices	*pParentMatTrans	= &((SceneNode *)parent)->m_pObject->m_TransformMatrix;


		//======================= ローカル変形行列の計算 ========================//
		Mat4f matScale, matRot, matRotScale, matTrans;

		// スケーリング行列を計算
		MatScale( matScale, pLocalSpace->m_Scale );//MatScale( matScale, pDerivedSpace->m_Scale );
//tcout << "RelativeScale =" << m_TransformAttrib.Scale.x << ", " << m_TransformAttrib.Scale.y << ", " << m_TransformAttrib.Scale.z << endl;
//tcout << "DerivedScale =" << m_DerivedTransform.Scale.x << ", " << m_DerivedTransform.Scale.y << ", " << m_DerivedTransform.Scale.z << endl;

		// 回転行列*スケーリング行列を計算
		Quat2Mat( matRot, pLocalSpace->m_Quaternion );
		Multiply( matRotScale, matRot, matScale );

		// 平行移動行列を作成する(スケール/回転後の移動)
		MatTranslation( matTrans, /*pDerivedSpace->m_Origin*/pLocalSpace->m_Origin );


		//======================= TransformMatricesの更新 =========================//
		Multiply( pMatTrans->matLocal, matTrans, matRotScale );
			
		if( parent->IsRoot() )
		{
			pMatTrans->matWorld = pMatTrans->matLocal;
		}
		else
		{
			// current matWorld = parent's matWorld * current matLocal
			Multiply( pMatTrans->matWorld, pParentMatTrans->matWorld, pMatTrans->matLocal );
		}


		//======================= DerivedSpaceの更新	=========================//
		pDerivedSpace->Transform( pMatTrans->matWorld );// 親ノード～自ノードまでの全てを含めたトランスフォーム行列

		//if(parent->IsRoot())
		//	*pDerivedSpace	= *pLocalSpace;
		//else
		//	pDerivedSpace->Transform( pParentMatTrans->matWorld );

		//=======================	MovableObjectの更新	==========================//
		m_pObject->Update();
	}



	void SceneNode::Info()
	{


	}
		



	void SceneNode::SetInheritScale( bool flag )
	{
		m_bInheritScale			= flag;
	}
	
	
	void SceneNode::SetInheritOrientation( bool flag )
	{
		m_bInheritOrientation	= flag;
	}

	// m_DerivedTransformの初期値を設定する.このメソッドはメッシュオブジェクトを階層化した直後に実行する.
	void SceneNode::InheritTransform()
	{
		if( !m_pObject || IsRoot() || ((SceneNode *)parent)->m_pObject==NULL )
			return;

		TransformAttribute *pLocalSpace			= &m_pObject->m_LocalSpace;
		TransformAttribute *pDerivedSpace			= &m_pObject->m_DerivedSpace;
		TransformAttribute *pParentDerivedSpace	= &((SceneNode *)parent)->m_pObject->m_DerivedSpace;


		// Interit Scale
		if( m_bInheritScale )
		{
			// ノードの継承スケールに、自分ノードの相対スケールをかける
			Multiply( pDerivedSpace->m_Scale, pLocalSpace->m_Scale, pParentDerivedSpace->m_Scale );
		}
#ifdef _DEBUG
		tcout << "RelativeScale =" << pLocalSpace->m_Scale.x << ", " << pLocalSpace->m_Scale.y << ", " << pLocalSpace->m_Scale.z << tendl;
		tcout << "DerivedScale =" << pDerivedSpace->m_Scale.x << ", " << pDerivedSpace->m_Scale.y << ", " << pDerivedSpace->m_Scale.z << tendl;
#endif
		// Inherit Translation (apply parent's scale)
		Multiply( pDerivedSpace->m_Origin, pLocalSpace->m_Origin, pParentDerivedSpace->m_Scale );

		// 
		//Subtract( m_DerivedTransform.Translation, m_TransformAttrib.Translation, parent->m_TransformAttrib.Translation );
		//Multiply( m_DerivedTransform.Translation, m_DerivedTransform.Translation, m_DerivedTransform.Scale );
	}

	
}// end of namespace