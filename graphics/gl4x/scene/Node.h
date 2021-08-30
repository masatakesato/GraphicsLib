#ifndef NODE_H
#define	NODE_H

#include	"./common/TString.h"

#include	"BufferLayout.h"


namespace OreOreLib
{

	//######################################################################//
	// SceneGraphNode															//
	//######################################################################//
	struct Node
	{   
		int					m_ID;			// node id
		unsigned short		m_NodeType;		// node type
		int					m_numChildren;	// num of child nodes
		
		Node				*next;		// next sibling node
		Node				*prev;		// previous sibling node
		Node				*parent;	// parent node
		Node				*children;	// child nodes

	
		Node( int id=0 );
		virtual ~Node();

		void AddSibling(int id=0);	// add siblings
		void AddChild(int id=0);	// add children
		void RemoveChildren();		// remove all children
	
		inline bool IsRoot()const{ return (this==parent); }
		inline bool IsLeaf()const{ return (children==NULL); }
		inline bool IsFirst()const{ return (prev==NULL); }
		inline bool IsLast()const{ return (next==NULL); }

		virtual void Update(){}
		virtual void Info();


	};



}// end of OreOreLib




#endif	// NODE_H //