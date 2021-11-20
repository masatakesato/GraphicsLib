#pragma once

#include	<graphics/common/OBJLoader.h>
#include	"../core/VertexLayouts.h"



namespace vk
{

	class MeshLoader : public OBJLoader
	{
	public:

		MeshLoader() : OBJLoader(){}
		~MeshLoader(){}

		void GenVertexList( OreOreLib::Memory<VertexLayout::Vertex>& vertexlist, OreOreLib::Memory<uint32>& indices );
		void GenVertexList( int& numVertAttrs, VertexLayout::Vertex*& vertices, int &numIndices, uint32*& indices );

	};


}// end of namespace vk