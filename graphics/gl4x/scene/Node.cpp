#include	"Node.h"



namespace OreOreLib
{

	//######################################################################//
	// SceneNode															//
	//######################################################################//

	Node::Node(int id)
	{
		m_ID			= id;
		m_NodeType		= 0;
		m_numChildren	= 0;
		//m_bInheritScale	= true;
		//m_bInheritOrientation	= true;

		next			= NULL;
		prev			= NULL;
		parent			= this;
		children		= NULL;
	}



	Node::~Node()
	{
		//======================== 子ノードをすべて削除 ====================//
		if(children)	RemoveChildren();
		
		//=================== 親ノードが持つ子ノード情報を更新 =============//
		if(parent)
		{
			if(parent->children==this)	parent->children = next;// 第一子だったら兄弟のポインタをつなぐ
			parent->m_numChildren--;
		}
		
		//==================== 兄弟ノードのリンクをつなぎなおす =============//
		if(prev)	prev->next	= next;
		if(next)	next->prev	= prev;
	}


	
	void Node::AddSibling(int id)
	{
		Node *psibling	= this;
		
		while(psibling->next != NULL)	psibling = psibling->next;

		psibling->next			= new Node(id);
		psibling->next->prev	= this;
		psibling->next->parent	= parent;

		if(parent)	parent->m_numChildren++;
	}



	void Node::AddChild(int id)
	{
		if(children==NULL)	// まだ子ノードがいない場合
		{
			children			= new Node(id);
			children->parent	= this;

			m_numChildren++;
		}
		else				// 既に子ノードがいる場合は最後尾に追加
		{
			Node	*psibling	= children;

			while(psibling->next != NULL)	psibling = psibling->next;

			psibling->next			= new Node(id);
			psibling->next->prev	= psibling;
			psibling->next->parent	= psibling->parent;

			m_numChildren++;
		}
	}
	


	void Node::RemoveChildren()
	{
		//================== 子ノードへ移動 ====================//
		Node *child	= children;

		//=========== 子ノード(兄弟ノード含む)を削除 ===========//
		while(child)
		{
			Node *tmp = child;
			child = child->next;
		
			delete tmp;
		}

		children		= NULL;
		m_numChildren	= 0;
	}



	void Node::Info()
	{
		tcout << "Node( " << this << ")" << tendl;
		tcout << "  parent: " << parent << tendl;
		tcout << "  children: " << children << tendl;
		tcout << "  next: " << next << tendl;
		tcout << "  prev: " << prev << tendl;

	}


}// end of namespace